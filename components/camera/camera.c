#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "audio_player.h"
#include "event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "gui_guider.h"
#include "lcd.h"
#include "nvs_flash.h"
#include "wifi_connect.h"

#include "camera.h"
#include "ui_event.h"

static const char *TAG = "app_camera";

#define CAMERA_FRAME_WIDTH      320
#define CAMERA_FRAME_HEIGHT     240
#define CAMERA_FRAME_PIXELS     (CAMERA_FRAME_WIDTH * CAMERA_FRAME_HEIGHT)
#define CAMERA_FRAME_BUF_SIZE   (CAMERA_FRAME_PIXELS * 2)
#define CAMERA_NVS_NAMESPACE    "user"
#define CAMERA_NVS_FRAME_KEY    "frame_seq"

typedef struct {
    uint8_t *data;
    uint8_t buf_index;
} frame_buf_info_t;

static uint8_t *frame_out_buf[2] = {NULL, NULL};
static SemaphoreHandle_t frame_buf_sem[2] = {NULL, NULL};
static QueueHandle_t xQueueLCDFrame = NULL;

QueueHandle_t g_Camera_event_queue = NULL;

static TaskHandle_t process_lcd_handle = NULL;
static TaskHandle_t process_camera_handle = NULL;
static lv_image_dsc_t s_camera_image = {0};
static bool s_camera_initialized = false;
static volatile bool s_camera_tasks_running = false;

uint8_t first_save_camera_frame_flag = 1;
int camera_frame_save_file_counter = 0;

extern lv_ui guider_ui;

static void app_camera_double_buff_init(void);
static void camera_frame_counter_restore(void);
static void camera_frame_counter_store(void);
static void camera_runtime_destroy(void);
static bool camera_runtime_blocked_by_background(void);
static void camera_ui_set_status_text(const char *text);
static bool camera_screen_is_active(void);
static bool camera_wait_task_exit(TaskHandle_t *handle, TickType_t timeout_ticks);
static void task_process_lcd(void *arg);
static void task_process_camera(void *arg);
static esp_err_t save_frame_to_bmp_internal(const uint8_t *frame_data, char *saved_name,
                                            size_t saved_name_size);
static void user_camera_init_handler(void *arg, esp_event_base_t event_base,
                                     int32_t event_id, void *event_data);
static void user_camera_deinit_handler(void *arg, esp_event_base_t event_base,
                                       int32_t event_id, void *event_data);

static bool camera_screen_is_active(void)
{
    bool is_active = false;

    if (!lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) {
        return false;
    }

    is_active = (guider_ui.camera != NULL && lv_obj_is_valid(guider_ui.camera) &&
                 lv_screen_active() == guider_ui.camera);
    lvgl_port_unlock();

    return is_active;
}

static bool camera_runtime_blocked_by_background(void)
{
    if (audio_player_get_state() != AUDIO_PLAYER_STATE_IDLE) {
        return true;
    }

    if (current_wifi_state != WIFI_STATE_CLOSED) {
        return true;
    }

    return false;
}

static void camera_ui_set_status_text(const char *text)
{
    if (text == NULL) {
        return;
    }

    if (!lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) {
        return;
    }

    if (guider_ui.camera_label_status != NULL &&
        lv_obj_is_valid(guider_ui.camera_label_status)) {
        lv_label_set_text(guider_ui.camera_label_status, text);
    }

    lvgl_port_unlock();
}

static bool camera_wait_task_exit(TaskHandle_t *handle, TickType_t timeout_ticks)
{
    TickType_t start = xTaskGetTickCount();

    while (*handle != NULL) {
        if ((xTaskGetTickCount() - start) >= timeout_ticks) {
            return false;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    return true;
}

static void camera_frame_counter_restore(void)
{
    nvs_handle_t handle;
    int32_t counter = 0;
    esp_err_t err = nvs_open(CAMERA_NVS_NAMESPACE, NVS_READWRITE, &handle);

    if (err != ESP_OK) {
        ESP_LOGW(TAG, "nvs open failed while restoring frame counter: %s",
                 esp_err_to_name(err));
        camera_frame_save_file_counter = 0;
        return;
    }

    err = nvs_get_i32(handle, CAMERA_NVS_FRAME_KEY, &counter);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        counter = 0;
    } else if (err != ESP_OK) {
        ESP_LOGW(TAG, "nvs get failed while restoring frame counter: %s",
                 esp_err_to_name(err));
        counter = 0;
    }

    nvs_close(handle);
    camera_frame_save_file_counter = counter;
}

static void camera_frame_counter_store(void)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(CAMERA_NVS_NAMESPACE, NVS_READWRITE, &handle);

    if (err != ESP_OK) {
        ESP_LOGW(TAG, "nvs open failed while storing frame counter: %s",
                 esp_err_to_name(err));
        return;
    }

    err = nvs_set_i32(handle, CAMERA_NVS_FRAME_KEY, camera_frame_save_file_counter);
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "nvs store failed while storing frame counter: %s",
                 esp_err_to_name(err));
    }

    nvs_close(handle);
}

esp_err_t bsp_camera_init(void)
{
    camera_config_t config = {0};
    esp_err_t err;
    sensor_t *sensor;

    if (s_camera_initialized) {
        return ESP_OK;
    }

    camera_dvp_pwdn_set(0);
    vTaskDelay(pdMS_TO_TICKS(20));

    config.ledc_channel = LEDC_CHANNEL_1;
    config.ledc_timer = LEDC_TIMER_1;
    config.pin_d0 = CAMERA_PIN_D0;
    config.pin_d1 = CAMERA_PIN_D1;
    config.pin_d2 = CAMERA_PIN_D2;
    config.pin_d3 = CAMERA_PIN_D3;
    config.pin_d4 = CAMERA_PIN_D4;
    config.pin_d5 = CAMERA_PIN_D5;
    config.pin_d6 = CAMERA_PIN_D6;
    config.pin_d7 = CAMERA_PIN_D7;
    config.pin_xclk = CAMERA_PIN_XCLK;
    config.pin_pclk = CAMERA_PIN_PCLK;
    config.pin_vsync = CAMERA_PIN_VSYNC;
    config.pin_href = CAMERA_PIN_HREF;
    config.pin_sccb_sda = -1;
    config.pin_sccb_scl = -1;
    config.sccb_i2c_port = BSP_I2C_NUM;
    config.pin_pwdn = CAMERA_PIN_PWDN;
    config.pin_reset = CAMERA_PIN_RESET;
    config.xclk_freq_hz = XCLK_FREQ_HZ;
    config.pixel_format = PIXFORMAT_RGB565;
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

    err = esp_camera_init(&config);
    if (err != ESP_OK) {
        camera_dvp_pwdn_set(1);
        ESP_LOGE(TAG, "camera init failed: %s", esp_err_to_name(err));
        return err;
    }

    sensor = esp_camera_sensor_get();
    if (sensor != NULL && sensor->id.PID == GC0308_PID) {
        sensor->set_hmirror(sensor, 1);
    }

    camera_frame_counter_restore();
    first_save_camera_frame_flag = 1;
    s_camera_initialized = true;
    return ESP_OK;
}

void bsp_camera_deinit(void)
{
    s_camera_tasks_running = false;

    if (xQueueLCDFrame != NULL) {
        frame_buf_info_t stop_frame = {
            .data = NULL,
            .buf_index = 0,
        };
        xQueueSend(xQueueLCDFrame, &stop_frame, 0);
    }
    if (frame_buf_sem[0] != NULL) {
        xSemaphoreGive(frame_buf_sem[0]);
    }
    if (frame_buf_sem[1] != NULL) {
        xSemaphoreGive(frame_buf_sem[1]);
    }

    (void)camera_wait_task_exit(&process_camera_handle, pdMS_TO_TICKS(1000));
    (void)camera_wait_task_exit(&process_lcd_handle, pdMS_TO_TICKS(1000));

    if (lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) {
        if (guider_ui.camera_img_preview != NULL &&
            lv_obj_is_valid(guider_ui.camera_img_preview)) {
            lv_image_set_src(guider_ui.camera_img_preview, NULL);
            lv_obj_invalidate(guider_ui.camera_img_preview);
        }
        lvgl_port_unlock();
    }

    if (process_camera_handle != NULL || process_lcd_handle != NULL) {
        ESP_LOGW(TAG, "camera tasks did not exit cleanly, skip camera deinit");
        return;
    }

    if (s_camera_initialized) {
        esp_camera_deinit();
        camera_dvp_pwdn_set(1);
        camera_frame_counter_store();
        s_camera_initialized = false;
    }

    camera_runtime_destroy();
    ui_runtime_set_active(UI_RUNTIME_TASK_CAMERA, false);
}

void camera_dvp_pwdn_set(uint8_t level)
{
    dvp_pwdn(level);
}

static void camera_runtime_destroy(void)
{
    if (xQueueLCDFrame != NULL) {
        vQueueDelete(xQueueLCDFrame);
        xQueueLCDFrame = NULL;
    }
    if (g_Camera_event_queue != NULL) {
        vQueueDelete(g_Camera_event_queue);
        g_Camera_event_queue = NULL;
    }

    if (frame_buf_sem[0] != NULL) {
        vSemaphoreDelete(frame_buf_sem[0]);
        frame_buf_sem[0] = NULL;
    }
    if (frame_buf_sem[1] != NULL) {
        vSemaphoreDelete(frame_buf_sem[1]);
        frame_buf_sem[1] = NULL;
    }

    if (frame_out_buf[0] != NULL) {
        heap_caps_free(frame_out_buf[0]);
        frame_out_buf[0] = NULL;
    }
    if (frame_out_buf[1] != NULL) {
        heap_caps_free(frame_out_buf[1]);
        frame_out_buf[1] = NULL;
    }

    memset(&s_camera_image, 0, sizeof(s_camera_image));
}

static void task_process_lcd(void *arg)
{
    frame_buf_info_t buf_info;
    bool first_frame = true;

    (void)arg;

    s_camera_image = (lv_image_dsc_t){
        .header = {
            .magic = LV_IMAGE_HEADER_MAGIC,
            .cf = LV_COLOR_FORMAT_RGB565_SWAPPED,
            .flags = LV_IMAGE_FLAGS_MODIFIABLE,
            .w = CAMERA_FRAME_WIDTH,
            .h = CAMERA_FRAME_HEIGHT,
            .stride = CAMERA_FRAME_WIDTH * 2,
        },
        .data_size = CAMERA_FRAME_BUF_SIZE,
        .data = NULL,
        .reserved = NULL,
        .reserved_2 = NULL,
    };

    while (s_camera_tasks_running) {
        camera_command_t camera_cmd;

        if (xQueueReceive(xQueueLCDFrame, &buf_info, pdMS_TO_TICKS(50)) != pdTRUE) {
            continue;
        }

        if (!s_camera_tasks_running) {
            break;
        }

        if (buf_info.data == NULL) {
            continue;
        }

        if (lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) {
            s_camera_image.data = buf_info.data;
            if (guider_ui.camera_img_preview != NULL &&
                lv_obj_is_valid(guider_ui.camera_img_preview)) {
                lv_image_set_src(guider_ui.camera_img_preview, &s_camera_image);
                lv_obj_invalidate(guider_ui.camera_img_preview);
            }
            lvgl_port_unlock();
        }

        if (first_frame) {
            camera_ui_set_status_text("Preview ready");
            first_frame = false;
        }

        if (g_Camera_event_queue != NULL &&
            xQueueReceive(g_Camera_event_queue, &camera_cmd, 0) == pdTRUE) {
            switch (camera_cmd.type) {
            case CAMER_CMD_STORE: {
                char saved_name[64] = {0};
                if (save_frame_to_bmp_internal(buf_info.data, saved_name,
                                               sizeof(saved_name)) == ESP_OK) {
                    camera_ui_set_status_text(saved_name);
                } else {
                    camera_ui_set_status_text("Save failed");
                }
                break;
            }
            default:
                break;
            }
        }

        if (frame_buf_sem[buf_info.buf_index] != NULL) {
            xSemaphoreGive(frame_buf_sem[buf_info.buf_index]);
        }
    }

    process_lcd_handle = NULL;
    vTaskDelete(NULL);
}

static void task_process_camera(void *arg)
{
    uint8_t current_buf_index = 0;

    (void)arg;

    while (s_camera_tasks_running) {
        camera_fb_t *frame = esp_camera_fb_get();
        if (frame == NULL) {
            if (!s_camera_tasks_running) {
                break;
            }
            ESP_LOGW(TAG, "camera frame is null");
            continue;
        }

        if (!s_camera_tasks_running) {
            esp_camera_fb_return(frame);
            break;
        }

        if (frame->len < CAMERA_FRAME_BUF_SIZE) {
            ESP_LOGW(TAG, "unexpected camera frame length: %u", (unsigned)frame->len);
            esp_camera_fb_return(frame);
            continue;
        }

        if (xSemaphoreTake(frame_buf_sem[current_buf_index], portMAX_DELAY) == pdTRUE) {
            frame_buf_info_t buf_info = {
                .data = frame_out_buf[current_buf_index],
                .buf_index = current_buf_index,
            };

            memcpy(frame_out_buf[current_buf_index], frame->buf, CAMERA_FRAME_BUF_SIZE);
            esp_camera_fb_return(frame);

            if (xQueueSend(xQueueLCDFrame, &buf_info, pdMS_TO_TICKS(50)) == pdTRUE) {
                current_buf_index = (current_buf_index + 1U) % 2U;
            } else {
                if (frame_buf_sem[current_buf_index] != NULL) {
                    xSemaphoreGive(frame_buf_sem[current_buf_index]);
                }
                if (!s_camera_tasks_running) {
                    break;
                }
            }
        } else {
            esp_camera_fb_return(frame);
        }
    }

    process_camera_handle = NULL;
    vTaskDelete(NULL);
}

void app_camera_lcd(void)
{
    app_camera_double_buff_init();
    s_camera_tasks_running = true;

    xTaskCreatePinnedToCore(task_process_camera, "task_process_camera", 4 * 1024, NULL,
                            5, &process_camera_handle, 1);
    xTaskCreatePinnedToCore(task_process_lcd, "task_process_lcd", 8 * 1024, NULL, 5,
                            &process_lcd_handle, 0);
}

static void app_camera_double_buff_init(void)
{
    if (frame_out_buf[0] != NULL || frame_out_buf[1] != NULL || xQueueLCDFrame != NULL ||
        g_Camera_event_queue != NULL) {
        return;
    }

    frame_out_buf[0] = heap_caps_malloc(CAMERA_FRAME_BUF_SIZE,
                                        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    frame_out_buf[1] = heap_caps_malloc(CAMERA_FRAME_BUF_SIZE,
                                        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (frame_out_buf[0] == NULL || frame_out_buf[1] == NULL) {
        ESP_LOGE(TAG, "failed to allocate camera frame buffers");
        camera_runtime_destroy();
        return;
    }

    frame_buf_sem[0] = xSemaphoreCreateBinary();
    frame_buf_sem[1] = xSemaphoreCreateBinary();
    if (frame_buf_sem[0] == NULL || frame_buf_sem[1] == NULL) {
        ESP_LOGE(TAG, "failed to create camera semaphores");
        camera_runtime_destroy();
        return;
    }

    xSemaphoreGive(frame_buf_sem[0]);
    xSemaphoreGive(frame_buf_sem[1]);

    xQueueLCDFrame = xQueueCreate(2, sizeof(frame_buf_info_t));
    g_Camera_event_queue = xQueueCreate(4, sizeof(camera_command_t));
    if (xQueueLCDFrame == NULL || g_Camera_event_queue == NULL) {
        ESP_LOGE(TAG, "failed to create camera queues");
        camera_runtime_destroy();
    }
}

static esp_err_t save_frame_to_bmp_internal(const uint8_t *frame_data, char *saved_name,
                                            size_t saved_name_size)
{
    char file_path[96];
    FILE *file;
    const int width = CAMERA_FRAME_WIDTH;
    const int height = CAMERA_FRAME_HEIGHT;
    const size_t row_size = (size_t)width * 3U;
    const size_t row_stride = (row_size + 3U) & ~3U;
    const size_t rgb888_size = row_size * (size_t)height;
    const size_t bmp_image_size = row_stride * (size_t)height;
    uint8_t *rgb888_buf;
    uint16_t *rgb565_data = (uint16_t *)frame_data;
    struct stat st;

    if (frame_data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (first_save_camera_frame_flag == 1) {
        first_save_camera_frame_flag = 0;
        if (stat(CAMERA_SAVED_PIC_PATH, &st) != 0) {
            if (mkdir(CAMERA_SAVED_PIC_PATH, 0777) != 0) {
                ESP_LOGE(TAG, "failed to create directory %s", CAMERA_SAVED_PIC_PATH);
                return ESP_FAIL;
            }
        }
    }

    snprintf(file_path, sizeof(file_path), "%s/fra_%04d.bmp", CAMERA_SAVED_PIC_PATH,
             camera_frame_save_file_counter++);

    file = fopen(file_path, "wb");
    if (file == NULL) {
        ESP_LOGE(TAG, "failed to open %s", file_path);
        return ESP_FAIL;
    }

    rgb888_buf = heap_caps_malloc(rgb888_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (rgb888_buf == NULL) {
        fclose(file);
        ESP_LOGE(TAG, "failed to allocate RGB888 buffer");
        return ESP_ERR_NO_MEM;
    }

    for (int i = 0; i < width * height; i++) {
        uint16_t pixel = rgb565_data[i];
        pixel = (uint16_t)(((pixel & 0x00FFU) << 8) | ((pixel & 0xFF00U) >> 8));
        uint8_t r5 = (pixel >> 11) & 0x1F;
        uint8_t g6 = (pixel >> 5) & 0x3F;
        uint8_t b5 = pixel & 0x1F;

        rgb888_buf[i * 3 + 0] = (uint8_t)((b5 << 3) | (b5 >> 2));
        rgb888_buf[i * 3 + 1] = (uint8_t)((g6 << 2) | (g6 >> 4));
        rgb888_buf[i * 3 + 2] = (uint8_t)((r5 << 3) | (r5 >> 2));
    }

    {
        uint8_t bmp_header[14] = {
            'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0,
        };
        uint8_t dib_header[40] = {
            40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        };
        uint32_t file_size = 54U + (uint32_t)bmp_image_size;
        uint32_t img_size = (uint32_t)bmp_image_size;
        int32_t dib_width = width;
        int32_t dib_height = height;
        uint8_t row_padding[3] = {0};

        bmp_header[2] = (uint8_t)(file_size & 0xFF);
        bmp_header[3] = (uint8_t)((file_size >> 8) & 0xFF);
        bmp_header[4] = (uint8_t)((file_size >> 16) & 0xFF);
        bmp_header[5] = (uint8_t)((file_size >> 24) & 0xFF);

        dib_header[4] = (uint8_t)(dib_width & 0xFF);
        dib_header[5] = (uint8_t)((dib_width >> 8) & 0xFF);
        dib_header[6] = (uint8_t)((dib_width >> 16) & 0xFF);
        dib_header[7] = (uint8_t)((dib_width >> 24) & 0xFF);
        dib_header[8] = (uint8_t)(dib_height & 0xFF);
        dib_header[9] = (uint8_t)((dib_height >> 8) & 0xFF);
        dib_header[10] = (uint8_t)((dib_height >> 16) & 0xFF);
        dib_header[11] = (uint8_t)((dib_height >> 24) & 0xFF);
        dib_header[20] = (uint8_t)(img_size & 0xFF);
        dib_header[21] = (uint8_t)((img_size >> 8) & 0xFF);
        dib_header[22] = (uint8_t)((img_size >> 16) & 0xFF);
        dib_header[23] = (uint8_t)((img_size >> 24) & 0xFF);

        fwrite(bmp_header, 1, sizeof(bmp_header), file);
        fwrite(dib_header, 1, sizeof(dib_header), file);

        for (int y = height - 1; y >= 0; y--) {
            const uint8_t *row_ptr = rgb888_buf + ((size_t)y * row_size);
            fwrite(row_ptr, 1, row_size, file);
            if (row_stride > row_size) {
                fwrite(row_padding, 1, row_stride - row_size, file);
            }
        }
    }

    heap_caps_free(rgb888_buf);
    fclose(file);

    if (saved_name != NULL && saved_name_size > 0) {
        snprintf(saved_name, saved_name_size, "Saved: fra_%04d.bmp",
                 camera_frame_save_file_counter - 1);
    }

    ESP_LOGI(TAG, "frame saved to %s", file_path);
    return ESP_OK;
}

void save_frame_to_bmp(const char *out_image)
{
    (void)save_frame_to_bmp_internal((const uint8_t *)out_image, NULL, 0);
}

bool camera_request_save(void)
{
    camera_command_t cmd = {
        .type = CAMER_CMD_STORE,
        .data = NULL,
    };

    if (!s_camera_initialized || g_Camera_event_queue == NULL) {
        return false;
    }

    return xQueueSend(g_Camera_event_queue, &cmd, 0) == pdTRUE;
}

void camera_register_camera_handler(void)
{
    ESP_ERROR_CHECK(esp_event_handler_register_with(ui_event_loop_handle, APP_EVENT,
                                                    APP_CAMERA_ENTER,
                                                    user_camera_init_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register_with(ui_event_loop_handle, APP_EVENT,
                                                    APP_CAMERA_EXIT,
                                                    user_camera_deinit_handler, NULL));
}

static void user_camera_init_handler(void *arg, esp_event_base_t event_base,
                                     int32_t event_id, void *event_data)
{
    esp_err_t err;

    (void)arg;
    (void)event_base;
    (void)event_id;
    (void)event_data;

    camera_ui_set_status_text("Camera starting...");

    if (i2c_master_bus_wait_all_done(i2c_bus_handle, -1) == ESP_OK) {
        ESP_LOGI(TAG, "i2c bus flushed before camera start");
    }

    vTaskDelay(pdMS_TO_TICKS(2000));

    if (!camera_screen_is_active()) {
        ESP_LOGI(TAG, "camera screen left before preview start");
        return;
    }

    if (camera_runtime_blocked_by_background()) {
        ESP_LOGW(TAG, "camera start blocked because music player or WiFi is active");
        camera_ui_set_status_text("Stop music/WiFi first");
        ui_runtime_set_active(UI_RUNTIME_TASK_CAMERA, false);
        return;
    }

    err = bsp_camera_init();
    if (err != ESP_OK) {
        camera_ui_set_status_text("Camera init failed");
        ui_runtime_set_active(UI_RUNTIME_TASK_CAMERA, false);
        return;
    }

    app_camera_lcd();
}

static void user_camera_deinit_handler(void *arg, esp_event_base_t event_base,
                                       int32_t event_id, void *event_data)
{
    (void)arg;
    (void)event_base;
    (void)event_id;
    (void)event_data;

    bsp_camera_deinit();
}
