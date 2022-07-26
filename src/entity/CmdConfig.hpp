#ifndef SPIFFS_h
#define SPIFFS_h
#include "Global.hpp"
#include "Command.hpp"
using namespace ATE;
#include "util/ConfigFile.hpp"

class CmdConfig
{
private:
    /* data */
public:
    CmdConfig(/* args */);
    ~CmdConfig();

    REQUEST_BODY_CONFIG* Parse(const char* json);

    char* Print(RESPONSE_BODY_CONFIG* res);
    char* Print(REQUEST_BODY_CONFIG* req);

    void Execute(REQUEST_BODY_CONFIG* req, RESPONSE_BODY_CONFIG* res);
    void Error(REQUEST_BODY_CONFIG* req, RESPONSE_BODY_CONFIG* res);
};



#endif