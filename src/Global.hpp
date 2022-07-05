#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"

#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "esp_wifi.h"

#ifndef GLOBAL_h
#define GLOBAL_h

#define G_BAUDRATE (115200)                       // 统一波特率
#define G_UART_BUFFER_SIZE (1024)

#define G_CHIP_INFO_ENABLED (false)                // 是否启用启动时输出芯片等信息

#define G_QUEUE_LENGTH (20)                       // 队列长度
#define G_QUEUE_ITEM_SIZE (sizeof(uint32_t))      // 队列项数据大小

#define G_PATTERN_CHR_NUM (3)

// Uart结构体
struct UartConfig {
    int8_t id;
    const char *name;
    uart_port_t uartNum;
    int32_t baudrate;
    int8_t rxPin;
    int8_t txPin;
};

// 定义3个Uart结构体
static const UartConfig G_UartMain = { 0, "PC - MCU Json", UART_NUM_0, G_BAUDRATE, 3, 1 };             // 主通讯端口，与PC交互
static const UartConfig G_UartSubJson = { 1, "MCU - Module Json", UART_NUM_1, G_BAUDRATE, 5, 18 };     // 子通讯端口(JSON模块通讯端口)，与JSON接口MCU交互
static const UartConfig G_UartSubData = { 2, "MCU - Module Data", UART_NUM_2, G_BAUDRATE, 22, 23 };    // 子通讯端口(数据报模块通讯端口)，与数据报接口MCU交互

// 对应3个Uart定义3个队列
static QueueHandle_t G_QueueMain;
static QueueHandle_t G_QueueSubJson;
static QueueHandle_t G_QueueSubData;

// 硬件模组结构体
struct McuConfig {
    int8_t id;
    const char *name;
    uart_port_t uartNum;
    bool isJson;
};

// 硬件模组配置
static const uint8_t G_MCU_COUNT = 8;
static const McuConfig G_MCU[8] = {
    { 0, "main", UART_NUM_0, true },
    { 1, "ps1", UART_NUM_1, true },
    { 2, "ps2", UART_NUM_1, true },
    { 3, "ps3", UART_NUM_1, true },
    { 4, "ps4", UART_NUM_1, true },
    { 5, "ps5", UART_NUM_1, true },
    { 6, "ps6", UART_NUM_2, false },
    { 7, "ps7", UART_NUM_2, false }
};

#endif
