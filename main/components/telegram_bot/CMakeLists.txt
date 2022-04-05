idf_component_register(SRCS "telegram_bot.c"
                    INCLUDE_DIRS "include"
                    REQUIRES nvs_flash esp-tls esp_http_client
                    EMBED_TXTFILES telegram_certificate.pem)

