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
#include "esp_crt_bundle.h"   
#include "secrets.h"
#include "esp_netif_sntp.h"
#include "driver/adc.h"


static const char *TAG = "UV_DEVICE";


static void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Inicializando SNTP...");

    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");

    esp_netif_sntp_init(&config);

    // Esperar a que sincronice
    time_t now = 0;
    struct tm timeinfo = {0};

    int retry = 0;
    const int retry_count = 10;

    while (timeinfo.tm_year < (2020 - 1900) && ++retry < retry_count)
    {
        ESP_LOGI(TAG, "Esperando sincronización de hora...");
        vTaskDelay(pdMS_TO_TICKS(2000));
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    ESP_LOGI(TAG, "Hora sincronizada");
}
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
        .crt_bundle_attach = esp_crt_bundle_attach,  // ← IMPORTANTE
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
static void adc_init(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);  // Resolución 0 - 4095
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11); 
    // ADC1_CHANNEL_6 = GPIO34
}
static float read_uv_sensor(void)
{
    int raw_sum = 0;

    for(int i = 0; i < 10; i++)
    {
        raw_sum += adc1_get_raw(ADC1_CHANNEL_6);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    int raw = raw_sum / 10;

    float voltage = (raw / 4095.0) * 3.3;

    float uv_index = voltage * 10.0;

    ESP_LOGI(TAG, "ADC Raw: %d Voltage: %.2f UV: %.2f", raw, voltage, uv_index);

    return uv_index;
}
void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
   

    wifi_init();

    vTaskDelay(pdMS_TO_TICKS(5000)); // Espera a que se establezca la conexión WiFi

     initialize_sntp();
     vTaskDelay(pdMS_TO_TICKS(5000));

     adc_init(); 

    while (1)
    {
        float uv_value = read_uv_sensor();

        ESP_LOGI(TAG, "Enviando UV: %.2f", uv_value);

        send_uv_data(uv_value);

        vTaskDelay(pdMS_TO_TICKS(180000)); // 3 minutos, esto se cambiar a futuro por un delay muy pequeño, para utilizar
        //contador par que sea mucho mas rapido, por ejemplo si se va el wifi, detecte de una que no hay conexion
        //reinicie el dispositivo o vuelva a tratar de conectarse
    }
}