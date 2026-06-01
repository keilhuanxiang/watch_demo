idf_component_register(SRCS "lcd.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver esp_lcd esp_lcd_touch_ft5x06 esp_lvgl_port)
