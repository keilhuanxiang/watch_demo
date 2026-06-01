#include <string.h>
#include "audio_log.h"
#include "audio_mp3.h"

static const char *TAG = "mp3";
static const uint16_t MAX_MP3_DECODE_ERRORS = 12;
static const uint16_t MAX_MP3_SYNC_MISSES = 24;

static size_t decode_synchsafe_size(const char size[4])
{
    return ((size_t)(size[0] & 0x7F) << 21) |
           ((size_t)(size[1] & 0x7F) << 14) |
           ((size_t)(size[2] & 0x7F) << 7) |
           ((size_t)(size[3] & 0x7F));
}

static void mp3_advance_after_failed_frame(mp3_instance *pInstance, uint8_t *frame_start)
{
    if (pInstance == NULL || frame_start == NULL) {
        return;
    }

    uint8_t *buffer_end = pInstance->data_buf + pInstance->bytes_in_data_buf;
    uint8_t *next_ptr = frame_start + 1;
    if (next_ptr > buffer_end) {
        next_ptr = buffer_end;
    }

    pInstance->read_ptr = next_ptr;
}

bool is_mp3(FILE *fp) {
    bool is_mp3_file = false;

    fseek(fp, 0, SEEK_SET);

    // see https://en.wikipedia.org/wiki/List_of_file_signatures
    uint8_t magic[3];
    if(sizeof(magic) == fread(magic, 1, sizeof(magic), fp)) {
        if((magic[0] == 0xFF) &&
            (magic[1] == 0xFB))
        {
            is_mp3_file = true;
        } else if((magic[0] == 0xFF) &&
                  (magic[1] == 0xF3))
        {
            is_mp3_file = true;
        } else if((magic[0] == 0xFF) &&
                  (magic[1] == 0xF2))
        {
            is_mp3_file = true;
        } else if((magic[0] == 0x49) &&
                  (magic[1] == 0x44) &&
                  (magic[2] == 0x33)) /* 'ID3' */
        {
            fseek(fp, 0, SEEK_SET);

            /* Get ID3 head */
            mp3_id3_header_v2_t tag;
            if (sizeof(mp3_id3_header_v2_t) == fread(&tag, 1, sizeof(mp3_id3_header_v2_t), fp)) {
                if (memcmp("ID3", (const void *) &tag, sizeof(tag.header)) == 0) {
                    is_mp3_file = true;
                }
            }
        }
    }

    // seek back to the start of the file to avoid
    // missing frames upon decode
    fseek(fp, 0, SEEK_SET);

    return is_mp3_file;
}

long skip_mp3_id3v2_tag(FILE *fp)
{
    if (fp == NULL) {
        return 0;
    }

    fseek(fp, 0, SEEK_SET);

    mp3_id3_header_v2_t tag{};
    if (sizeof(tag) != fread(&tag, 1, sizeof(tag), fp)) {
        fseek(fp, 0, SEEK_SET);
        return 0;
    }

    if (memcmp("ID3", (const void *)&tag, sizeof(tag.header)) != 0) {
        fseek(fp, 0, SEEK_SET);
        return 0;
    }

    size_t tag_payload_size = decode_synchsafe_size(tag.size);
    size_t footer_size = (tag.flag & 0x10) ? sizeof(tag) : 0;
    long audio_start = (long)(sizeof(tag) + tag_payload_size + footer_size);

    if (fseek(fp, audio_start, SEEK_SET) != 0) {
        ESP_LOGW(TAG, "failed to seek past ID3v2 tag, falling back to file start");
        fseek(fp, 0, SEEK_SET);
        return 0;
    }

    ESP_LOGI(TAG, "Skipping ID3v2 tag: header=%u payload=%u footer=%u start=%ld",
             (unsigned)sizeof(tag),
             (unsigned)tag_payload_size,
             (unsigned)footer_size,
             audio_start);
    return audio_start;
}

/**
 * @return true if data remains, false on error or end of file
 */
DECODE_STATUS decode_mp3(HMP3Decoder mp3_decoder, FILE *fp, decode_data *pData, mp3_instance *pInstance) {
    MP3FrameInfo frame_info{};

    size_t unread_bytes = pInstance->bytes_in_data_buf - (pInstance->read_ptr - pInstance->data_buf);

    /* somewhat arbitrary trigger to refill buffer - should always be enough for a full frame */
    if (unread_bytes < 1.25 * MAINBUF_SIZE && !pInstance->eof_reached) {
        uint8_t *write_ptr = pInstance->data_buf + unread_bytes;
        size_t free_space = pInstance->data_buf_size - unread_bytes;

    	/* move last, small chunk from end of buffer to start,
           then fill with new data */
        memmove(pInstance->data_buf, pInstance->read_ptr, unread_bytes);

        size_t nRead = fread(write_ptr, 1, free_space, fp);

        pInstance->bytes_in_data_buf = unread_bytes + nRead;
        pInstance->read_ptr = pInstance->data_buf;

        if ((nRead == 0) || feof(fp)) {
            pInstance->eof_reached = true;
        }

        LOGI_2("pos %ld, nRead %d, eof %d", ftell(fp), nRead, pInstance->eof_reached);

        unread_bytes = pInstance->bytes_in_data_buf;
    }

    LOGI_3("data_buf 0x%p, read 0x%p", pInstance->data_buf, pInstance->read_ptr);

    if(unread_bytes == 0) {
        LOGI_1("unread_bytes == 0, status done");
        return DECODE_STATUS_DONE;
    }

    /* Find MP3 sync word from read buffer */
    int offset = MP3FindSyncWord(pInstance->read_ptr, unread_bytes);

    LOGI_2("unread %d, total %d, offset 0x%x(%d)",
            unread_bytes, pInstance->bytes_in_data_buf, offset, offset);

    if (offset >= 0) {
        COMPILE_3(int starting_unread_bytes = unread_bytes);
        uint8_t *frame_start = pInstance->read_ptr + offset; /*!< Data start point */
        uint8_t *read_ptr = frame_start;
        unread_bytes -= offset;
        LOGI_3("read 0x%p, unread %d", read_ptr, unread_bytes);
        int mp3_dec_err = MP3Decode(mp3_decoder, &read_ptr, (int*)&unread_bytes, reinterpret_cast<int16_t *>(pData->samples), 
0);

        pInstance->read_ptr = read_ptr;

        if(mp3_dec_err == ERR_MP3_NONE) {
            /* Get MP3 frame info */
            MP3GetLastFrameInfo(mp3_decoder, &frame_info);
            int validated_channels = frame_info.nChans;
            int validated_output_samps = frame_info.outputSamps;
            int validated_sample_rate = frame_info.samprate;
            int validated_bits_per_sample = frame_info.bitsPerSample;

            if (validated_channels <= 0 ||
                validated_output_samps <= 0 ||
                validated_sample_rate <= 0 ||
                validated_bits_per_sample <= 0 ||
                (validated_output_samps % validated_channels) != 0) {
                pData->frame_count = 0;
                pInstance->continuous_error_count++;
                if (pInstance->read_ptr <= frame_start) {
                    mp3_advance_after_failed_frame(pInstance, frame_start);
                }
                ESP_LOGE(TAG,
                         "invalid frame info: ch=%d sr=%d bps=%d output=%d (err=%u)",
                         validated_channels,
                         validated_sample_rate,
                         validated_bits_per_sample,
                         validated_output_samps,
                         (unsigned)pInstance->continuous_error_count);
                if (pInstance->continuous_error_count >= MAX_MP3_DECODE_ERRORS) {
                    ESP_LOGE(TAG, "too many invalid MP3 frames, aborting track");
                    return DECODE_STATUS_ERROR;
                }
                return DECODE_STATUS_NO_DATA_CONTINUE;
            }

            pData->fmt.sample_rate = validated_sample_rate;
            pData->fmt.bits_per_sample = validated_bits_per_sample;
            pData->fmt.channels = validated_channels;
            pData->frame_count = (validated_output_samps / validated_channels);
            pInstance->continuous_error_count = 0;
            pInstance->continuous_sync_miss_count = 0;

            LOGI_3("mp3: channels %d, sr %d, bps %d, frame_count %d, processed %d",
                pData->fmt.channels,
                pData->fmt.sample_rate,
                pData->fmt.bits_per_sample,
                frame_info.outputSamps,
                starting_unread_bytes - unread_bytes);
        } else {
            if (pInstance->eof_reached) {
                ESP_LOGE(TAG, "status error %d, but EOF", mp3_dec_err);
                return DECODE_STATUS_DONE;
            } else if (mp3_dec_err == ERR_MP3_MAINDATA_UNDERFLOW) {
                // underflow indicates MP3Decode should be called again
                LOGI_1("underflow read ptr is 0x%p", read_ptr);
                return DECODE_STATUS_NO_DATA_CONTINUE;
            } else if (mp3_dec_err == ERR_MP3_INVALID_FRAMEHEADER ||
                       mp3_dec_err == ERR_MP3_FREE_BITRATE_SYNC) {
                pData->frame_count = 0;
                pInstance->continuous_sync_miss_count++;
                mp3_advance_after_failed_frame(pInstance, frame_start);
                ESP_LOGE(TAG, "frame sync error %d (count=%u)",
                         mp3_dec_err,
                         (unsigned)pInstance->continuous_sync_miss_count);
                if (pInstance->continuous_sync_miss_count >= MAX_MP3_SYNC_MISSES) {
                    ESP_LOGE(TAG, "too many MP3 sync misses, aborting track");
                    return DECODE_STATUS_ERROR;
                }
                return DECODE_STATUS_NO_DATA_CONTINUE;
            } else {
                // NOTE: some mp3 files result in misdetection of mp3 frame headers
                // and during decode these misdetected frames cannot be
                // decoded
                //
                // Rather than give up on the file by returning
                // DECODE_STATUS_ERROR, we ask the caller
                // to continue to call us, by returning DECODE_STATUS_NO_DATA_CONTINUE.
                //
                // The invalid frame data is skipped over as a search for the next frame
                // on the subsequent call to this function will start searching
                // AFTER the misdetected frmame header, dropping the invalid data.
                //
                // We may want to consider a more sophisticated approach here at a later time.
                pData->frame_count = 0;
                pInstance->continuous_error_count++;
                if (pInstance->read_ptr <= frame_start) {
                    mp3_advance_after_failed_frame(pInstance, frame_start);
                }
                ESP_LOGE(TAG, "status error %d (count=%u)",
                         mp3_dec_err,
                         (unsigned)pInstance->continuous_error_count);
                if (pInstance->continuous_error_count >= MAX_MP3_DECODE_ERRORS) {
                    ESP_LOGE(TAG, "too many MP3 decode errors, aborting track");
                    return DECODE_STATUS_ERROR;
                }
                return DECODE_STATUS_NO_DATA_CONTINUE;
            }
        }
    } else {
        // if we are dropping data there were no frames decoded
        pData->frame_count = 0;

        // drop an even count of words
        size_t words_to_drop = unread_bytes / BYTES_IN_WORD;
        size_t bytes_to_drop = words_to_drop * BYTES_IN_WORD;

        // if the unread bytes is less than BYTES_IN_WORD, we should drop any unread bytes
        // to avoid the situation where the file could have a few extra bytes at the end
        // of the file that isn't at least BYTES_IN_WORD and decoding would get stuck
        if(unread_bytes < BYTES_IN_WORD) {
            bytes_to_drop = unread_bytes;
        }

        // shift the read_ptr to drop the bytes in the buffer
        pInstance->read_ptr += bytes_to_drop;

        /* Sync word not found in frame. Drop data that was read until a word boundary */
        pInstance->continuous_sync_miss_count++;
        ESP_LOGE(TAG, "MP3 sync word not found, dropping %d bytes (count=%u)",
                 bytes_to_drop,
                 (unsigned)pInstance->continuous_sync_miss_count);
        if (pInstance->continuous_sync_miss_count >= MAX_MP3_SYNC_MISSES) {
            ESP_LOGE(TAG, "too many MP3 sync misses, aborting track");
            return DECODE_STATUS_ERROR;
        }
    }

    return DECODE_STATUS_CONTINUE;
}
