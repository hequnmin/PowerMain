#ifndef CMDBASIC_h
#define CMDBASIC_h

#include "Command.hpp"
using namespace ATE;
#include "esp_log.h"

class CmdBasic
{
private:
    /* data */
public:
    CmdBasic();
    ~CmdBasic();

    REQUEST_BODY_BASIC* Parse(const char* json);

    void Execute(REQUEST_BODY_BASIC* req, RESPONSE_BODY_BASIC* res);

    char* Print(RESPONSE_BODY_BASIC* res);
};


#endif