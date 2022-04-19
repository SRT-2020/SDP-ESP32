#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "/Users/stephentownsend/esp/esp-idf/examples/common_components/protocol_examples_common/include/protocol_examples_common.h"
#include "esp_sntp.h"
#include "time_sync.h"

static const char *TAG = "example";

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 48
#endif

/* Variable holding number of times ESP32 restarted since first boot.
 * It is placed into RTC memory using RTC_DATA_ATTR and
 * maintains its value when ESP32 wakes from deep sleep.
 */


void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

time_t obtain_time(void)
{
    ESP_ERROR_CHECK( esp_event_loop_create_default() );

    //ESP_ERROR_CHECK(example_connect());

    initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 15;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        // ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    time(&now);


    ESP_ERROR_CHECK( example_disconnect() );
    ESP_ERROR_CHECK(esp_event_loop_delete_default());


    return now;

}

void initialize_sntp(void)
{
    sntp_setoperatingmode(SNTP_OPMODE_POLL);

    sntp_setservername(0, "pool.ntp.org");

    sntp_set_time_sync_notification_cb(time_sync_notification_cb);

    sntp_init();
}



time_t sync_time(void)
{
    // ESP_LOGI(TAG, "Boot count: %d", boot_count);
    
    
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    // Is time set? If not, tm_year will be (1970 - 1900).
    now = obtain_time();
        // update 'now' variable with current time
    setenv("TZ", "EST5EDT,M3.2.0,M11.1.0", 1);
    tzset();

    return now;
    // char strftime_buf[64];

    // // Set timezone to Eastern Standard Time and print local time
    // setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
    // tzset();
    // localtime_r(&now, &timeinfo);
    // strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    // ESP_LOGI(TAG, "The current date/time in New York is: %s", strftime_buf);

    // ESP_LOGI(TAG, "Here is the thing: %d", timeinfo.tm_wday);

//     const int deep_sleep_sec = 10;
//     ESP_LOGI(TAG, "Entering deep sleep for %d seconds", deep_sleep_sec);
//     esp_deep_sleep(1000000LL * deep_sleep_sec);

}

