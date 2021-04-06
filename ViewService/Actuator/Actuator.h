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
#include "savedData.h"
#include "mcuclient/mcuclientdefine.h"
#include "Util.h"
#include "Singleton.hpp"
#include "XmlManager.h"
#include "MsgHandler.h"
#include "MqttConnection.hpp"

#define MQ_SERVER_NAME "/VIEW_ACTUAL_QUEU"

class Actuator : public Singleton<Actuator>, public Thread
{
public:
    Actuator();
    ~Actuator();

    int WarnShowTimerStart(int iTimeWaitBeforeStart);
    int WarnShowTimerStop();
    bool IsIgnOFFCheck();

protected:
    int WarnShowTimerInit();
    int WarnShowTimerDelete();

private:
    void Run();
    void MsgReciever();
    void MsgProcessor(std::string strMsg);

    int m_fdShareMemMcuData = -1;
    shem_t *m_pShareMemMcuData = NULL;
    timer_t m_timerId;

    XmlManager *m_xmlManager;

    MsgHandler *m_msgHandler;

    std::mutex m_mtxMsgListen;

    std::shared_ptr<BaseConnection> m_mqttClient = NULL;
};

#endif // !_ACTUATOR_H_
