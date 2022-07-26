#ifndef COMMAND_h
#define COMMAND_h

#include <stdio.h>
#include <vector>
#include <iostream>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"
#include "util/Util.hpp"

using namespace std;

namespace ATE
{

    class Command;

    enum COMMAND_ERROR { 
        COMMAND_ERROR_INVALID_REQUEST 
    };

    // 请求类型
    enum REQUEST_KEY {
        REQUEST_KEY_ERROR,          // 请求类型错误
        REQUEST_KEY_BASIC,          // 基本请求
        REQUEST_KEY_INFO,           // <获取系统信息>请求
        REQUEST_KEY_RESET,          // <复位/清零>请求
        REQUEST_KEY_CONFIG,         
        REQUEST_KEY_CUSTOM,         // <自定义>请求
        REQUEST_KEY_SETVOL,         // 电压设置
        REQUEST_KEY_GETVOL,         // 电压读数
    };

    // 基本请求结构体
    struct REQUEST_BODY_BASIC {
        int id;                     // 请求标识号
        char* cmd = NULL;     // 请求关键字
        REQUEST_KEY key;
        const char* err = NULL;     // 错误信息（多用于暂存解析JSON的错误信息）
        REQUEST_BODY_BASIC() : id(), cmd(), key(), err() {
        }
    };

    // 基本应答结构体
    struct RESPONSE_BODY_BASIC {
        int id;                     // 请求标识号          
        const char* err = NULL;     // 错误信息
    };

    // <获取系统信息>请求结构体
    struct REQUEST_BODY_INFO : public REQUEST_BODY_BASIC {
        int chn;
        int mcu;
        REQUEST_BODY_INFO() {
            key = REQUEST_KEY::REQUEST_KEY_INFO;
        }
    };

    // <获取系统信息>应答结构体
    struct RESPONSE_BODY_INFO : public RESPONSE_BODY_BASIC {
        char* chi = NULL;   // 芯片信息
        char* mac = NULL;   // 物理地址
        char* ver  = NULL;   // 嵌入系统版本
        int chn;            // 通道号
        int cnn;            // 连接号
    };

    // <复位/清零>请求结构体
    struct REQUEST_BODY_RESET : public REQUEST_BODY_BASIC {
        int chn;
        int mcu;
        REQUEST_BODY_RESET() {
            key = REQUEST_KEY::REQUEST_KEY_RESET;
        }
    };

    // <复位/清零>应答结构体
    struct RESPONSE_BODY_RESET : public RESPONSE_BODY_BASIC {
    };

    // <文件系统>请求结构体
    struct REQUEST_BODY_CONFIG : public REQUEST_BODY_BASIC {
        int chn;
        int mcu;
        REQUEST_BODY_CONFIG() {
            key = REQUEST_KEY::REQUEST_KEY_CONFIG;
        }
    };

    // <文件系统>应答结构体
    struct RESPONSE_BODY_CONFIG : public RESPONSE_BODY_BASIC {
    };

    // <自定义>请求结构体
    struct REQUEST_BODY_CUSTOM : public REQUEST_BODY_BASIC {
        unsigned char* data = NULL;     // 字节数组地址
        int len = 0;           // 字节数组长度
        REQUEST_BODY_CUSTOM() {
            key = REQUEST_KEY::REQUEST_KEY_CUSTOM;
        }
    };

    // <自定义>应答结构体
    struct RESPONSE_BODY_CUSTOM : public RESPONSE_BODY_BASIC {
        unsigned char* data = NULL;     // 字节数组地址
        int len = 0;           // 字节数组长度
    };

    // <电压设置>请求结构体
    struct REQUEST_BODY_SETVOL : public REQUEST_BODY_BASIC {
        vector<double> vol;             // 电芯电压设置
        REQUEST_BODY_SETVOL() {
            key = REQUEST_KEY::REQUEST_KEY_SETVOL;
        }
    };

    // <电压设置>应答结构体
    struct RESPONSE_BODY_SETVOL : public RESPONSE_BODY_BASIC {
    };

    // <电压读数>请求结构体
    struct REQUEST_BODY_GETVOL : public REQUEST_BODY_BASIC {
        vector<bool> rel;               // 继电器开关
        REQUEST_BODY_GETVOL() {
            key = REQUEST_KEY::REQUEST_KEY_GETVOL;
        }
    };

    // <电压读数>应答结构体
    struct RESPONSE_BODY_GETVOL : public RESPONSE_BODY_BASIC {
        vector<double> vol;             // 电芯电压读数
    };

    // 指令类
    class Command
    {
    private:
        /* data */
    public:
        Command(/* args */);
        ~Command();

        static bool isJson(const char* json);

        // 基本请求Json解析结构体
        bool parseBasic(const char* json, REQUEST_BODY_BASIC* basic);

        // <获取系统信息>请求Json解析结构体
        bool parseInfo(const char* json, REQUEST_BODY_INFO* info);

        // <复位/清零>请求Json解析结构体
        bool parseReset(const char* json, REQUEST_BODY_RESET* reset);

        // <自定义>请求Json解析结构体
        bool parseCustom(const char* json, REQUEST_BODY_CUSTOM* custom);

        // <电压设置>请求Json解析结构体
        bool parseSetvol(const char* json, REQUEST_BODY_SETVOL* setvol);

        // <电压读数>请求Json解析结构体
        bool parseGetvol(const char* json, REQUEST_BODY_GETVOL* getvol);

        // 基本应答体序列化Json
        char* printBasic(RESPONSE_BODY_BASIC* resBasic);

        // <获取系统信息>应答结构体序列化Json
        char* printInfo(RESPONSE_BODY_INFO* resInfo);

        // <复位/清零>应答结构体序列化Json
        char* printReset(RESPONSE_BODY_RESET* resReset);

        // <自定义>应答结构体序列化Json
        char* printCustom(RESPONSE_BODY_CUSTOM* resCustom);

        // <电压设置>应答结构体序列化Json
        char* printSetvol(RESPONSE_BODY_SETVOL* resSetvol);

        // <电压读数>应答结构体序列化Json
        char* printGetvol(RESPONSE_BODY_GETVOL* resGetvol);

        template<class T>
        bool parse(const char* json, T *req)
        {
            return true;
        }


    };

}

#endif
