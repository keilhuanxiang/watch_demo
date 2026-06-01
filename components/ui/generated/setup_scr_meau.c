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



void setup_scr_meau(lv_ui *ui)
{
    //Write codes meau
    ui->meau = lv_obj_create(NULL);
    lv_obj_set_size(ui->meau, 320, 240);
    lv_obj_set_scrollbar_mode(ui->meau, LV_SCROLLBAR_MODE_OFF);

    //Write style for meau, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->meau, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->meau, lv_color_hex(0x242424), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->meau, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes meau_btn_home
    ui->meau_btn_home = lv_button_create(ui->meau);
    lv_obj_set_pos(ui->meau_btn_home, 10, 10);
    lv_obj_set_size(ui->meau_btn_home, 70, 70);
    ui->meau_btn_home_label = lv_label_create(ui->meau_btn_home);
    lv_label_set_text(ui->meau_btn_home_label, "" LV_SYMBOL_HOME " ");
    lv_label_set_long_mode(ui->meau_btn_home_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->meau_btn_home_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->meau_btn_home, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->meau_btn_home_label, LV_PCT(100));

    //Write style for meau_btn_home, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->meau_btn_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->meau_btn_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->meau_btn_home, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->meau_btn_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->meau_btn_home, lv_color_hex(0x2900ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->meau_btn_home, &lv_font_montserratMedium_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->meau_btn_home, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->meau_btn_home, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes meau_label_home
    ui->meau_label_home = lv_label_create(ui->meau);
    lv_obj_set_pos(ui->meau_label_home, 20, 70);
    lv_obj_set_size(ui->meau_label_home, 50, 30);
    lv_label_set_text(ui->meau_label_home, "主页");
    lv_label_set_long_mode(ui->meau_label_home, LV_LABEL_LONG_WRAP);

    //Write style for meau_label_home, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->meau_label_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->meau_label_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->meau_label_home, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->meau_label_home, &lv_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->meau_label_home, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->meau_label_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->meau_label_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->meau_label_home, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->meau_label_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->meau_label_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->meau_label_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->meau_label_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->meau_label_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->meau_label_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes meau_btn_music
    ui->meau_btn_music = lv_button_create(ui->meau);
    lv_obj_set_pos(ui->meau_btn_music, 120, 13);
    lv_obj_set_size(ui->meau_btn_music, 70, 70);
    ui->meau_btn_music_label = lv_label_create(ui->meau_btn_music);
    lv_label_set_text(ui->meau_btn_music_label, "" LV_SYMBOL_AUDIO " ");
    lv_label_set_long_mode(ui->meau_btn_music_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->meau_btn_music_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->meau_btn_music, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->meau_btn_music_label, LV_PCT(100));

    //Write style for meau_btn_music, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->meau_btn_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->meau_btn_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->meau_btn_music, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->meau_btn_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->meau_btn_music, lv_color_hex(0xcfff00), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->meau_btn_music, &lv_font_montserratMedium_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->meau_btn_music, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->meau_btn_music, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes meau_label_music
    ui->meau_label_music = lv_label_create(ui->meau);
    lv_obj_set_pos(ui->meau_label_music, 130, 70);
    lv_obj_set_size(ui->meau_label_music, 50, 30);
    lv_label_set_text(ui->meau_label_music, "音乐");
    lv_label_set_long_mode(ui->meau_label_music, LV_LABEL_LONG_WRAP);

    //Write style for meau_label_music, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->meau_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->meau_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->meau_label_music, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->meau_label_music, &lv_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->meau_label_music, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->meau_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->meau_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->meau_label_music, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->meau_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->meau_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->meau_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->meau_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->meau_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->meau_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes meau_btn_wifi
    ui->meau_btn_wifi = lv_button_create(ui->meau);
    lv_obj_set_pos(ui->meau_btn_wifi, 230, 13);
    lv_obj_set_size(ui->meau_btn_wifi, 70, 70);
    ui->meau_btn_wifi_label = lv_label_create(ui->meau_btn_wifi);
    lv_label_set_text(ui->meau_btn_wifi_label, "" LV_SYMBOL_WIFI " ");
    lv_label_set_long_mode(ui->meau_btn_wifi_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->meau_btn_wifi_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->meau_btn_wifi, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->meau_btn_wifi_label, LV_PCT(100));

    //Write style for meau_btn_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->meau_btn_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->meau_btn_wifi, lv_color_hex(0x242424), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->meau_btn_wifi, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->meau_btn_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->meau_btn_wifi, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->meau_btn_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->meau_btn_wifi, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->meau_btn_wifi, &lv_font_montserratMedium_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->meau_btn_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->meau_btn_wifi, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes meau_label_wifi
    ui->meau_label_wifi = lv_label_create(ui->meau);
    lv_obj_set_pos(ui->meau_label_wifi, 240, 70);
    lv_obj_set_size(ui->meau_label_wifi, 50, 30);
    lv_label_set_text(ui->meau_label_wifi, "网络");
    lv_label_set_long_mode(ui->meau_label_wifi, LV_LABEL_LONG_WRAP);

    //Write style for meau_label_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->meau_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->meau_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->meau_label_wifi, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->meau_label_wifi, &lv_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->meau_label_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->meau_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->meau_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->meau_label_wifi, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->meau_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->meau_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->meau_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->meau_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->meau_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->meau_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes meau_btn_camera
    ui->meau_btn_camera = lv_button_create(ui->meau);
    lv_obj_set_pos(ui->meau_btn_camera, 10, 120);
    lv_obj_set_size(ui->meau_btn_camera, 70, 70);
    ui->meau_btn_camera_label = lv_label_create(ui->meau_btn_camera);
    lv_label_set_text(ui->meau_btn_camera_label, "" LV_SYMBOL_IMAGE " ");
    lv_label_set_long_mode(ui->meau_btn_camera_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->meau_btn_camera_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->meau_btn_camera, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->meau_btn_camera_label, LV_PCT(100));

    //Write style for meau_btn_camera, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->meau_btn_camera, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->meau_btn_camera, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->meau_btn_camera, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->meau_btn_camera, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->meau_btn_camera, lv_color_hex(0x1bff00), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->meau_btn_camera, &lv_font_montserratMedium_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->meau_btn_camera, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->meau_btn_camera, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes meau_label_camera
    ui->meau_label_camera = lv_label_create(ui->meau);
    lv_obj_set_pos(ui->meau_label_camera, 20, 177);
    lv_obj_set_size(ui->meau_label_camera, 50, 30);
    lv_label_set_text(ui->meau_label_camera, "相机");
    lv_label_set_long_mode(ui->meau_label_camera, LV_LABEL_LONG_WRAP);

    //Write style for meau_label_camera, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->meau_label_camera, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->meau_label_camera, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->meau_label_camera, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->meau_label_camera, &lv_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->meau_label_camera, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->meau_label_camera, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->meau_label_camera, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->meau_label_camera, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->meau_label_camera, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->meau_label_camera, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->meau_label_camera, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->meau_label_camera, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->meau_label_camera, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->meau_label_camera, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes meau_btn_document
    ui->meau_btn_document = lv_button_create(ui->meau);
    lv_obj_set_pos(ui->meau_btn_document, 120, 120);
    lv_obj_set_size(ui->meau_btn_document, 70, 70);
    ui->meau_btn_document_label = lv_label_create(ui->meau_btn_document);
    lv_label_set_text(ui->meau_btn_document_label, "" LV_SYMBOL_DIRECTORY " ");
    lv_label_set_long_mode(ui->meau_btn_document_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->meau_btn_document_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->meau_btn_document, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->meau_btn_document_label, LV_PCT(100));

    //Write style for meau_btn_document, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->meau_btn_document, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->meau_btn_document, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->meau_btn_document, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->meau_btn_document, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->meau_btn_document, lv_color_hex(0xfff700), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->meau_btn_document, &lv_font_montserratMedium_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->meau_btn_document, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->meau_btn_document, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes meau_label_document
    ui->meau_label_document = lv_label_create(ui->meau);
    lv_obj_set_pos(ui->meau_label_document, 130, 177);
    lv_obj_set_size(ui->meau_label_document, 50, 30);
    lv_label_set_text(ui->meau_label_document, "文件");
    lv_label_set_long_mode(ui->meau_label_document, LV_LABEL_LONG_WRAP);

    //Write style for meau_label_document, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->meau_label_document, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->meau_label_document, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->meau_label_document, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->meau_label_document, &lv_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->meau_label_document, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->meau_label_document, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->meau_label_document, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->meau_label_document, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->meau_label_document, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->meau_label_document, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->meau_label_document, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->meau_label_document, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->meau_label_document, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->meau_label_document, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes meau_btn_setting
    ui->meau_btn_setting = lv_button_create(ui->meau);
    lv_obj_set_pos(ui->meau_btn_setting, 230, 120);
    lv_obj_set_size(ui->meau_btn_setting, 70, 70);
    ui->meau_btn_setting_label = lv_label_create(ui->meau_btn_setting);
    lv_label_set_text(ui->meau_btn_setting_label, "" LV_SYMBOL_SETTINGS " ");
    lv_label_set_long_mode(ui->meau_btn_setting_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->meau_btn_setting_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->meau_btn_setting, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->meau_btn_setting_label, LV_PCT(100));

    //Write style for meau_btn_setting, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->meau_btn_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->meau_btn_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->meau_btn_setting, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->meau_btn_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->meau_btn_setting, lv_color_hex(0x797979), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->meau_btn_setting, &lv_font_montserratMedium_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->meau_btn_setting, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->meau_btn_setting, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes meau_label_setting
    ui->meau_label_setting = lv_label_create(ui->meau);
    lv_obj_set_pos(ui->meau_label_setting, 240, 177);
    lv_obj_set_size(ui->meau_label_setting, 50, 30);
    lv_label_set_text(ui->meau_label_setting, "设置");
    lv_label_set_long_mode(ui->meau_label_setting, LV_LABEL_LONG_WRAP);

    //Write style for meau_label_setting, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->meau_label_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->meau_label_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->meau_label_setting, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->meau_label_setting, &lv_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->meau_label_setting, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->meau_label_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->meau_label_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->meau_label_setting, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->meau_label_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->meau_label_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->meau_label_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->meau_label_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->meau_label_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->meau_label_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of meau.


    //Update current screen layout.
    lv_obj_update_layout(ui->meau);

    //Init events for screen.
    events_init_meau(ui);
}
