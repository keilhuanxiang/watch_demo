/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "event_loop.h"
#include "sdcard.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "music_player.h"
#include "bsp_codec.h"
#include "wifi_connect.h"
#include "wifi_ui_logic.h"
#include "ui_event.h"
#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


static void Main_btn_meau_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.meau, guider_ui.meau_del, &guider_ui.Main_del, setup_scr_meau, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        ui_event_set_screen_state(UI_SCREEN_MENU);
        break;
    }
    default:
        break;
    }
}

void events_init_Main (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->Main_btn_meau, Main_btn_meau_event_handler, LV_EVENT_ALL, ui);
}

static void meau_btn_home_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.Main, guider_ui.Main_del, &guider_ui.meau_del, setup_scr_Main, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        ui_event_set_screen_state(UI_SCREEN_MAIN);
        esp_event_post_to(ui_event_loop_handle, APP_EVENT, APP_MAIN_STATUS_UPDATE, 0, 0, portMAX_DELAY);
        break;
    }
    default:
        break;
    }
}

static void meau_btn_music_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if (!ui_runtime_can_start(UI_RUNTIME_TASK_MUSIC)) {
            break;
        }

        mp3_player_init();
        if (bsp_codec_init() != ESP_OK) {
            break;
        }

        ui_load_scr_animation(&guider_ui, &guider_ui.music, guider_ui.music_del, &guider_ui.meau_del, setup_scr_music, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        ui_event_set_screen_state(UI_SCREEN_MUSIC);
        int volume = get_sys_volume();
        if(volume == 0)
            lv_label_set_text(guider_ui.music_btn_volume_label, LV_SYMBOL_MUTE); 
        else if(volume <= 60 && volume > 0)
            lv_label_set_text(guider_ui.music_btn_volume_label, LV_SYMBOL_VOLUME_MID); 
        else
            lv_label_set_text(guider_ui.music_btn_volume_label, LV_SYMBOL_VOLUME_MAX); 
        bsp_codec_volume_set(volume, NULL);
        lv_slider_set_value(guider_ui.music_slider_volume, volume, LV_ANIM_OFF);  
        music_player_on_screen_enter();
        ui_runtime_set_active(UI_RUNTIME_TASK_MUSIC, true);
        break;
    }
    default:
        break;
    }
}

static void meau_btn_wifi_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.wifi, guider_ui.wifi_del, &guider_ui.meau_del, setup_scr_wifi, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        ui_event_set_screen_state(UI_SCREEN_WIFI);
        wifi_ui_logic_on_screen_enter();
        break;
    }
    default:
        break;
    }
}

static void meau_btn_camera_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if (!ui_runtime_can_start(UI_RUNTIME_TASK_CAMERA)) {
            break;
        }

        ui_runtime_set_active(UI_RUNTIME_TASK_CAMERA, true);
        ui_load_scr_animation(&guider_ui, &guider_ui.camera, guider_ui.camera_del, &guider_ui.meau_del, setup_scr_camera, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        ui_event_set_screen_state(UI_SCREEN_CAMERA);
        esp_event_post_to(ui_event_loop_handle, APP_EVENT, APP_CAMERA_ENTER, NULL, 0, portMAX_DELAY);
        break;
    }
    default:
        break;
    }
}

static void meau_btn_document_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_file_browser_open_from_menu();
        break;
    }
    default:
        break;
    }
}

static void meau_btn_setting_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.setting, guider_ui.setting_del, &guider_ui.meau_del, setup_scr_setting, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_meau (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->meau_btn_home, meau_btn_home_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->meau_btn_music, meau_btn_music_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->meau_btn_wifi, meau_btn_wifi_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->meau_btn_camera, meau_btn_camera_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->meau_btn_document, meau_btn_document_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->meau_btn_setting, meau_btn_setting_event_handler, LV_EVENT_ALL, ui);
}

static void music_btn_close_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        music_player_close();
        bsp_codec_deinit();
        ui_runtime_set_active(UI_RUNTIME_TASK_MUSIC, false);
        ui_load_scr_animation(&guider_ui, &guider_ui.meau, guider_ui.meau_del, &guider_ui.music_del, setup_scr_meau, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        ui_event_set_screen_state(UI_SCREEN_MENU);
        break;
    }
    default:
        break;
    }
}

static void music_btn_volume_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(lv_obj_has_flag(guider_ui.music_slider_volume, LV_OBJ_FLAG_HIDDEN))
        {
            // 当前已隐藏 -> 移除隐藏标志，显示控件
            lv_obj_clear_flag(guider_ui.music_slider_volume, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            // 当前未隐藏 -> 添加隐藏标志，隐藏控件
            lv_obj_add_flag(guider_ui.music_slider_volume, LV_OBJ_FLAG_HIDDEN);
        }
        break;
    }
    default:
        break;
    }
}

static void music_btn_list_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_file_browser_open_from_music();
        break;
    }
    default:
        break;
    }
}

static void music_slider_volume_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        int volume = lv_slider_get_value(guider_ui.music_slider_volume);
        if(volume == 0)
            lv_label_set_text(guider_ui.music_btn_volume_label, LV_SYMBOL_MUTE); 
        else if(volume <= 60 && volume > 0)
            lv_label_set_text(guider_ui.music_btn_volume_label, LV_SYMBOL_VOLUME_MID); 
        else
            lv_label_set_text(guider_ui.music_btn_volume_label, LV_SYMBOL_VOLUME_MAX); 
        bsp_codec_volume_set(volume, NULL); 
        change_sys_volume(volume);
        break;
    }
    default:
        break;
    }
}

static void music_btn_play_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        esp_event_post_to(ui_event_loop_handle, APP_EVENT, APP_MUSIC_PLAY_button_REQUEST, NULL, 0, portMAX_DELAY);
        break;
    }
    default:
        break;
    }
}

static void music_btn_prev_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        char prev_next_flag = 0;
        esp_event_post_to(ui_event_loop_handle, APP_EVENT, APP_MUSIC_PREV_NEXT_REQUEST, &prev_next_flag, sizeof(prev_next_flag), portMAX_DELAY);
        break;
    }
    default:
        break;
    }
}

static void music_btn_next_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        char prev_next_flag = 1;
        esp_event_post_to(ui_event_loop_handle, APP_EVENT, APP_MUSIC_PREV_NEXT_REQUEST, &prev_next_flag, sizeof(prev_next_flag), portMAX_DELAY);
        break;
    }
    default:
        break;
    }
}

static void music_btn_mode_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        music_player_cycle_mode();
        break;
    }
    default:
        break;
    }
}

static void music_btn_leave_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        music_player_on_screen_exit();
        ui_load_scr_animation(&guider_ui, &guider_ui.meau, guider_ui.meau_del, &guider_ui.music_del, setup_scr_meau, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        ui_event_set_screen_state(UI_SCREEN_MENU);
        break;
    }
    default:
        break;
    }
}

void events_init_music (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->music_slider_volume, music_slider_volume_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->music_btn_close, music_btn_close_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->music_btn_volume, music_btn_volume_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->music_btn_play, music_btn_play_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->music_btn_prev, music_btn_prev_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->music_btn_next, music_btn_next_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->music_btn_mode, music_btn_mode_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->music_btn_list, music_btn_list_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->music_btn_leave, music_btn_leave_event_handler, LV_EVENT_ALL, ui);
}

static void file_list_file_item0_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_file_browser_handle_back();
        break;
    }
    default:
        break;
    }
}

static void file_list_file_item1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_file_browser_open_parent_dir();
        break;
    }
    default:
        break;
    }
}

void events_init_file (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->file_list_file_item0, file_list_file_item0_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->file_list_file_item1, file_list_file_item1_event_handler, LV_EVENT_ALL, ui);
}

static void wifi_btn_close_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.meau, guider_ui.meau_del, &guider_ui.wifi_del, setup_scr_meau, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        ui_event_set_screen_state(UI_SCREEN_MENU);
        break;
    }
    default:
        break;
    }
}

static void wifi_sw_wifi_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        lv_obj_t * status_obj = lv_event_get_target(e);
        int status = lv_obj_has_state(status_obj, LV_STATE_CHECKED) ? true : false;

        if (status && !ui_runtime_can_start(UI_RUNTIME_TASK_WIFI)) {
            lv_obj_remove_state(status_obj, LV_STATE_CHECKED);
            break;
        }

        wifi_ui_logic_on_switch_toggle(status);
        if (status) {
            ui_runtime_set_active(UI_RUNTIME_TASK_WIFI, true);
        }
        break;
    }
    default:
        break;
    }
}

static void wifi_btn_disconnect_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        wifi_ui_logic_on_disconnect_prompt();
        break;
    }
    default:
        break;
    }
}

static void wifi_btn_fresh_list_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        wifi_ui_logic_on_refresh_request();
        break;
    }
    default:
        break;
    }
}

static void wifi_btn_connect_confirm_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        wifi_ui_logic_on_connect_confirm();
        break;
    }
    default:
        break;
    }
}

static void wifi_btn_connect_cancel_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        wifi_ui_logic_on_connect_cancel();
        break;
    }
    default:
        break;
    }
}

static void wifi_btn_diconnect_confirm_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        wifi_ui_logic_on_disconnect_confirm();
        break;
    }
    default:
        break;
    }
}

static void wifi_btn_disconnect_cancel_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        wifi_ui_logic_on_disconnect_cancel();
        break;
    }
    default:
        break;
    }
}

void events_init_wifi (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->wifi_btn_close, wifi_btn_close_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->wifi_sw_wifi, wifi_sw_wifi_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->wifi_btn_disconnect, wifi_btn_disconnect_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->wifi_btn_fresh_list, wifi_btn_fresh_list_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->wifi_btn_connect_confirm, wifi_btn_connect_confirm_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->wifi_btn_connect_cancel, wifi_btn_connect_cancel_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->wifi_btn_diconnect_confirm, wifi_btn_diconnect_confirm_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->wifi_btn_disconnect_cancel, wifi_btn_disconnect_cancel_event_handler, LV_EVENT_ALL, ui);
}

static void camera_btn_close_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        esp_event_post_to(ui_event_loop_handle, APP_EVENT, APP_CAMERA_EXIT, NULL, 0, portMAX_DELAY);
        ui_load_scr_animation(&guider_ui, &guider_ui.meau, guider_ui.meau_del, &guider_ui.camera_del, setup_scr_meau, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        ui_event_set_screen_state(UI_SCREEN_MENU);
        break;
    }
    default:
        break;
    }
}

void events_init_camera (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->camera_btn_close, camera_btn_close_event_handler, LV_EVENT_ALL, ui);
}

static void photo_btn_close_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.file, guider_ui.file_del,
                              &guider_ui.photo_del, setup_scr_file,
                              LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        ui_event_set_screen_state(UI_SCREEN_FILE);
        ui_file_browser_refresh_current_dir();
        break;
    }
    default:
        break;
    }
}

void events_init_photo (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->photo_btn_close, photo_btn_close_event_handler,
                        LV_EVENT_ALL, ui);
}

static void setting_list_1_item0_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.meau, guider_ui.meau_del, &guider_ui.setting_del, setup_scr_meau, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        ui_event_set_screen_state(UI_SCREEN_MENU);
        break;
    }
    default:
        break;
    }
}

void events_init_setting (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->setting_list_1_item0, setting_list_1_item0_event_handler, LV_EVENT_ALL, ui);
}

void events_init(lv_ui *ui)
{

}
