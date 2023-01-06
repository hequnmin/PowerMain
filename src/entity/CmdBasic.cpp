#include "CmdBasic.hpp"

CmdBasic::CmdBasic() {

}

CmdBasic::~CmdBasic() {

}

REQUEST_BODY_BASIC* CmdBasic::Parse(const char* json) {

    REQUEST_BODY_BASIC* reqBasic = new REQUEST_BODY_BASIC();

    cJSON *root, *id, *cmd;
    
    root = cJSON_Parse(json);
    if (root == NULL) {
        const char *err = cJSON_GetErrorPtr();
        if (err && *err != '\0' ) {
            reqBasic->err = err;
        } else {
            reqBasic->err = "json error.";
        }
        return reqBasic;

    } else {
        id = cJSON_GetObjectItem(root, "id");
        if (id == NULL) {
            reqBasic->err = "id error.";
            return reqBasic;
        }
        reqBasic->id = id->valueint;

        cmd = cJSON_GetObjectItem(root, "cmd");
        if (cmd == NULL) {
            reqBasic->err = "cmd error.";
            return reqBasic;
        }
        reqBasic->cmd = cmd->valuestring;

        if (strcmp(reqBasic->cmd, "info") == 0) {
            reqBasic->key = REQUEST_KEY::REQUEST_KEY_INFO;
        } else if (strcmp(reqBasic->cmd, "reset") == 0) {
            reqBasic->key = REQUEST_KEY::REQUEST_KEY_RESET;
        } else if (strcmp(reqBasic->cmd, "config") == 0) {
            reqBasic->key = REQUEST_KEY::REQUEST_KEY_CONFIG;
        } else if (strcmp(reqBasic->cmd, "custom") == 0) {
            reqBasic->key = REQUEST_KEY::REQUEST_KEY_CUSTOM;
        } else if (strcmp(reqBasic->cmd, "setvol") == 0) {
            reqBasic->key = REQUEST_KEY::REQUEST_KEY_SETVOL;
        } else if (strcmp(reqBasic->cmd, "getvol") == 0) {
            reqBasic->key = REQUEST_KEY::REQUEST_KEY_GETVOL;
        } else if (strcmp(reqBasic->cmd, "uart") == 0) {
            reqBasic->key = REQUEST_KEY::REQUEST_KEY_UART;
        } else {
            reqBasic->key = REQUEST_KEY::REQUEST_KEY_ERROR;
            reqBasic->err = "cmd error.";
            return reqBasic;
        }

        cJSON_Delete(root);
        return reqBasic;
    }
}

void CmdBasic::Execute(REQUEST_BODY_BASIC* req, RESPONSE_BODY_BASIC* res) {
    res->id = req->id;
    res->err = req->err;
    return;
}

char* CmdBasic::Print(RESPONSE_BODY_BASIC* res) {
    cJSON *doc = cJSON_CreateObject();
    
    cJSON_AddItemToObject(doc, "id", cJSON_CreateNumber(res->id));
    cJSON_AddItemToObject(doc, "err", cJSON_CreateString(res->err));

    return cJSON_Print(doc);
}
