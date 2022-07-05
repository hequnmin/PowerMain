#ifndef CMDINFO_h
#define CMDINFO_h

#include "Command.hpp"
using namespace ATE;
#include "Global.hpp"

class CmdInfo
{
private:
    /* data */

    // // 传输方法。区分对应的通讯端口和通讯格式，并写入串口。
    // void Transport(const char* json, REQUEST_BODY_INFO* req);
    
public:
    CmdInfo();
    ~CmdInfo();

    // Json内容解析，结果存于请求和应答结构体
    REQUEST_BODY_INFO* Parse(const char* json);

    // 响应结构体打印Json字符串
    char* Print(RESPONSE_BODY_INFO* res);
    char* Print(REQUEST_BODY_INFO* req);

    // 执行指令
    void Execute(REQUEST_BODY_INFO* req, RESPONSE_BODY_INFO* res);

    // 响应方法。写入主通讯串口。
    void Response(RESPONSE_BODY_INFO* res);
};


#endif

