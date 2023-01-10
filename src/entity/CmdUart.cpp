#include "CmdUart.hpp"

CmdUart::CmdUart(/* args */)
{
}

CmdUart::~CmdUart()
{
}

REQUEST_BODY_UART* CmdUart::Parse(const char* json) {
    REQUEST_BODY_UART* req = new REQUEST_BODY_UART();

    cJSON *doc, *id, *cmd, *data, *buff;
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
        if (strcmp(req->cmd, "uart") == 0) {
            req->key = REQUEST_KEY::REQUEST_KEY_UART;
        } else {
            req->key = REQUEST_KEY::REQUEST_KEY_ERROR;
            req->err = (char*)"cmd error.";
            return req;
        }

        data = cJSON_GetObjectItem(doc, "data");
        if (data != NULL) {
            char* datatmp = data->valuestring;
            
            int len = ceil(1.0 * strlen(datatmp) / 2);
            int siz = sizeof(unsigned char)*len;
            unsigned char* bytetmp = (unsigned char*)malloc(siz);
            bzero(bytetmp, siz);
            ATE::hexToByte(datatmp, bytetmp, len);
            req->data = bytetmp;
            req->len = len;
        }       
        
    }
    return req;
}

char* CmdUart::Print(RESPONSE_BODY_UART* res) {
    char* json;

    cJSON *doc = cJSON_CreateObject();
        
    cJSON_AddItemToObject(doc, "id", cJSON_CreateNumber(res->id));
    if (res->err != NULL) {
        cJSON_AddItemToObject(doc, "err", cJSON_CreateString(res->err));
    }

    if (res->data != NULL) {
        string* data = ATE::byteToHex(res->data, res->len);        
        cJSON_AddItemToObject(doc, "data", cJSON_CreateString(data->c_str()));
        cJSON_AddItemToObject(doc, "len", cJSON_CreateNumber(res->len));
    }

    json = cJSON_Print(doc);
    cJSON_Delete(doc);

    return json;
}

char* CmdUart::Print(REQUEST_BODY_UART* req) {
    char* json;

    cJSON *doc = cJSON_CreateObject();
        
    cJSON_AddItemToObject(doc, "id", cJSON_CreateNumber(req->id));
    if (req->err != NULL) {
        cJSON_AddItemToObject(doc, "err", cJSON_CreateString(req->err));
    }
    cJSON_AddItemToObject(doc, "cmd", cJSON_CreateString(req->cmd));


    json = cJSON_Print(doc);
    cJSON_Delete(doc);

    return json;
}

void CmdUart::Execute(REQUEST_BODY_UART* req, RESPONSE_BODY_UART* res) {
    res->id = req->id;
    res->err = req->err;

    if (req->data != NULL) {
        res->data = req->data;
        res->len = req->len;
    }

    char* buf = Print(res);
    uart_write_bytes(G_MCU[0].uartNum, buf, strlen(buf));
    free(buf);
}