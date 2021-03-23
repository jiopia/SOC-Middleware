#ifndef _ACTUATOR_H_
#define _ACTUATOR_H_

#include <semaphore.h>
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

#define MQ_SERVER_NAME "/VIEW_ACTUAL_QUEU"

class Actuator : public Singleton<Actuator>, public Thread
{
    // friend class Singleton<Actuator>;
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
    void run();
    void InitAllWarnInfo(bool isIgnOFF = false);

    void InitSomeSeriousWarn();
    void InitPEPS(bool isIgnOFF);
    void InitOverSpeed();
    void InitFollowMeHome();
    void InitTPMS();
    void InitBrakeFluid();
    void InitEPB(bool isIgnOFF);
    void InitAVH();
    void InitCDP();
    void InitDoor(bool isIgnOFF);
    void InitSeatBelt();
    void InitAirBag();
    void InitABS();
    void InitESP();
    void InitHDC();
    void InitEPS();
    void InitGearBox();
    void InitAVM();
    void InitSportMode();
    void InitFatigueDriving();
    void InitFuelLow();
    void InitWarmUp();
    void InitPhoneForget();
    void InitLDW();
    void InitWrongGear();
    void InitDTC();
    void InitSocLow();
    void InitParkingLight();

    int m_fdShareMemMcuData = -1;
    shem_t *m_pShareMemMcuData = NULL;
    timer_t m_timerId;

    XmlManager *m_xmlManager;
    
    static MsgHandler *m_msgHandler;

    static std::mutex m_mtxMsgListen;

private:
    void ActuatorMqttInit(const std::string &host, const int port);

    static int ActuatorMqttCallBack(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg);

    struct mosquitto *_mosq = nullptr;
    std::string _host;
    int _port;
    int _keep_alive = 60;
    bool _connected = false;
    std::string _mesg_pub_topic = "MessageHandler/publish";
    std::string _mesg_cb_topic = "MessageHandler/call_back";
};

#endif // !_ACTUATOR_H_
