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

void setup_scr_photo(lv_ui *ui)
{
    ui->photo = lv_obj_create(NULL);
    lv_obj_set_size(ui->photo, 320, 240);
    lv_obj_set_scrollbar_mode(ui->photo, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_style_bg_opa(ui->photo, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->photo, lv_color_hex(0x161616),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->photo, LV_GRAD_DIR_NONE,
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->photo, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->photo, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui->photo_img_view = lv_image_create(ui->photo);
    lv_obj_set_pos(ui->photo_img_view, 0, 40);
    lv_obj_set_size(ui->photo_img_view, 320, 200);
    lv_image_set_inner_align(ui->photo_img_view, LV_IMAGE_ALIGN_CONTAIN);
    lv_obj_set_style_bg_opa(ui->photo_img_view, 0,
                            LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->photo_img_view, 0,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui->photo_img_view, LV_OBJ_FLAG_SCROLLABLE);

    ui->photo_btn_close = lv_button_create(ui->photo);
    lv_obj_set_pos(ui->photo_btn_close, 0, 0);
    lv_obj_set_size(ui->photo_btn_close, 50, 50);
    ui->photo_btn_close_label = lv_label_create(ui->photo_btn_close);
    lv_label_set_text(ui->photo_btn_close_label, " " LV_SYMBOL_CLOSE " ");
    lv_label_set_long_mode(ui->photo_btn_close_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->photo_btn_close_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->photo_btn_close, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->photo_btn_close_label, LV_PCT(100));

    lv_obj_set_style_bg_opa(ui->photo_btn_close, 255,
                            LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->photo_btn_close, lv_color_hex(0x161616),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->photo_btn_close, LV_GRAD_DIR_NONE,
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->photo_btn_close, 0,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->photo_btn_close, 0,
                            LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->photo_btn_close, 0,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->photo_btn_close, lv_color_hex(0xffffff),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->photo_btn_close, &lv_font_montserratMedium_38,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->photo_btn_close, 255,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->photo_btn_close, LV_TEXT_ALIGN_CENTER,
                                LV_PART_MAIN | LV_STATE_DEFAULT);

    ui->photo_label_name = lv_label_create(ui->photo);
    lv_obj_set_pos(ui->photo_label_name, 60, 12);
    lv_obj_set_size(ui->photo_label_name, 248, 22);
    lv_label_set_text(ui->photo_label_name, "Photo preview");
    lv_label_set_long_mode(ui->photo_label_name, LV_LABEL_LONG_DOT);

    lv_obj_set_style_border_width(ui->photo_label_name, 0,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->photo_label_name, 0,
                            LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->photo_label_name, lv_color_hex(0xffffff),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->photo_label_name,
                               &lv_font_SourceHanSerifSC_Regular_16,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->photo_label_name, 255,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->photo_label_name, LV_TEXT_ALIGN_LEFT,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->photo_label_name, 0,
                            LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->photo_label_name, 0,
                             LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->photo_label_name, 0,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_update_layout(ui->photo);
    events_init_photo(ui);
}
