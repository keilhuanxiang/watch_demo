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



void setup_scr_camera(lv_ui *ui)
{
    //Write codes camera
    ui->camera = lv_obj_create(NULL);
    lv_obj_set_size(ui->camera, 320, 240);
    lv_obj_set_scrollbar_mode(ui->camera, LV_SCROLLBAR_MODE_OFF);

    //Write style for camera, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->camera, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->camera, lv_color_hex(0x242424), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->camera, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes camera_img_preview
    ui->camera_img_preview = lv_image_create(ui->camera);
    lv_obj_set_pos(ui->camera_img_preview, 0, 0);
    lv_obj_set_size(ui->camera_img_preview, 320, 240);
    lv_obj_set_style_bg_opa(ui->camera_img_preview, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->camera_img_preview, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui->camera_img_preview, LV_OBJ_FLAG_SCROLLABLE);

    //Write codes camera_btn_close
    ui->camera_btn_close = lv_button_create(ui->camera);
    lv_obj_set_pos(ui->camera_btn_close, 0, 0);
    lv_obj_set_size(ui->camera_btn_close, 50, 50);
    ui->camera_btn_close_label = lv_label_create(ui->camera_btn_close);
    lv_label_set_text(ui->camera_btn_close_label, " " LV_SYMBOL_CLOSE " ");
    lv_label_set_long_mode(ui->camera_btn_close_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->camera_btn_close_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->camera_btn_close, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->camera_btn_close_label, LV_PCT(100));

    //Write style for camera_btn_close, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->camera_btn_close, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->camera_btn_close, lv_color_hex(0x242424), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->camera_btn_close, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->camera_btn_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->camera_btn_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->camera_btn_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->camera_btn_close, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->camera_btn_close, &lv_font_montserratMedium_38, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->camera_btn_close, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->camera_btn_close, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes camera_label_status
    ui->camera_label_status = lv_label_create(ui->camera);
    lv_obj_set_pos(ui->camera_label_status, 60, 12);
    lv_obj_set_size(ui->camera_label_status, 248, 26);
    lv_label_set_text(ui->camera_label_status, "Camera starting...");
    lv_label_set_long_mode(ui->camera_label_status, LV_LABEL_LONG_DOT);

    //Write style for camera_label_status, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->camera_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->camera_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->camera_label_status, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->camera_label_status, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->camera_label_status, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->camera_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->camera_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->camera_label_status, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->camera_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->camera_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->camera_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->camera_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->camera_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->camera_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of camera.


    //Update current screen layout.
    lv_obj_update_layout(ui->camera);

    //Init events for screen.
    events_init_camera(ui);
}
