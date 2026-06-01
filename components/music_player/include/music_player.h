#pragma once

#include <stdint.h>

extern uint8_t g_sys_volume;

void mp3_player_init(void);
uint8_t get_sys_volume(void);
void change_sys_volume(uint8_t volume);
void music_player_refresh_mode_ui(void);
void music_player_cycle_mode(void);
void music_player_on_screen_enter(void);
void music_player_on_screen_exit(void);
void music_player_reset_session(void);
void music_player_close(void);