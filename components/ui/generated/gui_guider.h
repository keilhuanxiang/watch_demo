/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"


typedef struct
{
  
	lv_obj_t *Main;
	bool Main_del;
	lv_obj_t *Main_cont_hour;
	lv_obj_t *Main_img_hour1;
	lv_obj_t *Main_img_hour0;
	lv_obj_t *Main_cont_min;
	lv_obj_t *Main_img_min1;
	lv_obj_t *Main_img_min0;
	lv_obj_t *Main_btn_meau;
	lv_obj_t *Main_btn_meau_label;
	lv_obj_t *Main_cont_top;
	lv_obj_t *Main_label_date;
	lv_obj_t *Main_line_top;
	lv_obj_t *Main_label_week;
	lv_obj_t *Main_label_battery;
	lv_obj_t *Main_label_wifi;
	lv_obj_t *Main_label_music;
	lv_obj_t *meau;
	bool meau_del;
	lv_obj_t *meau_btn_home;
	lv_obj_t *meau_btn_home_label;
	lv_obj_t *meau_label_home;
	lv_obj_t *meau_btn_music;
	lv_obj_t *meau_btn_music_label;
	lv_obj_t *meau_label_music;
	lv_obj_t *meau_btn_wifi;
	lv_obj_t *meau_btn_wifi_label;
	lv_obj_t *meau_label_wifi;
	lv_obj_t *meau_btn_camera;
	lv_obj_t *meau_btn_camera_label;
	lv_obj_t *meau_label_camera;
	lv_obj_t *meau_btn_document;
	lv_obj_t *meau_btn_document_label;
	lv_obj_t *meau_label_document;
	lv_obj_t *meau_btn_setting;
	lv_obj_t *meau_btn_setting_label;
	lv_obj_t *meau_label_setting;
	lv_obj_t *music;
	bool music_del;
	lv_obj_t *music_slider_volume;
	lv_obj_t *music_btn_close;
	lv_obj_t *music_btn_close_label;
	lv_obj_t *music_btn_volume;
	lv_obj_t *music_btn_volume_label;
	lv_obj_t *music_slider_prog;
	lv_obj_t *music_label_play_time;
	lv_obj_t *music_label_total_time;
	lv_obj_t *music_btn_play;
	lv_obj_t *music_btn_play_label;
	lv_obj_t *music_btn_prev;
	lv_obj_t *music_btn_prev_label;
	lv_obj_t *music_btn_next;
	lv_obj_t *music_btn_next_label;
	lv_obj_t *music_btn_list;
	lv_obj_t *music_btn_list_label;
	lv_obj_t *music_label_name;
	lv_obj_t *music_img_player;
	lv_obj_t *music_btn_mode;
	lv_obj_t *music_btn_mode_label;
	lv_obj_t *music_btn_leave;
	lv_obj_t *music_btn_leave_label;
	lv_obj_t *file;
	bool file_del;
	lv_obj_t *file_list_file;
	lv_obj_t *file_list_file_item0;
	lv_obj_t *file_list_file_item1;
	lv_obj_t *file_list_file_item2;
	lv_obj_t *wifi;
	bool wifi_del;
	lv_obj_t *wifi_btn_close;
	lv_obj_t *wifi_btn_close_label;
	lv_obj_t *wifi_label_select_tip;
	lv_obj_t *wifi_cont_switch;
	lv_obj_t *wifi_label_switch_tip;
	lv_obj_t *wifi_sw_wifi;
	lv_obj_t *wifi_cont_connected;
	lv_obj_t *wifi_label_connected_img;
	lv_obj_t *wifi_label_connected_ssid;
	lv_obj_t *wifi_btn_disconnect;
	lv_obj_t *wifi_btn_disconnect_label;
	lv_obj_t *wifi_list_wifi;
	lv_obj_t *wifi_btn_fresh_list;
	lv_obj_t *wifi_btn_fresh_list_label;
	lv_obj_t *wifi_cont_connect;
	lv_obj_t *wifi_label_connect_ssid;
	lv_obj_t *wifi_ta_connect_password;
	lv_obj_t *wifi_btn_connect_confirm;
	lv_obj_t *wifi_btn_connect_confirm_label;
	lv_obj_t *wifi_label_connect_error;
	lv_obj_t *wifi_btn_connect_cancel;
	lv_obj_t *wifi_btn_connect_cancel_label;
	lv_obj_t *wifi_cont_disconnect;
	lv_obj_t *wifi_label_disconnect_tip;
	lv_obj_t *wifi_btn_diconnect_confirm;
	lv_obj_t *wifi_btn_diconnect_confirm_label;
	lv_obj_t *wifi_btn_disconnect_cancel;
	lv_obj_t *wifi_btn_disconnect_cancel_label;
	lv_obj_t *wifi_label_fresh_tip;
	lv_obj_t *camera;
	bool camera_del;
	lv_obj_t *camera_img_preview;
	lv_obj_t *camera_btn_close;
	lv_obj_t *camera_btn_close_label;
	lv_obj_t *camera_label_status;
	lv_obj_t *photo;
	bool photo_del;
	lv_obj_t *photo_img_view;
	lv_obj_t *photo_btn_close;
	lv_obj_t *photo_btn_close_label;
	lv_obj_t *photo_label_name;
	lv_obj_t *setting;
	bool setting_del;
	lv_obj_t *setting_list_1;
	lv_obj_t *setting_list_1_item0;
	lv_obj_t *g_kb_top_layer;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, uint32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                  uint32_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                  lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_completed_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_bottom_layer(void);

void setup_ui(lv_ui *ui);

void video_play(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;


void setup_scr_Main(lv_ui *ui);
void setup_scr_meau(lv_ui *ui);
void setup_scr_music(lv_ui *ui);
void setup_scr_file(lv_ui *ui);
void setup_scr_wifi(lv_ui *ui);
void setup_scr_camera(lv_ui *ui);
void setup_scr_photo(lv_ui *ui);
void setup_scr_setting(lv_ui *ui);
LV_IMAGE_DECLARE(_icon_mini_1_RGB565A8_70x92);
LV_IMAGE_DECLARE(_icon_mini_0_RGB565A8_70x92);
LV_IMAGE_DECLARE(_icon_mini_3_RGB565A8_70x92);
LV_IMAGE_DECLARE(_icon_mini_2_RGB565A8_70x92);
LV_IMAGE_DECLARE(_icon_mini_4_RGB565A8_70x92);
LV_IMAGE_DECLARE(_icon_mini_5_RGB565A8_70x92);
LV_IMAGE_DECLARE(_icon_mini_6_RGB565A8_70x92);
LV_IMAGE_DECLARE(_icon_mini_7_RGB565A8_70x92);
LV_IMAGE_DECLARE(_icon_mini_8_RGB565A8_70x92);
LV_IMAGE_DECLARE(_icon_mini_9_RGB565A8_70x92);
LV_IMAGE_DECLARE(_icon_player_RGB565A8_70x70);

LV_FONT_DECLARE(lv_font_montserratMedium_40)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_18)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_20)
LV_FONT_DECLARE(lv_font_montserratMedium_18)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_23)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_14)
LV_FONT_DECLARE(lv_font_montserratMedium_38)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_16)
LV_FONT_DECLARE(lv_font_montserratMedium_20)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_montserratMedium_25)
LV_FONT_DECLARE(lv_font_montserratMedium_15)


#ifdef __cplusplus
}
#endif
#endif
