#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "audio_player.h"
#include "bsp_codec.h"
#include "esp_event.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "gui_guider.h"
#include "lcd.h"
#include "mp3dec.h"
#include "music_player.h"
#include "sdcard.h"
#include "ui_event.h"
#include "src/libs/tjpgd/tjpgd.h"

static const char *TAG = "music_player";

#define MUSIC_PLAYER_TASK_PRIORITY 5
#define MUSIC_PLAYER_TASK_CORE     0

typedef struct {
    uint32_t total_ms;
    uint32_t played_ms;
    uint64_t bytes_written_total;
    uint32_t sample_rate;
    uint32_t bits_per_sample;
    uint32_t channels;
    bool has_duration;
} music_progress_state_t;

typedef struct {
    char header[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[30];
    char genre;
} __attribute__((packed)) music_mp3_id3_header_v1_t;

typedef struct {
    char header[3];
    char ver;
    char revision;
    char flag;
    char size[4];
} __attribute__((packed)) music_mp3_id3_header_v2_t;

typedef struct {
    uint8_t ChunkID[4];
    int32_t ChunkSize;
    uint8_t Format[4];
    uint8_t Subchunk1ID[4];
    int32_t Subchunk1Size;
    int16_t AudioFormat;
    int16_t NumChannels;
    int32_t SampleRate;
    int32_t ByteRate;
    int16_t BlockAlign;
    int16_t BitsPerSample;
} music_wav_header_t;

typedef struct {
    uint8_t SubchunkID[4];
    int32_t SubchunkSize;
} music_wav_subchunk_header_t;

typedef enum {
    MUSIC_COVER_SOURCE_NONE = 0,
    MUSIC_COVER_SOURCE_APIC,
    MUSIC_COVER_SOURCE_DIR_JPG,
    MUSIC_COVER_SOURCE_CACHE,
} music_cover_source_t;

static audio_player_config_t player_config = {0};
uint8_t g_sys_volume = 50;
static TickType_t g_last_track_switch_tick = 0;
static const TickType_t TRACK_SWITCH_DEBOUNCE_TICKS = pdMS_TO_TICKS(150);
static const uint16_t MAX_CONSECUTIVE_PLAYBACK_FAILURES = 3;
static portMUX_TYPE g_progress_lock = portMUX_INITIALIZER_UNLOCKED;
static music_progress_state_t g_progress_state = {0};
static lv_timer_t *g_progress_timer = NULL;
static bool g_music_screen_active = false;
static bool g_event_handlers_registered = false;
static bool g_player_initialized = false;
static bool g_ignore_next_idle_advance = false;
static bool g_last_track_failed = false;
static bool g_track_has_audio_output = false;
static uint16_t g_consecutive_playback_failures = 0;
static bool g_cover_ui_dirty = true;
static char g_current_track_name[SD_PATH_MAX / 2] = {0};
static char g_current_track_path[SD_PATH_MAX] = {0};
static uint32_t g_cover_request_id = 0;


typedef struct {
    bool valid;
    bool has_cover;
    bool from_cache;
    music_cover_source_t source;
    uint16_t source_w;
    uint16_t source_h;
    uint16_t final_w;
    uint16_t final_h;
    char mime[16];
} music_cover_meta_t;

typedef struct {
    bool valid;
    bool has_cover;
    bool from_cache;
    uint32_t request_id;
    lv_draw_buf_t *draw_buf;
    music_cover_meta_t meta;
} music_cover_result_t;

static void music_player_refresh_play_button_ui(void);

void music_play(char *path);

typedef enum {
    MUSIC_TRACK_START_REASON_USER_SELECT = 0,
    MUSIC_TRACK_START_REASON_MANUAL_PREV,
    MUSIC_TRACK_START_REASON_MANUAL_NEXT,
    MUSIC_TRACK_START_REASON_AUTO_NEXT,
} music_track_start_reason_t;

static size_t music_player_decode_synchsafe_size(const char size[4])
{
    return ((size_t)(size[0] & 0x7F) << 21) |
           ((size_t)(size[1] & 0x7F) << 14) |
           ((size_t)(size[2] & 0x7F) << 7) |
           ((size_t)(size[3] & 0x7F));
}

static uint32_t music_player_read_be32(const uint8_t *data)
{
    return ((uint32_t)data[0] << 24) |
           ((uint32_t)data[1] << 16) |
           ((uint32_t)data[2] << 8) |
           (uint32_t)data[3];
}

typedef struct {
    const uint8_t *data;
    size_t size;
    size_t offset;
    lv_draw_buf_t *decoded;
} music_player_apic_jpeg_ctx_t;

static bool music_player_is_mp3(FILE *fp)
{
    uint8_t magic[3] = {0};

    fseek(fp, 0, SEEK_SET);
    if (fread(magic, 1, sizeof(magic), fp) != sizeof(magic)) {
        fseek(fp, 0, SEEK_SET);
        return false;
    }

    bool is_mp3_file = false;
    if ((magic[0] == 0xFF && magic[1] == 0xFB) ||
        (magic[0] == 0xFF && magic[1] == 0xF3) ||
        (magic[0] == 0xFF && magic[1] == 0xF2) ||
        (magic[0] == 0x49 && magic[1] == 0x44 && magic[2] == 0x33)) {
        is_mp3_file = true;
    }

    fseek(fp, 0, SEEK_SET);
    return is_mp3_file;
}

static bool music_player_is_wav(FILE *fp, music_wav_header_t *header)
{
    fseek(fp, 0, SEEK_SET);
    if (fread(header, 1, sizeof(*header), fp) != sizeof(*header)) {
        fseek(fp, 0, SEEK_SET);
        return false;
    }

    bool is_wav_file = (memcmp(header->ChunkID, "RIFF", 4) == 0) &&
                       (memcmp(header->Format, "WAVE", 4) == 0);
    fseek(fp, 0, SEEK_SET);
    return is_wav_file;
}

static const char *music_player_get_mode_icon(void)
{
    switch (playlist_get_play_mode()) {
    case MUSIC_PLAY_MODE_RANDOM:
        return " " LV_SYMBOL_SHUFFLE " ";
    case MUSIC_PLAY_MODE_REPEAT_ONE:
        return " " LV_SYMBOL_LOOP " ";
    case MUSIC_PLAY_MODE_SEQUENCE:
    default:
        return " " LV_SYMBOL_REFRESH " ";
    }
}

static const char *music_player_get_volume_icon(uint8_t volume)
{
    if (volume == 0) {
        return LV_SYMBOL_MUTE;
    }

    if (volume <= 60) {
        return LV_SYMBOL_VOLUME_MID;
    }

    return LV_SYMBOL_VOLUME_MAX;
}

static void music_player_format_time(uint32_t time_ms, char *out, size_t out_len)
{
    uint32_t total_seconds = time_ms / 1000;
    uint32_t hours = total_seconds / 3600;
    uint32_t minutes = (total_seconds % 3600) / 60;
    uint32_t seconds = total_seconds % 60;

    if (hours > 0) {
        snprintf(out, out_len, "%02lu:%02lu:%02lu",
                 (unsigned long)hours,
                 (unsigned long)minutes,
                 (unsigned long)seconds);
    } else {
        snprintf(out, out_len, "%02lu:%02lu",
                 (unsigned long)minutes,
                 (unsigned long)seconds);
    }
}

static void music_player_progress_reset(void)
{
    uint32_t sample_rate = 0;
    uint32_t bits_per_sample = 0;
    uint32_t channels = 0;

    portENTER_CRITICAL(&g_progress_lock);
    sample_rate = g_progress_state.sample_rate;
    bits_per_sample = g_progress_state.bits_per_sample;
    channels = g_progress_state.channels;
    memset(&g_progress_state, 0, sizeof(g_progress_state));
    g_progress_state.sample_rate = sample_rate;
    g_progress_state.bits_per_sample = bits_per_sample;
    g_progress_state.channels = channels;
    portEXIT_CRITICAL(&g_progress_lock);
}

static void music_player_progress_set_total(uint32_t total_ms, bool has_duration)
{
    portENTER_CRITICAL(&g_progress_lock);
    g_progress_state.total_ms = total_ms;
    g_progress_state.has_duration = has_duration;
    portEXIT_CRITICAL(&g_progress_lock);
}

static void music_player_progress_set_format(uint32_t sample_rate,
                                             uint32_t bits_per_sample,
                                             uint32_t channels)
{
    portENTER_CRITICAL(&g_progress_lock);
    g_progress_state.sample_rate = sample_rate;
    g_progress_state.bits_per_sample = bits_per_sample;
    g_progress_state.channels = channels;
    portEXIT_CRITICAL(&g_progress_lock);
}

/**
 * @brief 添加已写入的音频字节数并更新播放进度
 * 
 * 该函数用于追踪音频播放进度，通过累计已写入的字节数来计算已播放的毫秒数。
 * 使用临界区保护共享的进度状态，确保线程安全。
 * 
 * @param bytes_written 本次写入的音频字节数
 */
static void music_player_progress_add_bytes(size_t bytes_written)
{
    // 首次成功写入数据时，标记轨道有音频输出，并重置失败计数
    if (bytes_written > 0 && !g_track_has_audio_output) {
        g_track_has_audio_output = true;
        g_last_track_failed = false;
        g_consecutive_playback_failures = 0;
    }

    // 进入临界区，保护进度状态的访问
    portENTER_CRITICAL(&g_progress_lock);

    // 检查音频格式参数是否有效
    if (g_progress_state.sample_rate == 0 ||
        g_progress_state.bits_per_sample == 0 ||
        g_progress_state.channels == 0) {
        portEXIT_CRITICAL(&g_progress_lock);
        return;
    }

    // 计算每秒字节数：采样率 * 声道数 * 每样本字节数
    uint64_t bytes_per_second =
        (uint64_t)g_progress_state.sample_rate *
        (uint64_t)g_progress_state.channels *
        ((uint64_t)g_progress_state.bits_per_sample / 8ULL);

    // 防止除零错误
    if (bytes_per_second == 0) {
        portEXIT_CRITICAL(&g_progress_lock);
        return;
    }

    // 累加已写入的总字节数
    g_progress_state.bytes_written_total += bytes_written;
    
    // 根据总字节数计算已播放时间（毫秒）
    g_progress_state.played_ms =
        (uint32_t)((g_progress_state.bytes_written_total * 1000ULL) / bytes_per_second);

    // 确保播放进度不超过总时长
    if (g_progress_state.has_duration &&
        g_progress_state.played_ms > g_progress_state.total_ms) {
        g_progress_state.played_ms = g_progress_state.total_ms;
    }

    // 退出临界区
    portEXIT_CRITICAL(&g_progress_lock);
}

static uint16_t music_player_get_failure_limit(void)
{
    if (g_playlist.music_count == 0) {
        return MAX_CONSECUTIVE_PLAYBACK_FAILURES;
    }

    if (g_playlist.music_count < MAX_CONSECUTIVE_PLAYBACK_FAILURES) {
        return g_playlist.music_count;
    }

    return MAX_CONSECUTIVE_PLAYBACK_FAILURES;
}

static void music_player_progress_snapshot(uint32_t *played_ms,
                                           uint32_t *total_ms,
                                           bool *has_duration)
{
    portENTER_CRITICAL(&g_progress_lock);
    *played_ms = g_progress_state.played_ms;
    *total_ms = g_progress_state.total_ms;
    *has_duration = g_progress_state.has_duration;
    portEXIT_CRITICAL(&g_progress_lock);
}

static void music_player_store_track_name(const char *path)
{
    const char *filename = strrchr(path, '/');
    filename = (filename != NULL) ? (filename + 1) : path;

    strncpy(g_current_track_name, filename, sizeof(g_current_track_name) - 1);
    g_current_track_name[sizeof(g_current_track_name) - 1] = '\0';
}

static void music_player_store_track_path(const char *path)
{
    strncpy(g_current_track_path, path, sizeof(g_current_track_path) - 1);
    g_current_track_path[sizeof(g_current_track_path) - 1] = '\0';
}

static void music_player_set_status_label(const char *prefix, const char *track_name)
{
    if (!g_music_screen_active) {
        return;
    }

    char status_buf[SD_PATH_MAX / 2] = {0};
    snprintf(status_buf, sizeof(status_buf), "%s%s", prefix, track_name);
    lv_label_set_text(guider_ui.music_label_name, status_buf);
}

static void music_player_refresh_progress_ui(void)
{
    if (!g_music_screen_active) {
        return;
    }

    uint32_t played_ms = 0;
    uint32_t total_ms = 0;
    bool has_duration = false;
    char played_buf[16] = {0};
    char total_buf[16] = {0};

    music_player_progress_snapshot(&played_ms, &total_ms, &has_duration);
    music_player_format_time(played_ms, played_buf, sizeof(played_buf));

    if (has_duration) {
        music_player_format_time(total_ms, total_buf, sizeof(total_buf));
    } else {
        strncpy(total_buf, "--:--", sizeof(total_buf) - 1);
    }

    lv_slider_set_range(guider_ui.music_slider_prog, 0, 1000);
    if (has_duration && total_ms > 0) {
        uint32_t slider_value = (played_ms * 1000UL) / total_ms;
        if (slider_value > 1000) {
            slider_value = 1000;
        }
        lv_slider_set_value(guider_ui.music_slider_prog, (int32_t)slider_value, LV_ANIM_OFF);
    } else {
        lv_slider_set_value(guider_ui.music_slider_prog, 0, LV_ANIM_OFF);
    }

    lv_label_set_text(guider_ui.music_label_play_time, played_buf);
    lv_label_set_text(guider_ui.music_label_total_time, total_buf);
}

static void music_player_reset_progress_ui_to_default(void)
{
    lv_slider_set_range(guider_ui.music_slider_prog, 0, 1000);
    lv_slider_set_value(guider_ui.music_slider_prog, 0, LV_ANIM_OFF);
    lv_label_set_text(guider_ui.music_label_play_time, "00:00");
    lv_label_set_text(guider_ui.music_label_total_time, "00:00");
}

static void music_player_progress_timer_cb(lv_timer_t *timer)
{
    music_player_refresh_progress_ui();
}

static void music_player_refresh_playback_state_ui(void)
{
    if (!g_music_screen_active) {
        return;
    }

    music_player_refresh_play_button_ui();

    if (g_current_track_name[0] != '\0') {
        music_player_refresh_progress_ui();
    } else {
        music_player_reset_progress_ui_to_default();
    }
}

static void music_player_refresh_play_button_ui(void)
{
    if (!g_music_screen_active) {
        return;
    }

    const char *play_symbol = LV_SYMBOL_PLAY;
    if (audio_player_get_state() == AUDIO_PLAYER_STATE_PLAYING) {
        play_symbol = LV_SYMBOL_PAUSE;
    }
    lv_label_set_text(guider_ui.music_btn_play_label, play_symbol);
}

static void music_player_refresh_volume_ui(void)
{
    uint8_t volume = get_sys_volume();
    lv_label_set_text(guider_ui.music_btn_volume_label, music_player_get_volume_icon(volume));
    lv_slider_set_value(guider_ui.music_slider_volume, volume, LV_ANIM_OFF);
}

static void music_player_sync_ui_from_session(void)
{
    if (!g_music_screen_active) {
        return;
    }

    music_player_refresh_mode_ui();
    music_player_refresh_volume_ui();
    music_player_refresh_play_button_ui();

    if (g_current_track_name[0] != '\0') {
        lv_label_set_text(guider_ui.music_label_name, g_current_track_name);
        music_player_refresh_progress_ui();
    } else {
        music_player_reset_progress_ui_to_default();
    }
}

static bool music_player_probe_wav_duration(FILE *fp, uint32_t *out_duration_ms)
{
    music_wav_header_t wav = {0};
    if (!music_player_is_wav(fp, &wav)) {
        return false;
    }

    fseek(fp, sizeof(music_wav_header_t), SEEK_SET);
    music_wav_subchunk_header_t subchunk = {0};
    while (fread(&subchunk, 1, sizeof(subchunk), fp) == sizeof(subchunk)) {
        if (memcmp(subchunk.SubchunkID, "data", 4) == 0) {
            if (wav.ByteRate > 0 && subchunk.SubchunkSize > 0) {
                uint64_t total_ms =
                    ((uint64_t)subchunk.SubchunkSize * 1000ULL) /
                    (uint64_t)wav.ByteRate;
                *out_duration_ms = (uint32_t)total_ms;
                fseek(fp, 0, SEEK_SET);
                return true;
            }
            break;
        }
        fseek(fp, subchunk.SubchunkSize, SEEK_CUR);
    }

    fseek(fp, 0, SEEK_SET);
    return false;
}

static bool music_player_probe_mp3_duration(FILE *fp, uint32_t *out_duration_ms)
{
    if (!music_player_is_mp3(fp)) {
        return false;
    }

    long file_size = 0;
    long audio_start = 0;
    long audio_end = 0;

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    if (file_size <= 0) {
        fseek(fp, 0, SEEK_SET);
        return false;
    }
    audio_end = file_size;

    fseek(fp, 0, SEEK_SET);
    music_mp3_id3_header_v2_t id3v2 = {0};
    if (fread(&id3v2, 1, sizeof(id3v2), fp) == sizeof(id3v2) &&
        memcmp(id3v2.header, "ID3", sizeof(id3v2.header)) == 0) {
        size_t tag_payload_size = music_player_decode_synchsafe_size(id3v2.size);
        size_t footer_size = (id3v2.flag & 0x10) ? sizeof(id3v2) : 0;
        audio_start = (long)(sizeof(id3v2) + tag_payload_size + footer_size);
    }

    if (file_size >= (long)sizeof(music_mp3_id3_header_v1_t)) {
        music_mp3_id3_header_v1_t id3v1 = {0};
        fseek(fp, file_size - (long)sizeof(id3v1), SEEK_SET);
        if (fread(&id3v1, 1, sizeof(id3v1), fp) == sizeof(id3v1) &&
            memcmp(id3v1.header, "TAG", sizeof(id3v1.header)) == 0) {
            audio_end -= (long)sizeof(id3v1);
        }
    }

    fseek(fp, audio_start, SEEK_SET);
    uint8_t *probe_buf = heap_caps_calloc(MAINBUF_SIZE * 2, sizeof(uint8_t),
                                          MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (probe_buf == NULL) {
        fseek(fp, 0, SEEK_SET);
        return false;
    }

    size_t bytes_read = fread(probe_buf, 1, MAINBUF_SIZE * 2, fp);
    if (bytes_read == 0) {
        free(probe_buf);
        fseek(fp, 0, SEEK_SET);
        return false;
    }

    int offset = MP3FindSyncWord(probe_buf, (int)bytes_read);
    if (offset < 0) {
        free(probe_buf);
        fseek(fp, 0, SEEK_SET);
        return false;
    }

    HMP3Decoder decoder = MP3InitDecoder();
    if (decoder == NULL) {
        free(probe_buf);
        fseek(fp, 0, SEEK_SET);
        return false;
    }

    MP3FrameInfo frame_info = {0};
    int err = MP3GetNextFrameInfo(decoder, &frame_info, probe_buf + offset);
    MP3FreeDecoder(decoder);

    if (err != ERR_MP3_NONE || frame_info.bitrate <= 0) {
        free(probe_buf);
        fseek(fp, 0, SEEK_SET);
        return false;
    }

    long payload_start = audio_start + offset;
    if (audio_end <= payload_start) {
        free(probe_buf);
        fseek(fp, 0, SEEK_SET);
        return false;
    }

    uint32_t samples_per_frame = 0;
    if (frame_info.nChans > 0) {
        samples_per_frame = (uint32_t)(frame_info.outputSamps / frame_info.nChans);
    }

    size_t metadata_search_len = bytes_read - (size_t)offset;
    const uint8_t *metadata_start = probe_buf + offset;
    if (metadata_search_len > 192U) {
        metadata_search_len = 192U;
    }

    if (samples_per_frame > 0 && frame_info.samprate > 0) {
        for (size_t i = 0; i + 12U <= metadata_search_len; ++i) {
            const uint8_t *tag_ptr = metadata_start + i;

            if ((memcmp(tag_ptr, "Xing", 4) == 0) ||
                (memcmp(tag_ptr, "Info", 4) == 0)) {
                uint32_t flags = music_player_read_be32(tag_ptr + 4);
                const uint8_t *field_ptr = tag_ptr + 8;
                const uint8_t *field_end = metadata_start + metadata_search_len;

                if ((flags & 0x1U) != 0U && (field_ptr + 4) <= field_end) {
                    uint32_t frame_count = music_player_read_be32(field_ptr);
                    uint64_t total_ms =
                        ((uint64_t)frame_count * (uint64_t)samples_per_frame * 1000ULL) /
                        (uint64_t)frame_info.samprate;
                    *out_duration_ms = (uint32_t)total_ms;
                    ESP_LOGI(TAG, "MP3 duration from %s tag: %lu frames -> %lu ms",
                             (memcmp(tag_ptr, "Xing", 4) == 0) ? "Xing" : "Info",
                             (unsigned long)frame_count,
                             (unsigned long)*out_duration_ms);
                    free(probe_buf);
                    fseek(fp, 0, SEEK_SET);
                    return true;
                }
            }

            if ((i + 18U) <= metadata_search_len && memcmp(tag_ptr, "VBRI", 4) == 0) {
                uint32_t frame_count = music_player_read_be32(tag_ptr + 14);
                if (frame_count > 0U) {
                    uint64_t total_ms =
                        ((uint64_t)frame_count * (uint64_t)samples_per_frame * 1000ULL) /
                        (uint64_t)frame_info.samprate;
                    *out_duration_ms = (uint32_t)total_ms;
                    ESP_LOGI(TAG, "MP3 duration from VBRI tag: %lu frames -> %lu ms",
                             (unsigned long)frame_count,
                             (unsigned long)*out_duration_ms);
                    free(probe_buf);
                    fseek(fp, 0, SEEK_SET);
                    return true;
                }
            }
        }
    }

    uint64_t audio_bytes = (uint64_t)(audio_end - payload_start);
    uint64_t total_ms = (audio_bytes * 8ULL * 1000ULL) / (uint64_t)frame_info.bitrate;
    *out_duration_ms = (uint32_t)total_ms;
    ESP_LOGI(TAG, "MP3 duration from bitrate estimate: %lu bps -> %lu ms",
             (unsigned long)frame_info.bitrate,
             (unsigned long)*out_duration_ms);

    free(probe_buf);
    fseek(fp, 0, SEEK_SET);
    return true;
}

static bool music_player_probe_duration(FILE *fp, uint32_t *out_duration_ms)
{
    if (music_player_probe_wav_duration(fp, out_duration_ms)) {
        return true;
    }

    if (music_player_probe_mp3_duration(fp, out_duration_ms)) {
        return true;
    }

    fseek(fp, 0, SEEK_SET);
    return false;
}

static void music_player_start_track(const char *path,
                                     music_track_start_reason_t reason)
{
    g_track_has_audio_output = false;
    g_last_track_failed = false;
    if (reason != MUSIC_TRACK_START_REASON_AUTO_NEXT) {
        g_consecutive_playback_failures = 0;
    }
    music_play((char *)path);
}

static int music_player_switch_track(bool next_track,
                                     music_track_start_reason_t reason)
{
    char path[SD_PATH_MAX] = {0};
    bool manual_request = (reason == MUSIC_TRACK_START_REASON_MANUAL_NEXT) ||
                          (reason == MUSIC_TRACK_START_REASON_MANUAL_PREV);
    int ret = 0;

    if (next_track) {
        ret = manual_request ? playlist_get_next_manual(path, sizeof(path))
                             : playlist_get_next(path, sizeof(path));
    } else {
        ret = manual_request ? playlist_get_prev_manual(path, sizeof(path))
                             : playlist_get_prev(path, sizeof(path));
    }

    if (ret != 0) {
        ESP_LOGW(TAG, "Failed to get %s music file",
                 next_track ? "next" : "previous");
        return ret;
    }

    music_player_start_track(path, reason);
    return 0;
}

void music_player_refresh_mode_ui(void)
{
    if (!g_music_screen_active) {
        return;
    }

    lv_label_set_text(guider_ui.music_btn_mode_label, music_player_get_mode_icon());
}

void music_player_cycle_mode(void)
{
    playlist_cycle_play_mode();
    music_player_refresh_mode_ui();
}

void music_player_on_screen_enter(void)
{
    g_music_screen_active = true;
    g_cover_ui_dirty = true;

    if (g_progress_timer == NULL) {
        g_progress_timer = lv_timer_create(music_player_progress_timer_cb, 200, NULL);
    }

    music_player_sync_ui_from_session();
}

void music_player_on_screen_exit(void)
{
    g_music_screen_active = false;
    if (g_progress_timer != NULL) {
        lv_timer_delete(g_progress_timer);
        g_progress_timer = NULL;
    }
}

void music_player_reset_session(void)
{
    g_current_track_name[0] = '\0';
    g_current_track_path[0] = '\0';
    g_last_track_switch_tick = 0;
    g_last_track_failed = false;
    g_track_has_audio_output = false;
    g_consecutive_playback_failures = 0;
    g_cover_request_id++;
    music_player_progress_reset();
}

void music_player_close(void)
{
    music_player_on_screen_exit();
    music_player_reset_session();

    if (!g_player_initialized) {
        return;
    }

    g_ignore_next_idle_advance = true;
    esp_err_t ret = audio_player_stop();
    if (ret != ESP_OK) {
        g_ignore_next_idle_advance = false;
        ESP_LOGW(TAG, "audio_player_stop failed while closing player: %s",
                 esp_err_to_name(ret));
    }
    pa_en(0);
}

void music_play(char *path)
{
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        ESP_LOGW(TAG, "Failed to open '%s'", path);
        return;
    }

    g_ignore_next_idle_advance = false;
    music_player_progress_reset();

    uint32_t total_ms = 0;
    bool has_duration = music_player_probe_duration(fp, &total_ms);
    music_player_progress_set_total(total_ms, has_duration);
    music_player_store_track_name(path);
    music_player_store_track_path(path);

    if (g_music_screen_active) {
        if (lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) {
            music_player_sync_ui_from_session();
            lvgl_port_unlock();
        }
    }

    fseek(fp, 0, SEEK_SET);
    ESP_LOGI(TAG, "Playing '%s'", path);
    esp_err_t ret = audio_player_play(fp);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "audio_player_play failed for '%s': %s",
                 path, esp_err_to_name(ret));
        fclose(fp);
    }
}

static void user_music_play_request_handler(void *arg, esp_event_base_t event_base,
                                            int32_t event_id, void *event_data)
{
    file_refresh_req_data_t *data = (file_refresh_req_data_t *)event_data;
    audio_player_state_t state = audio_player_get_state();

    playlist_set_current_by_name(data->name);
    music_player_start_track(data->current_path,
                             MUSIC_TRACK_START_REASON_USER_SELECT);
    if (state == AUDIO_PLAYER_STATE_PAUSE) {
        audio_player_pause();
    }

    char *music_name = heap_caps_malloc(100, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (music_name != NULL) {
        strcpy(music_name, data->name);
        esp_event_post_to(ui_event_loop_handle, APP_EVENT, APP_MUSIC_PLAY_RESPONSE,
                          music_name, strlen(music_name) + 1, portMAX_DELAY);
        free(music_name);
    }
}

static void user_music_play_button_request_handler(void *arg,
                                                   esp_event_base_t event_base,
                                                   int32_t event_id,
                                                   void *event_data)
{
    if (g_current_track_name[0] == '\0') {
        return;
    }

    audio_player_state_t state = audio_player_get_state();
    if (state == AUDIO_PLAYER_STATE_IDLE) {
        return;
    }

    if (state == AUDIO_PLAYER_STATE_PAUSE) {
        if (lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) {
            lv_label_set_text(guider_ui.music_btn_play_label, LV_SYMBOL_PAUSE);
            lvgl_port_unlock();
        }
        audio_player_resume();
        return;
    }

    if (state == AUDIO_PLAYER_STATE_PLAYING) {
        if (lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) {
            lv_label_set_text(guider_ui.music_btn_play_label, LV_SYMBOL_PLAY);
            lvgl_port_unlock();
        }
        audio_player_pause();
    }
}

static void user_music_prev_next_request_handler(void *arg,
                                                 esp_event_base_t event_base,
                                                 int32_t event_id,
                                                 void *event_data)
{
    char *flag = (char *)event_data;
    TickType_t now = xTaskGetTickCount();

    if (flag == NULL) {
        ESP_LOGW(TAG, "Prev/Next: invalid flag");
        return;
    }

    if ((now - g_last_track_switch_tick) < TRACK_SWITCH_DEBOUNCE_TICKS) {
        ESP_LOGW(TAG, "Prev/Next ignored: switching too fast");
        return;
    }
    g_last_track_switch_tick = now;

    if (*flag == 0) {
        ESP_LOGI(TAG, "Prev button pressed");
        music_player_switch_track(false, MUSIC_TRACK_START_REASON_MANUAL_PREV);
    } else {
        ESP_LOGI(TAG, "Next button pressed");
        music_player_switch_track(true, MUSIC_TRACK_START_REASON_MANUAL_NEXT);
    }
}

static void user_music_auto_next_request_handler(void *arg,
                                                 esp_event_base_t event_base,
                                                 int32_t event_id,
                                                 void *event_data)
{
    (void)arg;
    (void)event_base;
    (void)event_id;
    (void)event_data;

    music_player_switch_track(true, MUSIC_TRACK_START_REASON_AUTO_NEXT);
}

uint8_t get_sys_volume(void)
{
    return g_sys_volume;
}

void change_sys_volume(uint8_t volume)
{
    g_sys_volume = volume;
}

static esp_err_t _audio_player_mute_fn(AUDIO_PLAYER_MUTE_SETTING setting)
{
    uint8_t volume = get_sys_volume();
    bsp_codec_mute_set(setting == AUDIO_PLAYER_MUTE);
    if (setting == AUDIO_PLAYER_UNMUTE) {
        bsp_codec_volume_set(volume, NULL);
    }
    return ESP_OK;
}

static esp_err_t _audio_player_write_fn(void *audio_buffer, size_t len,
                                        size_t *bytes_written,
                                        uint32_t timeout_ms)
{
    esp_err_t ret = bsp_i2s_write(audio_buffer, len, bytes_written, timeout_ms);
    if (ret == ESP_OK && bytes_written != NULL) {
        music_player_progress_add_bytes(*bytes_written);
    }
    return ret;
}

static esp_err_t _audio_player_std_clock(uint32_t rate, uint32_t bits_cfg,
                                         i2s_slot_mode_t ch)
{
    music_player_progress_set_format(rate, bits_cfg,
                                     (ch == I2S_SLOT_MODE_MONO) ? 1U : 2U);
    return bsp_codec_set_fs(rate, bits_cfg, ch);
}

static void _audio_player_callback(audio_player_cb_ctx_t *ctx)
{
    ESP_LOGI(TAG, "ctx->audio_event = %d", ctx->audio_event);
    switch (ctx->audio_event) {
    case AUDIO_PLAYER_CALLBACK_EVENT_IDLE:
        if (g_ignore_next_idle_advance) 
        {
            g_ignore_next_idle_advance = false;
            if (g_music_screen_active) 
            {
                if (lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) 
                {
                    lv_label_set_text(guider_ui.music_btn_play_label, LV_SYMBOL_PLAY);
                    lvgl_port_unlock();
                }
            }
            break;
        }
        if (g_last_track_failed) {
            uint16_t failure_limit = music_player_get_failure_limit();
            g_last_track_failed = false;
            if (g_consecutive_playback_failures >= failure_limit) {
                ESP_LOGW(TAG, "Too many consecutive playback failures (%u), stopping auto-next",
                         (unsigned)g_consecutive_playback_failures);
                if (g_music_screen_active) {
                    if (lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) {
                        music_player_reset_progress_ui_to_default();
                        lv_label_set_text(guider_ui.music_btn_play_label, LV_SYMBOL_PLAY);
                        lv_label_set_text(guider_ui.music_label_name, "No playable tracks");
                        lvgl_port_unlock();
                    }
                }
                break;
            }
        }
        esp_err_t auto_next_ret = esp_event_post_to(ui_event_loop_handle, APP_EVENT,
                                                    APP_MUSIC_AUTO_NEXT_REQUEST,
                                                    NULL, 0, 0);
        if (auto_next_ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to enqueue auto-next request: %s",
                     esp_err_to_name(auto_next_ret));
        }
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_PLAYING:
        ESP_LOGI(TAG, "AUDIO_PLAYER_REQUEST_PLAY");
        pa_en(1);
        if (g_music_screen_active) {
            if (lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) {
                music_player_refresh_playback_state_ui();
                lvgl_port_unlock();
            }
        }
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_PAUSE:
        ESP_LOGI(TAG, "AUDIO_PLAYER_REQUEST_PAUSE");
        pa_en(0);
        if (g_music_screen_active) {
            if (lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) {
                music_player_refresh_playback_state_ui();
                lvgl_port_unlock();
            }
        }
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_PLAYBACK_FAILED:
        g_last_track_failed = true;
        g_consecutive_playback_failures++;
        ESP_LOGW(TAG, "Playback failed, skipping current track: %s", g_current_track_name);
        ESP_LOGW(TAG, "Consecutive playback failures: %u/%u",
                 (unsigned)g_consecutive_playback_failures,
                 (unsigned)music_player_get_failure_limit());
        pa_en(0);
        if (g_music_screen_active) {
            if (lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) {
                music_player_reset_progress_ui_to_default();
                music_player_set_status_label("Skip: ", g_current_track_name);
                lv_label_set_text(guider_ui.music_btn_play_label, LV_SYMBOL_PLAY);
                lvgl_port_unlock();
            }
        }
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_UNKNOWN_FILE_TYPE:
        g_last_track_failed = true;
        g_consecutive_playback_failures++;
        ESP_LOGW(TAG, "Unknown file type, skipping current track: %s", g_current_track_name);
        ESP_LOGW(TAG, "Consecutive playback failures: %u/%u",
                 (unsigned)g_consecutive_playback_failures,
                 (unsigned)music_player_get_failure_limit());
        pa_en(0);
        if (g_music_screen_active) {
            if (lvgl_port_lock(UI_LVGL_LOCK_TIMEOUT_MS)) {
                music_player_reset_progress_ui_to_default();
                music_player_set_status_label("Unsupported: ", g_current_track_name);
                lv_label_set_text(guider_ui.music_btn_play_label, LV_SYMBOL_PLAY);
                lvgl_port_unlock();
            }
        }
        break;
    default:
        break;
    }
}

void mp3_player_init(void)
{
    if (!g_player_initialized) 
    {
        player_config.mute_fn = _audio_player_mute_fn;
        player_config.write_fn = _audio_player_write_fn;
        player_config.clk_set_fn = _audio_player_std_clock;
        player_config.priority = MUSIC_PLAYER_TASK_PRIORITY;
        player_config.coreID = MUSIC_PLAYER_TASK_CORE;

        ESP_ERROR_CHECK(audio_player_new(player_config));
        ESP_ERROR_CHECK(audio_player_callback_register(_audio_player_callback, NULL));

        g_player_initialized = true;
    }

    if (!g_event_handlers_registered) {
        ESP_ERROR_CHECK(esp_event_handler_register_with(
            ui_event_loop_handle, APP_EVENT, APP_MUSIC_PLAY_REQUEST,
            user_music_play_request_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register_with(
            ui_event_loop_handle, APP_EVENT, APP_MUSIC_PLAY_button_REQUEST,
            user_music_play_button_request_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register_with(
            ui_event_loop_handle, APP_EVENT, APP_MUSIC_PREV_NEXT_REQUEST,
            user_music_prev_next_request_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register_with(
            ui_event_loop_handle, APP_EVENT, APP_MUSIC_AUTO_NEXT_REQUEST,
            user_music_auto_next_request_handler, NULL));
        g_event_handlers_registered = true;
    }
}
