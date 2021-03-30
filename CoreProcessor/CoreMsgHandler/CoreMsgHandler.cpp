#include "CoreMsgHandler.hpp"
#include "Forwarder.hpp"

CoreMsgHandler::CoreMsgHandler()
{
    m_mqttClient = std::make_shared<MqttConnection>(MQTT_HOST, MQTT_PORT, BDSTAR_MOSQUITTP);
    m_mqttClient->Run();
}

CoreMsgHandler::~CoreMsgHandler()
{
}

void CoreMsgHandler::Run()
{
    std::thread msgRecieveTh(std::bind(&CoreMsgHandler::MsgReciever, this));
    msgRecieveTh.detach();

    std::string strMsgSend;
    while (1)
    {
        InfoPrint("RUN......\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        time_t now_time = time(NULL);
        tm *t_tm = localtime(&now_time);

        strMsgSend = asctime(t_tm);
        strMsgSend = strMsgSend.substr(0, strMsgSend.length() - 1);
        m_mqttClient->MsgSend(std::string(MQTT_TOPIC_WARN_VIEW), strMsgSend);
    }
}

void CoreMsgHandler::MsgReciever()
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

void CoreMsgHandler::MsgProcessor(std::string strMsg)
{
    InfoPrint("Message:[%s] Comming\r\n", strMsg.c_str());
}