#include "Actuator.h"
#include "AudioControl.h"
#include "JsonHandler.h"
#include "Forwarder.hpp"
#include <semaphore.h>

sem_t g_warnShowSem;

Actuator::Actuator() : m_xmlManager(XmlManager::GetInstance()),
                       m_msgHandler(MsgHandler::GetInstance())
{
    /*while*/ if ((this->m_fdShareMemMcuData = shm_open(MCU_DATA_SHARED, O_RDWR, S_IRWXU)) == -1)
    {
        ErrPrint("Failed to open shared data [%s]!\n", MCU_DATA_SHARED);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    this->m_pShareMemMcuData = (shem_t *)mmap(0, sizeof(shem_t), PROT_READ | PROT_WRITE,
                                              MAP_SHARED, m_fdShareMemMcuData, 0);

    sem_init(&g_warnShowSem, 0, 0);

    std::vector<std::string> strTopicList;
    strTopicList.emplace_back(std::string(MQTT_TOPIC_WARN_VIEW));

    m_mqttClient = std::make_shared<MqttConnection>(MQTT_HOST, MQTT_PORT, BDSTAR_SOCKET_MQTT);
    m_mqttClient->Subscribe(strTopicList);
    m_mqttClient->Run();
}

Actuator::~Actuator()
{
    if (this->m_pShareMemMcuData != NULL)
    {
        munmap(m_pShareMemMcuData, sizeof(shem_t));
        close(m_fdShareMemMcuData);
        shm_unlink(MCU_DATA_SHARED);
        WarnShowTimerDelete();
    }
}

void Actuator::Run()
{
    InfoPrint("RUN......\n");

    m_msgHandler->Start();

    std::thread msgRecieveTh(std::bind(&Actuator::MsgReciever, this));
    msgRecieveTh.detach();

    while (1)
    {
        sem_wait(&g_warnShowSem);
        m_msgHandler->RemoveCurrentWarn();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Actuator::MsgReciever()
{
    while (1)
    {
        std::string strMsg = Forwarder::GetInstance()->MsgPop();
        if (!strMsg.empty())
        {
            MsgProcessor(strMsg);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Actuator::MsgProcessor(std::string strMsg)
{
    InfoPrint("Recieve Warning Message From Mqtt:[%s]\r\n", strMsg.c_str());
    std::string strViewName, strExtraInfo, strViewStatus;
    JsonHandler::GetInstance()->WarnMsgParse(strMsg, strViewName, strExtraInfo, strViewStatus);
    if (!strViewName.empty() &&
        !strExtraInfo.empty() &&
        !strViewStatus.empty())
    {
        this->m_msgHandler->SetWarnView(strViewName.c_str(),
                                        strExtraInfo.c_str(), strViewStatus.compare("ON") == 0 ? VIEW_ON : VIEW_OFF);
    }
}

static void WarnShowTimerThreadFun(union sigval arg)
{
    sem_post(&g_warnShowSem);
}

int Actuator::WarnShowTimerInit()
{
    struct sigevent event;
    int retCode;

    event.sigev_notify = SIGEV_THREAD;
    event.sigev_value.sival_ptr = NULL;
    event.sigev_notify_function = WarnShowTimerThreadFun;
    event.sigev_notify_attributes = NULL;

    retCode = timer_create(CLOCK_MONOTONIC, &event, &this->m_timerId);
    if (retCode != EOK)
    {
        ErrPrint("Failed to Create WarnShow Timer. ErrCode:[%s].\n", strerror(retCode));
        exit(1);
    }

    return retCode;
}

int Actuator::WarnShowTimerStart(int iTimeWaitBeforeStart)
{
    struct itimerspec iTimer;
    int retCode;

    /* The time wait for first start. */
    iTimer.it_value.tv_sec = iTimeWaitBeforeStart; // 3/5
    iTimer.it_value.tv_nsec = 0;

    /* the interval after start. */
    iTimer.it_interval.tv_sec = 0;
    iTimer.it_interval.tv_nsec = 0;

    retCode = timer_settime(this->m_timerId, 0, &iTimer, NULL);
    if (retCode != EOK)
    {
        ErrPrint("Failed to Set WarnShow Time. ErrCode:[%s].\n", strerror(retCode));
    }

    return retCode;
}

int Actuator::WarnShowTimerStop()
{
    struct itimerspec iTimer;
    int retCode;

    /* The time wait for first start.When value is 0, mean stop it. */
    iTimer.it_value.tv_sec = 0;
    iTimer.it_value.tv_nsec = 0;

    /* the interval after start. */
    iTimer.it_interval.tv_sec = 0;
    iTimer.it_interval.tv_nsec = 0;

    retCode = timer_settime(this->m_timerId, 0, &iTimer, NULL);
    if (retCode != EOK)
    {
        ErrPrint("Failed to Set WarnShow Timer. ErrCode:[%s].\n", strerror(retCode));
    }

    return retCode;
}

int Actuator::WarnShowTimerDelete()
{
    int retCode = timer_delete(this->m_timerId);
    if (retCode != EOK)
    {
        ErrPrint("Failed to Delete WarnShow Timer. ErrCode:[%s].\n", strerror(retCode));
    }

    return retCode;
}

bool Actuator::IsIgnOFFCheck()
{
    if (ACC_OFF == this->m_pShareMemMcuData->response_status_data.ignkey_status_response)
    {
        return true;
    }

    return false;
}
