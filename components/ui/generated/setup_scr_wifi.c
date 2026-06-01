/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_wifi(lv_ui *ui)
{
    //Write codes wifi
    ui->wifi = lv_obj_create(NULL);
    lv_obj_set_size(ui->wifi, 320, 240);
    lv_obj_set_scrollbar_mode(ui->wifi, LV_SCROLLBAR_MODE_OFF);

    //Write style for wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi, lv_color_hex(0x242424), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_btn_close
    ui->wifi_btn_close = lv_button_create(ui->wifi);
    lv_obj_set_pos(ui->wifi_btn_close, 0, 0);
    lv_obj_set_size(ui->wifi_btn_close, 50, 50);
    ui->wifi_btn_close_label = lv_label_create(ui->wifi_btn_close);
    lv_label_set_text(ui->wifi_btn_close_label, " " LV_SYMBOL_CLOSE " ");
    lv_label_set_long_mode(ui->wifi_btn_close_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->wifi_btn_close_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->wifi_btn_close, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->wifi_btn_close_label, LV_PCT(100));

    //Write style for wifi_btn_close, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->wifi_btn_close, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_btn_close, lv_color_hex(0x242424), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_btn_close, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->wifi_btn_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_btn_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_btn_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_btn_close, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_btn_close, &lv_font_montserratMedium_38, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_btn_close, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_btn_close, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_label_select_tip
    ui->wifi_label_select_tip = lv_label_create(ui->wifi);
    lv_obj_set_pos(ui->wifi_label_select_tip, 18, 95);
    lv_obj_set_size(ui->wifi_label_select_tip, 135, 20);
    lv_label_set_text(ui->wifi_label_select_tip, "选取附近的WLAN");
    lv_label_set_long_mode(ui->wifi_label_select_tip, LV_LABEL_LONG_WRAP);

    //Write style for wifi_label_select_tip, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_label_select_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_label_select_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_label_select_tip, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_label_select_tip, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_label_select_tip, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->wifi_label_select_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->wifi_label_select_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_label_select_tip, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_label_select_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_label_select_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_label_select_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_label_select_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_label_select_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_label_select_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_cont_switch
    ui->wifi_cont_switch = lv_obj_create(ui->wifi);
    lv_obj_set_pos(ui->wifi_cont_switch, 7, 45);
    lv_obj_set_size(ui->wifi_cont_switch, 309, 50);
    lv_obj_set_scrollbar_mode(ui->wifi_cont_switch, LV_SCROLLBAR_MODE_OFF);

    //Write style for wifi_cont_switch, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_cont_switch, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->wifi_cont_switch, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->wifi_cont_switch, lv_color_hex(0x242424), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->wifi_cont_switch, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_cont_switch, 17, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_cont_switch, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_cont_switch, lv_color_hex(0x424242), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_cont_switch, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_cont_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_cont_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_cont_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_cont_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_cont_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_label_switch_tip
    ui->wifi_label_switch_tip = lv_label_create(ui->wifi_cont_switch);
    lv_obj_set_pos(ui->wifi_label_switch_tip, -3, 14);
    lv_obj_set_size(ui->wifi_label_switch_tip, 100, 32);
    lv_label_set_text(ui->wifi_label_switch_tip, "WLAN");
    lv_label_set_long_mode(ui->wifi_label_switch_tip, LV_LABEL_LONG_WRAP);

    //Write style for wifi_label_switch_tip, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_label_switch_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_label_switch_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_label_switch_tip, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_label_switch_tip, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_label_switch_tip, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->wifi_label_switch_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->wifi_label_switch_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_label_switch_tip, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_label_switch_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_label_switch_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_label_switch_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_label_switch_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_label_switch_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_label_switch_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_sw_wifi
    ui->wifi_sw_wifi = lv_switch_create(ui->wifi_cont_switch);
    lv_obj_set_pos(ui->wifi_sw_wifi, 225, 7);
    lv_obj_set_size(ui->wifi_sw_wifi, 60, 30);

    //Write style for wifi_sw_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->wifi_sw_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_sw_wifi, lv_color_hex(0xe6e2e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_sw_wifi, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->wifi_sw_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_sw_wifi, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_sw_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for wifi_sw_wifi, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->wifi_sw_wifi, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->wifi_sw_wifi, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->wifi_sw_wifi, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->wifi_sw_wifi, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

    //Write style for wifi_sw_wifi, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->wifi_sw_wifi, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_sw_wifi, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_sw_wifi, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->wifi_sw_wifi, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_sw_wifi, 10, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes wifi_cont_connected
    ui->wifi_cont_connected = lv_obj_create(ui->wifi);
    lv_obj_set_pos(ui->wifi_cont_connected, 7, 95);
    lv_obj_set_size(ui->wifi_cont_connected, 309, 50);
    lv_obj_set_scrollbar_mode(ui->wifi_cont_connected, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->wifi_cont_connected, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->wifi_cont_connected, LV_OBJ_FLAG_HIDDEN);

    //Write style for wifi_cont_connected, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_cont_connected, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->wifi_cont_connected, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->wifi_cont_connected, lv_color_hex(0x242424), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->wifi_cont_connected, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_cont_connected, 17, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_cont_connected, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_cont_connected, lv_color_hex(0x1d7eee), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_cont_connected, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_cont_connected, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_cont_connected, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_cont_connected, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_cont_connected, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_cont_connected, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_label_connected_img
    ui->wifi_label_connected_img = lv_label_create(ui->wifi_cont_connected);
    lv_obj_set_pos(ui->wifi_label_connected_img, 15, 12);
    lv_obj_set_size(ui->wifi_label_connected_img, 35, 25);
    lv_label_set_text(ui->wifi_label_connected_img, "" LV_SYMBOL_WIFI " ");
    lv_label_set_long_mode(ui->wifi_label_connected_img, LV_LABEL_LONG_WRAP);

    //Write style for wifi_label_connected_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_label_connected_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_label_connected_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_label_connected_img, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_label_connected_img, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_label_connected_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->wifi_label_connected_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->wifi_label_connected_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_label_connected_img, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_label_connected_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_label_connected_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_label_connected_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_label_connected_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_label_connected_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_label_connected_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_label_connected_ssid
    ui->wifi_label_connected_ssid = lv_label_create(ui->wifi_cont_connected);
    lv_obj_set_pos(ui->wifi_label_connected_ssid, 53, 14);
    lv_obj_set_size(ui->wifi_label_connected_ssid, 150, 21);
    lv_label_set_text(ui->wifi_label_connected_ssid, "name");
    lv_label_set_long_mode(ui->wifi_label_connected_ssid, LV_LABEL_LONG_WRAP);

    //Write style for wifi_label_connected_ssid, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_label_connected_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_label_connected_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_label_connected_ssid, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_label_connected_ssid, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_label_connected_ssid, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->wifi_label_connected_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->wifi_label_connected_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_label_connected_ssid, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_label_connected_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_label_connected_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_label_connected_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_label_connected_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_label_connected_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_label_connected_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_btn_disconnect
    ui->wifi_btn_disconnect = lv_button_create(ui->wifi_cont_connected);
    lv_obj_set_pos(ui->wifi_btn_disconnect, 253, 8);
    lv_obj_set_size(ui->wifi_btn_disconnect, 30, 30);
    ui->wifi_btn_disconnect_label = lv_label_create(ui->wifi_btn_disconnect);
    lv_label_set_text(ui->wifi_btn_disconnect_label, " " LV_SYMBOL_RIGHT " ");
    lv_label_set_long_mode(ui->wifi_btn_disconnect_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->wifi_btn_disconnect_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->wifi_btn_disconnect, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->wifi_btn_disconnect_label, LV_PCT(100));

    //Write style for wifi_btn_disconnect, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->wifi_btn_disconnect, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_btn_disconnect, lv_color_hex(0x67b3f3), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_btn_disconnect, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->wifi_btn_disconnect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_btn_disconnect, 26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_btn_disconnect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_btn_disconnect, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_btn_disconnect, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_btn_disconnect, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_btn_disconnect, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_list_wifi
    ui->wifi_list_wifi = lv_list_create(ui->wifi);
    lv_obj_set_pos(ui->wifi_list_wifi, 7, 115);
    lv_obj_set_size(ui->wifi_list_wifi, 309, 125);
    lv_obj_set_scrollbar_mode(ui->wifi_list_wifi, LV_SCROLLBAR_MODE_OFF);

    //Write style state: LV_STATE_DEFAULT for &style_wifi_list_wifi_main_main_default
    static lv_style_t style_wifi_list_wifi_main_main_default;
    ui_init_style(&style_wifi_list_wifi_main_main_default);

    lv_style_set_pad_top(&style_wifi_list_wifi_main_main_default, 5);
    lv_style_set_pad_left(&style_wifi_list_wifi_main_main_default, 5);
    lv_style_set_pad_right(&style_wifi_list_wifi_main_main_default, 5);
    lv_style_set_pad_bottom(&style_wifi_list_wifi_main_main_default, 5);
    lv_style_set_bg_opa(&style_wifi_list_wifi_main_main_default, 255);
    lv_style_set_bg_color(&style_wifi_list_wifi_main_main_default, lv_color_hex(0x424242));
    lv_style_set_bg_grad_dir(&style_wifi_list_wifi_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_wifi_list_wifi_main_main_default, 1);
    lv_style_set_border_opa(&style_wifi_list_wifi_main_main_default, 255);
    lv_style_set_border_color(&style_wifi_list_wifi_main_main_default, lv_color_hex(0x424242));
    lv_style_set_border_side(&style_wifi_list_wifi_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_wifi_list_wifi_main_main_default, 17);
    lv_style_set_shadow_width(&style_wifi_list_wifi_main_main_default, 0);
    lv_obj_add_style(ui->wifi_list_wifi, &style_wifi_list_wifi_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_wifi_list_wifi_main_scrollbar_default
    static lv_style_t style_wifi_list_wifi_main_scrollbar_default;
    ui_init_style(&style_wifi_list_wifi_main_scrollbar_default);

    lv_style_set_radius(&style_wifi_list_wifi_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_wifi_list_wifi_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_wifi_list_wifi_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_wifi_list_wifi_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->wifi_list_wifi, &style_wifi_list_wifi_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_wifi_list_wifi_extra_btns_main_default
    static lv_style_t style_wifi_list_wifi_extra_btns_main_default;
    ui_init_style(&style_wifi_list_wifi_extra_btns_main_default);

    lv_style_set_pad_top(&style_wifi_list_wifi_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_wifi_list_wifi_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_wifi_list_wifi_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_wifi_list_wifi_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_wifi_list_wifi_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_wifi_list_wifi_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_text_font(&style_wifi_list_wifi_extra_btns_main_default, &lv_font_SourceHanSerifSC_Regular_14);
    lv_style_set_text_opa(&style_wifi_list_wifi_extra_btns_main_default, 255);
    lv_style_set_radius(&style_wifi_list_wifi_extra_btns_main_default, 10);
    lv_style_set_bg_opa(&style_wifi_list_wifi_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_wifi_list_wifi_extra_btns_main_default, lv_color_hex(0x424242));
    lv_style_set_bg_grad_dir(&style_wifi_list_wifi_extra_btns_main_default, LV_GRAD_DIR_NONE);

    //Write style state: LV_STATE_DEFAULT for &style_wifi_list_wifi_extra_texts_main_default
    static lv_style_t style_wifi_list_wifi_extra_texts_main_default;
    ui_init_style(&style_wifi_list_wifi_extra_texts_main_default);

    lv_style_set_pad_top(&style_wifi_list_wifi_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_wifi_list_wifi_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_wifi_list_wifi_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_wifi_list_wifi_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_wifi_list_wifi_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_wifi_list_wifi_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_wifi_list_wifi_extra_texts_main_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_wifi_list_wifi_extra_texts_main_default, 255);
    lv_style_set_radius(&style_wifi_list_wifi_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_wifi_list_wifi_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_wifi_list_wifi_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_wifi_list_wifi_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_wifi_list_wifi_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes wifi_btn_fresh_list
    ui->wifi_btn_fresh_list = lv_button_create(ui->wifi);
    lv_obj_set_pos(ui->wifi_btn_fresh_list, 275, 5);
    lv_obj_set_size(ui->wifi_btn_fresh_list, 40, 40);
    ui->wifi_btn_fresh_list_label = lv_label_create(ui->wifi_btn_fresh_list);
    lv_label_set_text(ui->wifi_btn_fresh_list_label, "" LV_SYMBOL_REFRESH " ");
    lv_label_set_long_mode(ui->wifi_btn_fresh_list_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->wifi_btn_fresh_list_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->wifi_btn_fresh_list, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->wifi_btn_fresh_list_label, LV_PCT(100));

    //Write style for wifi_btn_fresh_list, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->wifi_btn_fresh_list, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_btn_fresh_list, lv_color_hex(0x242424), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_btn_fresh_list, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->wifi_btn_fresh_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_btn_fresh_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_btn_fresh_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_btn_fresh_list, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_btn_fresh_list, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_btn_fresh_list, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_btn_fresh_list, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_cont_connect
    ui->wifi_cont_connect = lv_obj_create(ui->wifi);
    lv_obj_set_pos(ui->wifi_cont_connect, 7, 30);
    lv_obj_set_size(ui->wifi_cont_connect, 309, 150);
    lv_obj_set_scrollbar_mode(ui->wifi_cont_connect, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->wifi_cont_connect, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->wifi_cont_connect, LV_OBJ_FLAG_HIDDEN);

    //Write style for wifi_cont_connect, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_cont_connect, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->wifi_cont_connect, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->wifi_cont_connect, lv_color_hex(0x424242), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->wifi_cont_connect, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_cont_connect, 23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_cont_connect, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_cont_connect, lv_color_hex(0x424242), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_cont_connect, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_cont_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_cont_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_cont_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_cont_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_cont_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_label_connect_ssid
    ui->wifi_label_connect_ssid = lv_label_create(ui->wifi_cont_connect);
    lv_obj_set_pos(ui->wifi_label_connect_ssid, 25, 6);
    lv_obj_set_size(ui->wifi_label_connect_ssid, 250, 32);
    lv_label_set_text(ui->wifi_label_connect_ssid, "Label");
    lv_label_set_long_mode(ui->wifi_label_connect_ssid, LV_LABEL_LONG_WRAP);

    //Write style for wifi_label_connect_ssid, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_label_connect_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_label_connect_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_label_connect_ssid, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_label_connect_ssid, &lv_font_SourceHanSerifSC_Regular_23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_label_connect_ssid, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->wifi_label_connect_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->wifi_label_connect_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_label_connect_ssid, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_label_connect_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_label_connect_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_label_connect_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_label_connect_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_label_connect_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_label_connect_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_ta_connect_password
    ui->wifi_ta_connect_password = lv_textarea_create(ui->wifi_cont_connect);
    lv_obj_set_pos(ui->wifi_ta_connect_password, 6, 44);
    lv_obj_set_size(ui->wifi_ta_connect_password, 294, 1);
    lv_textarea_set_text(ui->wifi_ta_connect_password, "1");
    lv_textarea_set_placeholder_text(ui->wifi_ta_connect_password, "");
    lv_textarea_set_password_bullet(ui->wifi_ta_connect_password, "*");
    lv_textarea_set_password_mode(ui->wifi_ta_connect_password, false);
    lv_textarea_set_one_line(ui->wifi_ta_connect_password, true);
    lv_textarea_set_accepted_chars(ui->wifi_ta_connect_password, "");
    lv_textarea_set_max_length(ui->wifi_ta_connect_password, 32);
#if LV_USE_KEYBOARD
    lv_obj_add_event_cb(ui->wifi_ta_connect_password, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif

    //Write style for wifi_ta_connect_password, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->wifi_ta_connect_password, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_ta_connect_password, &lv_font_montserratMedium_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_ta_connect_password, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->wifi_ta_connect_password, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_ta_connect_password, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_ta_connect_password, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_ta_connect_password, lv_color_hex(0x8b8b8b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_ta_connect_password, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->wifi_ta_connect_password, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->wifi_ta_connect_password, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->wifi_ta_connect_password, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->wifi_ta_connect_password, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_ta_connect_password, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_ta_connect_password, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_ta_connect_password, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_ta_connect_password, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_ta_connect_password, 12, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for wifi_ta_connect_password, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->wifi_ta_connect_password, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_ta_connect_password, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_ta_connect_password, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_ta_connect_password, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes wifi_btn_connect_confirm
    ui->wifi_btn_connect_confirm = lv_button_create(ui->wifi_cont_connect);
    lv_obj_set_pos(ui->wifi_btn_connect_confirm, 16, 98);
    lv_obj_set_size(ui->wifi_btn_connect_confirm, 100, 35);
    ui->wifi_btn_connect_confirm_label = lv_label_create(ui->wifi_btn_connect_confirm);
    lv_label_set_text(ui->wifi_btn_connect_confirm_label, "连接");
    lv_label_set_long_mode(ui->wifi_btn_connect_confirm_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->wifi_btn_connect_confirm_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->wifi_btn_connect_confirm, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->wifi_btn_connect_confirm_label, LV_PCT(100));

    //Write style for wifi_btn_connect_confirm, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->wifi_btn_connect_confirm, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_btn_connect_confirm, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_btn_connect_confirm, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->wifi_btn_connect_confirm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_btn_connect_confirm, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_btn_connect_confirm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_btn_connect_confirm, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_btn_connect_confirm, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_btn_connect_confirm, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_btn_connect_confirm, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_label_connect_error
    ui->wifi_label_connect_error = lv_label_create(ui->wifi_cont_connect);
    lv_obj_set_pos(ui->wifi_label_connect_error, 84, 55);
    lv_obj_set_size(ui->wifi_label_connect_error, 150, 20);
    lv_obj_add_flag(ui->wifi_label_connect_error, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui->wifi_label_connect_error, "密码错误，请重试");
    lv_label_set_long_mode(ui->wifi_label_connect_error, LV_LABEL_LONG_WRAP);

    //Write style for wifi_label_connect_error, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_label_connect_error, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_label_connect_error, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_label_connect_error, lv_color_hex(0xf00000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_label_connect_error, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_label_connect_error, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->wifi_label_connect_error, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->wifi_label_connect_error, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_label_connect_error, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_label_connect_error, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_label_connect_error, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_label_connect_error, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_label_connect_error, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_label_connect_error, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_label_connect_error, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_btn_connect_cancel
    ui->wifi_btn_connect_cancel = lv_button_create(ui->wifi_cont_connect);
    lv_obj_set_pos(ui->wifi_btn_connect_cancel, 188, 98);
    lv_obj_set_size(ui->wifi_btn_connect_cancel, 100, 35);
    ui->wifi_btn_connect_cancel_label = lv_label_create(ui->wifi_btn_connect_cancel);
    lv_label_set_text(ui->wifi_btn_connect_cancel_label, "取消");
    lv_label_set_long_mode(ui->wifi_btn_connect_cancel_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->wifi_btn_connect_cancel_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->wifi_btn_connect_cancel, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->wifi_btn_connect_cancel_label, LV_PCT(100));

    //Write style for wifi_btn_connect_cancel, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->wifi_btn_connect_cancel, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_btn_connect_cancel, lv_color_hex(0x888888), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_btn_connect_cancel, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->wifi_btn_connect_cancel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_btn_connect_cancel, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_btn_connect_cancel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_btn_connect_cancel, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_btn_connect_cancel, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_btn_connect_cancel, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_btn_connect_cancel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_cont_disconnect
    ui->wifi_cont_disconnect = lv_obj_create(ui->wifi);
    lv_obj_set_pos(ui->wifi_cont_disconnect, 7, 44);
    lv_obj_set_size(ui->wifi_cont_disconnect, 309, 100);
    lv_obj_set_scrollbar_mode(ui->wifi_cont_disconnect, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->wifi_cont_disconnect, LV_OBJ_FLAG_HIDDEN);

    //Write style for wifi_cont_disconnect, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_cont_disconnect, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->wifi_cont_disconnect, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->wifi_cont_disconnect, lv_color_hex(0x424242), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->wifi_cont_disconnect, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_cont_disconnect, 23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_cont_disconnect, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_cont_disconnect, lv_color_hex(0x424242), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_cont_disconnect, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_cont_disconnect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_cont_disconnect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_cont_disconnect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_cont_disconnect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_cont_disconnect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_label_disconnect_tip
    ui->wifi_label_disconnect_tip = lv_label_create(ui->wifi_cont_disconnect);
    lv_obj_set_pos(ui->wifi_label_disconnect_tip, 28, 6);
    lv_obj_set_size(ui->wifi_label_disconnect_tip, 246, 32);
    lv_label_set_text(ui->wifi_label_disconnect_tip, "是否断开网络连接");
    lv_label_set_long_mode(ui->wifi_label_disconnect_tip, LV_LABEL_LONG_WRAP);

    //Write style for wifi_label_disconnect_tip, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_label_disconnect_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_label_disconnect_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_label_disconnect_tip, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_label_disconnect_tip, &lv_font_SourceHanSerifSC_Regular_23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_label_disconnect_tip, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->wifi_label_disconnect_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->wifi_label_disconnect_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_label_disconnect_tip, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_label_disconnect_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_label_disconnect_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_label_disconnect_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_label_disconnect_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_label_disconnect_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_label_disconnect_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_btn_diconnect_confirm
    ui->wifi_btn_diconnect_confirm = lv_button_create(ui->wifi_cont_disconnect);
    lv_obj_set_pos(ui->wifi_btn_diconnect_confirm, 12, 50);
    lv_obj_set_size(ui->wifi_btn_diconnect_confirm, 100, 35);
    ui->wifi_btn_diconnect_confirm_label = lv_label_create(ui->wifi_btn_diconnect_confirm);
    lv_label_set_text(ui->wifi_btn_diconnect_confirm_label, "确认");
    lv_label_set_long_mode(ui->wifi_btn_diconnect_confirm_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->wifi_btn_diconnect_confirm_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->wifi_btn_diconnect_confirm, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->wifi_btn_diconnect_confirm_label, LV_PCT(100));

    //Write style for wifi_btn_diconnect_confirm, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->wifi_btn_diconnect_confirm, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_btn_diconnect_confirm, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_btn_diconnect_confirm, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->wifi_btn_diconnect_confirm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_btn_diconnect_confirm, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_btn_diconnect_confirm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_btn_diconnect_confirm, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_btn_diconnect_confirm, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_btn_diconnect_confirm, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_btn_diconnect_confirm, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_btn_disconnect_cancel
    ui->wifi_btn_disconnect_cancel = lv_button_create(ui->wifi_cont_disconnect);
    lv_obj_set_pos(ui->wifi_btn_disconnect_cancel, 189, 50);
    lv_obj_set_size(ui->wifi_btn_disconnect_cancel, 100, 35);
    ui->wifi_btn_disconnect_cancel_label = lv_label_create(ui->wifi_btn_disconnect_cancel);
    lv_label_set_text(ui->wifi_btn_disconnect_cancel_label, "取消");
    lv_label_set_long_mode(ui->wifi_btn_disconnect_cancel_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->wifi_btn_disconnect_cancel_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->wifi_btn_disconnect_cancel, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->wifi_btn_disconnect_cancel_label, LV_PCT(100));

    //Write style for wifi_btn_disconnect_cancel, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->wifi_btn_disconnect_cancel, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_btn_disconnect_cancel, lv_color_hex(0x888888), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_btn_disconnect_cancel, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->wifi_btn_disconnect_cancel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_btn_disconnect_cancel, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_btn_disconnect_cancel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_btn_disconnect_cancel, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_btn_disconnect_cancel, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_btn_disconnect_cancel, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_btn_disconnect_cancel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_label_fresh_tip
    ui->wifi_label_fresh_tip = lv_label_create(ui->wifi);
    lv_obj_set_pos(ui->wifi_label_fresh_tip, 15, 223);
    lv_obj_set_size(ui->wifi_label_fresh_tip, 100, 23);
    lv_obj_add_flag(ui->wifi_label_fresh_tip, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->wifi_label_fresh_tip, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui->wifi_label_fresh_tip, "Searching......");
    lv_label_set_long_mode(ui->wifi_label_fresh_tip, LV_LABEL_LONG_WRAP);

    //Write style for wifi_label_fresh_tip, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_label_fresh_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_label_fresh_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_label_fresh_tip, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_label_fresh_tip, &lv_font_montserratMedium_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_label_fresh_tip, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->wifi_label_fresh_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->wifi_label_fresh_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_label_fresh_tip, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_label_fresh_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_label_fresh_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_label_fresh_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_label_fresh_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_label_fresh_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_label_fresh_tip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of wifi.


    //Update current screen layout.
    lv_obj_update_layout(ui->wifi);

    //Init events for screen.
    events_init_wifi(ui);
}
