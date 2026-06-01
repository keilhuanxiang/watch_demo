#include "wifi_ui_logic.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "esp_event.h"
#include "event_loop.h"
#include "freertos/FreeRTOS.h"
#include "gui_guider.h"

#define WIFI_UI_MAX_APS 3
#define WIFI_UI_SSID_MAX_LEN 33
#define WIFI_UI_IP_MAX_LEN 16

typedef struct {
    bool wifi_enabled;
    bool scan_in_progress;
    bool connect_in_progress;
    bool disconnect_in_progress;
    bool close_in_progress;
    bool connect_dialog_visible;
    bool disconnect_dialog_visible;
    bool connected;
    uint8_t ap_count;
    char ap_ssids[WIFI_UI_MAX_APS][WIFI_UI_SSID_MAX_LEN];
    char selected_ssid[WIFI_UI_SSID_MAX_LEN];
    char connected_ssid[WIFI_UI_SSID_MAX_LEN];
    char connected_ip[WIFI_UI_IP_MAX_LEN];
    char tip_text[32];
    char error_text[48];
} wifi_ui_state_t;

static wifi_ui_state_t s_wifi_ui_state = {0};
static lv_obj_t *s_wifi_ap_buttons[WIFI_UI_MAX_APS] = {0};

static bool wifi_ui_screen_ready(void);
static void wifi_ui_ensure_ap_buttons(void);
static void wifi_ui_set_ap_button_icon(lv_obj_t *btn, const void *icon);
static void wifi_ui_set_hidden(lv_obj_t *obj, bool hidden);
static void wifi_ui_set_enabled(lv_obj_t *obj, bool enabled);
static void wifi_ui_set_tip(const char *text);
static void wifi_ui_clear_error(void);
static void wifi_ui_clear_scan_list(void);
static void wifi_ui_rebuild_ap_list(void);
static void wifi_ui_render(void);
static bool wifi_ui_is_auth_error(wifi_err_reason_t reason);
static void wifi_ui_ap_click_event_cb(lv_event_t *e);

static bool wifi_ui_screen_ready(void)
{
    return guider_ui.wifi != NULL && lv_obj_is_valid(guider_ui.wifi);
}

static void wifi_ui_set_hidden(lv_obj_t *obj, bool hidden)
{
    if (obj == NULL || !lv_obj_is_valid(obj)) {
        return;
    }

    if (hidden) {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static void wifi_ui_set_enabled(lv_obj_t *obj, bool enabled)
{
    if (obj == NULL || !lv_obj_is_valid(obj)) {
        return;
    }

    if (enabled) {
        lv_obj_remove_state(obj, LV_STATE_DISABLED);
    } else {
        lv_obj_add_state(obj, LV_STATE_DISABLED);
    }
}

static void wifi_ui_set_tip(const char *text)
{
    if (text == NULL) {
        s_wifi_ui_state.tip_text[0] = '\0';
        return;
    }

    snprintf(s_wifi_ui_state.tip_text, sizeof(s_wifi_ui_state.tip_text), "%s", text);
}

static void wifi_ui_clear_error(void)
{
    s_wifi_ui_state.error_text[0] = '\0';
}

static void wifi_ui_clear_scan_list(void)
{
    memset(s_wifi_ui_state.ap_ssids, 0, sizeof(s_wifi_ui_state.ap_ssids));
    s_wifi_ui_state.ap_count = 0;
}

static void wifi_ui_ensure_ap_buttons(void)
{
    bool need_create = false;

    if (guider_ui.wifi_list_wifi == NULL || !lv_obj_is_valid(guider_ui.wifi_list_wifi)) {
        return;
    }

    for (uint8_t i = 0; i < WIFI_UI_MAX_APS; ++i) {
        if (s_wifi_ap_buttons[i] == NULL || !lv_obj_is_valid(s_wifi_ap_buttons[i])) {
            need_create = true;
            break;
        }
    }

    if (!need_create) {
        return;
    }

    for (uint8_t i = 0; i < WIFI_UI_MAX_APS; ++i) {
        lv_obj_t *btn;

        s_wifi_ap_buttons[i] = NULL;
        btn = lv_list_add_button(guider_ui.wifi_list_wifi, LV_SYMBOL_WIFI, "");
        if (btn == NULL) {
            continue;
        }

        lv_obj_set_style_bg_color(btn, lv_color_hex(0x424242), LV_PART_MAIN);
        lv_obj_set_style_border_color(btn, lv_color_hex(0x424242), LV_PART_MAIN);
        lv_obj_set_style_text_color(btn, lv_color_hex(0xffffff), LV_PART_MAIN);
        lv_obj_add_event_cb(btn, wifi_ui_ap_click_event_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
        s_wifi_ap_buttons[i] = btn;
    }
}

static void wifi_ui_set_ap_button_icon(lv_obj_t *btn, const void *icon)
{
    lv_obj_t *img;

    if (btn == NULL || !lv_obj_is_valid(btn) || lv_obj_get_child_count(btn) == 0) {
        return;
    }

    img = lv_obj_get_child(btn, 0);
    if (img != NULL && lv_obj_is_valid(img)) {
        lv_image_set_src(img, icon);
    }
}

static bool wifi_ui_is_auth_error(wifi_err_reason_t reason)
{
    switch (reason) {
    case WIFI_REASON_AUTH_EXPIRE:
    case WIFI_REASON_AUTH_LEAVE:
    case WIFI_REASON_AUTH_FAIL:
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
    case WIFI_REASON_HANDSHAKE_TIMEOUT:
    case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT:
    case WIFI_REASON_IE_IN_4WAY_DIFFERS:
    case WIFI_REASON_MIC_FAILURE:
        return true;
    default:
        return false;
    }
}

static void wifi_ui_rebuild_ap_list(void)
{
    bool buttons_enabled;

    if (guider_ui.wifi_list_wifi == NULL || !lv_obj_is_valid(guider_ui.wifi_list_wifi)) {
        return;
    }

    wifi_ui_ensure_ap_buttons();

    buttons_enabled = !(s_wifi_ui_state.scan_in_progress ||
                        s_wifi_ui_state.connected ||
                        s_wifi_ui_state.connect_in_progress ||
                        s_wifi_ui_state.close_in_progress ||
                        s_wifi_ui_state.connect_dialog_visible ||
                        s_wifi_ui_state.disconnect_dialog_visible ||
                        s_wifi_ui_state.disconnect_in_progress);

    if (!s_wifi_ui_state.wifi_enabled || s_wifi_ui_state.close_in_progress) {
        for (uint8_t i = 0; i < WIFI_UI_MAX_APS; ++i) {
            wifi_ui_set_hidden(s_wifi_ap_buttons[i], true);
        }
        return;
    }

    if (s_wifi_ui_state.ap_count == 0 && !s_wifi_ui_state.scan_in_progress) {
        if (s_wifi_ap_buttons[0] != NULL && lv_obj_is_valid(s_wifi_ap_buttons[0])) {
            wifi_ui_set_ap_button_icon(s_wifi_ap_buttons[0], LV_SYMBOL_CLOSE);
            lv_list_set_button_text(guider_ui.wifi_list_wifi, s_wifi_ap_buttons[0],
                                    "No networks");
            wifi_ui_set_enabled(s_wifi_ap_buttons[0], false);
            wifi_ui_set_hidden(s_wifi_ap_buttons[0], false);
        }

        for (uint8_t i = 1; i < WIFI_UI_MAX_APS; ++i) {
            wifi_ui_set_hidden(s_wifi_ap_buttons[i], true);
        }
        return;
    }

    for (uint8_t i = 0; i < WIFI_UI_MAX_APS; ++i) {
        if (s_wifi_ap_buttons[i] == NULL || !lv_obj_is_valid(s_wifi_ap_buttons[i])) {
            continue;
        }

        if (i < s_wifi_ui_state.ap_count) {
            wifi_ui_set_ap_button_icon(s_wifi_ap_buttons[i], LV_SYMBOL_WIFI);
            lv_list_set_button_text(guider_ui.wifi_list_wifi, s_wifi_ap_buttons[i],
                                    s_wifi_ui_state.ap_ssids[i]);
            wifi_ui_set_enabled(s_wifi_ap_buttons[i], buttons_enabled);
            wifi_ui_set_hidden(s_wifi_ap_buttons[i], false);
        } else {
            wifi_ui_set_hidden(s_wifi_ap_buttons[i], true);
        }
    }
}

static void wifi_ui_render(void)
{
    bool show_connect_dialog;
    bool show_disconnect_dialog;
    bool show_connected_card;
    bool show_list;

    if (!wifi_ui_screen_ready()) {
        return;
    }

    show_connect_dialog = s_wifi_ui_state.wifi_enabled &&
                          s_wifi_ui_state.connect_dialog_visible &&
                          !s_wifi_ui_state.close_in_progress;
    show_disconnect_dialog = s_wifi_ui_state.wifi_enabled &&
                             s_wifi_ui_state.disconnect_dialog_visible &&
                             !s_wifi_ui_state.close_in_progress;
    show_connected_card = s_wifi_ui_state.wifi_enabled && s_wifi_ui_state.connected &&
                          !show_connect_dialog && !show_disconnect_dialog &&
                          !s_wifi_ui_state.close_in_progress;
    show_list = s_wifi_ui_state.wifi_enabled && !s_wifi_ui_state.connected &&
                !show_connect_dialog && !show_disconnect_dialog &&
                !s_wifi_ui_state.close_in_progress;

    if (s_wifi_ui_state.wifi_enabled && !s_wifi_ui_state.close_in_progress) {
        lv_obj_add_state(guider_ui.wifi_sw_wifi, LV_STATE_CHECKED);
    } else {
        lv_obj_remove_state(guider_ui.wifi_sw_wifi, LV_STATE_CHECKED);
    }

    wifi_ui_set_enabled(guider_ui.wifi_sw_wifi, !s_wifi_ui_state.close_in_progress);
    wifi_ui_set_hidden(guider_ui.wifi_label_select_tip,
                       !show_list || s_wifi_ui_state.connected);
    wifi_ui_set_hidden(guider_ui.wifi_btn_fresh_list,
                       !show_list || s_wifi_ui_state.connected);
    wifi_ui_set_enabled(guider_ui.wifi_btn_fresh_list,
                        !s_wifi_ui_state.scan_in_progress &&
                        !s_wifi_ui_state.connect_in_progress &&
                        !s_wifi_ui_state.disconnect_in_progress &&
                        !s_wifi_ui_state.close_in_progress);
    wifi_ui_set_hidden(guider_ui.wifi_cont_connected, !show_connected_card);
    wifi_ui_set_hidden(guider_ui.wifi_cont_connect, !show_connect_dialog);
    wifi_ui_set_hidden(guider_ui.wifi_cont_disconnect, !show_disconnect_dialog);
    wifi_ui_set_hidden(guider_ui.wifi_list_wifi, !show_list);
    wifi_ui_set_hidden(guider_ui.wifi_label_fresh_tip, s_wifi_ui_state.tip_text[0] == '\0');
    wifi_ui_set_hidden(guider_ui.wifi_label_connect_error,
                       !show_connect_dialog || s_wifi_ui_state.error_text[0] == '\0');

    if (guider_ui.wifi_label_fresh_tip != NULL &&
        lv_obj_is_valid(guider_ui.wifi_label_fresh_tip)) {
        lv_label_set_text(guider_ui.wifi_label_fresh_tip,
                          s_wifi_ui_state.tip_text[0] == '\0' ? "" : s_wifi_ui_state.tip_text);
    }

    if (guider_ui.wifi_label_connected_ssid != NULL &&
        lv_obj_is_valid(guider_ui.wifi_label_connected_ssid)) {
        lv_label_set_text(guider_ui.wifi_label_connected_ssid,
                          s_wifi_ui_state.connected_ssid[0] == '\0'
                              ? "Connected"
                              : s_wifi_ui_state.connected_ssid);
    }

    if (guider_ui.wifi_label_connect_ssid != NULL &&
        lv_obj_is_valid(guider_ui.wifi_label_connect_ssid)) {
        lv_label_set_text(guider_ui.wifi_label_connect_ssid,
                          s_wifi_ui_state.selected_ssid[0] == '\0'
                              ? "Select Wi-Fi"
                              : s_wifi_ui_state.selected_ssid);
    }

    if (guider_ui.wifi_label_connect_error != NULL &&
        lv_obj_is_valid(guider_ui.wifi_label_connect_error)) {
        lv_label_set_text(guider_ui.wifi_label_connect_error,
                          s_wifi_ui_state.error_text[0] == '\0'
                              ? ""
                              : s_wifi_ui_state.error_text);
    }

    if (guider_ui.wifi_ta_connect_password != NULL &&
        lv_obj_is_valid(guider_ui.wifi_ta_connect_password)) {
        lv_textarea_set_password_mode(guider_ui.wifi_ta_connect_password, true);
        wifi_ui_set_enabled(guider_ui.wifi_ta_connect_password,
                            !s_wifi_ui_state.connect_in_progress);
    }

    wifi_ui_set_enabled(guider_ui.wifi_btn_connect_confirm,
                        !s_wifi_ui_state.connect_in_progress);
    wifi_ui_set_enabled(guider_ui.wifi_btn_connect_cancel,
                        !s_wifi_ui_state.connect_in_progress);
    wifi_ui_set_enabled(guider_ui.wifi_btn_disconnect,
                        !s_wifi_ui_state.close_in_progress &&
                        !s_wifi_ui_state.disconnect_in_progress);
    wifi_ui_set_enabled(guider_ui.wifi_btn_diconnect_confirm,
                        !s_wifi_ui_state.close_in_progress &&
                        !s_wifi_ui_state.disconnect_in_progress);
    wifi_ui_set_enabled(guider_ui.wifi_btn_disconnect_cancel,
                        !s_wifi_ui_state.close_in_progress &&
                        !s_wifi_ui_state.disconnect_in_progress);

    wifi_ui_rebuild_ap_list();
}

void wifi_ui_logic_on_screen_enter(void)
{
    if (current_wifi_state != WIFI_STATE_CLOSED &&
        current_wifi_state != WIFI_STATE_CLOSING) {
        s_wifi_ui_state.wifi_enabled = true;
    }

    if (current_wifi_state == WIFI_STATE_CLOSING) {
        s_wifi_ui_state.close_in_progress = true;
        s_wifi_ui_state.wifi_enabled = false;
        wifi_ui_set_tip("Turning off...");
    }

    if (current_wifi_state == WIFI_STATE_INITING ||
        (current_wifi_state == WIFI_STATE_BUSY && !s_wifi_ui_state.connect_in_progress &&
         !s_wifi_ui_state.connected)) {
        s_wifi_ui_state.wifi_enabled = true;
        s_wifi_ui_state.scan_in_progress = true;
        wifi_ui_set_tip("Searching...");
    }

    wifi_ui_render();
}

void wifi_ui_logic_on_switch_toggle(bool enabled)
{
    s_wifi_ui_state.connect_dialog_visible = false;
    s_wifi_ui_state.disconnect_dialog_visible = false;
    wifi_ui_clear_error();

    if (enabled) {
        s_wifi_ui_state.wifi_enabled = true;
        s_wifi_ui_state.scan_in_progress = true;
        s_wifi_ui_state.connect_in_progress = false;
        s_wifi_ui_state.disconnect_in_progress = false;
        s_wifi_ui_state.close_in_progress = false;
        s_wifi_ui_state.connected = false;
        s_wifi_ui_state.connected_ssid[0] = '\0';
        s_wifi_ui_state.connected_ip[0] = '\0';
        wifi_ui_set_tip("Searching...");
        esp_event_post_to(ui_event_loop_handle, APP_EVENT, APP_WIFI_SCAN_START, NULL, 0,
                          portMAX_DELAY);
    } else {
        s_wifi_ui_state.scan_in_progress = false;
        s_wifi_ui_state.connect_in_progress = false;
        s_wifi_ui_state.disconnect_in_progress = false;
        s_wifi_ui_state.close_in_progress = true;
        wifi_ui_set_tip("Turning off...");
        esp_event_post_to(ui_event_loop_handle, APP_EVENT, APP_WIFI_CLOSE, NULL, 0,
                          portMAX_DELAY);
    }

    wifi_ui_render();
}

void wifi_ui_logic_on_refresh_request(void)
{
    if (!s_wifi_ui_state.wifi_enabled || s_wifi_ui_state.connected ||
        s_wifi_ui_state.connect_in_progress || s_wifi_ui_state.disconnect_in_progress ||
        s_wifi_ui_state.close_in_progress) {
        return;
    }

    s_wifi_ui_state.scan_in_progress = true;
    wifi_ui_set_tip("Searching...");
    wifi_ui_clear_error();
    esp_event_post_to(ui_event_loop_handle, APP_EVENT, APP_WIFI_SCAN_START, NULL, 0,
                      portMAX_DELAY);
    wifi_ui_render();
}

void wifi_ui_logic_on_ap_selected(const char *ssid)
{
    if (ssid == NULL || ssid[0] == '\0') {
        return;
    }

    snprintf(s_wifi_ui_state.selected_ssid, sizeof(s_wifi_ui_state.selected_ssid), "%s",
             ssid);
    s_wifi_ui_state.connect_dialog_visible = true;
    s_wifi_ui_state.disconnect_dialog_visible = false;
    s_wifi_ui_state.connect_in_progress = false;
    s_wifi_ui_state.disconnect_in_progress = false;
    wifi_ui_clear_error();

    if (guider_ui.wifi_ta_connect_password != NULL &&
        lv_obj_is_valid(guider_ui.wifi_ta_connect_password)) {
        lv_textarea_set_text(guider_ui.wifi_ta_connect_password, "");
    }

    wifi_ui_render();
}

static void wifi_ui_ap_click_event_cb(lv_event_t *e)
{
    lv_obj_t *btn;
    const char *ssid;

    if (lv_event_get_code(e) != LV_EVENT_CLICKED || !wifi_ui_screen_ready()) {
        return;
    }

    btn = lv_event_get_target(e);
    ssid = lv_list_get_button_text(guider_ui.wifi_list_wifi, btn);
    wifi_ui_logic_on_ap_selected(ssid);
}

void wifi_ui_logic_on_connect_confirm(void)
{
    wifi_connect_data_t connect_data = {0};
    const char *password;

    if (!wifi_ui_screen_ready() || s_wifi_ui_state.selected_ssid[0] == '\0' ||
        guider_ui.wifi_ta_connect_password == NULL ||
        !lv_obj_is_valid(guider_ui.wifi_ta_connect_password)) {
        return;
    }

    password = lv_textarea_get_text(guider_ui.wifi_ta_connect_password);
    if (password == NULL) {
        password = "";
    }

    snprintf(connect_data.ssid, sizeof(connect_data.ssid), "%s",
             s_wifi_ui_state.selected_ssid);
    snprintf(connect_data.password, sizeof(connect_data.password), "%s", password);

    s_wifi_ui_state.scan_in_progress = false;
    s_wifi_ui_state.connect_in_progress = true;
    s_wifi_ui_state.disconnect_in_progress = false;
    s_wifi_ui_state.connected = false;
    s_wifi_ui_state.connected_ssid[0] = '\0';
    s_wifi_ui_state.connected_ip[0] = '\0';
    wifi_ui_clear_error();
    wifi_ui_set_tip("Connecting...");

    esp_event_post_to(ui_event_loop_handle, APP_EVENT, APP_WIFI_CONNECT_REQUEST,
                      &connect_data, sizeof(connect_data), portMAX_DELAY);
    wifi_ui_render();
}

void wifi_ui_logic_on_connect_cancel(void)
{
    s_wifi_ui_state.connect_dialog_visible = false;
    s_wifi_ui_state.connect_in_progress = false;
    s_wifi_ui_state.disconnect_in_progress = false;
    wifi_ui_clear_error();
    wifi_ui_set_tip("");
    wifi_ui_render();
}

void wifi_ui_logic_on_disconnect_prompt(void)
{
    if (!s_wifi_ui_state.connected || s_wifi_ui_state.close_in_progress) {
        return;
    }

    s_wifi_ui_state.disconnect_dialog_visible = true;
    s_wifi_ui_state.connect_dialog_visible = false;
    wifi_ui_render();
}

void wifi_ui_logic_on_disconnect_confirm(void)
{
    s_wifi_ui_state.disconnect_dialog_visible = false;
    s_wifi_ui_state.connect_dialog_visible = false;
    s_wifi_ui_state.connect_in_progress = false;
    s_wifi_ui_state.disconnect_in_progress = true;
    wifi_ui_set_tip("Disconnecting...");

    esp_event_post_to(ui_event_loop_handle, APP_EVENT, APP_WIFI_DISCONNECT_REQUEST, NULL,
                      0, portMAX_DELAY);
    wifi_ui_render();
}

void wifi_ui_logic_on_disconnect_cancel(void)
{
    s_wifi_ui_state.disconnect_dialog_visible = false;
    wifi_ui_render();
}

void wifi_ui_logic_on_scan_done(const wifi_scan_done_data_t *scan_data)
{
    uint8_t stored = 0;

    s_wifi_ui_state.scan_in_progress = false;
    wifi_ui_set_tip("");
    wifi_ui_clear_scan_list();

    if (scan_data != NULL && scan_data->ap_records != NULL) {
        for (uint16_t i = 0; i < scan_data->ap_count && stored < WIFI_UI_MAX_APS; ++i) {
            const char *ssid = (const char *)scan_data->ap_records[i].ssid;
            if (ssid == NULL || ssid[0] == '\0') {
                continue;
            }

            snprintf(s_wifi_ui_state.ap_ssids[stored],
                     sizeof(s_wifi_ui_state.ap_ssids[stored]), "%s", ssid);
            stored++;
        }
    }

    s_wifi_ui_state.ap_count = stored;
    wifi_ui_render();
}

void wifi_ui_logic_on_disconnected(const wifi_disconnect_event_data_t *disconnect_data)
{
    s_wifi_ui_state.scan_in_progress = false;
    s_wifi_ui_state.connect_in_progress = false;
    s_wifi_ui_state.disconnect_in_progress = false;
    s_wifi_ui_state.connected = false;
    s_wifi_ui_state.disconnect_dialog_visible = false;
    s_wifi_ui_state.connected_ssid[0] = '\0';
    s_wifi_ui_state.connected_ip[0] = '\0';
    wifi_ui_set_tip("");

    if (disconnect_data != NULL && disconnect_data->requested_by_user) {
        wifi_ui_clear_error();
    } else if (disconnect_data != NULL && s_wifi_ui_state.connect_dialog_visible) {
        if (wifi_ui_is_auth_error(disconnect_data->reason)) {
            snprintf(s_wifi_ui_state.error_text, sizeof(s_wifi_ui_state.error_text),
                     "密码错误,请重试");
        } else {
            snprintf(s_wifi_ui_state.error_text, sizeof(s_wifi_ui_state.error_text),
                     "连接失败");
        }
    } else if (disconnect_data != NULL) {
        snprintf(s_wifi_ui_state.error_text, sizeof(s_wifi_ui_state.error_text),
                 "断开连接");
        s_wifi_ui_state.connect_dialog_visible = true;
    }

    wifi_ui_render();
}

void wifi_ui_logic_on_got_ip(const char *ip_str)
{
    s_wifi_ui_state.wifi_enabled = true;
    s_wifi_ui_state.scan_in_progress = false;
    s_wifi_ui_state.connect_in_progress = false;
    s_wifi_ui_state.disconnect_in_progress = false;
    s_wifi_ui_state.close_in_progress = false;
    s_wifi_ui_state.connected = true;
    s_wifi_ui_state.connect_dialog_visible = false;
    s_wifi_ui_state.disconnect_dialog_visible = false;
    wifi_ui_clear_error();
    wifi_ui_set_tip("");

    snprintf(s_wifi_ui_state.connected_ssid, sizeof(s_wifi_ui_state.connected_ssid), "%s",
             s_wifi_ui_state.selected_ssid);
    if (ip_str != NULL) {
        snprintf(s_wifi_ui_state.connected_ip, sizeof(s_wifi_ui_state.connected_ip), "%s",
                 ip_str);
    } else {
        s_wifi_ui_state.connected_ip[0] = '\0';
    }

    wifi_ui_render();
}

void wifi_ui_logic_on_closed(void)
{
    memset(&s_wifi_ui_state, 0, sizeof(s_wifi_ui_state));
    wifi_ui_render();
}
