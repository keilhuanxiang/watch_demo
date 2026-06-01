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



void setup_scr_music(lv_ui *ui)
{
    //Write codes music
    ui->music = lv_obj_create(NULL);
    lv_obj_set_size(ui->music, 320, 240);
    lv_obj_set_scrollbar_mode(ui->music, LV_SCROLLBAR_MODE_OFF);

    //Write style for music, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->music, lv_color_hex(0x242424), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->music, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_slider_volume
    ui->music_slider_volume = lv_slider_create(ui->music);
    lv_obj_set_pos(ui->music_slider_volume, 290, 50);
    lv_obj_set_size(ui->music_slider_volume, 8, 90);
    lv_obj_add_flag(ui->music_slider_volume, LV_OBJ_FLAG_HIDDEN);
    lv_slider_set_range(ui->music_slider_volume, 0, 100);
    lv_slider_set_mode(ui->music_slider_volume, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->music_slider_volume, 50, LV_ANIM_OFF);

    //Write style for music_slider_volume, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_slider_volume, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->music_slider_volume, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->music_slider_volume, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_slider_volume, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->music_slider_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_slider_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for music_slider_volume, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_slider_volume, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->music_slider_volume, lv_color_hex(0x6e6e6e), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->music_slider_volume, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_slider_volume, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for music_slider_volume, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_slider_volume, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->music_slider_volume, lv_color_hex(0x8F8F8F), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->music_slider_volume, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_slider_volume, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes music_btn_close
    ui->music_btn_close = lv_button_create(ui->music);
    lv_obj_set_pos(ui->music_btn_close, 0, 0);
    lv_obj_set_size(ui->music_btn_close, 50, 50);
    ui->music_btn_close_label = lv_label_create(ui->music_btn_close);
    lv_label_set_text(ui->music_btn_close_label, "" LV_SYMBOL_CLOSE " ");
    lv_label_set_long_mode(ui->music_btn_close_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->music_btn_close_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->music_btn_close, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->music_btn_close_label, LV_PCT(100));

    //Write style for music_btn_close, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_btn_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->music_btn_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_btn_close, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_btn_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_btn_close, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_btn_close, &lv_font_montserratMedium_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_btn_close, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_btn_close, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_btn_volume
    ui->music_btn_volume = lv_button_create(ui->music);
    lv_obj_set_pos(ui->music_btn_volume, 270, 0);
    lv_obj_set_size(ui->music_btn_volume, 50, 50);
    ui->music_btn_volume_label = lv_label_create(ui->music_btn_volume);
    lv_label_set_text(ui->music_btn_volume_label, " " LV_SYMBOL_MUTE " ");
    lv_label_set_long_mode(ui->music_btn_volume_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->music_btn_volume_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->music_btn_volume, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->music_btn_volume_label, LV_PCT(100));

    //Write style for music_btn_volume, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_btn_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->music_btn_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_btn_volume, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_btn_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_btn_volume, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_btn_volume, &lv_font_montserratMedium_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_btn_volume, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_btn_volume, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_slider_prog
    ui->music_slider_prog = lv_slider_create(ui->music);
    lv_obj_set_pos(ui->music_slider_prog, 40, 153);
    lv_obj_set_size(ui->music_slider_prog, 240, 3);
    lv_obj_remove_flag(ui->music_slider_prog, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(ui->music_slider_prog, LV_OBJ_FLAG_PRESS_LOCK);
    lv_slider_set_range(ui->music_slider_prog, 0, 100);
    lv_slider_set_mode(ui->music_slider_prog, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->music_slider_prog, 0, LV_ANIM_OFF);

    //Write style for music_slider_prog, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_slider_prog, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->music_slider_prog, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->music_slider_prog, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_slider_prog, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->music_slider_prog, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_slider_prog, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for music_slider_prog, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_slider_prog, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->music_slider_prog, lv_color_hex(0x6e6e6e), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->music_slider_prog, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_slider_prog, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for music_slider_prog, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_slider_prog, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->music_slider_prog, lv_color_hex(0x8F8F8F), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->music_slider_prog, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_slider_prog, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes music_label_play_time
    ui->music_label_play_time = lv_label_create(ui->music);
    lv_obj_set_pos(ui->music_label_play_time, 0, 163);
    lv_obj_set_size(ui->music_label_play_time, 50, 16);
    lv_label_set_text(ui->music_label_play_time, "00:00");
    lv_label_set_long_mode(ui->music_label_play_time, LV_LABEL_LONG_WRAP);

    //Write style for music_label_play_time, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->music_label_play_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_label_play_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_label_play_time, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_label_play_time, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_label_play_time, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->music_label_play_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->music_label_play_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_label_play_time, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->music_label_play_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->music_label_play_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->music_label_play_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->music_label_play_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->music_label_play_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_label_play_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_label_total_time
    ui->music_label_total_time = lv_label_create(ui->music);
    lv_obj_set_pos(ui->music_label_total_time, 270, 163);
    lv_obj_set_size(ui->music_label_total_time, 50, 16);
    lv_label_set_text(ui->music_label_total_time, "00:00");
    lv_label_set_long_mode(ui->music_label_total_time, LV_LABEL_LONG_WRAP);

    //Write style for music_label_total_time, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->music_label_total_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_label_total_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_label_total_time, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_label_total_time, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_label_total_time, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->music_label_total_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->music_label_total_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_label_total_time, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->music_label_total_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->music_label_total_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->music_label_total_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->music_label_total_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->music_label_total_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_label_total_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_btn_play
    ui->music_btn_play = lv_button_create(ui->music);
    lv_obj_set_pos(ui->music_btn_play, 130, 180);
    lv_obj_set_size(ui->music_btn_play, 48, 48);
    ui->music_btn_play_label = lv_label_create(ui->music_btn_play);
    lv_label_set_text(ui->music_btn_play_label, "  " LV_SYMBOL_PLAY " ");
    lv_label_set_long_mode(ui->music_btn_play_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->music_btn_play_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->music_btn_play, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->music_btn_play_label, LV_PCT(100));

    //Write style for music_btn_play, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_btn_play, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->music_btn_play, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_btn_play, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_btn_play, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->music_btn_play, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->music_btn_play, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->music_btn_play, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_offset_x(ui->music_btn_play, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_offset_y(ui->music_btn_play, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_btn_play, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_btn_play, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_btn_play, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_btn_play, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_btn_prev
    ui->music_btn_prev = lv_button_create(ui->music);
    lv_obj_set_pos(ui->music_btn_prev, 65, 180);
    lv_obj_set_size(ui->music_btn_prev, 48, 48);
    ui->music_btn_prev_label = lv_label_create(ui->music_btn_prev);
    lv_label_set_text(ui->music_btn_prev_label, " " LV_SYMBOL_PREV " ");
    lv_label_set_long_mode(ui->music_btn_prev_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->music_btn_prev_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->music_btn_prev, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->music_btn_prev_label, LV_PCT(100));

    //Write style for music_btn_prev, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_btn_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->music_btn_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_btn_prev, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_btn_prev, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->music_btn_prev, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->music_btn_prev, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->music_btn_prev, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_offset_x(ui->music_btn_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_offset_y(ui->music_btn_prev, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_btn_prev, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_btn_prev, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_btn_prev, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_btn_prev, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_btn_next
    ui->music_btn_next = lv_button_create(ui->music);
    lv_obj_set_pos(ui->music_btn_next, 195, 180);
    lv_obj_set_size(ui->music_btn_next, 48, 48);
    ui->music_btn_next_label = lv_label_create(ui->music_btn_next);
    lv_label_set_text(ui->music_btn_next_label, " " LV_SYMBOL_NEXT " ");
    lv_label_set_long_mode(ui->music_btn_next_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->music_btn_next_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->music_btn_next, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->music_btn_next_label, LV_PCT(100));

    //Write style for music_btn_next, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_btn_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->music_btn_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_btn_next, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_btn_next, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->music_btn_next, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->music_btn_next, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->music_btn_next, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_offset_x(ui->music_btn_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_offset_y(ui->music_btn_next, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_btn_next, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_btn_next, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_btn_next, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_btn_next, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_btn_list
    ui->music_btn_list = lv_button_create(ui->music);
    lv_obj_set_pos(ui->music_btn_list, 260, 180);
    lv_obj_set_size(ui->music_btn_list, 48, 48);
    ui->music_btn_list_label = lv_label_create(ui->music_btn_list);
    lv_label_set_text(ui->music_btn_list_label, " " LV_SYMBOL_DIRECTORY " ");
    lv_label_set_long_mode(ui->music_btn_list_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->music_btn_list_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->music_btn_list, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->music_btn_list_label, LV_PCT(100));

    //Write style for music_btn_list, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_btn_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->music_btn_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_btn_list, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_btn_list, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->music_btn_list, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->music_btn_list, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->music_btn_list, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_offset_x(ui->music_btn_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_offset_y(ui->music_btn_list, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_btn_list, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_btn_list, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_btn_list, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_btn_list, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_label_name
    ui->music_label_name = lv_label_create(ui->music);
    lv_obj_set_pos(ui->music_label_name, 46, 10);
    lv_obj_set_size(ui->music_label_name, 221, 50);
    lv_label_set_text(ui->music_label_name, "感受音乐的力量");
    lv_label_set_long_mode(ui->music_label_name, LV_LABEL_LONG_WRAP);

    //Write style for music_label_name, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->music_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_label_name, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_label_name, &lv_font_SourceHanSerifSC_Regular_23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_label_name, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->music_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->music_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_label_name, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->music_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->music_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->music_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->music_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->music_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_img_player
    ui->music_img_player = lv_image_create(ui->music);
    lv_obj_set_pos(ui->music_img_player, 125, 65);
    lv_obj_set_size(ui->music_img_player, 70, 70);
    lv_obj_add_flag(ui->music_img_player, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->music_img_player, &_icon_player_RGB565A8_70x70);
    lv_image_set_pivot(ui->music_img_player, 50,50);
    lv_image_set_rotation(ui->music_img_player, 0);

    //Write style for music_img_player, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->music_img_player, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->music_img_player, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_btn_mode
    ui->music_btn_mode = lv_button_create(ui->music);
    lv_obj_set_pos(ui->music_btn_mode, 0, 180);
    lv_obj_set_size(ui->music_btn_mode, 48, 48);
    ui->music_btn_mode_label = lv_label_create(ui->music_btn_mode);
    lv_label_set_text(ui->music_btn_mode_label, " " LV_SYMBOL_REFRESH " ");
    lv_label_set_long_mode(ui->music_btn_mode_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->music_btn_mode_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->music_btn_mode, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->music_btn_mode_label, LV_PCT(100));

    //Write style for music_btn_mode, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_btn_mode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->music_btn_mode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_btn_mode, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_btn_mode, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->music_btn_mode, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->music_btn_mode, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->music_btn_mode, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_offset_x(ui->music_btn_mode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_offset_y(ui->music_btn_mode, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_btn_mode, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_btn_mode, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_btn_mode, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_btn_mode, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_btn_leave
    ui->music_btn_leave = lv_button_create(ui->music);
    lv_obj_set_pos(ui->music_btn_leave, 0, 60);
    lv_obj_set_size(ui->music_btn_leave, 50, 50);
    ui->music_btn_leave_label = lv_label_create(ui->music_btn_leave);
    lv_label_set_text(ui->music_btn_leave_label, "" LV_SYMBOL_BARS " ");
    lv_label_set_long_mode(ui->music_btn_leave_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->music_btn_leave_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->music_btn_leave, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->music_btn_leave_label, LV_PCT(100));

    //Write style for music_btn_leave, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_btn_leave, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->music_btn_leave, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_btn_leave, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_btn_leave, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_btn_leave, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_btn_leave, &lv_font_montserratMedium_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_btn_leave, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_btn_leave, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of music.


    //Update current screen layout.
    lv_obj_update_layout(ui->music);

    //Init events for screen.
    events_init_music(ui);
}
