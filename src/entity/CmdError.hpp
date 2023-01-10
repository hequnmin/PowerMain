#ifndef CMDERROR_h
#define CMDERROR_h

#include "Command.hpp"
using namespace ATE;
#include "Global.hpp"

class CmdError
{
private:
    /* data */

    
public:
    CmdError();
    ~CmdError();

    // 响应结构体打印Json字符串
    char* Print(RESPONSE_BODY_ERROR* res);


};


#endif

