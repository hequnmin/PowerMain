#include <stdio.h>
using namespace std;
#include <iostream>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "nvs_flash.h"
#include "Global.hpp"

#include "UartService.hpp"

extern "C" {
	void app_main(void);
}

void app_main(void) {
    
    // 芯片信息
    if (G_CHIP_INFO_ENABLED) {
        esp_chip_info_t chip_info;
        esp_chip_info(&chip_info);
        ESP_LOGI("Chip Info", "This is %s chip with %d CPU core(s), WiFi%s%s, ",
                CONFIG_IDF_TARGET,
                chip_info.cores,
                (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
                (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

        ESP_LOGI("Chip Info", "silicon revision %d, ", chip_info.revision);

        ESP_LOGI("Chip Info", "%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
                (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

        ESP_LOGI("Chip Info", "Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

        for (int i = 2; i >= 0; i--) {
            ESP_LOGI("Chip Info", "Restarting in %d seconds...\n", i);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        ESP_LOGI("Chip Info", "Welcome to SerialServo!\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    G_QueueMain = xQueueCreate(20, sizeof(UartConfig));
    G_QueueSubJson = xQueueCreate(20, sizeof(UartConfig));
    G_QueueSubData = xQueueCreate(20, sizeof(UartConfig));
    
    UartService uartService = UartService();
    uartService.MainTaskCreate();
    uartService.SubJsonTaskCreate();
    uartService.SubDataTaskCreate();

}

