#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "driver/i2s_std.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************/
/*********************    Audio Codec    *******************/
#define VOLUME_DEFAULT    60

#define CODEC_DEFAULT_SAMPLE_RATE          (48000)
#define CODEC_DEFAULT_BIT_WIDTH            (16)
#define CODEC_DEFAULT_ADC_VOLUME           (24.0)
#define CODEC_DEFAULT_CHANNEL              (2)

#define BSP_I2S_NUM                  I2S_NUM_1

#define GPIO_I2S_LRCK       (GPIO_NUM_13)
#define GPIO_I2S_MCLK       (GPIO_NUM_38)
#define GPIO_I2S_SCLK       (GPIO_NUM_14)
#define GPIO_I2S_SDIN       (GPIO_NUM_12)
#define GPIO_I2S_DOUT       (GPIO_NUM_45)
#define GPIO_PWR_CTRL       (GPIO_NUM_NC)

esp_err_t bsp_codec_init(void);
esp_err_t bsp_codec_deinit(void);
bool bsp_codec_is_initialized(void);
esp_err_t bsp_i2s_write(void *audio_buffer, size_t len, size_t *bytes_written,
                        uint32_t timeout_ms);
esp_err_t bsp_codec_set_fs(uint32_t rate, uint32_t bits_cfg, i2s_slot_mode_t ch);
esp_err_t bsp_codec_mute_set(bool enable);
esp_err_t bsp_codec_volume_set(int volume, int *volume_set);

/*********************    Audio Codec    *******************/
/***********************************************************/

#ifdef __cplusplus
}
#endif
