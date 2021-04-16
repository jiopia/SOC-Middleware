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

    m_connClient = std::make_shared<MqttConnection>(MQTT_HOST, MQTT_PORT, BDSTAR_SOCKET_MQTT);
    m_connClient->Run();
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

    switch (rData.MsgType)
    {
    case MTYPE_DUMMY:
        break;
    case MTYPE_ACTION:
        ActionMsgHandler(rData.MsgID, ucMsgData, iDataLen);
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

void CoreMsgHandler::ActionMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen)
{
    if (ucMsgData == NULL)
    {
        return;
    }

    // DebugPrint("Now Process Data Message. MsgId:[0x%02x], DataLen:[%d]\n", uiMsgId, iDataLen);
    switch (uiMsgId)
    {
    case MTYPE_ACTION_MSGID_DOOR_WARNING:
        InfoPrint("Door Status:[0x%02X]", ucMsgData[0]);
        break;
    default:
        break;
    }
}

/* 车速、发动机转速、燃油量、水温、里程 */
void CoreMsgHandler::DataMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen)
{
    if (ucMsgData == NULL)
    {
        return;
    }

    DebugPrint("Now Process Data Message. MsgId:[0x%02x], DataLen:[%d]\n", uiMsgId, iDataLen);
    switch (uiMsgId)
    {
    /* 车速&转速 */
    case MTYPE_DATA_MSGID_SPEED_AND_RPM:
    {
        unsigned int iEngineSpeedValue = (ucMsgData[1] << 8) | ucMsgData[0];
        unsigned int iVehicleSpeedValue = (ucMsgData[3] << 8) | ucMsgData[2];
        InfoPrint(" Engine Speed:[0x%02X%X], [%d r/min]", ucMsgData[1], ucMsgData[0], iEngineSpeedValue); //0x0FA0
        InfoPrint("Vehicle Speed:[0x%02X%X], [%d km/h]", ucMsgData[3], ucMsgData[2], iVehicleSpeedValue); //0x06F1

        {
            /* 车速 02 01 04 000006F1 */
            std::string strMsgType = HexToStr(MTYPE_DATA); //B3CC MsgType
            std::string strMsgId = HexToStr(0x01);         //B3CC MsgId(车速)
            std::string strLen = HexToStr(0x04);           //B3CC DataLength
            std::string strMsgRemain = HexToStr(0x00) + HexToStr(0x00);
            std::string strMsgData = strMsgRemain + HexToStr(ucMsgData[3]) + HexToStr(ucMsgData[2]);

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }

        {
            /* 转速 02 02 04 00000FA0 */
            std::string strMsgType = HexToStr(MTYPE_DATA); //B3CC MsgType
            std::string strMsgId = HexToStr(0x02);         //B3CC MsgId(转速)
            std::string strLen = HexToStr(0x04);           //B3CC DataLength
            std::string strMsgRemain = HexToStr(0x00) + HexToStr(0x00);
            std::string strMsgData = strMsgRemain + HexToStr(ucMsgData[1]) + HexToStr(ucMsgData[0]);

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }
    }
    break;
    /* 燃油量 */
    case MTYPE_DATA_MSGID_FULE_TRAVLE:
    {
        unsigned int iFuelVolume = (ucMsgData[1] << 8) | ucMsgData[0];
        InfoPrint("Fuel Volume:[0x%02X %02X]", ucMsgData[1], ucMsgData[0]); //燃油量、显示段

        /* 
            燃油量: 02 04 02 XXYY
            XX：燃油量(0%~100%)
            YY：显示段(仪表格段显示，目前最多支持8段)
        */
        static unsigned char iVolumeValue = 0x00;
        int iValumePercentValue = std::strtol(HexToStr(ucMsgData[1]).c_str(), 0, 16);
        InfoPrint("Valume Percent Value:[%d]\n", iValumePercentValue);
        if (0 == iValumePercentValue)
        {
            iVolumeValue = 0x00;
        }
        else if (1 <= iValumePercentValue && iValumePercentValue < 13)
        {
            iVolumeValue = 0x01;
        }
        else if (13 <= iValumePercentValue && iValumePercentValue < 25)
        {
            iVolumeValue = 0x02;
        }
        else if (25 <= iValumePercentValue && iValumePercentValue < 37)
        {
            iVolumeValue = 0x03;
        }
        else if (37 <= iValumePercentValue && iValumePercentValue < 49)
        {
            iVolumeValue = 0x04;
        }
        else if (49 <= iValumePercentValue && iValumePercentValue < 61)
        {
            iVolumeValue = 0x05;
        }
        else if (61 <= iValumePercentValue && iValumePercentValue < 73)
        {
            iVolumeValue = 0x06;
        }
        else if (73 <= iValumePercentValue && iValumePercentValue < 85)
        {
            iVolumeValue = 0x07;
        }
        else if (85 <= iValumePercentValue && iValumePercentValue <= 100)
        {
            iVolumeValue = 0x08;
        }

        std::string strMsgType = HexToStr(MTYPE_DATA); //B3CC MsgType
        std::string strMsgId = HexToStr(0x04);         //B3CC MsgId(燃油量)
        std::string strLen = HexToStr(0x01);           //B3CC DataLength
        std::string strMsgData = HexToStr(iVolumeValue);

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 水温 */
    case MTYPE_DATA_MSGID_COOLAND_TEMPERATURE:
    {
        unsigned int iCoolantTemp = (ucMsgData[1] << 8) | ucMsgData[0];
        InfoPrint("Coolant Temperature:[0x%02X %02X %02X]", ucMsgData[2], ucMsgData[1], ucMsgData[0]); //高8位、(低4位 + 有效位)、显示段
#if 0
        float fCoolantTemp;
        if (GET_BIT(ucMsgData[1], 3))
        {
            fCoolantTemp = 0.0;
        }
        else
        {
            fCoolantTemp = (((ucMsgData[2] & 0x7F) << 4) + ((ucMsgData[1] & 0xF0) >> 4)) * 0.1;
            if (GET_BIT(ucMsgData[2], 7) == 1)
            {
                fCoolantTemp = -1 * fCoolantTemp;
            }
        }
        InfoPrint("CoolantTemp:[%.4f]\n", fCoolantTemp);
        static unsigned char iVolumeValue = 0x00;
        if (fCoolantTemp < 0.0002)
        {
            iVolumeValue = 0x00;
        }
#endif

        /* 冷却液温度 02 03 04 000006F1 */
        std::string strMsgType = HexToStr(MTYPE_DATA); //B3CC MsgType
        std::string strMsgId = HexToStr(0x03);         //B3CC MsgId(冷却液温度)
        std::string strLen = HexToStr(0x01);           //B3CC DataLength
        std::string strMsgData = HexToStr(ucMsgData[2]);

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 总里程 */
    case MTYPE_DATA_MSGID_CAN_TOTAL_MILEAGE:
    {
    }
    break;
    /* 小计里程 */
    case MTYPE_DATA_MSGID_TOTAL_MILEAGE:
    {
    }
    break;
    /* 续航里程 */
    case MTYPE_DATA_MSGID_ENDURANCE_MILEAGE:
    {
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