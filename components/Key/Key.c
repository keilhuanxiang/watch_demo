#include "Key.h"

QueueHandle_t gpio_evt_queue = NULL;  // 定义队列句柄

// GPIO中断服务函数
void gpio_isr_handler(void* arg) 
{
    uint32_t gpio_num = (uint32_t) arg;  // 获取入口参数
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL); // 把入口参数值发送到队列
}

void Key_Init(void)
{
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));  // 创建队列

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE, // 下降沿触发中断
        .mode = GPIO_MODE_INPUT,        // 设置为输入模式
        .pin_bit_mask = (1ULL << GPIO_NUM_0), // 选择要配置的GPIO引脚
        .pull_down_en = GPIO_PULLDOWN_DISABLE, // 禁止下拉
        .pull_up_en = GPIO_PULLUP_ENABLE       // 启用上拉
    };
    gpio_config(&io_conf); // 配置GPIO引脚
    // 创建GPIO中断服务
    gpio_install_isr_service(0);
    // 给GPIO0添加中断处理
    gpio_isr_handler_add(GPIO_NUM_0, gpio_isr_handler, (void*) GPIO_NUM_0);
}



