idf_component_register(SRCS "music_player.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES chmorgan__esp-audio-player esp-libhelix-mp3 bsp_codec lcd event_loop ui_event)
