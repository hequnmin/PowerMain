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
        xTaskCreate(mainTaskEvent, "MainUart Task", 4096, (void *) &(G_UartMain), 11, NULL);
    } else {
        ESP_LOGI("UartService", "MainTaskCreate fail.\n");
    }
    
}

void UartService::mainTaskEvent(void *pvParameters)
{
    //Command cmd;

    UartConfig *uart = (UartConfig *)pvParameters;

    // Uart 驱动程序工作方式：
    // 1.检测到中断
    // 2.Uart等待120个字节（120个字节由驱动配置）
    // 3.Uart接收120个字节，传入缓存buffer，并触发事件，且设置event.size = 120，低级FIFO继续接收更多字节
    // 4.上游代码接收事件并读取120字节，缓存被清除
    // 5.再转入步骤2等待120个字节
    // 6.若接收到数据小于120个字节，因未达到120个字节阈值，不执行任何操作。
    // 7.继续监听中断
    // 8.返回步骤1

    uart_event_t event;
    
    uint8_t* dtmp = (uint8_t*) malloc(G_UART_BUFFER_SIZE);      // 接收数据
    uint8_t* dsub = (uint8_t*) malloc(G_UART_BUFFER_SIZE);      // 分段数据
    bzero(dsub, G_UART_BUFFER_SIZE);    // 清零接收数据

    int len = 0, sub = 0;
    int timeout = 50;    
    bool finish = false;

    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(G_QueueMain, (void * )&event, timeout / portTICK_RATE_MS)) {
            bzero(dsub, G_UART_BUFFER_SIZE);        // 清零分段数据
            switch(event.type) {
                case UART_DATA:
                    {
                        //uart_read_bytes(uart->uartNum, dtmp, event.size, portMAX_DELAY);
                        sub = uart_read_bytes(uart->uartNum, dsub, event.size, timeout / portTICK_RATE_MS);      

                        // 分段数据汇入总接收数据
                        if (event.size > 0) {
                            memcpy(dtmp+len,  dsub, sub);
                            len = len + sub;
                            // 检测结束符(回车换行符结尾)
                            if (dsub[event.size - 1] == 10 && dsub[event.size - 2] == 13) {
                                finish = true;
                            } else {
                                finish = false;
                            }
                        }

                        if (finish) {
                            finish = false;
                            
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
                                        case REQUEST_KEY_UART:
                                            {
                                                CmdUart cmdUart = CmdUart();
                                                REQUEST_BODY_UART* reqUart = new REQUEST_BODY_UART();
                                                RESPONSE_BODY_UART* resUart = new RESPONSE_BODY_UART();

                                                reqUart = cmdUart.Parse((const char*)dtmp);
                                                if (reqUart->err == NULL) {
                                                    cmdUart.Execute(reqUart, resUart);
                                                } else {
                                                    resUart->id = resBasic->id;
                                                    resUart->err = reqUart->err;
                                                    char* res = cmdUart.Print(resUart);
                                                    uart_write_bytes(uart->uartNum, (const char*)res, strlen(res));
                                                }
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
                                delete resBasic;

                            } else {
                                CmdError cmdError = CmdError();
                                RESPONSE_BODY_ERROR* resError = new RESPONSE_BODY_ERROR();
                                resError->err = "Illegal json.";
                                char* res = cmdError.Print(resError);
                                uart_write_bytes(uart->uartNum, (const char*) res, strlen(res));
                            }

                            bzero(dtmp, G_UART_BUFFER_SIZE);    // 清零接收数据
                            len = 0;
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

