#ifndef CONFIGFILE_h
#define CONFIGFILE_h
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"

class ConfigFile
{
private:
    /* data */
public:
    ConfigFile(/* args */);
    ~ConfigFile();

    void Initialize();
};




#endif