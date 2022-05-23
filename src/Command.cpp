#include "Command.hpp"

namespace ATE 
{
    Command::Command(/* args */)
    {
    }

    Command::~Command()
    {
    }

    bool Command::parseBasic(const char *json, REQUEST_BODY_BASIC *basic) {

        cJSON *cjsonRoot, *cjsonId, *cjsonCmd;
        
        cjsonRoot = cJSON_Parse(json);
        if (cjsonRoot == NULL) {
            const char *jsonError = cJSON_GetErrorPtr();
            if (jsonError && *jsonError != '\0' ) {
                basic->err = jsonError;
            } else {
                basic->err = "json error.";
            }
            return false;

        } else {
            cjsonId = cJSON_GetObjectItem(cjsonRoot, "id");
            if (cjsonId == NULL) {
                basic->err = "id error.";
                return false;
            }

            cjsonCmd = cJSON_GetObjectItem(cjsonRoot, "cmd");
            if (cjsonCmd == NULL) {
                basic->err = "cmd error.";
                return false;
            }

            basic->id = cjsonId->valueint;
            basic->cmd = cjsonCmd->valuestring;

            if (strcmp(basic->cmd, "info") == 0) {
                basic->key = REQUEST_KEY::REQUEST_KEY_INFO;
            } else if (strcmp(basic->cmd, "reset") == 0) {
                basic->key = REQUEST_KEY::REQUEST_KEY_RESET;
            } else if (strcmp(basic->cmd, "custom") == 0) {
                basic->key = REQUEST_KEY::REQUEST_KEY_CUSTOM;
            } else if (strcmp(basic->cmd, "setvol") == 0) {
                basic->key = REQUEST_KEY::REQUEST_KEY_SETVOL;
            } else if (strcmp(basic->cmd, "getvol") == 0) {
                basic->key = REQUEST_KEY::REQUEST_KEY_GETVOL;
            } else {
                basic->key = REQUEST_KEY::REQUEST_KEY_ERROR;
                basic->err = "cmd invalid.";
                return false;
            }

        }
        return true;
    }

    bool Command::parseInfo(const char* json, REQUEST_BODY_INFO* info) {

        cJSON *doc, *id, *cmd;
        doc = cJSON_Parse(json);
        if (doc == NULL) {
            return info;
        } else {

            id = cJSON_GetObjectItem(doc, "id");
            cmd = cJSON_GetObjectItem(doc, "cmd");

            info->id = id->valueint;
            info->cmd = cmd->valuestring;

            if (strcmp(info->cmd, "info") == 0) {
                info->key = REQUEST_KEY::REQUEST_KEY_INFO;
            } else {
                info->err = "cmd error.";
                return false;
            }
        }
        return info;
    }

    bool Command::parseReset(const char* json, REQUEST_BODY_RESET* reset) {

        cJSON *doc, *id, *cmd;
        doc = cJSON_Parse(json);
        if (doc == NULL) {
            return false;
        } else {

            id = cJSON_GetObjectItem(doc, "id");
            cmd = cJSON_GetObjectItem(doc, "cmd");

            reset->id = id->valueint;
            reset->cmd = cmd->valuestring;

            if (strcmp(reset->cmd, "reset") == 0) {
                reset->key = REQUEST_KEY::REQUEST_KEY_RESET;
            } else {
                reset->key = REQUEST_KEY::REQUEST_KEY_ERROR;
                reset->err = (char*)"cmd error.";
                return false;
            }

        }
        return true;
    }

    bool Command::parseCustom(const char* json, REQUEST_BODY_CUSTOM* custom) {
        cJSON *cjsonRoot, *cjsonId, *cjsonCmd, *cjsonData;
        cjsonRoot = cJSON_Parse(json);
        if (cjsonRoot == NULL) {
            return false;
        } else {

            cjsonId = cJSON_GetObjectItem(cjsonRoot, "id");
            cjsonCmd = cJSON_GetObjectItem(cjsonRoot, "cmd");
            cjsonData = cJSON_GetObjectItem(cjsonRoot, "data");

            custom->id = cjsonId->valueint;
            custom->cmd = cjsonCmd->valuestring;
            char *data = cjsonData->valuestring;

            int len = strlen(data);
            len % 2 == 0 ? len = len / 2 : len = (len + 1) / 2;
            custom->len = len;
            unsigned char *udata = (unsigned char*)malloc(len);
            ATE::hexToByte(data, udata, len);
            custom->data = udata;

            if (strcmp(custom->cmd, "custom") == 0) {
                custom->key = REQUEST_KEY::REQUEST_KEY_CUSTOM;
            } else {
                custom->key = REQUEST_KEY::REQUEST_KEY_ERROR;
                custom->err = (char*)"cmd error.";
                return false;
            }

        }
        return true;
    }

    bool Command::parseSetvol(const char* json, REQUEST_BODY_SETVOL* setvol) {
        cJSON *cjsonRoot, *cjsonId, *cjsonCmd, *cjsonVol;
        cjsonRoot = cJSON_Parse(json);
        if (cjsonRoot == NULL) {
            return false;
        } else {

            cjsonId = cJSON_GetObjectItem(cjsonRoot, "id");
            cjsonCmd = cJSON_GetObjectItem(cjsonRoot, "cmd");

            setvol->id = cjsonId->valueint;
            setvol->cmd = cjsonCmd->valuestring;

            if (strcmp(setvol->cmd, "setvol") == 0) {
                setvol->key = REQUEST_KEY::REQUEST_KEY_SETVOL;
            } else {
                setvol->key = REQUEST_KEY::REQUEST_KEY_ERROR;
                setvol->err = (char*)"cmd error.";
                return false;
            }

            cjsonVol = cJSON_GetObjectItem(cjsonRoot, "vol");
            if (cjsonVol != NULL) {
                int volSize = cJSON_GetArraySize(cjsonVol);
                for(int i = 0; i < volSize; i++) {
                    cJSON *citem = cJSON_GetArrayItem(cjsonVol, i);
                    setvol->vol.push_back(citem->valuedouble);
                }
            }

        }
        return true;
    }

    bool Command::parseGetvol(const char* json, REQUEST_BODY_GETVOL* getvol) {
        cJSON *cjsonRoot, *cjsonId, *cjsonCmd, *cjsonRel;
        cjsonRoot = cJSON_Parse(json);
        if (cjsonRoot == NULL) {
            return false;
        } else {

            cjsonId = cJSON_GetObjectItem(cjsonRoot, "id");
            cjsonCmd = cJSON_GetObjectItem(cjsonRoot, "cmd");

            getvol->id = cjsonId->valueint;
            getvol->cmd = cjsonCmd->valuestring;

            if (strcmp(getvol->cmd, "getvol") == 0) {
                getvol->key = REQUEST_KEY::REQUEST_KEY_SETVOL;
            } else {
                getvol->key = REQUEST_KEY::REQUEST_KEY_ERROR;
                getvol->err = (char*)"cmd error.";
                return false;
            }

            cjsonRel = cJSON_GetObjectItem(cjsonRoot, "rel");
            if (cjsonRel != NULL) {
                bool relSize = cJSON_GetArraySize(cjsonRel);
                for(int i = 0; i < relSize; i++) {
                    cJSON *citem = cJSON_GetArrayItem(cjsonRel, i);

                    getvol->rel.push_back((citem->valueint > 0));
                }
            }

        }
        return true;
    }

    char* Command::printBasic(RESPONSE_BODY_BASIC* resBasic) {
        cJSON *doc = cJSON_CreateObject();
        
        cJSON_AddItemToObject(doc, "id", cJSON_CreateNumber(resBasic->id));
        cJSON_AddItemToObject(doc, "err", cJSON_CreateString(resBasic->err));

        return cJSON_Print(doc);
    }

    char* Command::printInfo(RESPONSE_BODY_INFO* resInfo) {
        cJSON *doc = cJSON_CreateObject();
        
        cJSON_AddItemToObject(doc, "id", cJSON_CreateNumber(resInfo->id));
        cJSON_AddItemToObject(doc, "err", cJSON_CreateString(resInfo->err));

        cJSON_AddItemToObject(doc, "mac", cJSON_CreateString(resInfo->mac));
        cJSON_AddItemToObject(doc, "ver", cJSON_CreateString(resInfo->ver));
        cJSON_AddItemToObject(doc, "chi", cJSON_CreateString(resInfo->chi));

        cJSON_AddItemToObject(doc, "chn", cJSON_CreateNumber(resInfo->chn));
        cJSON_AddItemToObject(doc, "cnn", cJSON_CreateNumber(resInfo->cnn));


        return cJSON_Print(doc);
    }

    char* Command::printReset(RESPONSE_BODY_RESET* resReset) {
        cJSON *doc = cJSON_CreateObject();
        
        cJSON_AddItemToObject(doc, "id", cJSON_CreateNumber(resReset->id));
        cJSON_AddItemToObject(doc, "err", cJSON_CreateString(resReset->err));

        return cJSON_Print(doc);
    }

    char* Command::printCustom(RESPONSE_BODY_CUSTOM* resCustom) {
        cJSON *cjsonRoot = cJSON_CreateObject();
        
        cJSON_AddItemToObject(cjsonRoot, "id", cJSON_CreateNumber(resCustom->id));
        if (resCustom->err != NULL)
            cJSON_AddItemToObject(cjsonRoot, "err", cJSON_CreateString(resCustom->err));

        string* data = ATE::byteToHex(resCustom->data, resCustom->len);
        
        cJSON_AddItemToObject(cjsonRoot, "data", cJSON_CreateString(data->c_str()));

        return cJSON_Print(cjsonRoot);

    }

    char* Command::printSetvol(RESPONSE_BODY_SETVOL* resSetvol) {
        cJSON *cjsonRoot = cJSON_CreateObject();
        
        cJSON_AddItemToObject(cjsonRoot, "id", cJSON_CreateNumber(resSetvol->id));
        if (resSetvol->err != NULL)
            cJSON_AddItemToObject(cjsonRoot, "err", cJSON_CreateString(resSetvol->err));


        return cJSON_Print(cjsonRoot);

    }

    char* Command::printGetvol(RESPONSE_BODY_GETVOL* resGetvol) {
        cJSON *cjsonRoot = cJSON_CreateObject();
        
        cJSON_AddItemToObject(cjsonRoot, "id", cJSON_CreateNumber(resGetvol->id));
        if (resGetvol->err != NULL)
            cJSON_AddItemToObject(cjsonRoot, "err", cJSON_CreateString(resGetvol->err));

        cJSON_AddItemToObject(cjsonRoot, "err", cJSON_CreateString(resGetvol->err));

        // 电压读数数组序列化
        int volSize = resGetvol->vol.size();
        double vol[volSize];
        copy(resGetvol->vol.begin(), resGetvol->vol.end(), vol);
        cJSON_AddItemToObject(cjsonRoot,"vol", cJSON_CreateDoubleArray(vol,volSize));

        return cJSON_Print(cjsonRoot);

    }

}
