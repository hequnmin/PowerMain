#include "UartService.hpp"

UartService::UartService()
{

}

UartService::~UartService()
{
}


void UartService::mainUartInit(UartConfig uart) {	
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
    uart_driver_install(uart.uartNum, G_UART_BUFFER_SIZE * 2, G_UART_BUFFER_SIZE * 2, G_QUEUE_LENGTH, &G_QueueMain, 0);

}

void UartService::MainTaskCreate()
{

    mainUartInit(G_UartMain);

    // 创建任务
    if (G_QueueMain != NULL) {
        xTaskCreate(mainTaskEvent, "MainUart Task", 2048, (void *) &(G_UartMain), 11, NULL);
    } else {
        ESP_LOGI("UartService", "MainTaskCreate fail.\n");
    }
    
}

void UartService::mainTaskEvent(void *pvParameters)
{
    //Command cmd;

    UartConfig *uart = (UartConfig *)pvParameters;

    uart_event_t event;
    // size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(G_UART_BUFFER_SIZE);
    for(;;) {
        //Waiting for UART event.
        assert(dtmp);
        if(xQueueReceive(G_QueueMain, (void * )&event, (TickType_t)portMAX_DELAY)) {
            bzero(dtmp, G_UART_BUFFER_SIZE);
            switch(event.type) {
                case UART_DATA:
                    {
                        uart_read_bytes(uart->uartNum, dtmp, event.size, portMAX_DELAY);
                        // Todo
                        // uart_write_bytes(uart->uartNum, (const char*) dtmp, event.size);
                        // uart_write_bytes(G_UartSubJson.uartNum, (const char*) dtmp, event.size);
                        // uart_write_bytes(G_UartSubData.uartNum, (const char*) dtmp, event.size);
                        
                        // 检测是否Json
                        if (isJson((const char*)dtmp)) {
                            CmdBasic cmdBasic = CmdBasic();
                            REQUEST_BODY_BASIC* reqBasic = new REQUEST_BODY_BASIC();
                            RESPONSE_BODY_BASIC* resBasic = new RESPONSE_BODY_BASIC();
                            
                            reqBasic = cmdBasic.Parse((const char*)dtmp);
                            if (reqBasic->err == NULL) {
                                //uart_write_bytes(uart->uartNum, (const char*) dtmp, event.size);

                                cmdBasic.Execute(reqBasic, resBasic);
                                switch (reqBasic->key) {
                                    case REQUEST_KEY_INFO:
                                        {
                                            // uart_write_bytes(uart->uartNum, (const char*) dtmp, event.size);

                                            // 创建指令对象
                                            CmdInfo cmdInfo = CmdInfo();
                                            REQUEST_BODY_INFO* reqInfo = new REQUEST_BODY_INFO();
                                            RESPONSE_BODY_INFO* resInfo = new RESPONSE_BODY_INFO();
                                            
                                            // 解析指令
                                            reqInfo = cmdInfo.Parse((const char*)dtmp);
                                            if (reqInfo->err == NULL) {
                                                // uart_write_bytes(uart->uartNum, (const char*) dtmp, event.size);
                                                
                                                // 执行指令
                                                cmdInfo.Execute(reqInfo, resInfo);

                                                vTaskDelay(100 / portTICK_PERIOD_MS);
                                            } else {
                                                // 解析失败，直接返回错误
                                                resInfo->id = resBasic->id;
                                                resInfo->err = reqInfo->err;
                                                char* res = cmdInfo.Print(resInfo);
                                                uart_write_bytes(uart->uartNum, (const char*)res, strlen(res));
                                            }
                                            
                                            delete reqInfo;
                                            delete resInfo;
                                        }
                                        break;
                                    case REQUEST_KEY_RESET:
                                        {
                                            CmdReset cmdReset = CmdReset();
                                            REQUEST_BODY_RESET* reqReset = new REQUEST_BODY_RESET();
                                            RESPONSE_BODY_RESET* resReset = new RESPONSE_BODY_RESET();

                                            reqReset = cmdReset.Parse((const char*)dtmp);
                                            if (reqReset->err == NULL) {
                                                cmdReset.Execute(reqReset, resReset);
                                            } else {
                                                resReset->id = resBasic->id;
                                                resReset->err = reqReset->err;
                                                char* res = cmdReset.Print(resReset);
                                                uart_write_bytes(uart->uartNum, (const char*)res, strlen(res));
                                            }

                                            delete reqReset;
                                            delete resReset;
                                        }
                                        break;
                                    case REQUEST_KEY_CONFIG:
                                        {
                                            CmdConfig cmdConfig = CmdConfig();
                                            REQUEST_BODY_CONFIG* reqConfig = new REQUEST_BODY_CONFIG();
                                            RESPONSE_BODY_CONFIG* resConfig = new RESPONSE_BODY_CONFIG();

                                            reqConfig = cmdConfig.Parse((const char*)dtmp);
                                            if (reqConfig->err == NULL) {
                                                cmdConfig.Execute(reqConfig, resConfig);
                                            } else {
                                                cmdConfig.Error(reqConfig, resConfig);
                                            }

                                            delete reqConfig;
                                            delete resConfig;
                                        }
                                        break;
                                    default:
                                        break;
                                }
                            } else {
                                // ESP_LOGI("Debug", "cmdBasic.Parse Error... ");
                                resBasic->id = reqBasic->id;
                                resBasic->err = reqBasic->err;
                                char* res = cmdBasic.Print(resBasic);
                                uart_write_bytes(uart->uartNum, (const char*)res, strlen(res));
                            }
                            
                            delete reqBasic;
                            // delete resBasic;

                        } else {
                            uart_write_bytes(G_UartSubData.uartNum, (const char*) dtmp, event.size);
                        }
                    }
                    break;
                case UART_FIFO_OVF:
                    uart_flush_input(uart->uartNum);
                    xQueueReset(G_QueueMain);
                    break;
                case UART_BUFFER_FULL:
                    uart_flush_input(uart->uartNum);
                    xQueueReset(G_QueueMain);
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


void UartService::subJsonUartInit(UartConfig uart) {	
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
    uart_driver_install(uart.uartNum, G_UART_BUFFER_SIZE * 2, G_UART_BUFFER_SIZE * 2, G_QUEUE_LENGTH, &G_QueueSubJson, 0);

}

void UartService::SubJsonTaskCreate()
{
    subJsonUartInit(G_UartSubJson);

    // 创建任务
    if (G_QueueSubJson != NULL) {
        xTaskCreate(subJsonTaskEvent, "SubJsonUart Task", 2048, (void *) &(G_UartSubJson), 12, NULL);
    } else {
        ESP_LOGI("UartService", "SubJsonTaskCreate init fail.\n");
    }

}

void UartService::subJsonTaskEvent(void *pvParameters) {
    UartConfig *uart = (UartConfig *)pvParameters;
    //ESP_LOGI("Uart Task", "uart[%d] txPin:[%d]", uart->uartNum, uart->txPin);
    //ESP_LOGI("Uart Task", "uart[%d] rxPin:[%d]", uart->uartNum, uart->rxPin);

    uart_event_t event;
    // size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(G_UART_BUFFER_SIZE);
    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(G_QueueSubJson, (void * )&event, (TickType_t)portMAX_DELAY)) {
            bzero(dtmp, G_UART_BUFFER_SIZE);
            switch(event.type) {
                case UART_DATA:
                    //ESP_LOGI("Uart Task", "uart%d data", uart->uartNum);
                    uart_read_bytes(uart->uartNum, dtmp, event.size, portMAX_DELAY);

                    // uart_write_bytes(uart->uartNum, (const char*) dtmp, event.size);
                    //接受数据后，转发至主通讯端口
                    uart_write_bytes(G_UartMain.uartNum, (const char*) dtmp, event.size);

                    break;
                case UART_FIFO_OVF:
                    uart_flush_input(uart->uartNum);
                    xQueueReset(G_QueueSubJson);
                    break;
                case UART_BUFFER_FULL:
                    uart_flush_input(uart->uartNum);
                    xQueueReset(G_QueueSubJson);
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


void UartService::subDataUartInit(UartConfig uart) {	
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
    uart_driver_install(uart.uartNum, G_UART_BUFFER_SIZE * 2, G_UART_BUFFER_SIZE * 2, G_QUEUE_LENGTH, &G_QueueSubData, 0);
}

void UartService::SubDataTaskCreate()
{
    subDataUartInit(G_UartSubData);

    // 创建Uart2任务
    if (G_QueueSubData != NULL) {
        xTaskCreate(subDataTaskEvent, "Uart2 Task", 2048, (void *) &(G_UartSubData), 13, NULL);
    } else {
        ESP_LOGI("UartService", "SubDataTaskCreate fail.\n");
    }
}

void UartService::subDataTaskEvent(void *pvParameters) {
    UartConfig *uart = (UartConfig *)pvParameters;
    //ESP_LOGI("Uart Task", "uart[%d] txPin:[%d]", uart->uartNum, uart->txPin);
    //ESP_LOGI("Uart Task", "uart[%d] rxPin:[%d]", uart->uartNum, uart->rxPin);

    uart_event_t event;
    // size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(G_UART_BUFFER_SIZE);
    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(G_QueueSubData, (void * )&event, (TickType_t)portMAX_DELAY)) {
            bzero(dtmp, G_UART_BUFFER_SIZE);
            switch(event.type) {
                case UART_DATA:
                    //ESP_LOGI("Uart Task", "uart%d data", uart->uartNum);
                    uart_read_bytes(uart->uartNum, dtmp, event.size, portMAX_DELAY);

                    //uart_write_bytes(uart->uartNum, (const char*) dtmp, event.size);
                    //接受数据后，转发至主通讯端口
                    uart_write_bytes(G_UartMain.uartNum, (const char*) dtmp, event.size);

                    break;
                case UART_FIFO_OVF:
                    uart_flush_input(uart->uartNum);
                    xQueueReset(G_QueueSubData);
                    break;
                case UART_BUFFER_FULL:
                    uart_flush_input(uart->uartNum);
                    xQueueReset(G_QueueSubData);
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

