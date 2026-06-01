#pragma once

#include <stdbool.h>

#include "lvgl.h"
#include "wifi_connect.h"

#ifdef __cplusplus
extern "C" {
#endif

void wifi_ui_logic_on_screen_enter(void);
void wifi_ui_logic_on_switch_toggle(bool enabled);
void wifi_ui_logic_on_refresh_request(void);
void wifi_ui_logic_on_ap_selected(const char *ssid);
void wifi_ui_logic_on_connect_confirm(void);
void wifi_ui_logic_on_connect_cancel(void);
void wifi_ui_logic_on_disconnect_prompt(void);
void wifi_ui_logic_on_disconnect_confirm(void);
void wifi_ui_logic_on_disconnect_cancel(void);

void wifi_ui_logic_on_scan_done(const wifi_scan_done_data_t *scan_data);
void wifi_ui_logic_on_disconnected(const wifi_disconnect_event_data_t *disconnect_data);
void wifi_ui_logic_on_got_ip(const char *ip_str);
void wifi_ui_logic_on_closed(void);

#ifdef __cplusplus
}
#endif
