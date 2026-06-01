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



void setup_scr_file(lv_ui *ui)
{
    //Write codes file
    ui->file = lv_obj_create(NULL);
    lv_obj_set_size(ui->file, 320, 240);
    lv_obj_set_scrollbar_mode(ui->file, LV_SCROLLBAR_MODE_OFF);

    //Write style for file, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->file, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->file, lv_color_hex(0x242424), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->file, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes file_list_file
    ui->file_list_file = lv_list_create(ui->file);
    lv_obj_set_pos(ui->file_list_file, 0, 0);
    lv_obj_set_size(ui->file_list_file, 320, 240);
    lv_obj_set_scrollbar_mode(ui->file_list_file, LV_SCROLLBAR_MODE_OFF);
    ui->file_list_file_item0 = lv_list_add_button(ui->file_list_file, LV_SYMBOL_LEFT, "/");
    ui->file_list_file_item1 = lv_list_add_button(ui->file_list_file, LV_SYMBOL_LEFT, "上一页");
    ui->file_list_file_item2 = lv_list_add_button(ui->file_list_file, LV_SYMBOL_DIRECTORY, "save_2");

    //Write style state: LV_STATE_DEFAULT for &style_file_list_file_main_main_default
    static lv_style_t style_file_list_file_main_main_default;
    ui_init_style(&style_file_list_file_main_main_default);

    lv_style_set_pad_top(&style_file_list_file_main_main_default, 5);
    lv_style_set_pad_left(&style_file_list_file_main_main_default, 5);
    lv_style_set_pad_right(&style_file_list_file_main_main_default, 5);
    lv_style_set_pad_bottom(&style_file_list_file_main_main_default, 5);
    lv_style_set_bg_opa(&style_file_list_file_main_main_default, 255);
    lv_style_set_bg_color(&style_file_list_file_main_main_default, lv_color_hex(0x242424));
    lv_style_set_bg_grad_dir(&style_file_list_file_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_file_list_file_main_main_default, 1);
    lv_style_set_border_opa(&style_file_list_file_main_main_default, 255);
    lv_style_set_border_color(&style_file_list_file_main_main_default, lv_color_hex(0x242424));
    lv_style_set_border_side(&style_file_list_file_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_file_list_file_main_main_default, 3);
    lv_style_set_shadow_width(&style_file_list_file_main_main_default, 0);
    lv_obj_add_style(ui->file_list_file, &style_file_list_file_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_file_list_file_main_scrollbar_default
    static lv_style_t style_file_list_file_main_scrollbar_default;
    ui_init_style(&style_file_list_file_main_scrollbar_default);

    lv_style_set_radius(&style_file_list_file_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_file_list_file_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_file_list_file_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_file_list_file_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->file_list_file, &style_file_list_file_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_file_list_file_extra_btns_main_default
    static lv_style_t style_file_list_file_extra_btns_main_default;
    ui_init_style(&style_file_list_file_extra_btns_main_default);

    lv_style_set_pad_top(&style_file_list_file_extra_btns_main_default, 2);
    lv_style_set_pad_left(&style_file_list_file_extra_btns_main_default, 4);
    lv_style_set_pad_right(&style_file_list_file_extra_btns_main_default, 4);
    lv_style_set_pad_bottom(&style_file_list_file_extra_btns_main_default, 2);
    lv_style_set_border_width(&style_file_list_file_extra_btns_main_default, 1);
    lv_style_set_border_opa(&style_file_list_file_extra_btns_main_default, 255);
    lv_style_set_border_color(&style_file_list_file_extra_btns_main_default, lv_color_hex(0x242424));
    lv_style_set_border_side(&style_file_list_file_extra_btns_main_default, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_text_color(&style_file_list_file_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_text_font(&style_file_list_file_extra_btns_main_default, &lv_font_SourceHanSerifSC_Regular_23);
    lv_style_set_text_opa(&style_file_list_file_extra_btns_main_default, 255);
    lv_style_set_radius(&style_file_list_file_extra_btns_main_default, 2);
    lv_style_set_bg_opa(&style_file_list_file_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_file_list_file_extra_btns_main_default, lv_color_hex(0x242424));
    lv_style_set_bg_grad_dir(&style_file_list_file_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->file_list_file_item2, &style_file_list_file_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->file_list_file_item1, &style_file_list_file_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->file_list_file_item0, &style_file_list_file_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_file_list_file_extra_texts_main_default
    static lv_style_t style_file_list_file_extra_texts_main_default;
    ui_init_style(&style_file_list_file_extra_texts_main_default);

    lv_style_set_pad_top(&style_file_list_file_extra_texts_main_default, 2);
    lv_style_set_pad_left(&style_file_list_file_extra_texts_main_default, 4);
    lv_style_set_pad_right(&style_file_list_file_extra_texts_main_default, 4);
    lv_style_set_pad_bottom(&style_file_list_file_extra_texts_main_default, 2);
    lv_style_set_border_width(&style_file_list_file_extra_texts_main_default, 1);
    lv_style_set_border_opa(&style_file_list_file_extra_texts_main_default, 255);
    lv_style_set_border_color(&style_file_list_file_extra_texts_main_default, lv_color_hex(0xe1e6ee));
    lv_style_set_border_side(&style_file_list_file_extra_texts_main_default, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_text_color(&style_file_list_file_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_file_list_file_extra_texts_main_default, &lv_font_SourceHanSerifSC_Regular_14);
    lv_style_set_text_opa(&style_file_list_file_extra_texts_main_default, 255);
    lv_style_set_radius(&style_file_list_file_extra_texts_main_default, 2);
    lv_style_set_transform_width(&style_file_list_file_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_file_list_file_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_file_list_file_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_file_list_file_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //The custom code of file.


    //Update current screen layout.
    lv_obj_update_layout(ui->file);

    //Init events for screen.
    events_init_file(ui);
}
