#include <stdio.h>
#include "esp_http_client.h"
#include "protocol_examples_common.h"
#include "nvs_flash.h"
#include "messaging.h"
#include "esp_event.h"
#include "esp_log.h"

#define IP "192.168.1.7"


bool message_sent;

const char *TAG = "Message Handler";

static void obstacle_message(void *pvParameters)
{
    ESP_LOGI(TAG, "Sending Obstacle Message");
    char local_response_buffer[2048] = {0};

    esp_http_client_config_t config = {
        .host = "192.168.1.2",
        .port = 80,
        .path = "/obstacle/*",
        //.event_handler = _http_event_handler,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
        //.disable_auto_redirect = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
      
    char buf[32];

    sprintf(buf, "http://%s:80/obstacle", IP);


    esp_http_client_set_url(client, buf);
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_perform(client);

    message_sent = true;
    
    ESP_ERROR_CHECK(esp_event_loop_delete_default());
    ESP_ERROR_CHECK(example_disconnect());

    vTaskDelete(NULL);
}

static void ready_message(void *pvParameters)
{
    ESP_LOGI(TAG, "Sending Ready Message");

    char local_response_buffer[2048] = {0};

    esp_http_client_config_t config = {
        .host = IP,
        .port = 80,
        .path = "/ready/*",
        //.event_handler = _http_event_handler,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
        //.disable_auto_redirect = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    char buf[32];

    sprintf(buf, "http://%s:80/ready", IP);


    esp_http_client_set_url(client, buf);
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_perform(client);

    message_sent = true;

    ESP_ERROR_CHECK(esp_event_loop_delete_default());
    ESP_ERROR_CHECK(example_disconnect());

    vTaskDelete(NULL);
}

static void offline_message(void *pvParameters)
{
    ESP_LOGI(TAG, "Sending Off Line Message");

    char local_response_buffer[2048] = {0};

    esp_http_client_config_t config = {
        .host = IP,
        .port = 80,
        .path = "/off/*",
        //.event_handler = _http_event_handler,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
        //.disable_auto_redirect = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    char buf[32];

    sprintf(buf, "http://%s:80/off", IP);

    esp_http_client_set_url(client, buf);
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_perform(client);

    message_sent = true;

    ESP_ERROR_CHECK(esp_event_loop_delete_default());
    ESP_ERROR_CHECK(example_disconnect());

    vTaskDelete(NULL);
}

static void pickup_message(void *pvParameters)
{
    ESP_LOGI(TAG, "Sending Pickup Message");

    char local_response_buffer[2048] = {0};

    esp_http_client_config_t config = {
        .host = IP,
        .port = 80,
        .path = "/pickup/*",
        //.event_handler = _http_event_handler,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
        //.disable_auto_redirect = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    char buf[32];

    sprintf(buf, "http://%s:80/pickup", IP);


    esp_http_client_set_url(client, buf);
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_perform(client);

    message_sent = true;

    ESP_ERROR_CHECK(esp_event_loop_delete_default());
    ESP_ERROR_CHECK(example_disconnect());

    vTaskDelete(NULL);
}

void send_message(char *message)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    if(!strcmp(message, "obstacle")) xTaskCreate(&obstacle_message, "obstacle_message", 8192, NULL, 5, NULL);
    if(!strcmp(message, "ready")) xTaskCreate(&ready_message, "ready_message", 8192, NULL, 5, NULL);
    if(!strcmp(message, "offline")) xTaskCreate(&offline_message, "offline_message", 8192, NULL, 5, NULL);
    if(!strcmp(message, "pickup")) xTaskCreate(&pickup_message, "pickup_message", 8192, NULL, 5, NULL);
}
