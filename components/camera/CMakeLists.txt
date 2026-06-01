idf_component_register(SRCS "camera.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_image_effects esp_lvgl_port nvs_flash event_loop lcd ui ui_event
                    esp-audio-player wifi_connect
                    esp32-camera)
