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

//#define GPIO_OUTPUT_IO_1    CONFIG_GPIO_OUTPUT_1
#define GPIO_BIT_MASK  ((1ULL<<GPIO_NUM_2) | (1ULL<<GPIO_NUM_4)) 

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

    // // GPIO配置
    // gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);       // 设置引脚2为输入
    // gpio_set_level(GPIO_NUM_2, 1);                         // 设置引脚2为高电平

    // // GPIO初始化
    // gpio_config_t io_conf = {};
    // io_conf.intr_type = GPIO_INTR_DISABLE;          //禁用中断
    // io_conf.mode = GPIO_MODE_INPUT;                 //输入模式
    // io_conf.pull_up_en = GPIO_PULLUP_ENABLE;        //使能上拉
    // io_conf.pin_bit_mask = (1ULL<<GPIO_NUM_2) | (1ULL<<GPIO_NUM_4);
    // gpio_config(&io_conf);                          //配置GPIO
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // gpio_set_level(GPIO_NUM_2, 1);                  //设置GPIO2电平

    G_QueueMain = xQueueCreate(5, sizeof(UartConfig));
    G_QueueSubJson = xQueueCreate(5, sizeof(UartConfig));
    G_QueueSubData = xQueueCreate(5, sizeof(UartConfig));
    
    UartService uartService = UartService();
    uartService.MainTaskCreate();
    uartService.SubJsonTaskCreate();
    uartService.SubDataTaskCreate();

}

