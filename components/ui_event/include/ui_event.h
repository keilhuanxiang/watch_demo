#pragma once

#include "sdcard.h"
#include "event_loop.h"
#include "wifi_connect.h"

#define  SD_REFRESH_FILE_COUNT_MAX     256
#define  UI_LVGL_LOCK_TIMEOUT_MS       100U

typedef struct {
    bool wifi;
    bool music;
    bool camera;
} ui_main_status_t;

typedef enum {
    UI_SCREEN_MAIN = 0,
    UI_SCREEN_MENU,
    UI_SCREEN_MUSIC,
    UI_SCREEN_FILE,
    UI_SCREEN_WIFI,
    UI_SCREEN_CAMERA,
    UI_SCREEN_PHOTO,
    UI_SCREEN_SETTING,
} ui_screen_state_t;

typedef enum {
    UI_FILE_ENTRY_MENU = 0,
    UI_FILE_ENTRY_MUSIC,
} ui_file_browser_entry_t;

typedef enum {
    UI_RUNTIME_TASK_MUSIC = 0,
    UI_RUNTIME_TASK_WIFI,
    UI_RUNTIME_TASK_CAMERA,
} ui_runtime_task_t;

typedef enum {
    UI_NAV_TARGET_MAIN = 0,
    UI_NAV_TARGET_MENU,
    UI_NAV_TARGET_MUSIC,
    UI_NAV_TARGET_MUSIC_FROM_FILE,
    UI_NAV_TARGET_WIFI,
    UI_NAV_TARGET_CAMERA,
    UI_NAV_TARGET_SETTING,
    UI_NAV_TARGET_FILE_FROM_MENU,
    UI_NAV_TARGET_FILE_FROM_MUSIC,
    UI_NAV_TARGET_FILE_BACK,
    UI_NAV_TARGET_FILE_FROM_PHOTO,
    UI_NAV_TARGET_PHOTO,
    UI_NAV_TARGET_MENU_CLOSE_MUSIC,
    UI_NAV_TARGET_MENU_LEAVE_MUSIC,
    UI_NAV_TARGET_MENU_CLOSE_CAMERA,
} ui_nav_target_t;

extern ui_main_status_t ui_main_status;
extern volatile ui_screen_state_t g_ui_screen_state;

void my_ui_task(void *par);
void ui_event_refresh_main_clock(void);
void ui_event_set_screen_state(ui_screen_state_t state);
ui_screen_state_t ui_event_get_screen_state(void);
bool ui_runtime_can_start(ui_runtime_task_t task);
void ui_runtime_set_active(ui_runtime_task_t task, bool active);
bool ui_runtime_is_active(ui_runtime_task_t task);
uint8_t ui_runtime_task_count(void);
void ui_nav_request(ui_nav_target_t target);
void ui_file_browser_open_from_menu(void);
void ui_file_browser_open_from_music(void);
void ui_file_browser_handle_back(void);
void ui_file_browser_open_parent_dir(void);
void ui_file_browser_refresh_current_dir(void);
