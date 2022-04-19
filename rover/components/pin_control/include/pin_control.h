#ifndef PIN_CONTROL_H
#define PIN_CONTROL_H

#include "driver/gpio.h"
#include "driver/adc.h"
#include <stdbool.h>

int pinMode(int pinNum, gpio_mode_t mode);
int digitalWrite(int pinNum, uint32_t level);
int digitalRead(int pinNum);
int analogADC1Read(adc1_channel_t channel, bool useMultisampling, uint32_t totalSamples);
void analogADC1Setup(adc1_channel_t channel, adc_atten_t atten, adc_bits_width_t width);
#endif