
#include "CmdReset.hpp"

CmdReset::CmdReset(/* args */)
{
    
}

CmdReset::~CmdReset()
{
}

REQUEST_BODY_RESET* CmdReset::Parse(const char* json) {
    REQUEST_BODY_RESET* req = new REQUEST_BODY_RESET();

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
        if (strcmp(req->cmd, "reset") == 0) {
            req->key = REQUEST_KEY::REQUEST_KEY_RESET;
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

char* CmdReset::Print(RESPONSE_BODY_RESET* res) {
    char* json;

    cJSON *doc = cJSON_CreateObject();
        
    cJSON_AddItemToObject(doc, "id", cJSON_CreateNumber(res->id));
    if (res->err != NULL) {
        cJSON_AddItemToObject(doc, "err", cJSON_CreateString(res->err));
    }

    json = cJSON_Print(doc);
    cJSON_Delete(doc);

    return json;
}

char* CmdReset::Print(REQUEST_BODY_RESET* req) {
    char* json;

    cJSON *doc = cJSON_CreateObject();
        
    cJSON_AddItemToObject(doc, "id", cJSON_CreateNumber(req->id));
    if (req->err != NULL) {
        cJSON_AddItemToObject(doc, "err", cJSON_CreateString(req->err));
    }
    cJSON_AddItemToObject(doc, "cmd", cJSON_CreateString(req->cmd));
    cJSON_AddItemToObject(doc, "chn", cJSON_CreateNumber(req->chn));
    cJSON_AddItemToObject(doc, "mcu", cJSON_CreateNumber(req->mcu));

    json = cJSON_Print(doc);
    cJSON_Delete(doc);

    return json;
}

void CmdReset::Execute(REQUEST_BODY_RESET* req, RESPONSE_BODY_RESET* res) {
    res->id = req->id;
    
    if (G_MCU[req->mcu].isJson) {
        char* json = Print(res);
        uart_write_bytes(G_MCU[req->mcu].uartNum, json, strlen(json));
        free(json);
    } else {
        //A2 0E 01 0C 00 00 00 00 00 00 00 00 00 00 00 00
        unsigned char buf[16] = { 0xa2, 0x0e, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        buf[2] = req->chn;
        uart_write_bytes(G_MCU[req->mcu].uartNum, buf, 16);
        free(buf);
    }

    if (req->mcu == 0) {
        vTaskDelay(50 / portTICK_PERIOD_MS);
        fflush(stdout);
        esp_restart();
    }
    
}
