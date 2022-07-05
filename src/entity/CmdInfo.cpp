#include "CmdInfo.hpp"

CmdInfo::CmdInfo() {
}

CmdInfo::~CmdInfo() {

}

REQUEST_BODY_INFO* CmdInfo::Parse(const char* json) {
    REQUEST_BODY_INFO* reqInfo = new REQUEST_BODY_INFO();

    cJSON *doc, *id, *cmd, *chn, *mcu;
    doc = cJSON_Parse(json);
    if (doc == NULL) {
        const char *err = cJSON_GetErrorPtr();
        if (err && *err != '\0' ) {
            reqInfo->err = err;
        } else {
            reqInfo->err = "json error.";
        }
        return reqInfo;
    } else {
        id = cJSON_GetObjectItem(doc, "id");
        if (id == NULL) {
            reqInfo->err = "id error.";
            return reqInfo;
        }
        reqInfo->id = id->valueint;

        cmd = cJSON_GetObjectItem(doc, "cmd");
        if (cmd == NULL) {
            reqInfo->err = "cmd error.";
            return reqInfo;
        } 
        reqInfo->cmd = cmd->valuestring;
        if (strcmp(reqInfo->cmd, "info") == 0) {
            reqInfo->key = REQUEST_KEY::REQUEST_KEY_INFO;
        } else {
            reqInfo->err = "cmd error.";
            return reqInfo;
        }

        chn = cJSON_GetObjectItem(doc, "chn");
        if (chn == NULL) {
            reqInfo->err = "chn error.";
            return reqInfo;
        }
        reqInfo->chn = chn->valueint;

        mcu = cJSON_GetObjectItem(doc, "mcu");
        if (mcu == NULL) {
            reqInfo->err = "mcu error.";
            return reqInfo;
        }

        reqInfo->mcu = mcu->valueint;
        if (reqInfo->mcu > G_MCU_COUNT) {
            reqInfo->err = "mcu error.";
            return reqInfo;
        }

    }
    return reqInfo;
}

char* CmdInfo::Print(RESPONSE_BODY_INFO* res) {
   cJSON *doc = cJSON_CreateObject();
        
    cJSON_AddItemToObject(doc, "id", cJSON_CreateNumber(res->id));
    cJSON_AddItemToObject(doc, "err", cJSON_CreateString(res->err));

    cJSON_AddItemToObject(doc, "mac", cJSON_CreateString(res->mac));
    cJSON_AddItemToObject(doc, "ver", cJSON_CreateString(res->ver));
    cJSON_AddItemToObject(doc, "chi", cJSON_CreateString(res->chi));

    cJSON_AddItemToObject(doc, "chn", cJSON_CreateNumber(res->chn));
    cJSON_AddItemToObject(doc, "cnn", cJSON_CreateNumber(res->cnn));


    return cJSON_Print(doc);
}

char* CmdInfo::Print(REQUEST_BODY_INFO* req) {
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

void CmdInfo::Execute(REQUEST_BODY_INFO* req, RESPONSE_BODY_INFO* res) {
    res->id = req->id;
    res->err = req->err;
    if (req->mcu == 0) {
        // MCU = 0 主MCU，直接赋值系统信息并回值。

        res->id = req->id;
        res->chn = req->chn;
        res->chn = req->chn;

        uint8_t mac[6] = {0};
        esp_efuse_mac_get_default(mac);
        char macStr[18] = { 0 };
        sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        res->mac = macStr;

        char* json = Print(req);
        uart_write_bytes(G_MCU[req->mcu].uartNum, json, strlen(json));

    } else {
        if (G_MCU[req->mcu].isJson) {
            char* buf = Print(req);
            uart_write_bytes(G_MCU[req->mcu].uartNum, buf, strlen(buf));
        } else {
            const unsigned char buf[5] = { 0xdd, 0xdd, 0xdd, 0xdd, 0xdd };
            uart_write_bytes(G_MCU[req->mcu].uartNum, buf, 5);
        }
    }

    return;
}

void CmdInfo::Response(RESPONSE_BODY_INFO* res) {
    char* json = Print(res);
    uart_write_bytes(G_UartMain.uartNum, json, strlen(json));
}