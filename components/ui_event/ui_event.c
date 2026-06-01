#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_event.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "gui_guider.h"
#include "music_player.h"
#include "rtc_time.h"
#include "ui_event.h"
#include "wifi_ui_logic.h"

static const char *TAG = "ui_event";
static const uint16_t FILE_LIST_PAGE_SIZE = 3;

ui_main_status_t ui_main_status = {false, false, false};
volatile ui_screen_state_t g_ui_screen_state = UI_SCREEN_MAIN;

typedef enum {
    SD_FRESH_RESQUEST = 0x00,
    MUSIC_LIST_RESQUEST,
    UI_MSG_WIFI_SCAN_DONE,
    UI_MSG_WIFI_DISCONNECTED,
    UI_MSG_WIFI_GOT_IP,
    UI_MSG_WIFI_CLOSED,
    UI_MSG_MAIN_STATUS_UPDATE,
} ui_message_type_t;

typedef struct {
    ui_message_type_t type;
    void *data;
} ui_message_t;

typedef struct {
    ui_file_browser_entry_t entry;
    file_filter_mode_t filter_mode;
    char last_menu_dir[SD_PATH_MAX];
    char last_music_dir[SD_PATH_MAX];
} ui_file_browser_context_t;

static QueueHandle_t ui_message_queue;
static file_list_data_t *g_file_list_page_data = NULL;
static uint16_t g_file_list_page_index = 0;
static lv_obj_t *g_file_page_prev_btn = NULL;
static lv_obj_t *g_file_page_next_btn = NULL;
static lv_obj_t *g_file_page_status_label = NULL;
static bool g_file_list_render_pending = false;
static ui_file_browser_context_t g_file_browser_ctx = {
    .entry = UI_FILE_ENTRY_MENU,
    .filter_mode = FILE_FILTER_ALL,
    .last_menu_dir = "/sdcard/",
    .last_music_dir = "",
};
static char g_photo_view_file_name[SD_PATH_MAX / 2] = {0};
static char g_photo_view_abs_path[SD_PATH_MAX] = {0};
static char g_photo_view_lvgl_src[SD_PATH_MAX + 3] = {0};
static lv_timer_t *g_main_clock_timer = NULL;
static bool g_main_clock_has_valid_cache = false;
static int g_main_clock_hour = -1;
static int g_main_clock_min = -1;
static int g_main_clock_year = -1;
static int g_main_clock_mon = -1;
static int g_main_clock_day = -1;
static int g_main_clock_wday = -1;

static void sd_refresh_response_handler(void *arg, esp_event_base_t event_base,
                                        int32_t event_id, void *event_data);
static void user_music_play_response_handler(void *arg, esp_event_base_t event_base,
                                             int32_t event_id, void *event_data);
static void wifi_scan_done_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data);
static void wifi_disconnected_handler(void *arg, esp_event_base_t event_base,
                                      int32_t event_id, void *event_data);
static void wifi_got_ip_handler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data);
static void wifi_closed_handler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data);
static void ui_message_release(ui_message_t *msg);
static void ui_file_list_release_data(file_list_data_t *data);
static void ui_file_list_reset_page_state(void);
static void ui_file_list_clear_dynamic_items(void);
static void ui_file_list_invalidate_pager_controls(void);
static void ui_file_list_page_prev_event_cb(lv_event_t *e);
static void ui_file_list_page_next_event_cb(lv_event_t *e);
static void ui_file_list_ensure_pager_controls(void);
static void ui_file_list_render_async_cb(void *user_data);
static void ui_file_list_request_render_async(void);
static void ui_file_list_render_current_page(void);
static void ui_file_list_update_pager_controls(void);
static bool ui_file_type_is_image(file_type_t type);
static bool ui_file_type_is_audio(file_type_t type);
static void ui_join_sd_path(char *out_path, size_t out_path_size, const char *dir_path,
                            const char *name);
static void ui_photo_view_prepare(const char *file_name, const char *abs_path);
static void ui_photo_view_apply(void);
static void ui_file_browser_apply_entry_ui(void);
static void ui_file_browser_set_entry(ui_file_browser_entry_t entry);
static void ui_file_browser_request_refresh(const char *base_path, const char *name);
static void ui_file_browser_store_current_dir(const char *path);
static const char *ui_file_browser_get_resume_dir(ui_file_browser_entry_t entry);
static const lv_image_dsc_t *ui_main_clock_get_digit_image(uint8_t digit);
static const char *ui_main_clock_get_weekday(int weekday);
static void ui_main_clock_refresh(void);
static void ui_main_clock_timer_cb(lv_timer_t *timer);
static void ui_main_status_update_locked(void);
static void ui_main_status_update(void);
void file_list_click_cb(lv_event_t *e);

uint8_t ui_runtime_task_count(void)
{
    uint8_t count = 0;

    if (ui_main_status.music) {
        count++;
    }
    if (ui_main_status.wifi) {
        count++;
    }
    if (ui_main_status.camera) {
        count++;
    }

    return count;
}

bool ui_runtime_is_active(ui_runtime_task_t task)
{
    switch (task) {
    case UI_RUNTIME_TASK_MUSIC:
        return ui_main_status.music;
    case UI_RUNTIME_TASK_WIFI:
        return ui_main_status.wifi;
    case UI_RUNTIME_TASK_CAMERA:
        return ui_main_status.camera;
    default:
        return false;
    }
}

void ui_runtime_set_active(ui_runtime_task_t task, bool active)
{
    switch (task) {
    case UI_RUNTIME_TASK_MUSIC:
        ui_main_status.music = active;
        break;
    case UI_RUNTIME_TASK_WIFI:
        ui_main_status.wifi = active;
        break;
    case UI_RUNTIME_TASK_CAMERA:
        ui_main_status.camera = active;
        break;
    default:
        return;
    }
}

bool ui_runtime_can_start(ui_runtime_task_t task)
{
    if (ui_runtime_is_active(task)) {
        return true;
    }

    return ui_runtime_task_count() < 2U;
}

void ui_event_set_screen_state(ui_screen_state_t state)
{
    g_ui_screen_state = state;
}

ui_screen_state_t ui_event_get_screen_state(void)
{
    return g_ui_screen_state;
}

static bool ui_file_type_is_image(file_type_t type)
{
    return type == FILE_TYPE_BMP || type == FILE_TYPE_JPG || type == FILE_TYPE_PNG;
}

static bool ui_file_type_is_audio(file_type_t type)
{
    return type == FILE_TYPE_MP3 || type == FILE_TYPE_FLAC || type == FILE_TYPE_WAV;
}

static void ui_join_sd_path(char *out_path, size_t out_path_size, const char *dir_path,
                            const char *name)
{
    size_t dir_len;

    if (out_path == NULL || out_path_size == 0 || dir_path == NULL || name == NULL) {
        return;
    }

    dir_len = strlen(dir_path);
    if (dir_len > 0 && dir_path[dir_len - 1] == '/') {
        snprintf(out_path, out_path_size, "%s%s", dir_path, name);
    } else {
        snprintf(out_path, out_path_size, "%s/%s", dir_path, name);
    }
}

static void ui_photo_view_prepare(const char *file_name, const char *abs_path)
{
    if (file_name == NULL || abs_path == NULL) {
        return;
    }

    snprintf(g_photo_view_file_name, sizeof(g_photo_view_file_name), "%s", file_name);
    snprintf(g_photo_view_abs_path, sizeof(g_photo_view_abs_path), "%s", abs_path);
    snprintf(g_photo_view_lvgl_src, sizeof(g_photo_view_lvgl_src), "S:%s", abs_path);
}

static void ui_photo_view_apply(void)
{
    if (guider_ui.photo_label_name != NULL &&
        lv_obj_is_valid(guider_ui.photo_label_name)) {
        lv_label_set_text(guider_ui.photo_label_name, g_photo_view_file_name);
    }

    if (guider_ui.photo_img_view != NULL && lv_obj_is_valid(guider_ui.photo_img_view)) {
        lv_image_set_src(guider_ui.photo_img_view, g_photo_view_lvgl_src);
    }
}

static void ui_file_browser_apply_entry_ui(void)
{
    const char *entry_text;

    if (guider_ui.file_list_file == NULL || !lv_obj_is_valid(guider_ui.file_list_file)) {
        return;
    }

    entry_text =
        (g_file_browser_ctx.entry == UI_FILE_ENTRY_MUSIC) ? "<- Player" : "<- Menu";

    if (guider_ui.file_list_file_item0 != NULL &&
        lv_obj_is_valid(guider_ui.file_list_file_item0)) {
        lv_list_set_button_text(guider_ui.file_list_file, guider_ui.file_list_file_item0,
                                entry_text);
    }

    if (guider_ui.file_list_file_item1 != NULL &&
        lv_obj_is_valid(guider_ui.file_list_file_item1)) {
        lv_list_set_button_text(guider_ui.file_list_file, guider_ui.file_list_file_item1,
                                "..");
    }
}

static void ui_file_browser_set_entry(ui_file_browser_entry_t entry)
{
    g_file_browser_ctx.entry = entry;
    g_file_browser_ctx.filter_mode =
        (entry == UI_FILE_ENTRY_MUSIC) ? FILE_FILTER_AUDIO_ONLY : FILE_FILTER_ALL;
}

static void ui_file_browser_store_current_dir(const char *path)
{
    if (path == NULL || path[0] == '\0') {
        return;
    }

    if (g_file_browser_ctx.entry == UI_FILE_ENTRY_MUSIC) {
        snprintf(g_file_browser_ctx.last_music_dir, sizeof(g_file_browser_ctx.last_music_dir),
                 "%s", path);
    } else {
        snprintf(g_file_browser_ctx.last_menu_dir, sizeof(g_file_browser_ctx.last_menu_dir),
                 "%s", path);
    }
}

static const char *ui_file_browser_get_resume_dir(ui_file_browser_entry_t entry)
{
    if (entry == UI_FILE_ENTRY_MUSIC) {
        if (g_file_browser_ctx.last_music_dir[0] != '\0') {
            return g_file_browser_ctx.last_music_dir;
        }
        if (sd_current_dir[0] != '\0') {
            return sd_current_dir;
        }
    } else if (g_file_browser_ctx.last_menu_dir[0] != '\0') {
        return g_file_browser_ctx.last_menu_dir;
    }

    return "/sdcard/";
}

static void ui_file_browser_request_refresh(const char *base_path, const char *name)
{
    file_refresh_req_data_t data = {0};

    data.is_directory = true;
    data.filter_mode = g_file_browser_ctx.filter_mode;

    if (name != NULL) {
        snprintf(data.name, sizeof(data.name), "%s", name);
    }

    if (base_path != NULL && base_path[0] != '\0') {
        snprintf(data.current_path, sizeof(data.current_path), "%s", base_path);
    } else {
        snprintf(data.current_path, sizeof(data.current_path), "%s", "/sdcard/");
    }

    esp_event_post_to(ui_event_loop_handle, APP_EVENT, APP_FILE_REFRESH_REQUEST, &data,
                      sizeof(data), portMAX_DELAY);
}

static const lv_image_dsc_t *ui_main_clock_get_digit_image(uint8_t digit)
{
    switch (digit) {
    case 0:
        return &_icon_mini_0_RGB565A8_70x92;
    case 1:
        return &_icon_mini_1_RGB565A8_70x92;
    case 2:
        return &_icon_mini_2_RGB565A8_70x92;
    case 3:
        return &_icon_mini_3_RGB565A8_70x92;
    case 4:
        return &_icon_mini_4_RGB565A8_70x92;
    case 5:
        return &_icon_mini_5_RGB565A8_70x92;
    case 6:
        return &_icon_mini_6_RGB565A8_70x92;
    case 7:
        return &_icon_mini_7_RGB565A8_70x92;
    case 8:
        return &_icon_mini_8_RGB565A8_70x92;
    case 9:
        return &_icon_mini_9_RGB565A8_70x92;
    default:
        return &_icon_mini_0_RGB565A8_70x92;
    }
}


static const char *ui_main_clock_get_weekday(int weekday)
{
    static const char *const weekday_names[] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
    };

    if (weekday < 0 ||
        weekday >= (int)(sizeof(weekday_names) / sizeof(weekday_names[0]))) {
        return "Mon";
    }

    return weekday_names[weekday];
}

static void ui_main_clock_refresh(void)
{
    char date_buf[16];
    unsigned int year;
    unsigned int month;
    unsigned int day;
    struct tm timeinfo;
    bool time_changed = false;
    bool date_changed = false;

    if (!rtc_time_get_local(&timeinfo)) {
        g_main_clock_has_valid_cache = false;
        return;
    }

    time_changed = !g_main_clock_has_valid_cache ||
                   g_main_clock_hour != timeinfo.tm_hour ||
                   g_main_clock_min != timeinfo.tm_min;
    date_changed = !g_main_clock_has_valid_cache ||
                   g_main_clock_year != timeinfo.tm_year ||
                   g_main_clock_mon != timeinfo.tm_mon ||
                   g_main_clock_day != timeinfo.tm_mday ||
                   g_main_clock_wday != timeinfo.tm_wday;

    if (time_changed) {
        if (guider_ui.Main_img_hour0 != NULL &&
            lv_obj_is_valid(guider_ui.Main_img_hour0)) {
            lv_image_set_src(
                guider_ui.Main_img_hour0,
                ui_main_clock_get_digit_image((uint8_t)(timeinfo.tm_hour / 10)));
        }
        if (guider_ui.Main_img_hour1 != NULL &&
            lv_obj_is_valid(guider_ui.Main_img_hour1)) {
            lv_image_set_src(
                guider_ui.Main_img_hour1,
                ui_main_clock_get_digit_image((uint8_t)(timeinfo.tm_hour % 10)));
        }
        if (guider_ui.Main_img_min0 != NULL &&
            lv_obj_is_valid(guider_ui.Main_img_min0)) {
            lv_image_set_src(
                guider_ui.Main_img_min0,
                ui_main_clock_get_digit_image((uint8_t)(timeinfo.tm_min / 10)));
        }
        if (guider_ui.Main_img_min1 != NULL &&
            lv_obj_is_valid(guider_ui.Main_img_min1)) {
            lv_image_set_src(
                guider_ui.Main_img_min1,
                ui_main_clock_get_digit_image((uint8_t)(timeinfo.tm_min % 10)));
        }
    }

    if (date_changed) {
        year = (unsigned int)(timeinfo.tm_year + 1900);
        month = (unsigned int)(timeinfo.tm_mon + 1);
        day = (unsigned int)timeinfo.tm_mday;

        snprintf(date_buf, sizeof(date_buf), "%04u/%02u/%02u", year, month, day);

        if (guider_ui.Main_label_date != NULL &&
            lv_obj_is_valid(guider_ui.Main_label_date)) {
            lv_label_set_text(guider_ui.Main_label_date, date_buf);
        }
        if (guider_ui.Main_label_week != NULL &&
            lv_obj_is_valid(guider_ui.Main_label_week)) {
            lv_label_set_text(guider_ui.Main_label_week,
                              ui_main_clock_get_weekday(timeinfo.tm_wday));
        }
    }

    g_main_clock_has_valid_cache = true;
    g_main_clock_hour = timeinfo.tm_hour;
    g_main_clock_min = timeinfo.tm_min;
    g_main_clock_year = timeinfo.tm_year;
    g_main_clock_mon = timeinfo.tm_mon;
    g_main_clock_day = timeinfo.tm_mday;
    g_main_clock_wday = timeinfo.tm_wday;
}

static void ui_main_clock_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    ui_main_clock_refresh();
}

void ui_event_refresh_main_clock(void)
{
    ui_main_clock_refresh();
}

void ui_file_browser_open_from_menu(void)
{
    ui_file_browser_set_entry(UI_FILE_ENTRY_MENU);
    ui_load_scr_animation(&guider_ui, &guider_ui.file, guider_ui.file_del,
                          &guider_ui.meau_del, setup_scr_file, LV_SCR_LOAD_ANIM_NONE, 0, 0,
                          true, true);
    ui_event_set_screen_state(UI_SCREEN_FILE);
    ui_file_browser_apply_entry_ui();
    ui_file_browser_request_refresh(ui_file_browser_get_resume_dir(UI_FILE_ENTRY_MENU),
                                    "");
}

void ui_file_browser_open_from_music(void)
{
    ui_file_browser_set_entry(UI_FILE_ENTRY_MUSIC);
    music_player_on_screen_exit();
    ui_load_scr_animation(&guider_ui, &guider_ui.file, guider_ui.file_del,
                          &guider_ui.music_del, setup_scr_file, LV_SCR_LOAD_ANIM_NONE, 0, 0,
                          true, true);
    ui_event_set_screen_state(UI_SCREEN_FILE);
    ui_file_browser_apply_entry_ui();
    ui_file_browser_request_refresh(ui_file_browser_get_resume_dir(UI_FILE_ENTRY_MUSIC),
                                    "");
}

void ui_file_browser_handle_back(void)
{
    ui_file_list_reset_page_state();
    ui_file_list_invalidate_pager_controls();

    if (g_file_browser_ctx.entry == UI_FILE_ENTRY_MUSIC) {
        ui_load_scr_animation(&guider_ui, &guider_ui.music, guider_ui.music_del,
                              &guider_ui.file_del, setup_scr_music, LV_SCR_LOAD_ANIM_NONE,
                              0, 0, true, true);
        ui_event_set_screen_state(UI_SCREEN_MUSIC);
        music_player_on_screen_enter();
    } else {
        ui_load_scr_animation(&guider_ui, &guider_ui.meau, guider_ui.meau_del,
                              &guider_ui.file_del, setup_scr_meau, LV_SCR_LOAD_ANIM_NONE,
                              0, 0, true, true);
        ui_event_set_screen_state(UI_SCREEN_MENU);
    }
}

void ui_file_browser_open_parent_dir(void)
{
    char parent_path[SD_PATH_MAX] = {0};
    size_t len;
    int32_t i;

    snprintf(parent_path, sizeof(parent_path), "%s", sd_current_dir);
    len = strlen(parent_path);
    if (len <= strlen("/sdcard/")) {
        return;
    }

    i = (int32_t)len - 2;
    while (i >= 0 && parent_path[i] != '/') {
        i--;
    }
    if (i < 0) {
        return;
    }

    parent_path[i + 1] = '\0';
    ui_file_browser_request_refresh(parent_path, "");
}

void ui_file_browser_refresh_current_dir(void)
{
    ui_file_browser_apply_entry_ui();
    ui_file_browser_request_refresh(sd_current_dir, "");
}

static void ui_file_list_release_data(file_list_data_t *data)
{
    if (data == NULL) {
        return;
    }

    free(data->items);
    free(data);
}

static void ui_message_release(ui_message_t *msg)
{
    if (msg == NULL || msg->data == NULL) {
        return;
    }

    switch (msg->type) {
    case SD_FRESH_RESQUEST:
        ui_file_list_release_data((file_list_data_t *)msg->data);
        break;
    case MUSIC_LIST_RESQUEST:
    case UI_MSG_WIFI_DISCONNECTED:
    case UI_MSG_WIFI_GOT_IP:
        free(msg->data);
        break;
    case UI_MSG_WIFI_SCAN_DONE: {
        wifi_scan_done_data_t *ap_list = (wifi_scan_done_data_t *)msg->data;
        free(ap_list->ap_records);
        free(ap_list);
        break;
    }
    default:
        break;
    }

    msg->data = NULL;
}

static void ui_file_list_invalidate_pager_controls(void)
{
    g_file_page_prev_btn = NULL;
    g_file_page_next_btn = NULL;
    g_file_page_status_label = NULL;
}

static void ui_file_list_reset_page_state(void)
{
    if (g_file_list_render_pending) {
        lv_async_call_cancel(ui_file_list_render_async_cb, NULL);
        g_file_list_render_pending = false;
    }

    if (g_file_list_page_data != NULL) {
        ui_file_list_release_data(g_file_list_page_data);
        g_file_list_page_data = NULL;
    }

    g_file_list_page_index = 0;
}

static void ui_file_list_clear_dynamic_items(void)
{
    if (guider_ui.file_list_file == NULL || !lv_obj_is_valid(guider_ui.file_list_file)) 
    {
        return;
    }

    for (int32_t i = (int32_t)lv_obj_get_child_count(guider_ui.file_list_file) - 1;
         i >= 2; i--) {
        lv_obj_t *child = lv_obj_get_child(guider_ui.file_list_file, i);
        if (child != NULL) 
        {
            lv_obj_delete(child);
        }
    }
}

static void ui_file_list_page_prev_event_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    if (g_file_list_page_data == NULL || g_file_list_page_index == 0) {
        return;
    }

    g_file_list_page_index--;
    ui_file_list_request_render_async();
}

static void ui_file_list_page_next_event_cb(lv_event_t *e)
{
    uint16_t limit;
    uint16_t page_count;

    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    if (g_file_list_page_data == NULL) {
        return;
    }

    limit = g_file_list_page_data->item_count;
    if (limit > SD_REFRESH_FILE_COUNT_MAX) {
        limit = SD_REFRESH_FILE_COUNT_MAX;
    }

    page_count = (uint16_t)((limit + FILE_LIST_PAGE_SIZE - 1U) / FILE_LIST_PAGE_SIZE);
    if ((g_file_list_page_index + 1U) >= page_count) {
        return;
    }

    g_file_list_page_index++;
    ui_file_list_request_render_async();
}

static void ui_file_list_render_async_cb(void *user_data)
{
    (void)user_data;

    g_file_list_render_pending = false;

    if (guider_ui.file == NULL || !lv_obj_is_valid(guider_ui.file) ||
        lv_screen_active() != guider_ui.file) {
        return;
    }

    ui_file_list_render_current_page();
}

static void ui_file_list_request_render_async(void)
{
    if (g_file_list_render_pending) {
        return;
    }

    if (lv_async_call(ui_file_list_render_async_cb, NULL) == LV_RESULT_OK) {
        g_file_list_render_pending = true;
    } else {
        ESP_LOGW(TAG, "Failed to schedule file list page render");
    }
}

static void ui_file_list_ensure_pager_controls(void)
{
    if (guider_ui.file == NULL || !lv_obj_is_valid(guider_ui.file)) {
        return;
    }

    if (g_file_page_prev_btn != NULL && !lv_obj_is_valid(g_file_page_prev_btn)) {
        ui_file_list_invalidate_pager_controls();
    }
    if (g_file_page_next_btn != NULL && !lv_obj_is_valid(g_file_page_next_btn)) {
        ui_file_list_invalidate_pager_controls();
    }
    if (g_file_page_status_label != NULL && !lv_obj_is_valid(g_file_page_status_label)) {
        ui_file_list_invalidate_pager_controls();
    }

    if (guider_ui.file_list_file != NULL && lv_obj_is_valid(guider_ui.file_list_file)) {
        lv_obj_set_size(guider_ui.file_list_file, 320, 190);
    }

    if (g_file_page_prev_btn == NULL) {
        g_file_page_prev_btn = lv_button_create(guider_ui.file);
        lv_obj_set_pos(g_file_page_prev_btn, 8, 198);
        lv_obj_set_size(g_file_page_prev_btn, 96, 34);
        lv_obj_add_event_cb(g_file_page_prev_btn, ui_file_list_page_prev_event_cb,
                            LV_EVENT_CLICKED, NULL);
        lv_obj_t *label = lv_label_create(g_file_page_prev_btn);
        lv_label_set_text(label, "Prev");
        lv_obj_set_style_text_font(label, &lv_font_SourceHanSerifSC_Regular_23,
                                   LV_PART_MAIN);
        lv_obj_center(label);
    }

    if (g_file_page_next_btn == NULL) {
        g_file_page_next_btn = lv_button_create(guider_ui.file);
        lv_obj_set_pos(g_file_page_next_btn, 216, 198);
        lv_obj_set_size(g_file_page_next_btn, 96, 34);
        lv_obj_add_event_cb(g_file_page_next_btn, ui_file_list_page_next_event_cb,
                            LV_EVENT_CLICKED, NULL);
        lv_obj_t *label = lv_label_create(g_file_page_next_btn);
        lv_label_set_text(label, "Next");
        lv_obj_set_style_text_font(label, &lv_font_SourceHanSerifSC_Regular_23,
                                   LV_PART_MAIN);
        lv_obj_center(label);
    }

    if (g_file_page_status_label == NULL) {
        g_file_page_status_label = lv_label_create(guider_ui.file);
        lv_obj_set_pos(g_file_page_status_label, 112, 205);
        lv_obj_set_size(g_file_page_status_label, 96, 24);
        lv_obj_set_style_text_align(g_file_page_status_label, LV_TEXT_ALIGN_CENTER,
                                    LV_PART_MAIN);
    }
}

static void ui_file_list_update_pager_controls(void)
{
    uint16_t limit = 0;
    uint16_t page_count = 1;
    char page_buf[24] = {0};

    ui_file_list_ensure_pager_controls();

    if (g_file_page_prev_btn == NULL || !lv_obj_is_valid(g_file_page_prev_btn) ||
        g_file_page_next_btn == NULL || !lv_obj_is_valid(g_file_page_next_btn) ||
        g_file_page_status_label == NULL || !lv_obj_is_valid(g_file_page_status_label)) {
        ui_file_list_invalidate_pager_controls();
        return;
    }

    if (g_file_list_page_data != NULL) {
        limit = g_file_list_page_data->item_count;
        if (limit > SD_REFRESH_FILE_COUNT_MAX) {
            limit = SD_REFRESH_FILE_COUNT_MAX;
        }
        if (limit > 0) {
            page_count = (uint16_t)((limit + FILE_LIST_PAGE_SIZE - 1U) /
                                    FILE_LIST_PAGE_SIZE);
        }
    }

    if (g_file_list_page_index == 0) {
        lv_obj_add_state(g_file_page_prev_btn, LV_STATE_DISABLED);
    } else {
        lv_obj_remove_state(g_file_page_prev_btn, LV_STATE_DISABLED);
    }

    if ((g_file_list_page_index + 1U) >= page_count) {
        lv_obj_add_state(g_file_page_next_btn, LV_STATE_DISABLED);
    } else {
        lv_obj_remove_state(g_file_page_next_btn, LV_STATE_DISABLED);
    }

    snprintf(page_buf, sizeof(page_buf), "%u/%u",
             (unsigned)(page_count == 0 ? 0 : (g_file_list_page_index + 1U)),
             (unsigned)page_count);
    lv_label_set_text(g_file_page_status_label, page_buf);
}

static void ui_file_list_render_current_page(void)
{
    uint16_t limit;
    uint16_t start;
    uint16_t end;

    ui_file_list_clear_dynamic_items();
    ui_file_list_ensure_pager_controls();

    if (g_file_list_page_data == NULL) {
        ui_file_list_update_pager_controls();
        return;
    }

    limit = g_file_list_page_data->item_count;
    if (limit > SD_REFRESH_FILE_COUNT_MAX) {
        limit = SD_REFRESH_FILE_COUNT_MAX;
    }

    start = (uint16_t)(g_file_list_page_index * FILE_LIST_PAGE_SIZE);
    if (start >= limit) {
        g_file_list_page_index = 0;
        start = 0;
    }

    end = start + FILE_LIST_PAGE_SIZE;
    if (end > limit) {
        end = limit;
    }

    for (uint16_t i = start; i < end; i++) 
    {
        file_item_t *item = &g_file_list_page_data->items[i];
        const char *icon = LV_SYMBOL_FILE;

        if (item->type == FILE_TYPE_DIR) 
        {
            icon = LV_SYMBOL_DIRECTORY;
        } 
        else if (ui_file_type_is_audio(item->type)) 
        {
            icon = LV_SYMBOL_AUDIO;
        }
        else if (ui_file_type_is_image(item->type))
        {
            icon = LV_SYMBOL_IMAGE;
        }

        lv_obj_t *btn = lv_list_add_button(guider_ui.file_list_file, icon, item->name);
        lv_obj_set_style_text_font(btn, &lv_font_SourceHanSerifSC_Regular_23,
                                   LV_PART_MAIN);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x242424), LV_PART_MAIN);
        lv_obj_set_style_text_color(btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_add_event_cb(btn, file_list_click_cb, LV_EVENT_CLICKED,
                            (void *)(uintptr_t)item->type);
    }

    ui_file_list_update_pager_controls();
}

static void wifi_scan_done_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data)
{
    wifi_scan_done_data_t *cp_data = malloc(sizeof(wifi_scan_done_data_t));
    ui_message_t msg;

    if (cp_data == NULL) {
        return;
    }

    cp_data->ap_count = ((wifi_scan_done_data_t *)event_data)->ap_count;
    cp_data->ap_records = ((wifi_scan_done_data_t *)event_data)->ap_records;
    msg.type = UI_MSG_WIFI_SCAN_DONE;
    msg.data = (void *)cp_data;
    xQueueSend(ui_message_queue, &msg, portMAX_DELAY);
}

static void wifi_disconnected_handler(void *arg, esp_event_base_t event_base,
                                      int32_t event_id, void *event_data)
{
    wifi_disconnect_event_data_t *cp_data = malloc(sizeof(wifi_disconnect_event_data_t));
    ui_message_t msg;

    if (cp_data == NULL) {
        return;
    }

    memcpy(cp_data, event_data, sizeof(wifi_disconnect_event_data_t));
    msg.type = UI_MSG_WIFI_DISCONNECTED;
    msg.data = cp_data;
    xQueueSend(ui_message_queue, &msg, portMAX_DELAY);
}

static void wifi_got_ip_handler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data)
{
    char *cp_data;
    ui_message_t msg;
    size_t ip_len = strlen((const char *)event_data) + 1;

    cp_data = malloc(ip_len);
    if (cp_data == NULL) {
        return;
    }

    memcpy(cp_data, event_data, ip_len);
    msg.type = UI_MSG_WIFI_GOT_IP;
    msg.data = cp_data;
    xQueueSend(ui_message_queue, &msg, portMAX_DELAY);
}

static void wifi_closed_handler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data)
{
    ui_message_t msg = {
        .type = UI_MSG_WIFI_CLOSED,
        .data = NULL,
    };

    xQueueSend(ui_message_queue, &msg, portMAX_DELAY);
}

static void main_status_update_handler(void *arg, esp_event_base_t event_base,
                                        int32_t event_id, void *event_data)
{
    ui_message_t msg = {
        .type = UI_MSG_MAIN_STATUS_UPDATE,
        .data = NULL,
    };
    xQueueSend(ui_message_queue, &msg, portMAX_DELAY);
}



void my_ui_task(void *par)
{
    (void)par;
    ui_message_queue = xQueueCreate(10, sizeof(ui_message_t));
    if (ui_message_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create UI message queue");
        vTaskDelete(NULL);
    }

    ESP_ERROR_CHECK(esp_event_handler_register_with(
        ui_event_loop_handle, APP_EVENT, APP_FILE_REFRESH_RESPONSE,
        sd_refresh_response_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register_with(
        ui_event_loop_handle, APP_EVENT, APP_MUSIC_PLAY_RESPONSE,
        user_music_play_response_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register_with(
        ui_event_loop_handle, APP_EVENT, APP_WIFI_SCAN_DONE,
        wifi_scan_done_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register_with(
        ui_event_loop_handle, APP_EVENT, APP_WIFI_DISCONNECTED,
        wifi_disconnected_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register_with(
        ui_event_loop_handle, APP_EVENT, APP_GET_IP, wifi_got_ip_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register_with(
        ui_event_loop_handle, APP_EVENT, APP_WIFI_CLOSED_DONE,
        wifi_closed_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register_with(
        ui_event_loop_handle, APP_EVENT, APP_MAIN_STATUS_UPDATE,
        main_status_update_handler, NULL));

    if (lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) {
        if (g_main_clock_timer == NULL) {
            g_main_clock_timer = lv_timer_create(ui_main_clock_timer_cb, 1000, NULL);
        }
        ui_main_clock_refresh();
        lvgl_port_unlock();
    }

    ui_message_t received_msg;
    while (1) {
        if (xQueueReceive(ui_message_queue, &received_msg, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        if (!lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) {
            ESP_LOGW(TAG, "Skip UI message %d: LVGL lock timeout", received_msg.type);
            ui_message_release(&received_msg);
            continue;
        }

        switch (received_msg.type) {
        case SD_FRESH_RESQUEST: 
        {
            file_list_data_t *res_data = (file_list_data_t *)(received_msg.data);
            ui_file_list_reset_page_state();
            strcpy(sd_current_dir, res_data->current_path);
            ui_file_browser_store_current_dir(res_data->current_path);
            ui_file_browser_apply_entry_ui();

            ui_file_list_clear_dynamic_items();

            if (res_data->item_count == 0) {
                lv_list_add_button(guider_ui.file_list_file, LV_SYMBOL_CLOSE,
                                   "empty directory");
                ui_file_list_ensure_pager_controls();
                ui_file_list_update_pager_controls();
                ui_file_list_release_data(res_data);
            } 
            else 
            {
                if (res_data->item_count > SD_REFRESH_FILE_COUNT_MAX) 
                {
                    res_data->item_count = SD_REFRESH_FILE_COUNT_MAX;
                }

                g_file_list_page_data = res_data;
                g_file_list_page_index = 0;
                ui_file_list_render_current_page();
            }
            break;
        }
        case MUSIC_LIST_RESQUEST:
            ESP_LOGI(TAG, "Received music file: %s", (char *)received_msg.data);
            if (ui_event_get_screen_state() != UI_SCREEN_MUSIC) {
                ui_file_list_reset_page_state();
                ui_file_list_invalidate_pager_controls();
                ui_load_scr_animation(&guider_ui, &guider_ui.music, guider_ui.music_del,
                                      &guider_ui.file_del, setup_scr_music,
                                      LV_SCR_LOAD_ANIM_NONE, 0, 0, false, true);
                ui_event_set_screen_state(UI_SCREEN_MUSIC);
                music_player_on_screen_enter();
            }
            free(received_msg.data);
            break;
        case UI_MSG_WIFI_SCAN_DONE: {
            wifi_scan_done_data_t *ap_list = (wifi_scan_done_data_t *)received_msg.data;
            wifi_ui_logic_on_scan_done(ap_list);
            if (ap_list != NULL) {
                free(ap_list->ap_records);
            }
            free(received_msg.data);
            break;
        }
        case UI_MSG_WIFI_DISCONNECTED:
            wifi_ui_logic_on_disconnected(
                (wifi_disconnect_event_data_t *)received_msg.data);
            free(received_msg.data);
            break;
        case UI_MSG_WIFI_GOT_IP:
            wifi_ui_logic_on_got_ip((const char *)received_msg.data);
            free(received_msg.data);
            break;
        case UI_MSG_WIFI_CLOSED:
            ui_runtime_set_active(UI_RUNTIME_TASK_WIFI, false);
            wifi_ui_logic_on_closed();
            break;
        case UI_MSG_MAIN_STATUS_UPDATE:
            ui_main_status_update_locked();
            break;
        default:
            ESP_LOGW(TAG, "Received unknown event type: %d", received_msg.type);
            break;
        }

        lvgl_port_unlock();
    }
}

static void sd_refresh_response_handler(void *arg, esp_event_base_t event_base,
                                        int32_t event_id, void *event_data)
{
    file_list_data_t *cp_data = heap_caps_malloc(
        sizeof(file_list_data_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (cp_data == NULL) {
        return;
    }

    memcpy(cp_data, event_data, sizeof(file_list_data_t));

    ui_message_t msg = {
        .type = SD_FRESH_RESQUEST,
        .data = (void *)cp_data,
    };
    xQueueSend(ui_message_queue, &msg, portMAX_DELAY);
}

void file_list_click_cb(lv_event_t *e)
{
    lv_obj_t *button = lv_event_get_target(e);
    const char *name = lv_list_get_button_text(guider_ui.file_list_file, button);
    file_type_t file_type = (file_type_t)(uintptr_t)lv_event_get_user_data(e);
    char selected_name[SD_PATH_MAX / 2] = {0};
    char full_path[SD_PATH_MAX] = {0};

    if (name == NULL) {
        return;
    }

    snprintf(selected_name, sizeof(selected_name), "%s", name);

    if (file_type == FILE_TYPE_DIR) {
        ui_file_browser_request_refresh(sd_current_dir, selected_name);
    } else if (ui_file_type_is_audio(file_type)) {
        file_refresh_req_data_t data = {0};

        if (g_file_browser_ctx.entry == UI_FILE_ENTRY_MENU) {
            ESP_LOGI(TAG, "Ignore audio file click from menu file browser: %s",
                     selected_name);
            return;
        }

        ui_file_list_reset_page_state();
        ui_file_list_invalidate_pager_controls();
        ui_load_scr_animation(&guider_ui, &guider_ui.music, guider_ui.music_del,
                              &guider_ui.file_del, setup_scr_music,
                              LV_SCR_LOAD_ANIM_NONE, 0, 0, false, true);
        ui_event_set_screen_state(UI_SCREEN_MUSIC);
        music_player_on_screen_enter();

        snprintf(data.name, sizeof(data.name), "%s", selected_name);
        ui_join_sd_path(data.current_path, sizeof(data.current_path), sd_current_dir,
                        selected_name);
        esp_event_post_to(ui_event_loop_handle, APP_EVENT, APP_MUSIC_PLAY_REQUEST,
                          &data, sizeof(data), portMAX_DELAY);
    } else if (ui_file_type_is_image(file_type)) {
        ui_join_sd_path(full_path, sizeof(full_path), sd_current_dir, selected_name);
        ui_photo_view_prepare(selected_name, full_path);
        ui_load_scr_animation(&guider_ui, &guider_ui.photo, guider_ui.photo_del,
                              &guider_ui.file_del, setup_scr_photo,
                              LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        ui_photo_view_apply();
        ui_event_set_screen_state(UI_SCREEN_PHOTO);
    }
}

static void user_music_play_response_handler(void *arg, esp_event_base_t event_base,
                                             int32_t event_id, void *event_data)
{
    char *file_name = (char *)event_data;
    size_t len = strlen(file_name) + 1;
    char *copy = heap_caps_malloc(len, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (copy == NULL) {
        return;
    }

    strcpy(copy, file_name);
    ui_message_t msg = {
        .type = MUSIC_LIST_RESQUEST,
        .data = copy,
    };
    xQueueSend(ui_message_queue, &msg, portMAX_DELAY);
}

static void ui_main_status_update(void)
{
    if (!lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) 
    {
        return;
    }
    ui_main_status_update_locked();
    lvgl_port_unlock();
}

static void ui_main_status_update_locked(void)
{
    if(lv_scr_act() != guider_ui.Main) {
        return;
    }

    if (ui_main_status.wifi) {
        lv_obj_clear_flag(guider_ui.Main_label_wifi, LV_OBJ_FLAG_HIDDEN);
    } 
    else 
    {
        lv_obj_add_flag(guider_ui.Main_label_wifi, LV_OBJ_FLAG_HIDDEN);
    }
    if (ui_main_status.music) 
    {
        lv_obj_clear_flag(guider_ui.Main_label_music, LV_OBJ_FLAG_HIDDEN);
    } 
    else 
    {
        lv_obj_add_flag(guider_ui.Main_label_music, LV_OBJ_FLAG_HIDDEN);
    }
}
