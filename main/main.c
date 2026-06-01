#include <stdio.h>

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "Key.h"
#include "bsp_codec.h"
#include "camera.h"
#include "music_player.h"
#include "event_loop.h"
#include "events_init.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "gui_guider.h"
#include "lcd.h"
#include "nvs_flash.h"
#include "rtc_time.h"
#include "sdcard.h"
#include "ui_event.h"
#include "wifi_connect.h"

static const char *TAG = "main";

lv_ui guider_ui;
static TaskHandle_t s_key_task_handle = NULL;
static TaskHandle_t s_ui_task_handle = NULL;

static void stack_monitor_task(void *arg);
static void log_task_stack_watermark(const char *name, TaskHandle_t handle);
static void log_heap_caps_stats(void);

void Key_Task(void *arg);
void ui_main_init(void);

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    ui_event_loop_init();
    rtc_time_init();
    sdcard_init_mount_fs();
    Key_Init();
    bsp_i2c_init();
    pca9557_init();
    bsp_lvgl_start();

    if (lvgl_port_lock(0)) 
    {
        setup_ui(&guider_ui);
        events_init(&guider_ui);
        ui_event_refresh_main_clock();
        lvgl_port_unlock();
    }
    wifi_init();
    camera_register_camera_handler();

    BaseType_t key_task_ok = xTaskCreatePinnedToCoreWithCaps(
        Key_Task, "Key_Task", 3072, NULL, 10, &s_key_task_handle, 1,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    configASSERT(key_task_ok == pdPASS);

    BaseType_t ui_task_ok = xTaskCreatePinnedToCoreWithCaps(
        my_ui_task, "my_ui_task", 5120, NULL, 4, &s_ui_task_handle, 1,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    configASSERT(ui_task_ok == pdPASS);

    BaseType_t monitor_task_ok = xTaskCreatePinnedToCoreWithCaps(
        stack_monitor_task, "stack_monitor", 3072, NULL, 1, NULL, 1,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    configASSERT(monitor_task_ok == pdPASS);
    
}

static void log_task_stack_watermark(const char *name, TaskHandle_t handle)
{
    if (handle == NULL) {
        ESP_LOGW(TAG, "Stack monitor: task '%s' handle is NULL", name);
        return;
    }

    UBaseType_t watermark_words = uxTaskGetStackHighWaterMark(handle);
    size_t watermark_bytes = watermark_words * sizeof(StackType_t);
    ESP_LOGI(TAG, "Stack free: %s -> %u words (%u bytes)",
             name,
             (unsigned)watermark_words,
             (unsigned)watermark_bytes);
}

static void log_heap_caps_stats(void)
{
    size_t internal_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    size_t internal_largest = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);
    size_t dma_free = heap_caps_get_free_size(MALLOC_CAP_DMA);
    size_t dma_largest = heap_caps_get_largest_free_block(MALLOC_CAP_DMA);

    ESP_LOGI(TAG,
             "Heap caps: internal free=%u largest=%u, dma free=%u largest=%u",
             (unsigned)internal_free,
             (unsigned)internal_largest,
             (unsigned)dma_free,
             (unsigned)dma_largest);
}

static void stack_monitor_task(void *arg)
{
    (void)arg;

    const TickType_t log_period = pdMS_TO_TICKS(10000);

    for (;;) {
        vTaskDelay(log_period);

        log_task_stack_watermark("Key_Task", s_key_task_handle);
        log_task_stack_watermark("my_ui_task", s_ui_task_handle);
        log_task_stack_watermark("taskLVGL", xTaskGetHandle("taskLVGL"));
        log_task_stack_watermark("ui_event_loop", xTaskGetHandle("ui_event_loop"));
        log_heap_caps_stats();
    }
}

void Key_Task(void *arg)
{
    uint8_t io_num;
    uint8_t light = 100;
    ui_screen_state_t current_screen;

    (void)arg;

    for (;;) {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) 
        {
            current_screen = ui_event_get_screen_state();

            if (current_screen == UI_SCREEN_MAIN) 
            {
                light += 10;
                if (light > 100) 
                {
                    light = 0;
                }
                bsp_display_brightness_set(light);
            }
            else if (current_screen == UI_SCREEN_MUSIC) 
            {
                g_sys_volume += 5;
                if (g_sys_volume > 100) 
                {
                    g_sys_volume = 0;
                }
                change_sys_volume(g_sys_volume);
                if (lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) 
                {
                    lv_slider_set_value(guider_ui.music_slider_volume, g_sys_volume, LV_ANIM_OFF);
                    lv_obj_send_event(guider_ui.music_slider_volume, LV_EVENT_VALUE_CHANGED, NULL);
                    lvgl_port_unlock();
                }
            }
            else if (current_screen == UI_SCREEN_CAMERA)
            {
                camera_request_save();
            }

        }
    }
}
