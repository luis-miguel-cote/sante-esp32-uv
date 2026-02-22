#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"

#include "secrets.h"
static const char *TAG = "UV_DEVICE";

static void wifi_init(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();

    ESP_LOGI(TAG, "Conectando a WiFi...");
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    wifi_init();

    vTaskDelay(pdMS_TO_TICKS(5000)); // Espera a que se establezca la conexión WiFi
    while (1)
    {
        ESP_LOGI(TAG, "ESP32 funcionando...");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}