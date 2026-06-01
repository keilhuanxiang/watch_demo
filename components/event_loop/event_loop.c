#include <stdio.h>
#include "event_loop.h"

ESP_EVENT_DEFINE_BASE(APP_EVENT);

esp_event_loop_handle_t   ui_event_loop_handle;

void  ui_event_loop_init(void)
{
    esp_event_loop_args_t loop_args = {
        .queue_size = 8,
        .task_name = "ui_event_loop",
        .task_priority = 5,
        .task_stack_size = 5632,
        .task_core_id = 1
    };
    
    ESP_ERROR_CHECK(esp_event_loop_create(&loop_args, &ui_event_loop_handle));
}
