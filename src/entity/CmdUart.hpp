#ifndef CMDUART_h
#define CMDUART_h
#include "Global.hpp"
#include "Command.hpp"
using namespace ATE;

class CmdUart
{
private:
    /* data */
public:
    CmdUart(/* args */);
    ~CmdUart();

    REQUEST_BODY_UART* Parse(const char* json);

    char* Print(RESPONSE_BODY_UART* res);
    char* Print(REQUEST_BODY_UART* req);

    // 执行指令
    void Execute(REQUEST_BODY_UART* req, RESPONSE_BODY_UART* res);


};

#endif