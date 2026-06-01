idf_component_register(SRCS "bsp_codec.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_driver_i2s lcd esp_codec_dev)
