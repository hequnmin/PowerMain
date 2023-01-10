#include "CmdError.hpp"

CmdError::CmdError() {
}

CmdError::~CmdError() {

}

char* CmdError::Print(RESPONSE_BODY_ERROR* res) {
    char* json;

    cJSON *doc = cJSON_CreateObject();
        
    if (res->err != NULL) {
        cJSON_AddItemToObject(doc, "err", cJSON_CreateString(res->err));
    }

    json = cJSON_Print(doc);
    cJSON_Delete(doc);
    
    return json;
}

