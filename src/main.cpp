#include <stdio.h>
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
#include <iostream>
using namespace std;
#include "Command.hpp"
using namespace ATE;

#define BAUDRATE (115200)                       // 统一波特率
#define UART_BUFFER_SIZE (1024)

#define CHIP_INFO_ENABLED (true)                // 是否启用启动时输出芯片等信息

#define QUEUE_LENGTH (20)                       // 队列长度
#define QUEUE_ITEM_SIZE (sizeof(uint32_t))      // 队列项数据大小

#define PATTERN_CHR_NUM (3)

// Uart结构体
struct UartConfig {
    const char *name;
    uart_port_t uartNum;
    int32_t baudrate;
    int8_t rxPin;
    int8_t txPin;
};

// 定义3个Uart结构体
static const UartConfig uarts[3] = 
{
    { "PC - MCU Json", UART_NUM_0, 115200, 3, 1 },          // PC vs MCU 通讯端口
    { "MCU - Module Json", UART_NUM_1, 115200, 5, 18 },     // MCU vs Module 通讯端口，Json格式指令
    { "MCU - Module Bytes", UART_NUM_2, 115200, 22, 23 },   // MCU vs Module 通讯端口，字节指令
};

// 对应3个Uart定义3个队列
static QueueHandle_t uartQueue0;
static QueueHandle_t uartQueue1;
static QueueHandle_t uartQueue2;

static void uartInit0(UartConfig uart) {	
    uart_config_t uart_config = {
        .baud_rate = uart.baudrate,					    //波特率
        .data_bits = UART_DATA_8_BITS,					//数据位
        .parity = UART_PARITY_DISABLE,					//校验位
        .stop_bits = UART_STOP_BITS_1,					//停止位
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,			//流控位
        .source_clk = UART_SCLK_APB,
    };

    uart_param_config(uart.uartNum, &uart_config);

    // Set UART pins	
    uart_set_pin(uart.uartNum, uart.txPin, uart.rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Install UART driver using an event queue here
    uart_driver_install(uart.uartNum, UART_BUFFER_SIZE * 2, UART_BUFFER_SIZE * 2, QUEUE_LENGTH, &uartQueue0, 0);

}

static void uartInit1(UartConfig uart) {	
    uart_config_t uart_config = {
        .baud_rate = uart.baudrate,					    //波特率
        .data_bits = UART_DATA_8_BITS,					//数据位
        .parity = UART_PARITY_DISABLE,					//校验位
        .stop_bits = UART_STOP_BITS_1,					//停止位
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,			//流控位
        .source_clk = UART_SCLK_APB,
    };

    uart_param_config(uart.uartNum, &uart_config);

    // Set UART pins	
    uart_set_pin(uart.uartNum, uart.txPin, uart.rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Install UART driver using an event queue here
    uart_driver_install(uart.uartNum, UART_BUFFER_SIZE * 2, UART_BUFFER_SIZE * 2, QUEUE_LENGTH, &uartQueue1, 0);

}

static void uartInit2(UartConfig uart) {	
    uart_config_t uart_config = {
        .baud_rate = uart.baudrate,					    //波特率
        .data_bits = UART_DATA_8_BITS,					//数据位
        .parity = UART_PARITY_DISABLE,					//校验位
        .stop_bits = UART_STOP_BITS_1,					//停止位
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,			//流控位
        .source_clk = UART_SCLK_APB,
    };

    uart_param_config(uart.uartNum, &uart_config);

    // Set UART pins	
    uart_set_pin(uart.uartNum, uart.txPin, uart.rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Install UART driver using an event queue here
    uart_driver_install(uart.uartNum, UART_BUFFER_SIZE * 2, UART_BUFFER_SIZE * 2, QUEUE_LENGTH, &uartQueue2, 0);
}

static void uartTask0(void *pvParameters) {
    UartConfig *uart = (UartConfig *)pvParameters;
    //ESP_LOGI("Uart Task", "uart[%d] txPin:[%d]", uart->uartNum, uart->txPin);
    //ESP_LOGI("uartTaskReceiver", "uart[%d] rxPin:[%d]", uart->uartNum, uart->rxPin);

    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(UART_BUFFER_SIZE);
    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(uartQueue0, (void * )&event, (TickType_t)portMAX_DELAY)) {
            bzero(dtmp, UART_BUFFER_SIZE);
            switch(event.type) {
                case UART_DATA:
                    //ESP_LOGI("Uart Task", "uart%d data", uart->uartNum);
                    uart_read_bytes(uart->uartNum, dtmp, event.size, portMAX_DELAY);

                    // Todo
                    uart_write_bytes(uart->uartNum, (const char*) dtmp, event.size);
                

                    break;
                case UART_FIFO_OVF:
                    uart_flush_input(uart->uartNum);
                    xQueueReset(uartQueue0);
                    break;
                case UART_BUFFER_FULL:
                    uart_flush_input(uart->uartNum);
                    xQueueReset(uartQueue0);
                    break;
                case UART_BREAK:
                    ESP_LOGI("Uart Task", "uart%d rx break", uart->uartNum);
                    break;
                case UART_PARITY_ERR:
                    ESP_LOGI("Uart Task", "uart%d parity error", uart->uartNum);
                    break;
                case UART_FRAME_ERR:
                    ESP_LOGI("Uart Task", "uart%d frame error", uart->uartNum);
                    break;
                default:
                    break;
            }
        }

    }

    free(dtmp);
    dtmp = NULL;

    vTaskDelete(NULL);

}

static void uartTask1(void *pvParameters) {
    UartConfig *uart = (UartConfig *)pvParameters;
    //ESP_LOGI("Uart Task", "uart[%d] txPin:[%d]", uart->uartNum, uart->txPin);
    //ESP_LOGI("Uart Task", "uart[%d] rxPin:[%d]", uart->uartNum, uart->rxPin);

    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(UART_BUFFER_SIZE);
    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(uartQueue1, (void * )&event, (TickType_t)portMAX_DELAY)) {
            bzero(dtmp, UART_BUFFER_SIZE);
            switch(event.type) {
                case UART_DATA:
                    //ESP_LOGI("Uart Task", "uart%d data", uart->uartNum);
                    uart_read_bytes(uart->uartNum, dtmp, event.size, portMAX_DELAY);
                    uart_write_bytes(uart->uartNum, (const char*) dtmp, event.size);
                    break;
                case UART_FIFO_OVF:
                    uart_flush_input(uart->uartNum);
                    xQueueReset(uartQueue1);
                    break;
                case UART_BUFFER_FULL:
                    uart_flush_input(uart->uartNum);
                    xQueueReset(uartQueue1);
                    break;
                case UART_BREAK:
                    ESP_LOGI("Uart Task", "uart%d rx break", uart->uartNum);
                    break;
                case UART_PARITY_ERR:
                    ESP_LOGI("Uart Task", "uart%d parity error", uart->uartNum);
                    break;
                case UART_FRAME_ERR:
                    ESP_LOGI("Uart Task", "uart%d frame error", uart->uartNum);
                    break;
                default:
                    break;
            }
        }

    }

    free(dtmp);
    dtmp = NULL;

    vTaskDelete(NULL);

}

static void uartTask2(void *pvParameters) {
    UartConfig *uart = (UartConfig *)pvParameters;
    //ESP_LOGI("Uart Task", "uart[%d] txPin:[%d]", uart->uartNum, uart->txPin);
    //ESP_LOGI("Uart Task", "uart[%d] rxPin:[%d]", uart->uartNum, uart->rxPin);

    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(UART_BUFFER_SIZE);
    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(uartQueue2, (void * )&event, (TickType_t)portMAX_DELAY)) {
            bzero(dtmp, UART_BUFFER_SIZE);
            switch(event.type) {
                case UART_DATA:
                    //ESP_LOGI("Uart Task", "uart%d data", uart->uartNum);
                    uart_read_bytes(uart->uartNum, dtmp, event.size, portMAX_DELAY);
                    uart_write_bytes(uart->uartNum, (const char*) dtmp, event.size);
                    break;
                case UART_FIFO_OVF:
                    uart_flush_input(uart->uartNum);
                    xQueueReset(uartQueue2);
                    break;
                case UART_BUFFER_FULL:
                    uart_flush_input(uart->uartNum);
                    xQueueReset(uartQueue2);
                    break;
                case UART_BREAK:
                    ESP_LOGI("Uart Task", "uart%d rx break", uart->uartNum);
                    break;
                case UART_PARITY_ERR:
                    ESP_LOGI("Uart Task", "uart%d parity error", uart->uartNum);
                    break;
                case UART_FRAME_ERR:
                    ESP_LOGI("Uart Task", "uart%d frame error", uart->uartNum);
                    break;
                default:
                    break;
            }
        }

    }

    free(dtmp);
    dtmp = NULL;

    vTaskDelete(NULL);

}

// void restart() {
//     printf("Restarting now.\n");

//     fflush(stdout);
//     esp_restart();
// }

extern "C" void app_main() {
    
    // 芯片信息
    if (CHIP_INFO_ENABLED) {
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

    // 队列初始化
    uartQueue0 = xQueueCreate(20, sizeof(UartConfig));
    uartQueue1 = xQueueCreate(20, sizeof(UartConfig));
    uartQueue2 = xQueueCreate(20, sizeof(UartConfig));

    // Uart初始化
    uartInit0(uarts[0]);
    uartInit1(uarts[1]);
    uartInit2(uarts[2]);

    // 创建Uart0任务
    if (uartQueue0 != NULL) {
        xTaskCreate(uartTask0, "Uart0 Task", 2048, (void *) &(uarts[0]), 11, NULL);
    } else {
        ESP_LOGI("Uart0 Init", "Uart0 init fail.\n");
    }

    // 创建Uart1任务
    if (uartQueue1 != NULL) {
        xTaskCreate(uartTask1, "Uart1 Task", 2048, (void *) &(uarts[1]), 12, NULL);
    } else {
        ESP_LOGI("Uart1 Init", "Uart1 init fail.\n");
    }

    // 创建Uart2任务
    if (uartQueue2 != NULL) {
        xTaskCreate(uartTask2, "Uart2 Task", 2048, (void *) &(uarts[2]), 13, NULL);
    } else {
        ESP_LOGI("Uart2 Init", "Uart2 init fail.\n");
    }
    
}

