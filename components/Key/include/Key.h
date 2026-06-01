#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

extern QueueHandle_t gpio_evt_queue;  // 外部队列句柄

void Key_Init(void);
