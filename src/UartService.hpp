#ifndef UARTSERVICE_h
#define UARTSERVICE_h

#include <stdio.h>
#include "esp_log.h"
#include "Global.hpp"
#include "entity/Command.hpp"
using namespace ATE;
#include "entity/CmdBasic.hpp"
#include "entity/CmdInfo.hpp"
#include "entity/CmdReset.hpp"
#include "entity/CmdConfig.hpp"
#include "util/Util.hpp"

class UartService
{
private:
    // static Command command;

    static void mainUartInit(UartConfig uart);
    static void mainTaskEvent(void *pvParameters);

    static void subJsonUartInit(UartConfig uart);
    static void subJsonTaskEvent(void *pvParameters);

    static void subDataUartInit(UartConfig uart);
    static void subDataTaskEvent(void *pvParameters);

public:
    void MainTaskCreate();
    void SubJsonTaskCreate();
    void SubDataTaskCreate();

    UartService();
    ~UartService();
};

#endif