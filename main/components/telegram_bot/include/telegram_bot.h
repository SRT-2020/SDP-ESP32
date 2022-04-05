#ifndef TELEGRAM_BOT_H
#define TELEGRAM_BOT_H


#include "esp_http_client.h"
#include "esp_event.h"






void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void wifi_init_sta(void);
esp_err_t _http_event_handler(esp_http_client_event_t *evt);
void https_telegram_sendMessage_perform_post(void);
void http_test_task(void *pvParameters);
void send_to_bot(void);


#endif
