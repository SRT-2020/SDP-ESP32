idf_component_register(SRCS "messaging.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_http_client protocol_examples_common nvs_flash)
