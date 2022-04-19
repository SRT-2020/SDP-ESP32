#include <stdio.h>
#include "time_control.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

unsigned long millis() {
	return (unsigned long) esp_timer_get_time() / 1000;
}