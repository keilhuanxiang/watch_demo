idf_component_register(
    SRCS "rtc_time.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_event esp_netif event_loop nvs_flash
)
