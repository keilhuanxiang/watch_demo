idf_component_register(SRCS "ui_event.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_event esp_lvgl_port sdcard event_loop rtc_time ui music_player wifi_connect)
