#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "pin_control.h"
#include "constants.h"
#include "analogWrite.h"
#include "driver/adc.h"
#include "time_control.h"
#include "motor_control.h"
#include <string.h>
#include <stdlib.h>
#include "driver/adc.h"
#include "Photo_Cell_Calibrator.h"
#include "delay.h"
#include "telegram_bot.h"
#include "esp_http_client.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include <sys/time.h>
#include "esp_log.h"
#include "esp_sleep.h"
#include "/Users/stephentownsend/esp/SDP-ESP32/main/components/time_sync/include/time_sync.h"

bool objectDetected = false;
bool towardsPickup = true;
bool messageSent = false;
bool atHome = true;
bool atPickup = false;
bool findAttempt = false;
bool wasRev = false;
bool wasForward = true;

// uint8_t RIGHT_PHOTO = 34;
// uint8_t LEFT_PHOTO = 39;
uint8_t LEFT_LED = 16;
uint8_t RIGHT_LED = 17;
uint8_t ULTRA_DIRECTION = 26;
uint8_t ULTRA_DETECTION = 36;
int motorSpeed = 200;
uint8_t tempPin = 12;

#include "esp_adc_cal.h"

/* Can use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/

//#define DEFAULT_VREF 1100

#define BLINK_GPIO 14
#define LED (GPIO_NUM_13)

void wait(int ms)
{
    vTaskDelay(ms/portTICK_PERIOD_MS);
}

void setup(void)
{
    pinMode(ULTRA_DETECTION, INPUT);
    setupMotors();

    //setupPhotoresistors(LEFT_LED, RIGHT_LED, ADC1_CHANNEL_3, ADC1_CHANNEL_6, 3, ADC_ATTEN_DB_11, ULTRA_DIRECTION);
}

photoVals photoValStruct = {
		.Left_Photo = -1,
		.Right_Photo = -1,
};



void app_main(void)
{

    while(1)
    {
        objectDetected = digitalRead(ULTRA_DETECTION);

        if(!objectDetected)
        {

            forward(motorSpeed);
            switch(trackLine(ADC1_CHANNEL_3, ADC1_CHANNEL_6, true, &photoValStruct, true))
            {
                case 1: // Left side triggered
                    digitalWrite(LEFT_LED, 1);
                    rotate_right(motorSpeed);
                    delay(200);
                    break;
                
                case 2: // Right side triggered
                    digitalWrite(RIGHT_LED, 1);
                    rotate_left(motorSpeed);
                    delay(200);
                    break;
                
                case -1: // Both off the line
                    stop();
                    delay(200); 
                    break;

                default: // Default;
                    forward(motorSpeed);
                    digitalWrite(LEFT_LED, 0);
                    digitalWrite(RIGHT_LED, 0);
                    break;
            }
        }

        else
        {
            stop();

            // int x = 0;
            // stop();
            // while(objectDetected)
            // {
            //     if(x >= 5000)
            //     {
            //         send_to_bot();
            //     }
            //     stop();
            //     objectDetected = digitalRead(ULTRA_DETECTION);
            //     x++;
            // }
        }
    }
} 

        
