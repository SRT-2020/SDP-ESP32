#include "delay.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


void delay(int ms)
{
  vTaskDelay(ms/portTICK_PERIOD_MS);
}