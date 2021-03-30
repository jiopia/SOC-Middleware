#ifndef _CORE_MSG_HANDLER_H_
#define _CORE_MSG_HANDLER_H_

//Utils
#include "Util.h"
#include "Thread.h"
#include "Singleton.hpp"
#include "PriorityQueue.hpp"
#include "mosquitto.h"
#include "MqttConnection.hpp"

//bdsdk
#include "savedData.h"
#include "recvDataDefine.h"
#include "systemdefine.h"

class CoreMsgHandler : public Singleton<CoreMsgHandler>, public Thread
{
public:
    CoreMsgHandler();
    ~CoreMsgHandler();

private:
    void Run();

    void MsgReciever();

    void MsgProcessor(std::string strMsg);

    std::shared_ptr<BaseConnection> m_mqttClient = NULL;
};

#endif //!_CORE_MSG_HANDLER_H_