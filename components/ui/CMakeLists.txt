file(GLOB_RECURSE UI_SRCS
    "${CMAKE_CURRENT_LIST_DIR}/custom/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/generated/*.c"
)

idf_component_register(
    SRCS ${UI_SRCS}
    INCLUDE_DIRS
        "."
        "custom"
        "generated"
        "generated/guider_fonts"
        "generated/images"
    REQUIRES lvgl esp_lvgl_port
    PRIV_REQUIRES sdcard event_loop music_player bsp_codec chmorgan__esp-audio-player wifi_connect ui_event
)
