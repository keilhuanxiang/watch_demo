idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES Key lcd ui nvs_flash sdcard event_loop ui_event bsp_codec rtc_time wifi_connect camera
                    )
