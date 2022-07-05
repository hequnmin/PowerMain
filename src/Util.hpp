#ifndef UTIL_h
#define UTIL_h

#include <stdio.h>
#include <iostream>
using namespace std;
#include <cstring>
#include <math.h>
#include "cJSON.h"

namespace ATE 
{

    unsigned char toByte(char c);
    void hexToByte(const char *src, unsigned char *dst, int len);
    string* byteToHex(unsigned char *src, int len);

    bool isJson(const char* src);
}

#endif