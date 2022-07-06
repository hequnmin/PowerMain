#ifndef CMDRESET_h
#define CMDRESET_h
#include "Global.hpp"
#include "Command.hpp"
using namespace ATE;

class CmdReset
{
private:
    /* data */
public:
    CmdReset(/* args */);
    ~CmdReset();

    REQUEST_BODY_RESET* Parse(const char* json);

    char* Print(RESPONSE_BODY_RESET* res);
    char* Print(REQUEST_BODY_RESET* req);

    void Execute(REQUEST_BODY_RESET* req, RESPONSE_BODY_RESET* res);

};

#endif