/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include "draw/lv_image_decoder_private.h"

#include <string.h>

typedef struct {
    lv_fs_file_t file;
    uint32_t px_offset;
    int32_t px_width;
    int32_t px_height;
    uint32_t bpp;
    int32_t row_size_bytes;
} bmp_compat_dsc_t;

static lv_result_t bmp_compat_info(lv_image_decoder_t *decoder,
                                   lv_image_decoder_dsc_t *dsc,
                                   lv_image_header_t *header);
static lv_result_t bmp_compat_open(lv_image_decoder_t *decoder,
                                   lv_image_decoder_dsc_t *dsc);
static lv_result_t bmp_compat_get_area(lv_image_decoder_t *decoder,
                                       lv_image_decoder_dsc_t *dsc,
                                       const lv_area_t *full_area,
                                       lv_area_t *decoded_area);
static void bmp_compat_close(lv_image_decoder_t *decoder,
                             lv_image_decoder_dsc_t *dsc);

void bmp_compat_decoder_init(void)
{
    static bool s_registered = false;
    lv_image_decoder_t *decoder;

    if (s_registered) {
        return;
    }

    decoder = lv_image_decoder_create();
    if (decoder == NULL) {
        return;
    }

    lv_image_decoder_set_info_cb(decoder, bmp_compat_info);
    lv_image_decoder_set_open_cb(decoder, bmp_compat_open);
    lv_image_decoder_set_get_area_cb(decoder, bmp_compat_get_area);
    lv_image_decoder_set_close_cb(decoder, bmp_compat_close);
    decoder->name = "BMP_COMPAT";

    s_registered = true;
}

static lv_result_t bmp_compat_info(lv_image_decoder_t *decoder,
                                   lv_image_decoder_dsc_t *dsc,
                                   lv_image_header_t *header)
{
    const char *file_name;
    uint8_t bmp_header[54];
    uint32_t width = 0;
    int32_t height = 0;
    uint16_t bpp = 0;

    LV_UNUSED(decoder);

    if (dsc->src_type != LV_IMAGE_SRC_FILE) {
        return LV_RESULT_INVALID;
    }

    file_name = dsc->src;
    if (lv_strcmp(lv_fs_get_ext(file_name), "bmp") != 0) {
        return LV_RESULT_INVALID;
    }

    lv_fs_read(&dsc->file, bmp_header, sizeof(bmp_header), NULL);
    if (bmp_header[0] != 0x42 || bmp_header[1] != 0x4d) {
        return LV_RESULT_INVALID;
    }

    lv_memcpy(&width, bmp_header + 18, sizeof(width));
    lv_memcpy(&height, bmp_header + 22, sizeof(height));
    lv_memcpy(&bpp, bmp_header + 28, sizeof(bpp));

    header->w = width;
    header->h = height < 0 ? (uint32_t)(-height) : (uint32_t)height;

    switch (bpp) {
    case 16:
        header->cf = LV_COLOR_FORMAT_RGB565;
        break;
    case 24:
        header->cf = LV_COLOR_FORMAT_RGB888;
        break;
    case 32:
        header->cf = LV_COLOR_FORMAT_ARGB8888;
        break;
    default:
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

static lv_result_t bmp_compat_open(lv_image_decoder_t *decoder,
                                   lv_image_decoder_dsc_t *dsc)
{
    const char *file_name;
    bmp_compat_dsc_t bmp_dsc;
    uint8_t bmp_header[54];
    lv_fs_res_t fs_res;

    LV_UNUSED(decoder);

    if (dsc->src_type != LV_IMAGE_SRC_FILE) {
        return LV_RESULT_INVALID;
    }

    file_name = dsc->src;
    if (lv_strcmp(lv_fs_get_ext(file_name), "bmp") != 0) {
        return LV_RESULT_INVALID;
    }

    lv_memzero(&bmp_dsc, sizeof(bmp_dsc));
    fs_res = lv_fs_open(&bmp_dsc.file, file_name, LV_FS_MODE_RD);
    if (fs_res != LV_FS_RES_OK) {
        return LV_RESULT_INVALID;
    }

    lv_fs_read(&bmp_dsc.file, bmp_header, sizeof(bmp_header), NULL);
    if (bmp_header[0] != 0x42 || bmp_header[1] != 0x4d) {
        lv_fs_close(&bmp_dsc.file);
        return LV_RESULT_INVALID;
    }

    lv_memcpy(&bmp_dsc.px_offset, bmp_header + 10, sizeof(bmp_dsc.px_offset));
    lv_memcpy(&bmp_dsc.px_width, bmp_header + 18, sizeof(bmp_dsc.px_width));
    lv_memcpy(&bmp_dsc.px_height, bmp_header + 22, sizeof(bmp_dsc.px_height));
    lv_memcpy(&bmp_dsc.bpp, bmp_header + 28, sizeof(bmp_dsc.bpp));
    bmp_dsc.row_size_bytes = (int32_t)(((bmp_dsc.bpp * (uint32_t)bmp_dsc.px_width + 31U) / 32U) * 4U);

    dsc->user_data = lv_malloc(sizeof(bmp_compat_dsc_t));
    if (dsc->user_data == NULL) {
        lv_fs_close(&bmp_dsc.file);
        return LV_RESULT_INVALID;
    }

    lv_memcpy(dsc->user_data, &bmp_dsc, sizeof(bmp_compat_dsc_t));
    return LV_RESULT_OK;
}

static lv_result_t bmp_compat_get_area(lv_image_decoder_t *decoder,
                                       lv_image_decoder_dsc_t *dsc,
                                       const lv_area_t *full_area,
                                       lv_area_t *decoded_area)
{
    bmp_compat_dsc_t *bmp_dsc = dsc->user_data;
    lv_draw_buf_t *decoded = (lv_draw_buf_t *)dsc->decoded;
    bool top_down;
    int32_t abs_height;
    int32_t y;
    uint32_t file_pos;
    uint32_t line_width_bytes;

    LV_UNUSED(decoder);

    if (bmp_dsc == NULL) {
        return LV_RESULT_INVALID;
    }

    if (decoded_area->y1 == LV_COORD_MIN) {
        int32_t width_px;
        lv_draw_buf_t *reshaped;

        *decoded_area = *full_area;
        decoded_area->y2 = decoded_area->y1;
        width_px = lv_area_get_width(full_area);

        reshaped = lv_draw_buf_reshape(decoded, dsc->header.cf, (uint32_t)width_px, 1,
                                       LV_STRIDE_AUTO);
        if (reshaped == NULL) {
            if (decoded != NULL) {
                lv_draw_buf_destroy(decoded);
            }
            decoded = lv_draw_buf_create((uint32_t)width_px, 1, dsc->header.cf,
                                         LV_STRIDE_AUTO);
            if (decoded == NULL) {
                dsc->decoded = NULL;
                return LV_RESULT_INVALID;
            }
        } else {
            decoded = reshaped;
        }

        dsc->decoded = decoded;
    } else {
        decoded_area->y1++;
        decoded_area->y2++;
    }

    if (decoded_area->y1 > full_area->y2) {
        return LV_RESULT_INVALID;
    }

    top_down = bmp_dsc->px_height < 0;
    abs_height = top_down ? -bmp_dsc->px_height : bmp_dsc->px_height;
    y = top_down ? decoded_area->y1 : ((abs_height - 1) - decoded_area->y1);
    file_pos = bmp_dsc->px_offset + (uint32_t)(bmp_dsc->row_size_bytes * y);
    file_pos += (uint32_t)decoded_area->x1 * (bmp_dsc->bpp / 8U);
    line_width_bytes = (uint32_t)lv_area_get_width(full_area) * (bmp_dsc->bpp / 8U);

    lv_fs_seek(&bmp_dsc->file, file_pos, LV_FS_SEEK_SET);
    lv_fs_read(&bmp_dsc->file, decoded->data, line_width_bytes, NULL);

    return LV_RESULT_OK;
}

static void bmp_compat_close(lv_image_decoder_t *decoder,
                             lv_image_decoder_dsc_t *dsc)
{
    bmp_compat_dsc_t *bmp_dsc = dsc->user_data;

    LV_UNUSED(decoder);

    if (bmp_dsc != NULL) {
        lv_fs_close(&bmp_dsc->file);
        lv_free(bmp_dsc);
    }

    if (dsc->decoded != NULL) {
        lv_draw_buf_destroy((lv_draw_buf_t *)dsc->decoded);
    }
}
