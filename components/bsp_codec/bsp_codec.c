#include "bsp_codec.h"

#include <assert.h>

#include "esp_check.h"
#include "esp_codec_dev.h"
#include "esp_codec_dev_defaults.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lcd.h"
#include "es7210_adc.h"
#include "es8311_codec.h"
#include "audio_codec_if.h"
#include "audio_codec_ctrl_if.h"
#include "audio_codec_data_if.h"
#include "audio_codec_gpio_if.h"

static const char *TAG = "bsp_codec";

static esp_codec_dev_handle_t play_dev_handle;
static esp_codec_dev_handle_t record_dev_handle;
static const audio_codec_if_t *speaker_codec_if = NULL;
static const audio_codec_if_t *microphone_codec_if = NULL;
static const audio_codec_ctrl_if_t *speaker_ctrl_if = NULL;
static const audio_codec_ctrl_if_t *microphone_ctrl_if = NULL;
static const audio_codec_gpio_if_t *codec_gpio_if = NULL;

static i2s_chan_handle_t i2s_tx_chan = NULL;
static i2s_chan_handle_t i2s_rx_chan = NULL;
static const audio_codec_data_if_t *i2s_data_if = NULL;
static SemaphoreHandle_t codec_mutex = NULL;
static bool codec_initialized = false;
static bool play_dev_opened = false;
static uint32_t play_dev_rate = 0;
static uint32_t play_dev_bits = 0;
static i2s_slot_mode_t play_dev_channel_mode = I2S_SLOT_MODE_STEREO;

typedef struct {
    const audio_codec_if_t      *codec_if;
    const audio_codec_data_if_t *data_if;
    const audio_codec_vol_if_t  *sw_vol;
    esp_codec_dev_type_t         dev_caps;
    bool                         input_opened;
    bool                         output_opened;
    int                          volume;
    float                        mic_gain;
    bool                         muted;
    bool                         mic_muted;
    bool                         sw_vol_alloced;
    esp_codec_dev_vol_curve_t    vol_curve;
    bool                         disable_when_closed;
} bsp_codec_dev_internal_t;

static esp_err_t bsp_codec_lock(void)
{
    if (codec_mutex == NULL) {
        codec_mutex = xSemaphoreCreateMutex();
        ESP_RETURN_ON_FALSE(codec_mutex != NULL, ESP_ERR_NO_MEM, TAG,
                            "Failed to create codec mutex");
    }
    BaseType_t ok = xSemaphoreTake(codec_mutex, portMAX_DELAY);
    return (ok == pdTRUE) ? ESP_OK : ESP_FAIL;
}

static void bsp_codec_unlock(void)
{
    if (codec_mutex != NULL) {
        xSemaphoreGive(codec_mutex);
    }
}

static esp_err_t bsp_codec_reconfigure_open_device(esp_codec_dev_handle_t handle,
                                                   const esp_codec_dev_sample_info_t *fs)
{
    bsp_codec_dev_internal_t *dev = (bsp_codec_dev_internal_t *)handle;
    ESP_RETURN_ON_FALSE(dev != NULL, ESP_ERR_INVALID_STATE, TAG,
                        "Invalid playback codec handle");
    ESP_RETURN_ON_FALSE(dev->data_if != NULL, ESP_ERR_INVALID_STATE, TAG,
                        "Playback codec data interface missing");

    esp_codec_dev_sample_info_t fs_local = *fs;

    if (dev->data_if->set_fmt) {
        int ret = dev->data_if->set_fmt(dev->data_if, dev->dev_caps, &fs_local);
        ESP_RETURN_ON_FALSE(ret == ESP_CODEC_DEV_OK, ESP_FAIL, TAG,
                            "Playback data fmt reconfig failed: %d", ret);
    }
    if (dev->data_if->enable) {
        int ret = dev->data_if->enable(dev->data_if, dev->dev_caps, true);
        ESP_RETURN_ON_FALSE(ret == ESP_CODEC_DEV_OK, ESP_FAIL, TAG,
                            "Playback data enable after reconfig failed: %d", ret);
    }
    if (dev->codec_if && dev->codec_if->set_fs) {
        int ret = dev->codec_if->set_fs(dev->codec_if, &fs_local);
        ESP_RETURN_ON_FALSE(ret == ESP_CODEC_DEV_OK, ESP_FAIL, TAG,
                            "Playback codec fs reconfig failed: %d", ret);
    }
    return ESP_OK;
}

/**
 * @brief 初始化I2S音频接口
 * @return ESP_OK 初始化成功，ESP_FAIL 初始化失败
 * @note 配置I2S为16kHz采样率、32位、立体声模式，初始化TX/RX通道
 *       创建I2S数据接口供编解码器使用
 */
static esp_err_t bsp_audio_init(void)
{
    esp_err_t ret = ESP_FAIL;
    if (i2s_tx_chan && i2s_rx_chan) {
        return ESP_OK;
    }

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(BSP_I2S_NUM, I2S_ROLE_MASTER);
    chan_cfg.auto_clear = true;
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &i2s_tx_chan, &i2s_rx_chan));

    const i2s_std_config_t std_cfg_default = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(32, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = GPIO_I2S_MCLK,
            .bclk = GPIO_I2S_SCLK,
            .ws = GPIO_I2S_LRCK,
            .dout = GPIO_I2S_DOUT,
            .din = GPIO_I2S_SDIN,
        },
    };

    if (i2s_tx_chan != NULL) {
        ESP_GOTO_ON_ERROR(i2s_channel_init_std_mode(i2s_tx_chan, &std_cfg_default),
                          err, TAG, "I2S TX channel initialization failed");
        // ESP_GOTO_ON_ERROR(i2s_channel_enable(i2s_tx_chan), err, TAG,
        //                   "I2S TX enabling failed");
    }
    if (i2s_rx_chan != NULL) {
        ESP_GOTO_ON_ERROR(i2s_channel_init_std_mode(i2s_rx_chan, &std_cfg_default),
                          err, TAG, "I2S RX channel initialization failed");
        ESP_GOTO_ON_ERROR(i2s_channel_enable(i2s_rx_chan), err, TAG,
                          "I2S RX enabling failed");
    }

    audio_codec_i2s_cfg_t i2s_cfg = {
        .port = BSP_I2S_NUM,
        .rx_handle = i2s_rx_chan,
        .tx_handle = i2s_tx_chan,
    };
    i2s_data_if = audio_codec_new_i2s_data(&i2s_cfg);
    if (i2s_data_if == NULL) {
        goto err;
    }

    return ESP_OK;

err:
    if (i2s_tx_chan) {
        i2s_del_channel(i2s_tx_chan);
        i2s_tx_chan = NULL;
    }
    if (i2s_rx_chan) {
        i2s_del_channel(i2s_rx_chan);
        i2s_rx_chan = NULL;
    }

    return ret;
}

static void bsp_audio_deinit(void)
{
    if (i2s_tx_chan != NULL) {
        i2s_channel_disable(i2s_tx_chan);
        i2s_del_channel(i2s_tx_chan);
        i2s_tx_chan = NULL;
    }
    if (i2s_rx_chan != NULL) {
        i2s_channel_disable(i2s_rx_chan);
        i2s_del_channel(i2s_rx_chan);
        i2s_rx_chan = NULL;
    }
    if (i2s_data_if != NULL) {
        audio_codec_delete_data_if(i2s_data_if);
        i2s_data_if = NULL;
    }
}

/**
 * @brief 初始化扬声器音频编解码器（ES8311）
 * @return 编解码器设备句柄
 * @note 配置ES8311为DAC模式，通过I2C控制，支持功放控制
 *       设置硬件增益参数，创建输出设备实例
 */
static esp_codec_dev_handle_t bsp_audio_codec_speaker_init(void)
{
    if (i2s_data_if == NULL) {
        ESP_ERROR_CHECK(bsp_audio_init());
    }
    assert(i2s_data_if);
    assert(i2c_bus_handle);

    if (codec_gpio_if == NULL) {
        codec_gpio_if = audio_codec_new_gpio();
    }

    audio_codec_i2c_cfg_t i2c_cfg = {
        .port = BSP_I2C_NUM,
        .addr = ES8311_CODEC_DEFAULT_ADDR,
        .bus_handle = i2c_bus_handle,
    };
    speaker_ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);
    assert(speaker_ctrl_if);

    esp_codec_dev_hw_gain_t gain = {
        .pa_voltage = 5.0,
        .codec_dac_voltage = 3.3,
    };

    es8311_codec_cfg_t es8311_cfg = {
        .ctrl_if = speaker_ctrl_if,
        .gpio_if = codec_gpio_if,
        .codec_mode = ESP_CODEC_DEV_WORK_MODE_DAC,
        .pa_pin = GPIO_PWR_CTRL,
        .pa_reverted = false,
        .master_mode = false,
        .use_mclk = true,
        .digital_mic = false,
        .invert_mclk = false,
        .invert_sclk = false,
        .hw_gain = gain,
    };
    speaker_codec_if = es8311_codec_new(&es8311_cfg);
    assert(speaker_codec_if);

    esp_codec_dev_cfg_t codec_dev_cfg = {
        .dev_type = ESP_CODEC_DEV_TYPE_OUT,
        .codec_if = speaker_codec_if,
        .data_if = i2s_data_if,
    };
    return esp_codec_dev_new(&codec_dev_cfg);
}

/**
 * @brief 初始化麦克风音频编解码器（ES7210）
 * @return 编解码器设备句柄
 * @note 配置ES7210为ADC模式，通过I2C控制，创建输入设备实例
 */
static esp_codec_dev_handle_t bsp_audio_codec_microphone_init(void)
{
    if (i2s_data_if == NULL) {
        ESP_ERROR_CHECK(bsp_audio_init());
    }
    assert(i2s_data_if);
    assert(i2c_bus_handle);

    audio_codec_i2c_cfg_t i2c_cfg = {
        .port = BSP_I2C_NUM,
        .addr = 0x82,
        .bus_handle = i2c_bus_handle,
    };
    microphone_ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);
    assert(microphone_ctrl_if);

    es7210_codec_cfg_t es7210_cfg = {
        .ctrl_if = microphone_ctrl_if,
    };
    microphone_codec_if = es7210_codec_new(&es7210_cfg);
    assert(microphone_codec_if);

    esp_codec_dev_cfg_t codec_dev_cfg = {
        .dev_type = ESP_CODEC_DEV_TYPE_IN,
        .codec_if = microphone_codec_if,
        .data_if = i2s_data_if,
    };
    return esp_codec_dev_new(&codec_dev_cfg);
}

/**
 * @brief 设置音频编解码器采样参数
 * @param rate 采样率（如16000）
 * @param bits_cfg 位深度（如16）
 * @param ch 通道模式（单声道/立体声）
 * @return ESP_OK 设置成功
 * @note 重新配置播放设备的采样率、位深度和通道数
 */
esp_err_t bsp_codec_set_fs(uint32_t rate, uint32_t bits_cfg, i2s_slot_mode_t ch)
{
    ESP_RETURN_ON_FALSE(rate > 0, ESP_ERR_INVALID_ARG, TAG, "Invalid sample rate");
    ESP_RETURN_ON_FALSE(bits_cfg > 0, ESP_ERR_INVALID_ARG, TAG, "Invalid bits config");
    ESP_RETURN_ON_FALSE(play_dev_handle != NULL, ESP_ERR_INVALID_STATE, TAG,
                        "Playback codec not initialized");

    esp_err_t ret = bsp_codec_lock();
    if (ret != ESP_OK) {
        return ret;
    }

    int channels = (ch == I2S_SLOT_MODE_MONO) ? 1 : 2;

    esp_codec_dev_sample_info_t fs = {
        .sample_rate = (int)rate,
        .channel = channels,
        .bits_per_sample = (int)bits_cfg,
    };

    if (play_dev_opened &&
        play_dev_rate == rate &&
        play_dev_bits == bits_cfg &&
        play_dev_channel_mode == ch) {
        bsp_codec_unlock();
        return ESP_OK;
    }

    if (play_dev_opened == false) {
        int open_ret = esp_codec_dev_open(play_dev_handle, &fs);
        ret = (open_ret == ESP_CODEC_DEV_OK) ? ESP_OK : ESP_FAIL;
        if (ret == ESP_OK) {
            play_dev_opened = true;
        }
    } else {
        ret = bsp_codec_reconfigure_open_device(play_dev_handle, &fs);
    }

    if (ret == ESP_OK) {
        play_dev_rate = rate;
        play_dev_bits = bits_cfg;
        play_dev_channel_mode = ch;
    }

    bsp_codec_unlock();
    return ret;
}

/**
 * @brief 初始化音频编解码器模块
 * @return ESP_OK 初始化成功
 * @note 同时初始化扬声器（ES8311）和麦克风（ES7210）编解码器
 *       并设置默认采样参数
 */
esp_err_t bsp_codec_init(void)
{
    if (codec_initialized) {
        return ESP_OK;
    }

    if (codec_mutex == NULL) {
        codec_mutex = xSemaphoreCreateMutex();
        ESP_RETURN_ON_FALSE(codec_mutex != NULL, ESP_ERR_NO_MEM, TAG,
                            "Failed to create codec mutex");
    }

    play_dev_handle = bsp_audio_codec_speaker_init();
    assert(play_dev_handle && "play_dev_handle not initialized");

    record_dev_handle = bsp_audio_codec_microphone_init();
    assert(record_dev_handle && "record_dev_handle not initialized");

    play_dev_opened = false;
    play_dev_rate = 0;
    play_dev_bits = 0;
    play_dev_channel_mode = I2S_SLOT_MODE_STEREO;
    esp_err_t ret = bsp_codec_set_fs(CODEC_DEFAULT_SAMPLE_RATE, CODEC_DEFAULT_BIT_WIDTH,
                                     CODEC_DEFAULT_CHANNEL);
    if (ret == ESP_OK) {
        codec_initialized = true;
    }
    return ret;
}

esp_err_t bsp_codec_deinit(void)
{
    esp_err_t ret = bsp_codec_lock();
    if (ret != ESP_OK) {
        return ret;
    }

    if (play_dev_handle != NULL) {
        esp_codec_dev_close(play_dev_handle);
        esp_codec_dev_delete(play_dev_handle);
        play_dev_handle = NULL;
    }
    if (record_dev_handle != NULL) {
        esp_codec_dev_close(record_dev_handle);
        esp_codec_dev_delete(record_dev_handle);
        record_dev_handle = NULL;
    }

    if (speaker_codec_if != NULL) {
        audio_codec_delete_codec_if(speaker_codec_if);
        speaker_codec_if = NULL;
    }
    if (microphone_codec_if != NULL) {
        audio_codec_delete_codec_if(microphone_codec_if);
        microphone_codec_if = NULL;
    }
    if (speaker_ctrl_if != NULL) {
        audio_codec_delete_ctrl_if(speaker_ctrl_if);
        speaker_ctrl_if = NULL;
    }
    if (microphone_ctrl_if != NULL) {
        audio_codec_delete_ctrl_if(microphone_ctrl_if);
        microphone_ctrl_if = NULL;
    }
    if (codec_gpio_if != NULL) {
        audio_codec_delete_gpio_if(codec_gpio_if);
        codec_gpio_if = NULL;
    }

    bsp_audio_deinit();

    play_dev_opened = false;
    play_dev_rate = 0;
    play_dev_bits = 0;
    play_dev_channel_mode = I2S_SLOT_MODE_STEREO;
    codec_initialized = false;

    bsp_codec_unlock();
    return ESP_OK;
}

bool bsp_codec_is_initialized(void)
{
    return codec_initialized;
}

/**
 * @brief 向I2S写入音频数据（播放）
 * @param audio_buffer 音频数据缓冲区
 * @param len 数据长度（字节）
 * @param bytes_written 输出参数，实际写入的字节数
 * @param timeout_ms 超时时间（当前未使用）
 * @return ESP_OK 写入成功
 * @note 通过播放设备句柄将音频数据写入编解码器
 */
esp_err_t bsp_i2s_write(void *audio_buffer, size_t len, size_t *bytes_written,
                        uint32_t timeout_ms)
{
    esp_err_t lock_ret = bsp_codec_lock();
    if (lock_ret != ESP_OK) {
        if (bytes_written) {
            *bytes_written = 0;
        }
        return lock_ret;
    }

    esp_err_t ret = esp_codec_dev_write(play_dev_handle, audio_buffer, len);

    bsp_codec_unlock();
    if (bytes_written) {
        *bytes_written = (ret == ESP_OK) ? len : 0;
    }
    return ret;
}

/**
 * @brief 设置扬声器静音状态
 * @param enable true 静音，false 取消静音
 * @return ESP_OK 设置成功
 */
esp_err_t bsp_codec_mute_set(bool enable)
{
    esp_err_t ret = bsp_codec_lock();
    if (ret != ESP_OK) {
        return ret;
    }
    ret = esp_codec_dev_set_out_mute(play_dev_handle, enable);
    bsp_codec_unlock();
    return ret;
}

/**
 * @brief 设置扬声器音量
 * @param volume 音量值（范围取决于编解码器）
 * @param volume_set 输出参数，返回实际设置的音量值
 * @return ESP_OK 设置成功
 */
esp_err_t bsp_codec_volume_set(int volume, int *volume_set)
{
    esp_err_t ret = bsp_codec_lock();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Skip volume update while codec is busy");
        return ret;
    }
    ret = esp_codec_dev_set_out_vol(play_dev_handle, (float)volume);
    bsp_codec_unlock();
    if (ret == ESP_OK && volume_set) {
        *volume_set = volume;
    }
    return ret;
}
