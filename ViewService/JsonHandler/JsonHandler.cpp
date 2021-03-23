#include "JsonHandler.h"

char *JsonHandler::GetAudioSendData(std::string strViewName, std::string strExtraInfo, VIEW_STATUS viewStatus)
{
    cJSON *root;
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "keyname1", cJSON_CreateString(strViewName.c_str()));
    cJSON_AddItemToObject(root, "keyname2", cJSON_CreateString(strExtraInfo.c_str()));
    cJSON_AddItemToObject(root, "onoff", cJSON_CreateString(GetViewStatus(viewStatus).c_str()));
    char *sendData = NULL;
    if (root != NULL)
    {
        sendData = cJSON_PrintUnformatted(root);
        cJSON_Delete(root);
    }

    return sendData;
}

/* {"pid":"warningpage","warningpage":[{"pid":"tiresystemfault","status":"ON"}],"status":"ON"} */
char *JsonHandler::GetWarnJsonInfo(std::string strViewName, std::string strExtraInfo, VIEW_STATUS viewStatus)
{
    cJSON *arrayElement = cJSON_CreateObject();
    cJSON_AddItemToObject(arrayElement, "pid", cJSON_CreateString(strExtraInfo.c_str()));
    cJSON_AddItemToObject(arrayElement, "status", cJSON_CreateString(GetViewStatus(viewStatus).c_str()));

    cJSON *pageArray = cJSON_CreateArray();
    cJSON_AddItemToArray(pageArray, arrayElement);

    cJSON *warningPageRoot = cJSON_CreateObject();
    cJSON_AddItemToObject(warningPageRoot, "pid", cJSON_CreateString("warningpage"));
    cJSON_AddItemToObject(warningPageRoot, "warningpage", pageArray);
    cJSON_AddItemToObject(warningPageRoot, "status", cJSON_CreateString(GetViewStatus(viewStatus).c_str()));

    char *warnData = NULL;
    if (warningPageRoot != NULL && pageArray != NULL)
    {
        warnData = cJSON_PrintUnformatted(warningPageRoot);
        cJSON_Delete(warningPageRoot);
    }

    return warnData;
}

/* {"keyname1":"pepsbatterylow","keyname2":"pepsbatterylow","onoff":"ON"} */
void JsonHandler::WarnMsgParse(const std::string &strMsgInfo, std::string &strViewName, std::string &strExtraInfo, std::string &strViewStatus)
{
    cJSON *root = cJSON_Parse(strMsgInfo.c_str());
    if (root == NULL)
    {
        return;
    }

    GetJsonParse(root, "keyname1", strViewName);
    GetJsonParse(root, "keyname2", strExtraInfo);
    GetJsonParse(root, "onoff", strViewStatus);
    if (root != NULL)
    {
        cJSON_Delete(root);
    }
}

void JsonHandler::GetJsonParse(cJSON *root, const char *keyname, std::string &strValue)
{
    cJSON *item = cJSON_GetObjectItem(root, keyname);

    if (item == NULL || !cJSON_IsString(item))
    {
        return;
    }

    if (item->valuestring == NULL)
    {
        return;
    }

    strValue = item->valuestring;
}

std::string JsonHandler::GetViewStatus(VIEW_STATUS viewStatus)
{
    switch (viewStatus)
    {
    case VIEW_ON:
        return std::string("ON");
    default:
        return std::string("OFF");
    }
}