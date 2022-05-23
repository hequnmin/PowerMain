#ifndef UTIL_h
#define UTIL_h

#include <stdio.h>
#include <iostream>
using namespace std;
#include <cstring>
#include <math.h>


namespace ATE 
{

    unsigned char toByte(char c);
    void hexToByte(const char *src, unsigned char *dst, int len);
    string* byteToHex(unsigned char *src, int len);
}

#endif