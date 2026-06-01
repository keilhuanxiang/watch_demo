#pragma once

#include "driver/sdmmc_host.h"
#include "esp_event.h"
#include "esp_log.h"
#include "sdmmc_cmd.h"

#define MOUNT_POINT "/sdcard"
#define SD_CMD_PIN GPIO_NUM_48
#define SD_CLK_PIN GPIO_NUM_47
#define SD_DATA0_PIN GPIO_NUM_21
#define SD_PATH_MAX 1024

typedef enum {
    FILE_TYPE_UNKNOWN = 0,
    FILE_TYPE_DIR,
    FILE_TYPE_TXT,
    FILE_TYPE_MP3,
    FILE_TYPE_FLAC,
    FILE_TYPE_BMP,
    FILE_TYPE_JPG,
    FILE_TYPE_PNG,
    FILE_TYPE_WAV
} file_type_t;

typedef enum {
    FILE_FILTER_ALL = 0,
    FILE_FILTER_AUDIO_ONLY,
} file_filter_mode_t;

typedef enum {
    MUSIC_PLAY_MODE_SEQUENCE = 0,
    MUSIC_PLAY_MODE_RANDOM,
    MUSIC_PLAY_MODE_REPEAT_ONE,
    MUSIC_PLAY_MODE_MAX
} music_play_mode_t;

void sdcard_init_mount_fs(void);

typedef struct {
    bool is_directory;
    char name[SD_PATH_MAX / 2];
    char current_path[SD_PATH_MAX];
    char ext_name[16];
    file_filter_mode_t filter_mode;
} file_refresh_req_data_t;

typedef struct {
    char name[SD_PATH_MAX / 2];
    file_type_t type;
} file_item_t;

typedef struct {
    char current_path[SD_PATH_MAX];
    file_item_t *items;
    uint16_t item_count;
} file_list_data_t;

typedef struct {
    char dir_path[SD_PATH_MAX];
    file_item_t *music_list;
    uint16_t music_count;
    uint16_t current_index;
    music_play_mode_t play_mode;
} music_playlist_t;

extern char sd_current_dir[SD_PATH_MAX];
extern music_playlist_t g_playlist;

void playlist_update(const char *dir_path, file_item_t *items, uint16_t item_count);
int playlist_set_current_by_name(const char *name);
int playlist_get_prev(char *out_path, size_t max_len);
int playlist_get_next(char *out_path, size_t max_len);
int playlist_get_prev_manual(char *out_path, size_t max_len);
int playlist_get_next_manual(char *out_path, size_t max_len);
music_play_mode_t playlist_get_play_mode(void);
music_play_mode_t playlist_cycle_play_mode(void);
const char *playlist_get_play_mode_label(void);
