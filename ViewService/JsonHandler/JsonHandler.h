#ifndef _JSON_HANDLER_H
#define _JSON_HANDLER_H

#include "Util.h"
#include "cJSON.h"
#include "Singleton.hpp"
#include "ViewInfo.hpp"

class JsonHandler : public Singleton<JsonHandler>
{
    friend class Singleton<JsonHandler>;

public:
    char *GetAudioSendData(std::string strViewName, std::string strExtraInfo, VIEW_STATUS viewStatus);

    char *GetWarnJsonInfo(std::string strViewName, std::string strExtraInfo, VIEW_STATUS viewStatus);

    void WarnMsgParse(const std::string &strMsgInfo, std::string &strViewName, std::string &strExtraInfo, std::string &strViewStatus);

private:
    void GetJsonParse(cJSON *root, const char *keyname, std::string &strValue);

    std::string GetViewStatus(VIEW_STATUS viewStatus);
};

#endif //_JSON_HANDLER_H