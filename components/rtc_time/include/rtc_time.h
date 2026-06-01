#pragma once

#include <stdbool.h>
#include <time.h>

void rtc_time_init(void);
bool rtc_time_get_local(struct tm *timeinfo);
