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

static void send_uv_data(float uv_value)
{
    char post_data[200];

    sprintf(post_data,
            "{\"api_key\":\"%s\",\"valor_uv\":%.2f}",
            API_KEY,
            uv_value);

    esp_http_client_config_t config = {
        .url = API_URL,
        .method = HTTP_METHOD_POST,

        
        .skip_cert_common_name_check = true,
        .cert_pem = NULL,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP Status = %d",
                 esp_http_client_get_status_code(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTP POST failed: %s",
                 esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    wifi_init();

    vTaskDelay(pdMS_TO_TICKS(5000)); // Espera a que se establezca la conexión WiFi
    while (1)
    {
        float uv_value = 7.5; // Simulado por ahora

        ESP_LOGI(TAG, "Enviando UV: %.2f", uv_value);

        send_uv_data(uv_value);

        vTaskDelay(pdMS_TO_TICKS(180000)); // 3 minutos
    }
}