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
void app_main(void)
{
    while (1)
    {
        printf("ESP32 funcionando...\n");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}