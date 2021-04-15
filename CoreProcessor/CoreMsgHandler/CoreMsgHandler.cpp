#include "CoreMsgHandler.hpp"
#include "Forwarder.hpp"
#include "MsgDataDefine.h"

static void ReadCallBackFunc(const ECPVehicleValue &rData);

CoreMsgHandler::CoreMsgHandler()
{
    std::unique_ptr<ECPComm> comm = std::make_unique<ECPComm>(); //实例化一个Comm设备
    ptrECP = new ECPInterface(std::move(comm));                  //实例化ECP对象

    // auto ReadCallBackFunc = std::bind(&CoreMsgHandler::E02_MsgProcessor, this, std::placeholders::_1);
    ptrECP->registerGetDataCallBackFunc(ReadCallBackFunc, ECP_TYPE_MCU); //注册数据接收函数的回调函数，同时需要传一个回调函数的类型ECP_TYPE_MCU;

    m_mqttClient = std::make_shared<MqttConnection>(MQTT_HOST, MQTT_PORT, BDSTAR_SOCKET_MQTT);
    m_mqttClient->Run();
}

CoreMsgHandler::~CoreMsgHandler()
{
    if ((ptrECP) != NULL)
    {
        delete ptrECP;
        ptrECP = NULL;
    }
}

void CoreMsgHandler::E02_MsgReciever(ECPVehicleValue rData)
{
    const unsigned char *ucMsgData = rData.RawData.data();
    int iDataLen = rData.RawData.size();
    // InfoPrint("now callback: length 0x%x  ---- MsgType:0x%x, MsgID:0x%x ", (uint32_t)(rData.length), rData.MsgType, rData.MsgID);
    // for (int i = 0; i < rData.RawData.size(); i++)
    // {
    //     printf(" 0x%02x", ucMsgData[i]);
    // }
    // printf("\n");

    //deal with data
    switch (rData.MsgType)
    {
    case MTYPE_DUMMY:
        break;
    case MTYPE_ACTION:
        ActionMsgHandler(rData.MsgID, ucMsgData);
        break;
    case MTYPE_DATA:
        DataMsgHandler(rData.MsgID, ucMsgData, iDataLen);
        break;
    case MTYPE_NODE_ALIVE:
        NodeAliveMsgHandler(rData.MsgID, ucMsgData);
        break;
    case MTYPE_INTERACTIVE:
        InteractiveMsgHandler(rData.MsgID, ucMsgData);
        break;
    case MTYPE_SETUP:
        SetupMsgHandler(rData.MsgID, ucMsgData);
        break;
    case MTYPE_KEY:
        KeyMsgHandler(rData.MsgID, ucMsgData);
        break;
    case MTYPE_EOL:
        EOLMsgHandler(rData.MsgID, ucMsgData);
        break;
    default:
        break;
    }

    return;
}

void CoreMsgHandler::Run()
{
    std::thread msgRecieveTh(std::bind(&CoreMsgHandler::MsgReciever, this));
    msgRecieveTh.detach();

    InfoPrint("RUN......\n");
    std::string strMsgSend;
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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

void CoreMsgHandler::ActionMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData)
{
}

/* 车速、发动机转速、燃油量、里程 */
void CoreMsgHandler::DataMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen)
{
    if (ucMsgData == NULL)
    {
        return;
    }

    DebugPrint("Now Process Data Message. MsgId:[0x%02x]\n", uiMsgId);
    switch (uiMsgId)
    {
    case MTYPE_DATA_MSGID_SPEED_AND_RPM:
    {
        unsigned int iEngineSpeedValue = (ucMsgData[1] << 8) | ucMsgData[0];
        unsigned int iVehicleSpeedValue = (ucMsgData[3] << 8) | ucMsgData[2];
        InfoPrint(" Engine Speed:[0x%02X%X], [%d r/min]", ucMsgData[1], ucMsgData[0], iEngineSpeedValue); //0x0FA0
        InfoPrint("Vehicle Speed:[0x%02X%X], [%d km/h]", ucMsgData[3], ucMsgData[2], iVehicleSpeedValue); //0x06F1

        {
            /* 车速 */
            std::string strMsgType = "02";
            std::string strMsgId = "01"; //B3CC 车速
            std::string strLen = "04";
            std::string strMsgData = "000006F1"; //0x0FA0

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_mqttClient->MsgSend(std::string(MQTT_TOPIC_WARN_MIDDLEWARE_TO_HMI), strMsgSend);
        }

        {
            /* 转速 */
            std::string strMsgType = "02";
            std::string strMsgId = "02"; //B3CC 转速
            std::string strLen = "04";
            std::string strMsgData = "00000FA0"; //0x0FA0

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_mqttClient->MsgSend(std::string(MQTT_TOPIC_WARN_MIDDLEWARE_TO_HMI), strMsgSend);
        }
    }
    break;
    default:
        break;
    }
}

void CoreMsgHandler::NodeAliveMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData) {}

void CoreMsgHandler::InteractiveMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData) {}

void CoreMsgHandler::SetupMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData) {}

void CoreMsgHandler::KeyMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData) {}

void CoreMsgHandler::EOLMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData) {}

static void ReadCallBackFunc(const ECPVehicleValue &rData)
{
    CoreMsgHandler::GetInstance()->E02_MsgReciever(rData);
}