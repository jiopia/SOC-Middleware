#ifndef _JSON_HANDLER_H
#define _JSON_HANDLER_H

#include "Util.h"
#include "cJSON.h"
#include "Singleton.hpp"

class JsonHandler : public Singleton<JsonHandler>
{
    friend class Singleton<JsonHandler>;

public:
    /* 中间件内部转发的报警信息 */
    std::string GetWarnSendData(std::string strViewName, std::string strExtraInfo, std::string strViewStatus);

    /* 中间件发送给HMI的报警信息 */
    char *GetWarnJsonInfo(std::string strViewName, std::string strExtraInfo, std::string strViewStatus);

    void WarnMsgParse(const std::string &strMsgInfo, std::string &strViewName, std::string &strExtraInfo, std::string &strViewStatus);

private:
    void GetJsonParse(cJSON *root, const char *keyname, std::string &strValue);
};

#endif //_JSON_HANDLER_H