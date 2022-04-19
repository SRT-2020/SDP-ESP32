#include <stdio.h>
#include "pin_control.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF 1100

gpio_config_t input_config;
gpio_config_t output_config;
bool ADC1Setup = false;

static esp_adc_cal_characteristics_t *adc_chars;

static const adc_unit_t unit = ADC_UNIT_1;

int pinMode(int pinNum, gpio_mode_t mode) {
	gpio_reset_pin(pinNum);
	return gpio_set_direction(pinNum, mode);
}

int digitalRead(int pinNum) {
	// WARNING: If consistently reading 0 then you might be reading from a pin that is not set to input or is both Input and Output
	return gpio_get_level(pinNum);
}

int digitalWrite(int pinNum, uint32_t level)
{
	if (level == 1 || level == 0) {
		return gpio_set_level(pinNum, level);
	}
	else {return -1;}
}

/* Taken from the adc1 example from the esp idf documentation */
static void check_efuse(void)
{
#if CONFIG_IDF_TARGET_ESP32
    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }
    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("Cannot retrieve eFuse Two Point calibration values. Default calibration values will be used.\n");
    }
#else
#error "This example is configured for ESP32/ESP32S2."
#endif
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

void analogADC1Setup(adc1_channel_t channel, adc_atten_t atten, adc_bits_width_t width) {
	/*
	 * This function will only setup ADC1
	 * It will configure attenuation and adc reading width for the given channel
	 * I recommend Attenuation = ADC_ATTEN_DB_6 for our photoresistors.
	 */
	check_efuse();
	adc1_config_width(width);
	adc1_config_channel_atten(channel, atten);
	adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
	esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
	print_char_val_type(val_type);
	ADC1Setup = true;
}

int analogADC1Read(adc1_channel_t channel, bool useMultisampling, uint32_t totalSamples) {
	/*
	 * This function is being used only on ADC1. ADC2 is reserved for WiFi
	 * There is functionality to multisample and return the average of the multi sampling result
	 */
	if (ADC1Setup) {
		gpio_num_t gpioPinNum;
		esp_err_t errorVal;
		errorVal = adc1_pad_get_io_num(channel, &gpioPinNum);
		assert(errorVal == ESP_OK);
		//printf("Reading from ADC1 Channel: %d @ GPIO Pin %d\n", channel, gpioPinNum);
		if (useMultisampling && totalSamples) {
			uint32_t adcReading = 0;
			for (int i = 0; i < totalSamples; i++) {
				adcReading += adc1_get_raw(channel);
			}
			adcReading /= totalSamples;
			return adcReading;
		}
		else {
			return adc1_get_raw(channel);
		}
	}
	else {printf("ADC1 has not been configured yet.");}
	return -1;
}