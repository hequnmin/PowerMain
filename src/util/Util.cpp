#include "Util.hpp"

namespace ATE
{
    
    unsigned char toByte(char c)
    {
        unsigned char value = 0;
        if (c >= '0' && c <= '9')
            value = c - '0';
        else if (c >= 'A' && c <= 'Z')
            value = c - 'A' + 10;
        else if (c >= 'a' && c <= 'z')
            value = c - 'a' + 10;
        return value;
    }
 
    void hexToByte(const char *src, unsigned char *dst, int len)
    {
        int index;
        for (int i = 0; i < len; i++){
            index = i * 2;
            dst[i] = ((toByte(src[index]))<<4) | toByte(src[index+1]);
        }
    }

    string* byteToHex(unsigned char *src, int len)
    {
        string* dst= new string();
        for (int i = 0; i < len; i++) {
            char hexH, hexL;
            int value = src[i]; //直接将unsigned char赋值给整型的值，系统会正动强制转换
            int valH = value/16;
            int valL = value % 16;

            //将商转成字母
            (valH >= 0 && valH <= 9) ?  hexH = (char)(48 + valH) : hexH = (char)(55 + valH);

            //将余数转成字母
            (valL >= 0 && valL <= 9) ? hexL = (char)(48 + valL) : hexL = (char)(55 + valL);

            //将字母连接成串
            *dst = *dst + hexH + hexL;
        }
        return dst;
    }

    bool isJson(const char* src)
    {
        cJSON *cjson;
        
        cjson = cJSON_Parse(src);
        if (cjson == NULL) {
            return false;
        }
        cJSON_Delete(cjson);        // 释放内存
        return true;
    }
}

