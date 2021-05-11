#ifndef _ACTUATOR_H_
#define _ACTUATOR_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include "mosquitto.h"
#include "Util.h"
#include "Singleton.hpp"
#include "XmlManager.h"
#include "MsgHandler.h"
#include "MqttConnection.hpp"

class Actuator : public Singleton<Actuator>, public Thread
{
public:
    Actuator();
    ~Actuator();

    int WarnShowTimerStart(int iTimeWaitBeforeStart);
    int WarnShowTimerStop();
    bool IsIgnOFFCheck();
    void MsgSend(std::string strTopic, std::string strMsg);

protected:
    int WarnShowTimerInit();
    int WarnShowTimerDelete();

private:
    void Run();
    void MsgReciever();
    void MsgProcessor(std::string strMsg);

    timer_t m_timerId;

    XmlManager *m_xmlManager;

    MsgHandler *m_msgHandler;

    std::mutex m_mtxMsgListen;

    std::shared_ptr<BaseConnection> m_mqttClient = NULL;
};

#endif // !_ACTUATOR_H_
