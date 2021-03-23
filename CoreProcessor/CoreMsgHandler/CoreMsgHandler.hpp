#ifndef _CORE_MSG_HANDLER_H_
#define _CORE_MSG_HANDLER_H_

//Utils
#include "Util.h"
#include "Thread.h"
#include "Singleton.hpp"
#include "PriorityQueue.hpp"
#include "mosquitto.h"

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
    void run();

    void CoreMqttInit(const std::string &host, const int port);

    static int CoreMqttCallBack(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg);

    struct mosquitto *_mosq = nullptr;
    std::string _host;
    int _port;
    int _keep_alive = 60;
    bool _connected = false;
    std::string _mesg_pub_topic = "MessageHandler/publish";
    std::string _mesg_cb_topic = "MessageHandler/call_back";

}

#endif //!_CORE_MSG_HANDLER_H_