#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "esp_event.h"
#include "esp_heap_caps.h"
#include "esp_random.h"
#include "esp_vfs_fat.h"
#include "event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdcard.h"
#include "sdmmc_cmd.h"
#include "ui_event.h"

static const char *TAG = "sdcard";

static bool sd_mount_flag = false;
static sdmmc_card_t *card = NULL;

char sd_current_dir[SD_PATH_MAX] = "/sdcard/";
music_playlist_t g_playlist = {
    .play_mode = MUSIC_PLAY_MODE_SEQUENCE,
};

static void user_sd_refresh_handler(void *arg, esp_event_base_t event_base,
                                    int32_t event_id, void *event_data);

static file_type_t get_file_type(const char *filename)
{
    const char *ext = strrchr(filename, '.');
    if (ext == NULL) {
        return FILE_TYPE_UNKNOWN;
    }

    if (strcasecmp(ext, ".txt") == 0) {
        return FILE_TYPE_TXT;
    }
    if (strcasecmp(ext, ".mp3") == 0) {
        return FILE_TYPE_MP3;
    }
    if (strcasecmp(ext, ".flac") == 0) {
        return FILE_TYPE_FLAC;
    }
    if (strcasecmp(ext, ".bmp") == 0) {
        return FILE_TYPE_BMP;
    }
    if (strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0) {
        return FILE_TYPE_JPG;
    }
    if (strcasecmp(ext, ".png") == 0) {
        return FILE_TYPE_PNG;
    }
    if (strcasecmp(ext, ".wav") == 0) {
        return FILE_TYPE_WAV;
    }

    return FILE_TYPE_UNKNOWN;
}

static bool file_type_matches_filter(file_type_t type, file_filter_mode_t filter_mode)
{
    if (type == FILE_TYPE_DIR) {
        return true;
    }

    switch (filter_mode) {
    case FILE_FILTER_AUDIO_ONLY:
        return type == FILE_TYPE_MP3 || type == FILE_TYPE_FLAC || type == FILE_TYPE_WAV;
    case FILE_FILTER_ALL:
    default:
        return type == FILE_TYPE_TXT || type == FILE_TYPE_MP3 ||
               type == FILE_TYPE_FLAC || type == FILE_TYPE_BMP ||
               type == FILE_TYPE_JPG || type == FILE_TYPE_PNG ||
               type == FILE_TYPE_WAV;
    }
}

void sdcard_init_mount_fs(void)
{
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };

    ESP_LOGI(TAG, "Initializing SD card");
    ESP_LOGI(TAG, "Using SDMMC peripheral");

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = 1;
    slot_config.clk = SD_CLK_PIN;
    slot_config.cmd = SD_CMD_PIN;
    slot_config.d0 = SD_DATA0_PIN;
    slot_config.d1 = GPIO_NUM_NC;
    slot_config.d2 = GPIO_NUM_NC;
    slot_config.d3 = GPIO_NUM_NC;
    slot_config.d4 = GPIO_NUM_NC;
    slot_config.d5 = GPIO_NUM_NC;
    slot_config.d6 = GPIO_NUM_NC;
    slot_config.d7 = GPIO_NUM_NC;
    slot_config.cd = GPIO_NUM_NC;
    slot_config.wp = GPIO_NUM_NC;
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ESP_LOGI(TAG, "Mounting filesystem");
    esp_err_t ret = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config,
                                            &mount_config, &card);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s)",
                     esp_err_to_name(ret));
        }
        return;
    }

    sd_mount_flag = true;
    ESP_LOGI(TAG, "Filesystem mounted");

    ESP_ERROR_CHECK(esp_event_handler_register_with(
        ui_event_loop_handle, APP_EVENT, APP_FILE_REFRESH_REQUEST,
        user_sd_refresh_handler, NULL));
}

static int playlist_format_current_path(char *out_path, size_t max_len)
{
    if (out_path == NULL || max_len == 0 || g_playlist.music_count == 0) {
        return -1;
    }

    snprintf(out_path, max_len, "%s%s", g_playlist.dir_path,
             g_playlist.music_list[g_playlist.current_index].name);
    return 0;
}

static uint16_t playlist_random_index(void)
{
    if (g_playlist.music_count <= 1) {
        return g_playlist.current_index;
    }

    uint16_t next_index = g_playlist.current_index;
    while (next_index == g_playlist.current_index) {
        next_index = esp_random() % g_playlist.music_count;
    }
    return next_index;
}

static int playlist_get_prev_internal(char *out_path, size_t max_len, bool manual_request)
{
    if (g_playlist.music_count == 0) {
        return -1;
    }

    switch (g_playlist.play_mode) {
    case MUSIC_PLAY_MODE_RANDOM:
        g_playlist.current_index = playlist_random_index();
        break;
    case MUSIC_PLAY_MODE_REPEAT_ONE:
        if (!manual_request) {
            break;
        }
        if (g_playlist.current_index == 0) {
            g_playlist.current_index = g_playlist.music_count - 1;
        } else {
            g_playlist.current_index--;
        }
        break;
    case MUSIC_PLAY_MODE_SEQUENCE:
    default:
        if (g_playlist.current_index == 0) {
            g_playlist.current_index = g_playlist.music_count - 1;
        } else {
            g_playlist.current_index--;
        }
        break;
    }

    if (playlist_format_current_path(out_path, max_len) != 0) {
        return -1;
    }

    ESP_LOGI(TAG, "Prev (%s%s): [%d/%d] %s",
             playlist_get_play_mode_label(),
             (g_playlist.play_mode == MUSIC_PLAY_MODE_REPEAT_ONE && manual_request) ? "-manual" : "",
             g_playlist.current_index + 1, g_playlist.music_count, out_path);
    return 0;
}

static int playlist_get_next_internal(char *out_path, size_t max_len, bool manual_request)
{
    if (g_playlist.music_count == 0) {
        return -1;
    }

    switch (g_playlist.play_mode) {
    case MUSIC_PLAY_MODE_RANDOM:
        g_playlist.current_index = playlist_random_index();
        break;
    case MUSIC_PLAY_MODE_REPEAT_ONE:
        if (!manual_request) {
            break;
        }
        g_playlist.current_index = (g_playlist.current_index + 1) % g_playlist.music_count;
        break;
    case MUSIC_PLAY_MODE_SEQUENCE:
    default:
        g_playlist.current_index = (g_playlist.current_index + 1) % g_playlist.music_count;
        break;
    }

    if (playlist_format_current_path(out_path, max_len) != 0) {
        return -1;
    }

    ESP_LOGI(TAG, "Next (%s%s): [%d/%d] %s",
             playlist_get_play_mode_label(),
             (g_playlist.play_mode == MUSIC_PLAY_MODE_REPEAT_ONE && manual_request) ? "-manual" : "",
             g_playlist.current_index + 1, g_playlist.music_count, out_path);
    return 0;
}

static void user_sd_refresh_handler(void *arg, esp_event_base_t event_base,
                                    int32_t event_id, void *event_data)
{
    if (!sd_mount_flag) {
        file_list_data_t data = {
            .current_path = "please mount sd first",
            .item_count = 0,
        };

        esp_event_post_to(ui_event_loop_handle, APP_EVENT,
                          APP_FILE_REFRESH_RESPONSE, &data, sizeof(data),
                          portMAX_DELAY);
        return;
    }

    file_refresh_req_data_t *data = (file_refresh_req_data_t *)event_data;
    file_list_data_t result = {0};

    if (!data->is_directory) {
        if (strcasecmp(data->ext_name, "bmp") == 0) {
        }
        return;
    }

    if (strlen(data->name) != 0) {
        strcat(data->current_path, data->name);
        strcat(data->current_path, "/");
    }
    ESP_LOGI(TAG, "refresh target full path is %s ", data->current_path);

    strcpy(result.current_path, data->current_path);

    DIR *dir = opendir(data->current_path);
    if (dir == NULL) {
        return;
    }

    uint16_t capacity = 10;
    result.items = heap_caps_malloc(capacity * sizeof(file_item_t),
                                    MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    result.item_count = 0;
    if (result.items == NULL) {
        closedir(dir);
        return;
    }

    struct dirent *entry;
    uint16_t scan_yield_count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (result.item_count >= capacity) {
            capacity *= 2;
            result.items = heap_caps_realloc(
                result.items, capacity * sizeof(file_item_t),
                MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
            if (result.items == NULL) {
                closedir(dir);
                return;
            }
        }

        strcpy(result.items[result.item_count].name, entry->d_name);

        char *full_path = heap_caps_malloc(SD_PATH_MAX,
                                           MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (full_path == NULL) {
            continue;
        }

        int actual_num = snprintf(full_path, SD_PATH_MAX, "%s%s",
                                  data->current_path, entry->d_name);
        if (actual_num >= 0 && actual_num < SD_PATH_MAX) {
            full_path[actual_num] = '\0';
        } else {
            full_path[SD_PATH_MAX - 1] = '\0';
        }

        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            result.items[result.item_count].type = FILE_TYPE_DIR;
        } else {
            result.items[result.item_count].type = get_file_type(full_path);
            if (!file_type_matches_filter(result.items[result.item_count].type,
                                          data->filter_mode)) {
                free(full_path);
                continue;
            }
        }
        result.item_count++;
        free(full_path);

        scan_yield_count++;
        if ((scan_yield_count % 8) == 0) {
            vTaskDelay(1);
        }
    }

    closedir(dir);
    playlist_update(result.current_path, result.items, result.item_count);
    esp_event_post_to(ui_event_loop_handle, APP_EVENT, APP_FILE_REFRESH_RESPONSE,
                      &result, sizeof(result), portMAX_DELAY);
}

void playlist_update(const char *dir_path, file_item_t *items, uint16_t item_count)
{
    if (g_playlist.music_list != NULL) {
        free(g_playlist.music_list);
        g_playlist.music_list = NULL;
    }

    g_playlist.music_list = heap_caps_malloc(item_count * sizeof(file_item_t),
                                             MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (g_playlist.music_list == NULL) {
        g_playlist.music_count = 0;
        return;
    }

    strncpy(g_playlist.dir_path, dir_path, SD_PATH_MAX - 1);
    g_playlist.dir_path[SD_PATH_MAX - 1] = '\0';

    g_playlist.music_count = 0;
    for (uint16_t i = 0; i < item_count; i++) {
        if (items[i].type == FILE_TYPE_MP3 || items[i].type == FILE_TYPE_FLAC ||
            items[i].type == FILE_TYPE_WAV) {
            memcpy(&g_playlist.music_list[g_playlist.music_count], &items[i],
                   sizeof(file_item_t));
            g_playlist.music_count++;
        }
    }

    g_playlist.current_index = 0;
    g_playlist.play_mode = g_playlist.play_mode % MUSIC_PLAY_MODE_MAX;
    ESP_LOGI(TAG, "Playlist updated: %d music files in %s", g_playlist.music_count,
             dir_path);
}

int playlist_set_current_by_name(const char *name)
{
    if (name == NULL || g_playlist.music_count == 0) {
        return -1;
    }

    for (uint16_t i = 0; i < g_playlist.music_count; i++) {
        if (strcmp(g_playlist.music_list[i].name, name) == 0) {
            g_playlist.current_index = i;
            ESP_LOGI(TAG, "Current track set: [%d/%d] %s",
                     g_playlist.current_index + 1, g_playlist.music_count,
                     g_playlist.music_list[g_playlist.current_index].name);
            return 0;
        }
    }

    ESP_LOGW(TAG, "Track '%s' not found in current playlist", name);
    return -1;
}

int playlist_get_prev(char *out_path, size_t max_len)
{
    return playlist_get_prev_internal(out_path, max_len, false);
}

int playlist_get_next(char *out_path, size_t max_len)
{
    return playlist_get_next_internal(out_path, max_len, false);
}

int playlist_get_prev_manual(char *out_path, size_t max_len)
{
    return playlist_get_prev_internal(out_path, max_len, true);
}

int playlist_get_next_manual(char *out_path, size_t max_len)
{
    return playlist_get_next_internal(out_path, max_len, true);
}

music_play_mode_t playlist_get_play_mode(void)
{
    return g_playlist.play_mode;
}

music_play_mode_t playlist_cycle_play_mode(void)
{
    g_playlist.play_mode = (g_playlist.play_mode + 1) % MUSIC_PLAY_MODE_MAX;
    ESP_LOGI(TAG, "Play mode switched to %s", playlist_get_play_mode_label());
    return g_playlist.play_mode;
}

const char *playlist_get_play_mode_label(void)
{
    switch (g_playlist.play_mode) {
    case MUSIC_PLAY_MODE_RANDOM:
        return "RND";
    case MUSIC_PLAY_MODE_REPEAT_ONE:
        return "ONE";
    case MUSIC_PLAY_MODE_SEQUENCE:
    default:
        return "SEQ";
    }
}
