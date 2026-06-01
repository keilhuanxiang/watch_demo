#include "lcd.h"

static const char *TAG = "esp32_s3_szp";

/* Keep each LVGL flush within a fixed row window so the SPI panel IO queue
 * sees a consistent transfer size that matches the draw buffer.
 */
#define BSP_LCD_DRAW_BUF_HEIGHT   (20)  // 20-line partial buffer keeps SPI flushes bounded
#define BSP_LCD_DRAW_BUF_PIXELS   (BSP_LCD_H_RES * BSP_LCD_DRAW_BUF_HEIGHT)
#define BSP_LCD_DRAW_BUF_BYTES    (BSP_LCD_DRAW_BUF_PIXELS * sizeof(uint16_t))

i2c_master_bus_handle_t i2c_bus_handle = NULL;
i2c_master_dev_handle_t pca9557_dev_handle = NULL;

/******************************************************************************/
/***************************  I2C 锟?*******************************************/
void bsp_i2c_init(void)
{
    i2c_master_bus_config_t i2c_conf = {
        .i2c_port = BSP_I2C_NUM,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .scl_io_num = BSP_I2C_SCL,
        .sda_io_num = BSP_I2C_SDA,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_conf, &i2c_bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = PCA9557_SENSOR_ADDR,
        .scl_speed_hz = BSP_I2C_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &dev_cfg, &pca9557_dev_handle));
}
/***************************  I2C 锟? *******************************************/
/*******************************************************************************/


/***********************************************************/
/***************    IO閹碘晛鐫嶉懞顖滃 锟?  *************************/

esp_err_t pca9557_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_transmit_receive(pca9557_dev_handle, &reg_addr, 1, data, len, 1000 / portTICK_PERIOD_MS);
}

esp_err_t pca9557_register_write_byte(uint8_t reg_addr, uint8_t data)
{
    uint8_t write_buf[2] = {reg_addr, data};

    return i2c_master_transmit(pca9557_dev_handle, write_buf, sizeof(write_buf), 1000 / portTICK_PERIOD_MS);
}

// 閸掓繂顫愰崠鏈燙A9557 IO閹碘晛鐫嶉懞顖滃
void pca9557_init(void)
{
    // 閸愭瑥鍙嗛幒褍鍩楀鏇″壖姒涙顓婚崐?DVP_PWDN=1  PA_EN = 0  LCD_CS = 1
    pca9557_register_write_byte(PCA9557_OUTPUT_PORT, 0x05);
    pca9557_register_write_byte(PCA9557_CONFIGURATION_PORT, 0xf8);
}

// 鐠佸墽鐤哖CA9557閼侯垳澧栭惃鍕厙娑撶嫪O瀵洝鍓兼潏鎾冲毉妤傛ü缍嗛悽闈涢挬
esp_err_t pca9557_set_output_state(uint8_t gpio_bit, uint8_t level)
{
    uint8_t data;
    esp_err_t res = ESP_FAIL;

    pca9557_register_read(PCA9557_OUTPUT_PORT, &data, 1);
    res = pca9557_register_write_byte(PCA9557_OUTPUT_PORT, SET_BITS(data, gpio_bit, level));

    return res;
}

// 閹貉冨煑 PCA9557_LCD_CS 瀵洝鍓兼潏鎾冲毉妤傛ü缍嗛悽闈涢挬 閸欏倹锟?鏉堟挸鍤担搴ｆ暩锟?閸欏倹锟?鏉堟挸鍤妯兼暩锟?
void lcd_cs(uint8_t level)
{
    pca9557_set_output_state(LCD_CS_GPIO, level);
}

// 閹貉冨煑 PCA9557_PA_EN 瀵洝鍓兼潏鎾冲毉妤傛ü缍嗛悽闈涢挬 閸欏倹锟?鏉堟挸鍤担搴ｆ暩锟?閸欏倹锟?鏉堟挸鍤妯兼暩锟?
void pa_en(uint8_t level)
{
    pca9557_set_output_state(PA_EN_GPIO, level);
}

// 閹貉冨煑 PCA9557_DVP_PWDN 瀵洝鍓兼潏鎾冲毉妤傛ü缍嗛悽闈涢挬 閸欏倹锟?鏉堟挸鍤担搴ｆ暩锟?閸欏倹锟?鏉堟挸鍤妯兼暩锟?
void dvp_pwdn(uint8_t level)
{
    pca9557_set_output_state(DVP_PWDN_GPIO, level);
}

/***************    IO閹碘晛鐫嶉懞顖滃 锟?  *************************/
/***********************************************************/


/***********************************************************/
/****************    LCD閺勫墽銇氱仦?锟?  *************************/

esp_err_t bsp_display_brightness_init(void)
{
    // Setup LEDC peripheral for PWM backlight control
    const ledc_channel_config_t LCD_backlight_channel = {
        .gpio_num = BSP_LCD_BACKLIGHT,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LCD_LEDC_CH,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = 1,
        .duty = 0,
        .hpoint = 0,
        .flags.output_invert = true
    };
    const ledc_timer_config_t LCD_backlight_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = 1,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK
    };

    ESP_ERROR_CHECK(ledc_timer_config(&LCD_backlight_timer));
    ESP_ERROR_CHECK(ledc_channel_config(&LCD_backlight_channel));

    return ESP_OK;
}

// 閼冲苯鍘滄禍顔煎鐠佸墽锟?
esp_err_t bsp_display_brightness_set(int brightness_percent)
{
    if (brightness_percent > 100) {
        brightness_percent = 100;
    } else if (brightness_percent < 0) {
        brightness_percent = 0;
    }

    ESP_LOGI(TAG, "Setting LCD backlight: %d%%", brightness_percent);
    // LEDC resolution set to 10bits, thus: 100% = 1023
    uint32_t duty_cycle = (1023 * brightness_percent) / 100;
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LCD_LEDC_CH, duty_cycle));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LCD_LEDC_CH));

    return ESP_OK;
}

// 閸忔娊妫撮懗灞藉帨
esp_err_t bsp_display_backlight_off(void)
{
    return bsp_display_brightness_set(0);
}

esp_err_t bsp_display_backlight_on(void)
{
    return bsp_display_brightness_set(100);
}

static esp_lcd_panel_handle_t panel_handle = NULL;
static esp_lcd_panel_io_handle_t io_handle = NULL;

static esp_err_t lcd_wait_pending_io(void)
{
    if (io_handle == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Synchronous tx_param call drains queued color transactions first. */
    return esp_lcd_panel_io_tx_param(io_handle, -1, NULL, 0);
}

// 濞戝弶娅犵仦蹇撳灥婵锟?
esp_err_t bsp_display_new(void)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_ERROR(bsp_display_brightness_init(), TAG, "Brightness init failed");
    // 閸掓繂顫愰崠鏈I閹崵锟?
    ESP_LOGD(TAG, "Initialize SPI bus");
    const spi_bus_config_t buscfg = {
        .sclk_io_num = BSP_LCD_SPI_CLK,
        .mosi_io_num = BSP_LCD_SPI_MOSI,
        .miso_io_num = GPIO_NUM_NC,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .max_transfer_sz = BSP_LCD_DRAW_BUF_BYTES,
    };
    ESP_RETURN_ON_ERROR(spi_bus_initialize(BSP_LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO), TAG, "SPI init failed");
    ESP_LOGD(TAG, "Install panel IO");
    const esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = BSP_LCD_DC,
        .cs_gpio_num = BSP_LCD_SPI_CS,
        .pclk_hz = BSP_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = 2,
        .trans_queue_depth = 3,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_LCD_SPI_NUM, &io_config, &io_handle), err, TAG, "New panel IO failed");
    // 閸掓繂顫愰崠鏍ㄦ伀閺呰泛鐫嗘す鍗炲З閼侯垳澧朣T7789
    ESP_LOGD(TAG, "Install LCD driver");
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = BSP_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,
        .bits_per_pixel = BSP_LCD_BITS_PER_PIXEL,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle), err, TAG, "New panel failed");

    esp_lcd_panel_reset(panel_handle);
    lcd_cs(0);
    esp_lcd_panel_init(panel_handle);  // 閸掓繂顫愰崠鏍帳缂冾喖鐦庣€涙ê锟?
    esp_lcd_panel_invert_color(panel_handle, true); // 妫版粏澹婇崣宥堟祮
    esp_lcd_panel_swap_xy(panel_handle, true);  // 閺勫墽銇氱紙鏄忔祮
    esp_lcd_panel_mirror(panel_handle, true, false); // 闂€婊冨剼

    return ret;

err:
    if (panel_handle) {
        esp_lcd_panel_del(panel_handle);
    }
    if (io_handle) {
        esp_lcd_panel_io_del(io_handle);
    }
    spi_bus_free(BSP_LCD_SPI_NUM);
    return ret;
}

esp_err_t bsp_lcd_init(void)
{
    esp_err_t ret = ESP_OK;

    ret = bsp_display_new(); // 濞戝弶娅犵仦 锟?
    lcd_set_color(0xffff); // 鐠佸墽鐤嗛弫鏉戠潌閼冲本娅欐鎴ｅ
    ret = esp_lcd_panel_disp_on_off(panel_handle, true);
    ret = bsp_display_backlight_on();

    return ret;
}

// 閺勫墽銇氶崶鍓у
void lcd_draw_pictrue(int x_start, int y_start, int x_end, int y_end, const unsigned char *gImage)
{
    size_t pixels_byte_size = (x_end - x_start) * (y_end - y_start) * 2;
    uint16_t *pixels = (uint16_t *)heap_caps_malloc(pixels_byte_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    if (pixels == NULL) {
        ESP_LOGE(TAG, "Memory for bitmap is not enough");
        return;
    }

    memcpy(pixels, gImage, pixels_byte_size);

    esp_err_t ret = esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_end, y_end, pixels);
    if (ret == ESP_OK) {
        ret = lcd_wait_pending_io();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "lcd_draw_pictrue failed: %s", esp_err_to_name(ret));
    }

    heap_caps_free(pixels);
}

void lcd_set_color(uint16_t color)
{
    uint16_t *buffer = (uint16_t *)heap_caps_malloc(BSP_LCD_H_RES * sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);

    if (buffer == NULL) {
        ESP_LOGE(TAG, "Memory for bitmap is not enough");
        return;
    }

    for (size_t i = 0; i < BSP_LCD_H_RES; i++) {
        buffer[i] = color;
    }

    esp_err_t ret = ESP_OK;
    for (int y = 0; y < BSP_LCD_V_RES; y++) {
        ret = esp_lcd_panel_draw_bitmap(panel_handle, 0, y, BSP_LCD_H_RES, y + 1, buffer);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "lcd_set_color draw failed at row %d: %s", y, esp_err_to_name(ret));
            break;
        }
    }

    if (ret == ESP_OK) {
        ret = lcd_wait_pending_io();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "lcd_set_color wait failed: %s", esp_err_to_name(ret));
        }
    }

    heap_caps_free(buffer);
}
/***************    LCD閺勫墽銇氱仦?锟?  *************************/
/***********************************************************/



/***********************************************************/
/****************    LVGL 锟?  *************************/

static esp_lcd_touch_handle_t tp;
static lv_disp_t *disp;
static lv_indev_t *disp_indev = NULL;
// 濞戝弶娅犵仦蹇撳灥婵锟?濞ｈ濮濴VGL閹恒儱锟?
static lv_disp_t *bsp_display_lcd_init(void)
{
    /* 閸掓繂顫愰崠鏍ㄦ伀閺呰泛锟?*/
    bsp_display_new(); // 濞戝弶娅犵仦蹇涒攳閸斻劌鍨垫慨瀣
    lcd_set_color(0xffff); // 鐠佸墽鐤嗛弫鏉戠潌閼冲本娅欓惂鍊熷
    esp_lcd_panel_disp_on_off(panel_handle, true); // 閹垫挸绱戝☉鍙夋珷鐏炲繑妯夌粈?
    /* 濞戝弶娅犵仦蹇斿潑閸旂嚕VGL閹恒儱锟?*/
    ESP_LOGD(TAG, "Add LCD screen");
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = BSP_LCD_DRAW_BUF_PIXELS,
        .double_buffer = false, // 鏄惁寮€鍚弻缂撳瓨
        .hres = BSP_LCD_H_RES,
        .vres = BSP_LCD_V_RES,
        .monochrome = false,
        .color_format = LV_COLOR_FORMAT_RGB565,
        .rotation = {
            .swap_xy = true,  // 閺勵垰鎯佺紙鏄忔祮
            .mirror_x = true, // x閺傜懓鎮滈弰顖氭儊闂€婊冨剼
            .mirror_y = false, // y閺傜懓鎮滈弰顖氭儊闂€婊冨剼
        },
        .flags = {
            .buff_dma = true,     // Keep LVGL buffers DMA-capable for direct SPI transfers
            .buff_spiram = true,  // Move LVGL draw buffers into PSRAM now that flush completion is stable
        }
    };

    return lvgl_port_add_disp(&disp_cfg);
}

// 鐟欙附鎳滅仦蹇撳灥婵锟?
esp_err_t bsp_touch_new(esp_lcd_touch_handle_t *ret_touch)
{
    ESP_RETURN_ON_FALSE(i2c_bus_handle != NULL, ESP_ERR_INVALID_STATE, TAG, "I2C bus not initialized");

    /* Initialize touch */
    esp_lcd_touch_config_t tp_cfg = {
        .x_max = BSP_LCD_V_RES,
        .y_max = BSP_LCD_H_RES,
        .rst_gpio_num = GPIO_NUM_NC, // Shared with LCD reset
        .int_gpio_num = GPIO_NUM_NC,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 1,
            .mirror_x = 1,
            .mirror_y = 0,
        },
    };
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_FT5x06_CONFIG();

    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(i2c_bus_handle, &tp_io_config, &tp_io_handle), TAG, "");
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_ft5x06(tp_io_handle, &tp_cfg, ret_touch));

    return ESP_OK;
}

// 鐟欙附鎳滅仦蹇撳灥婵锟?濞ｈ濮濴VGL閹恒儱锟?
static lv_indev_t *bsp_display_indev_init(lv_disp_t *disp)
{
    /* 閸掓繂顫愰崠鏍曢幗绋跨潌 */
    ESP_ERROR_CHECK(bsp_touch_new(&tp));
    assert(tp);

    /* 濞ｈ濮濴VGL閹恒儱锟?*/
    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = disp,
        .handle = tp,
    };

    return lvgl_port_add_touch(&touch_cfg);
}

void bsp_lvgl_start(void)
{
    /* 閸掓繂顫愰崠鏈橵GL */
    lvgl_port_cfg_t lvgl_cfg =
    {
        .task_priority = 4,
        .task_stack = 10240,
        .task_affinity = 0,
        .task_max_sleep_ms = 500,
        .task_stack_caps = MALLOC_CAP_INTERNAL | MALLOC_CAP_DEFAULT,
        .timer_period_ms = 5,
    };
    lvgl_port_init(&lvgl_cfg);

    /* 閸掓繂顫愰崠鏍ㄦ伀閺呰泛锟?楠炶埖鍧婇崝鐕GL閹恒儱锟?*/
    disp = bsp_display_lcd_init();

    /* 閸掓繂顫愰崠鏍曢幗绋跨潌 楠炶埖鍧婇崝鐕GL閹恒儱锟?*/
    disp_indev = bsp_display_indev_init(disp);

    /* 閹垫挸绱戝☉鍙夋珷鐏炲繗鍎楅崗?*/
    bsp_display_backlight_on();

}

/***************    LVGL 锟?  *************************/
/***********************************************************/
