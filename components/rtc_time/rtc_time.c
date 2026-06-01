#include "rtc_time.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "esp_attr.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"

static const char *TAG = "rtc_time";

#define RTC_TIME_MAGIC 0x54494D45UL
#define RTC_TIME_VALID_YEAR 2020
#define RTC_TIME_SYNC_RETRY_COUNT 10
#define RTC_TIME_SYNC_WAIT_MS 2000
#define RTC_TIME_NVS_NAMESPACE "rtc_time"
#define RTC_TIME_NVS_KEY_EPOCH "last_epoch"

RTC_NOINIT_ATTR static uint32_t s_rtc_magic;
RTC_NOINIT_ATTR static uint32_t s_rtc_time_synced;
RTC_NOINIT_ATTR static int64_t s_rtc_cached_epoch;
RTC_NOINIT_ATTR static uint32_t s_rtc_cached_epoch_valid;

static bool s_has_local_time = false;
static bool s_is_time_synced = false;

static bool rtc_time_get_system_local(struct tm *timeinfo);
static bool rtc_time_is_valid(const struct tm *timeinfo);
static void rtc_time_init_rtc_state(void);
static void rtc_time_update_runtime_state(bool synced);
static void rtc_time_save_rtc_cache(time_t epoch, bool synced);
static esp_err_t rtc_time_save_nvs_epoch(time_t epoch);
static bool rtc_time_set_epoch(time_t epoch, bool synced, const char *source);
static bool rtc_time_restore_from_rtc_cache(void);
static bool rtc_time_restore_from_nvs(void);
static bool rtc_time_restore_from_build_time(void);
static int rtc_time_parse_month(const char *month);
static void rtc_time_log_current(const char *prefix);
static void rtc_time_sync_notification_cb(struct timeval *tv);
static void rtc_time_obtain_sntp(void);
static void rtc_time_sntp_request_handler(void *arg, esp_event_base_t event_base,
                                          int32_t event_id, void *event_data);

static bool rtc_time_get_system_local(struct tm *timeinfo)
{
    time_t now;

    if (timeinfo == NULL) {
        return false;
    }

    time(&now);
    localtime_r(&now, timeinfo);
    return rtc_time_is_valid(timeinfo);
}

static bool rtc_time_is_valid(const struct tm *timeinfo)
{
    int year;

    if (timeinfo == NULL) {
        return false;
    }

    year = timeinfo->tm_year + 1900;
    return year >= RTC_TIME_VALID_YEAR;
}

static void rtc_time_init_rtc_state(void)
{
    if (s_rtc_magic != RTC_TIME_MAGIC) {
        s_rtc_magic = RTC_TIME_MAGIC;
        s_rtc_time_synced = 0;
        s_rtc_cached_epoch = 0;
        s_rtc_cached_epoch_valid = 0;
    }
}

static void rtc_time_update_runtime_state(bool synced)
{
    s_has_local_time = true;
    s_is_time_synced = synced;
    s_rtc_time_synced = synced ? 1U : 0U;
}

static void rtc_time_save_rtc_cache(time_t epoch, bool synced)
{
    s_rtc_cached_epoch = (int64_t)epoch;
    s_rtc_cached_epoch_valid = 1U;
    s_rtc_time_synced = synced ? 1U : 0U;
}

static esp_err_t rtc_time_save_nvs_epoch(time_t epoch)
{
    esp_err_t ret;
    nvs_handle_t nvs_handle;

    ret = nvs_open(RTC_TIME_NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "nvs_open failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = nvs_set_i64(nvs_handle, RTC_TIME_NVS_KEY_EPOCH, (int64_t)epoch);
    if (ret == ESP_OK) {
        ret = nvs_commit(nvs_handle);
    }

    nvs_close(nvs_handle);

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "save nvs epoch failed: %s", esp_err_to_name(ret));
    }

    return ret;
}

static bool rtc_time_set_epoch(time_t epoch, bool synced, const char *source)
{
    struct timeval tv = {
        .tv_sec = epoch,
        .tv_usec = 0,
    };
    struct tm timeinfo;

    if (epoch <= 0) {
        return false;
    }

    if (settimeofday(&tv, NULL) != 0) {
        ESP_LOGE(TAG, "settimeofday failed for %s", source);
        return false;
    }

    if (!rtc_time_get_system_local(&timeinfo)) {
        ESP_LOGW(TAG, "time from %s is not considered valid", source);
        return false;
    }

    rtc_time_update_runtime_state(synced);
    rtc_time_save_rtc_cache(epoch, synced);
    ESP_LOGI(TAG, "time restored from %s", source);
    rtc_time_log_current("current local time");
    return true;
}

static bool rtc_time_restore_from_rtc_cache(void)
{
    if (s_rtc_cached_epoch_valid == 0U || s_rtc_cached_epoch <= 0) {
        return false;
    }

    return rtc_time_set_epoch((time_t)s_rtc_cached_epoch, false, "rtc cache");
}

static bool rtc_time_restore_from_nvs(void)
{
    esp_err_t ret;
    int64_t saved_epoch = 0;
    nvs_handle_t nvs_handle;

    ret = nvs_open(RTC_TIME_NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (ret != ESP_OK) {
        return false;
    }

    ret = nvs_get_i64(nvs_handle, RTC_TIME_NVS_KEY_EPOCH, &saved_epoch);
    nvs_close(nvs_handle);
    if (ret != ESP_OK || saved_epoch <= 0) {
        return false;
    }

    return rtc_time_set_epoch((time_t)saved_epoch, false, "nvs cache");
}

static int rtc_time_parse_month(const char *month)
{
    static const char *const month_names[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };

    for (int i = 0; i < 12; i++) {
        if (strncmp(month, month_names[i], 3) == 0) {
            return i;
        }
    }

    return -1;
}

static bool rtc_time_restore_from_build_time(void)
{
    char month_name[4] = {0};
    int day = 0;
    int year = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;
    int month = -1;
    struct tm build_tm = {0};
    time_t build_epoch;

    if (sscanf(__DATE__, "%3s %d %d", month_name, &day, &year) != 3) {
        return false;
    }
    if (sscanf(__TIME__, "%d:%d:%d", &hour, &min, &sec) != 3) {
        return false;
    }

    month = rtc_time_parse_month(month_name);
    if (month < 0) {
        return false;
    }

    build_tm.tm_year = year - 1900;
    build_tm.tm_mon = month;
    build_tm.tm_mday = day;
    build_tm.tm_hour = hour;
    build_tm.tm_min = min;
    build_tm.tm_sec = sec;
    build_tm.tm_isdst = -1;

    build_epoch = mktime(&build_tm);
    if (build_epoch <= 0) {
        return false;
    }

    return rtc_time_set_epoch(build_epoch, false, "build time");
}

static void rtc_time_log_current(const char *prefix)
{
    struct tm timeinfo;
    char time_buf[32];

    if (!rtc_time_get_system_local(&timeinfo)) {
        ESP_LOGW(TAG, "%s unavailable", prefix);
        return;
    }

    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    ESP_LOGI(TAG, "%s: %s", prefix, time_buf);
}

static void rtc_time_sync_notification_cb(struct timeval *tv)
{
    (void)tv;
    ESP_LOGI(TAG, "sntp sync callback received");
}

static void rtc_time_obtain_sntp(void)
{
    esp_err_t ret;
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");

    config.sync_cb = rtc_time_sync_notification_cb;

    ret = esp_netif_sntp_init(&config);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "esp_netif_sntp_init failed: %s", esp_err_to_name(ret));
        return;
    }

    for (int retry = 0; retry < RTC_TIME_SYNC_RETRY_COUNT; retry++) {
        ret = esp_netif_sntp_sync_wait(pdMS_TO_TICKS(RTC_TIME_SYNC_WAIT_MS));
        if (ret == ESP_OK) {
            time_t now;

            time(&now);
            rtc_time_update_runtime_state(true);
            rtc_time_save_rtc_cache(now, true);
            rtc_time_save_nvs_epoch(now);
            rtc_time_log_current("sntp synced time");
            break;
        }

        ESP_LOGI(TAG, "waiting for sntp sync... (%d/%d)", retry + 1,
                 RTC_TIME_SYNC_RETRY_COUNT);
    }

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "sntp sync timeout, keep local running time");
    }

    esp_netif_sntp_deinit();
}

static void rtc_time_sntp_request_handler(void *arg, esp_event_base_t event_base,
                                          int32_t event_id, void *event_data)
{
    (void)arg;
    (void)event_base;
    (void)event_id;
    (void)event_data;

    if (s_is_time_synced) {
        ESP_LOGI(TAG, "time already synced, skip sntp request");
        return;
    }

    ESP_LOGI(TAG, "received sntp request, start network sync");
    rtc_time_obtain_sntp();
}

void rtc_time_init(void)
{
    struct tm timeinfo;

    rtc_time_init_rtc_state();

    setenv("TZ", "CST-8", 1);
    tzset();

    s_has_local_time = false;
    s_is_time_synced = false;

    if (rtc_time_get_system_local(&timeinfo)) {
        s_has_local_time = true;
        s_is_time_synced = (s_rtc_time_synced != 0U);
        time_t now;

        time(&now);
        rtc_time_save_rtc_cache(now, s_is_time_synced);
        rtc_time_log_current("boot local time");
    } else if (!rtc_time_restore_from_rtc_cache() &&
               !rtc_time_restore_from_nvs() &&
               !rtc_time_restore_from_build_time()) {
        ESP_LOGW(TAG, "no local time source available yet");
    }

    ESP_ERROR_CHECK(esp_event_handler_register_with(
        ui_event_loop_handle, APP_EVENT, APP_SNTP_REQUEST,
        rtc_time_sntp_request_handler, NULL));
}

bool rtc_time_get_local(struct tm *timeinfo)
{
    time_t now;

    if (!rtc_time_get_system_local(timeinfo)) {
        s_has_local_time = false;
        return false;
    }

    time(&now);
    rtc_time_save_rtc_cache(now, s_is_time_synced);
    s_has_local_time = true;
    return true;
}
