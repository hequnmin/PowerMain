
#include "CmdConfig.hpp"

CmdConfig::CmdConfig(/* args */)
{

}

CmdConfig::~CmdConfig()
{
}

REQUEST_BODY_CONFIG* CmdConfig::Parse(const char* json) {
    REQUEST_BODY_CONFIG* req = new REQUEST_BODY_CONFIG();

    cJSON *doc, *id, *cmd, *mcu;
    doc = cJSON_Parse(json);
    if (doc == NULL) {
        const char *err = cJSON_GetErrorPtr();
        if (err && *err != '\0' ) {
            req->err = err;
        } else {
            req->err = "json error.";
        }
        return req;
    } else {

        id = cJSON_GetObjectItem(doc, "id");
        req->id = id->valueint;
        
        cmd = cJSON_GetObjectItem(doc, "cmd");
        req->cmd = cmd->valuestring;
        if (strcmp(req->cmd, "config") == 0) {
            req->key = REQUEST_KEY::REQUEST_KEY_CONFIG;
        } else {
            req->key = REQUEST_KEY::REQUEST_KEY_ERROR;
            req->err = (char*)"cmd error.";
            return req;
        }

        mcu = cJSON_GetObjectItem(doc, "mcu");
        if (mcu == NULL) {
            req->err = "mcu error.";
            return req;
        }
        req->mcu = mcu->valueint;
        if (req->mcu > G_MCU_COUNT) {
            req->err = "mcu error.";
            return req;
        }
    }
    return req;
}

char* CmdConfig::Print(RESPONSE_BODY_CONFIG* res) {
    cJSON *doc = cJSON_CreateObject();
        
    cJSON_AddItemToObject(doc, "id", cJSON_CreateNumber(res->id));
    if (res->err != NULL) {
        cJSON_AddItemToObject(doc, "err", cJSON_CreateString(res->err));
    }

    return cJSON_Print(doc);
}

char* CmdConfig::Print(REQUEST_BODY_CONFIG* req) {
    cJSON *doc = cJSON_CreateObject();
        
    cJSON_AddItemToObject(doc, "id", cJSON_CreateNumber(req->id));
    if (req->err != NULL) {
        cJSON_AddItemToObject(doc, "err", cJSON_CreateString(req->err));
    }
    cJSON_AddItemToObject(doc, "cmd", cJSON_CreateString(req->cmd));
    cJSON_AddItemToObject(doc, "chn", cJSON_CreateNumber(req->chn));
    cJSON_AddItemToObject(doc, "mcu", cJSON_CreateNumber(req->mcu));

    return cJSON_Print(doc);
}

void CmdConfig::Execute(REQUEST_BODY_CONFIG* req, RESPONSE_BODY_CONFIG* res) {
    res->id = req->id;

    ConfigFile configFile = ConfigFile();
    configFile.Initialize();

    char* json = Print(res);
    uart_write_bytes(G_UartMain.uartNum, json, strlen(json));
}

void CmdConfig::Error(REQUEST_BODY_CONFIG* req, RESPONSE_BODY_CONFIG* res) {
    res->id = req->id;
    res->err = req->err;

    char* json = Print(res);
    uart_write_bytes(G_UartMain.uartNum, json, strlen(json));
}