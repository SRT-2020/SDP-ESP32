#include <stdio.h>
#include <ctype.h>
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
#include "Photo_Cell_Calibrator.h"
#include "motor_control.h"
#include "string.h"
#include "esp_log.h"
#include "messaging.h"
#include "time_sync.h"
#include "set_run_time.h"
#include <sys/time.h>
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_event.h"

bool objectDetected = false;
bool loadDetected = false;
bool towards_pickup;
bool offline_message_sent = false;
bool at_pickup;

bool done = false;
bool went = false;


// uint8_t RIGHT_PHOTO = 34;
// uint8_t LEFT_PHOTO = 39;
uint8_t LEFT_LED = 12;
uint8_t RIGHT_LED = 4;
uint8_t ULTRA_DIRECTION = 26;
uint8_t ULTRA_DETECTION = 36;
uint8_t LOAD_DETECTION = 25;
int motorSpeed = 255;
uint16_t obstacle_removal_wait_time = 10000; // In ms
uint16_t load_removal_wait_time = 5000; // In ms

RTC_DATA_ATTR int boot_count = -1;
int execution_buffer = 2;
int temp_day;
char temp_buf[3];

static const char *TAG = "Trash-E";

void wait(int ms)
{
    vTaskDelay(ms/portTICK_PERIOD_MS);
}

void setup(void)
{
    pinMode(ULTRA_DETECTION, INPUT);
    pinMode(LOAD_DETECTION, INPUT);
    //pinMode(ULTRA_DIRECTION, OUTPUT);

    setupMotors();

    //analogADC1Setup(ADC1_CHANNEL_3, ADC_ATTEN_DB_11, 3);
    //analogADC1Setup(ADC1_CHANNEL_6, ADC_ATTEN_DB_11, 3);
    setupPhotoresistors(LEFT_LED, RIGHT_LED, ADC1_CHANNEL_3, ADC1_CHANNEL_6, 3, ADC_ATTEN_DB_11, ULTRA_DIRECTION);

    //digitalWrite(ULTRA_DIRECTION, 0);
}

photoVals photoValStruct = {
		.Left_Photo = -1,
		.Right_Photo = -1,
};

struct Run_Time
{
    int run_day;
    int run_hour;
}run_time;

bool compare_times(void)
{
    time_t now;
    struct tm timeinfo;
    char strftime_buf[64];

    now = sync_time();
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "Amherst is: %s", strftime_buf);

    bool day = (timeinfo.tm_wday == run_time.run_day);
    bool time = execution_buffer >= (abs(run_time.run_hour - timeinfo.tm_hour));

    ESP_LOGI(TAG, "BOOLEAN DAY TIME %d %d", day, time);

    if(!(day&&time)) // Go to sleep
    {
        return false;
    }
    else
    {
        return true;
    }
}

int get_run_time(void)
{
    //bzero(time_buf, sizeof(time_buf));

    set_run_time();

    while(!get_user_input_done);

    ESP_LOGI(TAG, "Time_Buf Loaded");


    if(!(isdigit(time_buf[0]) || isdigit(time_buf[1]) || isdigit(time_buf[2])))
    {
        ESP_LOGE(TAG, "User input contain non-numbers");
    }

	ESP_LOGW(TAG, "%d", strcmp(time_buf, temp_buf));

	if(went && strcmp(time_buf, temp_buf))
	{
		ESP_LOGI(TAG, "Time Has Changed");
		strcpy(temp_buf, time_buf);
		went = false;
	}

	else strcpy(temp_buf, time_buf);

	

    char hour_str[2];
    bzero(hour_str, sizeof(hour_str));

    strncat(hour_str, &time_buf[1], 1);
    strncat(hour_str, &time_buf[2], 1);

    run_time.run_day = time_buf[0] - '0';
	
    run_time.run_hour = atoi(hour_str);

    if(run_time.run_day < 0 || run_time.run_day > 6)
    {
        ESP_LOGE(TAG, "Day input out of bounds");
        return -1;
    }

    if(run_time.run_hour < 0 || run_time.run_hour > 23)
    {
        ESP_LOGE(TAG, "Hour input out of bounds");
        return -1;
    }

    return 0;
}

void run_trashe() {

	while(towards_pickup)
	{
		//digitalWrite(ULTRA_DIRECTION, 1);
		objectDetected = digitalRead(ULTRA_DETECTION);

		switch(trackLine(ADC1_CHANNEL_3, ADC1_CHANNEL_6, true, &photoValStruct, towards_pickup))
		{
			case 1: // Left side triggered
				ESP_LOGI(TAG, "Left Side Triggered");
				digitalWrite(LEFT_LED, 1);
				stop();
				vTaskDelay(200/portTICK_RATE_MS);
				rotate_right(motorSpeed);
				vTaskDelay(500 / portTICK_RATE_MS);
				break;

			case 2: // Right side triggered
				ESP_LOGI(TAG, "Right Side Triggered");
				digitalWrite(RIGHT_LED, 1);
				stop();
				vTaskDelay(200 / portTICK_RATE_MS);
				rotate_left(motorSpeed);
				vTaskDelay(500 / portTICK_RATE_MS);
				break;
			case 3: // At Pickup Location
				stop();
				vTaskDelay(1000 / portTICK_RATE_MS);
				if (trackLine(ADC1_CHANNEL_3, ADC1_CHANNEL_6, true, &photoValStruct, towards_pickup) == 3) {
					digitalWrite(RIGHT_LED, 1);
					digitalWrite(LEFT_LED, 1);
					vTaskDelay(500 / portTICK_RATE_MS);
					digitalWrite(RIGHT_LED, 0);
					digitalWrite(LEFT_LED, 0);
					ESP_LOGI(TAG, "At Pickup Location");
					at_pickup = true;
					vTaskDelay(3000 / portTICK_RATE_MS);
				}
				break;
			case -1: // Both off the line
				stop();
				ESP_LOGI(TAG, "Off the Line");
				if(!offline_message_sent)
				{
					//send_message("offline");
				} 
				offline_message_sent = true;
				vTaskDelay(500 / portTICK_RATE_MS);
				break;

			default: // Normal Case, Trashe should keep moving towards its destination
				ESP_LOGI(TAG, "Moving Forward");
				forward(motorSpeed);
				digitalWrite(LEFT_LED, 0);
				digitalWrite(RIGHT_LED, 0);
				offline_message_sent = false;
				vTaskDelay(500 / portTICK_RATE_MS);
				break;
		}

		while(at_pickup)
		{
			// send at pickup message
			int startTime = millis();
			while (millis() - startTime < load_removal_wait_time) {
				loadDetected = digitalRead(LOAD_DETECTION);
				if (!loadDetected) {
					break;
				}
			}

			at_pickup = false;
			towards_pickup = false;
		}

		if(objectDetected)
		{
			stop();
			ESP_LOGI(TAG, "Obstacle Detected");
			send_message("obstacle");
			int startTime = millis();
			while(objectDetected)
			{
				objectDetected = digitalRead(ULTRA_DETECTION);
				if(millis() - startTime >= load_removal_wait_time)
				{
					towards_pickup = false;
					break;
				}
			}
		}
	}

	while(!towards_pickup)
	{
		//digitalWrite(ULTRA_DIRECTION, 0);
		objectDetected = digitalRead(ULTRA_DETECTION);

		switch(trackLine(ADC1_CHANNEL_3, ADC1_CHANNEL_6, true, &photoValStruct, false))
		{
			case 1: // Left side triggered
				ESP_LOGI(TAG, "Left Side Triggered");
				digitalWrite(LEFT_LED, 1);
				stop();
				vTaskDelay(200/portTICK_RATE_MS);
				rotate_left(motorSpeed);
				vTaskDelay(500 / portTICK_RATE_MS);
				break;

			case 2: // Right side triggered
				ESP_LOGI(TAG, "Right Side Triggered");
				digitalWrite(RIGHT_LED, 1);
				stop();
				vTaskDelay(200 / portTICK_RATE_MS);
				rotate_right(motorSpeed);
				vTaskDelay(500 / portTICK_RATE_MS);
				break;
			case 3:
				ESP_LOGI(TAG, "At Home");
				stop();
				//send done message
				done = true;
				return;
				break;
			case -1: // Both off the line
				ESP_LOGI(TAG, "Off the Line");
				stop();
				if(!offline_message_sent)
				{
					//send off line message
				}
				offline_message_sent = true;
				vTaskDelay(500 / portTICK_RATE_MS);
				break;

			default: // Normal Case, Trashe should keep moving towards its destination
				ESP_LOGI(TAG, "Moving in Reverse");
				reverse(motorSpeed);
				digitalWrite(LEFT_LED, 0);
				digitalWrite(RIGHT_LED, 0);
				offline_message_sent = false;
				vTaskDelay(500 / portTICK_RATE_MS);
				break;
		}

		if(objectDetected)
		{
			ESP_LOGI(TAG, "Obstacle Detected");
			stop();
			send_message("obstacle"); 
			while(objectDetected)
			{
				objectDetected = digitalRead(ULTRA_DETECTION);
			}
		}

		if(done)
		{
			ESP_LOGI(TAG, "Done");
			return;
		}
		

	}
}

bool message_sent = false;

void app_main(void)
{
	setup();

	ESP_ERROR_CHECK(nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
	
    ESP_ERROR_CHECK(example_connect());

	while(1)
	{

		ESP_LOGI(TAG, "Top of Loop");

		get_run_time();

		bool go = compare_times();

		if(go && !went)
		{
			send_message("ready");
			ESP_LOGI(TAG, "Running Trash-E");
			at_pickup = false;
			towards_pickup = true;
			done = false;
			run_trashe();
			went = true;
			temp_day = run_time.run_day;
		}

		ESP_LOGI(TAG, "Waiting for Time Update");


		wait(5000); // wait 30 seconds

	}
// 	
//     ESP_LOGI(TAG, "Entering deep sleep for %d seconds", deep_sleep_sec);
//     esp_deep_sleep(1000000LL * deep_sleep_sec);
} 
