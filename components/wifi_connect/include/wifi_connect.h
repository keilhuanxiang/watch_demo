#pragma once


#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_wifi.h"




// WiFi Status Enumeration
typedef enum {
    WIFI_STATE_CLOSED=0x00,     // Close: All resources have been deinitialized
    WIFI_STATE_CLOSING,         // Closing,  no API calls can be made at this time.
    WIFI_STATE_INITING,         // Initializing:  esp_wifi_start() has been called, but the START event has not been received yet.
    WIFI_STATE_IDLE,            // Idle:  Initialization complete, ready to call scan/connect functions.
    WIFI_STATE_BUSY,            // Busy:  currently scanning or connecting
    WIFI_STATE_CONNECTED,       // Successfully connected to WiFi
    WIFI_STATE_GETIP,           // Successfully obtained IP address
} wifi_state_t;

typedef struct {
    uint16_t ap_count;
    wifi_ap_record_t *ap_records;
} wifi_scan_done_data_t;

typedef struct {
    char ssid[64];
    char password[64];
} wifi_connect_data_t;

typedef struct {
    wifi_err_reason_t reason;
    uint8_t requested_by_user;
} wifi_disconnect_event_data_t;

extern  wifi_state_t  current_wifi_state;

extern SemaphoreHandle_t wifi_mutex ;         

void wifi_init(void);



