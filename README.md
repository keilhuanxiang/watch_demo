# ESP32-S3 智能手表 Demo

基于 ESP-IDF、LVGL 和 ESP32-S3 的智能手表/便携终端示例工程。项目集成了 320x240 SPI LCD 触摸屏、SD 卡文件系统、音乐播放、Wi-Fi 扫描与连接、SNTP 时间同步、摄像头预览与拍照保存等功能，适合作为 ESP32-S3 多媒体交互设备的学习和二次开发模板。

## 功能特性

- LVGL 图形界面：包含主界面、菜单、文件、音乐、相机、Wi-Fi、设置等页面。
- 触摸屏输入：基于 FT5x06 触摸控制器。
- LCD 显示：ST7789 SPI 屏幕，分辨率 320x240，RGB565 色彩格式。
- SD 卡文件系统：通过 SDMMC 1-bit 模式挂载 `/sdcard`，支持目录浏览和文件过滤。
- 音乐播放：支持从 SD 卡播放 MP3、WAV、FLAC 文件，包含上一曲、下一曲、播放/暂停、音量调节、顺序/随机/单曲循环等逻辑。
- 音频输出：通过 I2S 与板载 codec/功放输出音频。
- Wi-Fi：支持扫描热点、连接/断开 Wi-Fi，并在联网后触发 SNTP 时间同步。
- RTC 时间管理：支持 RTC 缓存、NVS 缓存、编译时间兜底和 SNTP 校时。
- 摄像头：支持 QVGA RGB565 预览，并将当前帧保存为 BMP 到 SD 卡。
- 按键交互：主界面调节背光，音乐界面调节音量，相机界面触发拍照保存。

## 硬件环境

本工程面向 ESP32-S3 开发板，当前 `sdkconfig` 配置目标为 `esp32s3`，开启 PSRAM，Flash 大小为 16 MB。

主要外设连接如下：

| 模块 | 接口/芯片 | GPIO |
| --- | --- | --- |
| LCD | ST7789 / SPI3 | MOSI: GPIO40, CLK: GPIO41, DC: GPIO39, BL: GPIO42 |
| 触摸 | FT5x06 / I2C | SDA: GPIO1, SCL: GPIO2 |
| IO 扩展 | PCA9557 / I2C | I2C 地址: `0x19` |
| SD 卡 | SDMMC 1-bit | CMD: GPIO48, CLK: GPIO47, D0: GPIO21 |
| I2S 音频 | I2S0 | MCLK: GPIO38, BCLK: GPIO14, WS: GPIO13, DOUT: GPIO45 |
| 摄像头 | DVP | XCLK: GPIO5, PCLK: GPIO7, VSYNC: GPIO3, HREF: GPIO46, D0-D7: GPIO16/18/8/17/15/6/4/9 |

PCA9557 扩展 IO 当前用于控制：

- `LCD_CS`
- `PA_EN`
- `DVP_PWDN`

## 软件依赖

- ESP-IDF 5.x 建议环境
- CMake / Ninja
- Python 与 ESP-IDF 工具链
- 依赖组件由 ESP-IDF Component Manager 管理，主要包括：
  - `espressif/esp_lvgl_port`
  - `espressif/esp_lcd_touch_ft5x06`
  - `espressif/esp_codec_dev`
  - `espressif/esp32-camera`
  - `espressif/esp_image_effects`
  - `chmorgan/esp-audio-player`，本项目使用 `components/esp-audio-player` 本地覆盖版本

## 目录结构

```text
.
├── main/                    # app_main 入口和组件依赖清单
├── components/
│   ├── bsp_codec/           # 音频 codec/I2S 板级支持
│   ├── camera/              # 摄像头预览、保存 BMP
│   ├── event_loop/          # 应用事件循环
│   ├── Key/                 # 按键输入
│   ├── lcd/                 # LCD、触摸、LVGL port 初始化
│   ├── music_player/        # 音乐播放、播放列表、进度和 UI 同步
│   ├── rtc_time/            # 本地时间、NVS 缓存、SNTP 同步
│   ├── sdcard/              # SD 卡挂载、文件扫描、播放列表
│   ├── ui/                  # LVGL 生成界面和自定义 UI 逻辑
│   ├── ui_event/            # UI 事件处理
│   └── wifi_connect/        # Wi-Fi 扫描、连接、断开
├── managed_components/      # ESP-IDF 管理组件
├── scripts/idf-build.ps1    # Windows 下 ESP-IDF 构建辅助脚本
├── partitions.csv           # 分区表
├── sdkconfig                # 当前工程配置
└── CMakeLists.txt
```

## 编译与烧录

先确保已经安装并配置 ESP-IDF 环境，然后在项目根目录执行：

```bash
idf.py set-target esp32s3
idf.py build
idf.py -p COMx flash monitor
```

其中 `COMx` 请替换为实际串口号，例如 `COM5`。

Windows 环境也可以使用项目内脚本：

```powershell
.\scripts\idf-build.ps1 build
.\scripts\idf-build.ps1 -p COMx flash monitor
```

脚本会优先读取环境变量 `IDF_PATH`、`IDF_TOOLS_PATH`，也会尝试从 `.vscode/settings.json` 中解析 ESP-IDF 路径。

## 使用说明

1. 将 SD 卡格式化为 FAT 文件系统，并放入音乐、图片等文件。
2. 将 SD 卡插入开发板，烧录固件后启动。
3. 进入文件页面可浏览 SD 卡目录。
4. 进入音乐页面后选择音频文件播放，支持播放/暂停、上一曲/下一曲、音量和播放模式切换。
5. 进入 Wi-Fi 页面可扫描并连接热点，连接成功后会自动触发 SNTP 校时。
6. 进入相机页面可实时预览画面，按键可保存当前帧到 `/sdcard/camera/fra_xxxx.bmp`。

## 开发说明

- UI 文件主要由 GUI Guider/LVGL 生成，位于 `components/ui/generated`。
- 自定义 UI 逻辑位于 `components/ui/custom`、`components/ui_event`。
- 音乐播放通过应用事件触发，播放列表由 SD 卡目录扫描结果更新。
- 相机启动时会检查音乐播放和 Wi-Fi 状态，避免多外设同时占用资源导致运行不稳定。
- 工程大量使用 PSRAM 存放任务栈、图像缓冲和文件列表，目标板需要具备可用 PSRAM。

## 已知限制

- 硬件引脚与当前开发板绑定较强，移植到其他板卡时需要修改 `components/*/include` 下的 GPIO 定义。
- 音乐播放、Wi-Fi、摄像头等外设任务同时运行时资源压力较大，当前相机页面会主动限制后台音乐和 Wi-Fi。
- README 中的硬件信息根据当前代码和 `sdkconfig` 整理，如硬件版本有变化，请同步更新引脚表。

## License

本仓库未声明开源许可证。公开发布或二次分发前，建议补充明确的 `LICENSE` 文件。
