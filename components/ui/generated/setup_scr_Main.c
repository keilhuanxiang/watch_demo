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



void setup_scr_Main(lv_ui *ui)
{
    //Write codes Main
    ui->Main = lv_obj_create(NULL);
    lv_obj_set_size(ui->Main, 320, 240);
    lv_obj_set_scrollbar_mode(ui->Main, LV_SCROLLBAR_MODE_OFF);

    //Write style for Main, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->Main, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->Main, lv_color_hex(0x242424), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->Main, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes Main_cont_hour
    ui->Main_cont_hour = lv_obj_create(ui->Main);
    lv_obj_set_pos(ui->Main_cont_hour, 4, 69);
    lv_obj_set_size(ui->Main_cont_hour, 154, 100);
    lv_obj_set_scrollbar_mode(ui->Main_cont_hour, LV_SCROLLBAR_MODE_OFF);

    //Write style for Main_cont_hour, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->Main_cont_hour, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->Main_cont_hour, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->Main_cont_hour, lv_color_hex(0xffa300), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->Main_cont_hour, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Main_cont_hour, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->Main_cont_hour, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->Main_cont_hour, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->Main_cont_hour, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->Main_cont_hour, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->Main_cont_hour, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Main_cont_hour, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->Main_cont_hour, lv_color_hex(0xffa300), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->Main_cont_hour, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->Main_cont_hour, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_offset_x(ui->Main_cont_hour, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_offset_y(ui->Main_cont_hour, 2, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes Main_img_hour1
    ui->Main_img_hour1 = lv_image_create(ui->Main_cont_hour);
    lv_obj_set_pos(ui->Main_img_hour1, 74, 3);
    lv_obj_set_size(ui->Main_img_hour1, 70, 92);
    lv_obj_add_flag(ui->Main_img_hour1, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->Main_img_hour1, &_icon_mini_1_RGB565A8_70x92);
    lv_image_set_pivot(ui->Main_img_hour1, 50,50);
    lv_image_set_rotation(ui->Main_img_hour1, 0);

    //Write style for Main_img_hour1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->Main_img_hour1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->Main_img_hour1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes Main_img_hour0
    ui->Main_img_hour0 = lv_image_create(ui->Main_cont_hour);
    lv_obj_set_pos(ui->Main_img_hour0, 4, 3);
    lv_obj_set_size(ui->Main_img_hour0, 70, 92);
    lv_obj_add_flag(ui->Main_img_hour0, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->Main_img_hour0, &_icon_mini_0_RGB565A8_70x92);
    lv_image_set_pivot(ui->Main_img_hour0, 50,50);
    lv_image_set_rotation(ui->Main_img_hour0, 0);

    //Write style for Main_img_hour0, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->Main_img_hour0, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->Main_img_hour0, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes Main_cont_min
    ui->Main_cont_min = lv_obj_create(ui->Main);
    lv_obj_set_pos(ui->Main_cont_min, 160, 69);
    lv_obj_set_size(ui->Main_cont_min, 154, 100);
    lv_obj_set_scrollbar_mode(ui->Main_cont_min, LV_SCROLLBAR_MODE_OFF);

    //Write style for Main_cont_min, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->Main_cont_min, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->Main_cont_min, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->Main_cont_min, lv_color_hex(0xffa300), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->Main_cont_min, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Main_cont_min, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->Main_cont_min, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->Main_cont_min, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->Main_cont_min, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->Main_cont_min, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->Main_cont_min, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Main_cont_min, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->Main_cont_min, lv_color_hex(0xffa300), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->Main_cont_min, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->Main_cont_min, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_offset_x(ui->Main_cont_min, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_offset_y(ui->Main_cont_min, 2, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes Main_img_min1
    ui->Main_img_min1 = lv_image_create(ui->Main_cont_min);
    lv_obj_set_pos(ui->Main_img_min1, 77, 3);
    lv_obj_set_size(ui->Main_img_min1, 70, 92);
    lv_obj_add_flag(ui->Main_img_min1, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->Main_img_min1, &_icon_mini_3_RGB565A8_70x92);
    lv_image_set_pivot(ui->Main_img_min1, 50,50);
    lv_image_set_rotation(ui->Main_img_min1, 0);

    //Write style for Main_img_min1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->Main_img_min1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->Main_img_min1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes Main_img_min0
    ui->Main_img_min0 = lv_image_create(ui->Main_cont_min);
    lv_obj_set_pos(ui->Main_img_min0, 8, 3);
    lv_obj_set_size(ui->Main_img_min0, 70, 92);
    lv_obj_add_flag(ui->Main_img_min0, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->Main_img_min0, &_icon_mini_2_RGB565A8_70x92);
    lv_image_set_pivot(ui->Main_img_min0, 50,50);
    lv_image_set_rotation(ui->Main_img_min0, 0);

    //Write style for Main_img_min0, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->Main_img_min0, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->Main_img_min0, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes Main_btn_meau
    ui->Main_btn_meau = lv_button_create(ui->Main);
    lv_obj_set_pos(ui->Main_btn_meau, 264, 189);
    lv_obj_set_size(ui->Main_btn_meau, 48, 43);
    ui->Main_btn_meau_label = lv_label_create(ui->Main_btn_meau);
    lv_label_set_text(ui->Main_btn_meau_label, "" LV_SYMBOL_BARS "");
    lv_label_set_long_mode(ui->Main_btn_meau_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->Main_btn_meau_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->Main_btn_meau, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->Main_btn_meau_label, LV_PCT(100));

    //Write style for Main_btn_meau, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->Main_btn_meau, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->Main_btn_meau, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Main_btn_meau, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Main_btn_meau, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->Main_btn_meau, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->Main_btn_meau, &lv_font_montserratMedium_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->Main_btn_meau, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->Main_btn_meau, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes Main_cont_top
    ui->Main_cont_top = lv_obj_create(ui->Main);
    lv_obj_set_pos(ui->Main_cont_top, 0, 0);
    lv_obj_set_size(ui->Main_cont_top, 320, 65);
    lv_obj_set_scrollbar_mode(ui->Main_cont_top, LV_SCROLLBAR_MODE_OFF);

    //Write style for Main_cont_top, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->Main_cont_top, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->Main_cont_top, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->Main_cont_top, lv_color_hex(0x242424), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->Main_cont_top, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Main_cont_top, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->Main_cont_top, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->Main_cont_top, lv_color_hex(0x242424), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->Main_cont_top, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->Main_cont_top, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->Main_cont_top, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->Main_cont_top, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->Main_cont_top, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Main_cont_top, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes Main_label_date
    ui->Main_label_date = lv_label_create(ui->Main_cont_top);
    lv_obj_set_pos(ui->Main_label_date, 70, 30);
    lv_obj_set_size(ui->Main_label_date, 100, 25);
    lv_label_set_text(ui->Main_label_date, "2025/01/01");
    lv_label_set_long_mode(ui->Main_label_date, LV_LABEL_LONG_WRAP);

    //Write style for Main_label_date, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->Main_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Main_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->Main_label_date, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->Main_label_date, &lv_font_SourceHanSerifSC_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->Main_label_date, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->Main_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->Main_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->Main_label_date, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->Main_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->Main_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->Main_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->Main_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->Main_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Main_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes Main_line_top
    ui->Main_line_top = lv_line_create(ui->Main_cont_top);
    lv_obj_set_pos(ui->Main_line_top, 0, 0);
    lv_obj_set_size(ui->Main_line_top, 320, 65);
    static lv_point_precise_t Main_line_top[] = {{0, 55},{180, 55},{200, 20},{320, 20}};
    lv_line_set_points(ui->Main_line_top, Main_line_top, 4);

    //Write style for Main_line_top, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->Main_line_top, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->Main_line_top, lv_color_hex(0x0085ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->Main_line_top, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->Main_line_top, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes Main_label_week
    ui->Main_label_week = lv_label_create(ui->Main_cont_top);
    lv_obj_set_pos(ui->Main_label_week, 0, 0);
    lv_obj_set_size(ui->Main_label_week, 70, 25);
    lv_label_set_text(ui->Main_label_week, "星期一");
    lv_label_set_long_mode(ui->Main_label_week, LV_LABEL_LONG_WRAP);

    //Write style for Main_label_week, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->Main_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Main_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->Main_label_week, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->Main_label_week, &lv_font_SourceHanSerifSC_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->Main_label_week, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->Main_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->Main_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->Main_label_week, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->Main_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->Main_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->Main_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->Main_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->Main_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Main_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes Main_label_battery
    ui->Main_label_battery = lv_label_create(ui->Main_cont_top);
    lv_obj_set_pos(ui->Main_label_battery, 300, 1);
    lv_obj_set_size(ui->Main_label_battery, 16, 16);
    lv_label_set_text(ui->Main_label_battery, "" LV_SYMBOL_BATTERY_EMPTY " ");
    lv_label_set_long_mode(ui->Main_label_battery, LV_LABEL_LONG_WRAP);

    //Write style for Main_label_battery, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->Main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->Main_label_battery, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->Main_label_battery, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->Main_label_battery, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->Main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->Main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->Main_label_battery, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->Main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->Main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->Main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->Main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->Main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes Main_label_wifi
    ui->Main_label_wifi = lv_label_create(ui->Main_cont_top);
    lv_obj_set_pos(ui->Main_label_wifi, 280, 1);
    lv_obj_set_size(ui->Main_label_wifi, 16, 16);
    lv_obj_add_flag(ui->Main_label_wifi, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui->Main_label_wifi, "" LV_SYMBOL_WIFI " ");
    lv_label_set_long_mode(ui->Main_label_wifi, LV_LABEL_LONG_WRAP);

    //Write style for Main_label_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->Main_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Main_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->Main_label_wifi, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->Main_label_wifi, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->Main_label_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->Main_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->Main_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->Main_label_wifi, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->Main_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->Main_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->Main_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->Main_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->Main_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Main_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes Main_label_music
    ui->Main_label_music = lv_label_create(ui->Main_cont_top);
    lv_obj_set_pos(ui->Main_label_music, 260, 1);
    lv_obj_set_size(ui->Main_label_music, 16, 16);
    lv_obj_add_flag(ui->Main_label_music, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui->Main_label_music, "" LV_SYMBOL_AUDIO " ");
    lv_label_set_long_mode(ui->Main_label_music, LV_LABEL_LONG_WRAP);

    //Write style for Main_label_music, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->Main_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->Main_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->Main_label_music, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->Main_label_music, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->Main_label_music, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->Main_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->Main_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->Main_label_music, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->Main_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->Main_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->Main_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->Main_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->Main_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->Main_label_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of Main.


    //Update current screen layout.
    lv_obj_update_layout(ui->Main);

    //Init events for screen.
    events_init_Main(ui);
}
