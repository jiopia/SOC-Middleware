#include "CoreMsgHandler.hpp"
#include "Forwarder.hpp"

static void ReadCallBackFunc(const ECPVehicleValue &rData);

CoreMsgHandler::CoreMsgHandler()
{
    memset(&m_vehicleWorkData_t, 0, sizeof(m_vehicleWorkData_t));

    std::unique_ptr<ECPComm> comm = std::make_unique<ECPComm>(); //实例化一个Comm设备
    ptrECP = new ECPInterface(std::move(comm));                  //实例化ECP对象

    // auto ReadCallBackFunc = std::bind(&CoreMsgHandler::E02_MsgProcessor, this, std::placeholders::_1);
    ptrECP->registerGetDataCallBackFunc(ReadCallBackFunc, ECP_TYPE_MCU); //注册数据接收函数的回调函数，同时需要传一个回调函数的类型ECP_TYPE_MCU;

    std::vector<std::string> strTopicList;
    strTopicList.emplace_back(MQTT_TOPIC_WARN_HMI_TO_MW);

    m_connClient = std::make_shared<MqttConnection>(MQTT_HOST, MQTT_PORT, BDSTAR_SOCKET_MQTT);
    m_connClient->Subscribe(strTopicList);

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
        NodeAliveMsgHandler(rData.MsgID, ucMsgData, iDataLen);
        break;
    case MTYPE_INTERACTIVE:
        InteractiveMsgHandler(rData.MsgID, ucMsgData, iDataLen);
        break;
    case MTYPE_SETUP:
        SetupMsgHandler(rData.MsgID, ucMsgData, iDataLen);
        break;
    case MTYPE_KEY:
        KeyMsgHandler(rData.MsgID, ucMsgData, iDataLen);
        break;
    case MTYPE_EOL:
        EOLMsgHandler(rData.MsgID, ucMsgData, iDataLen);
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
    if (!strMsg.empty())
    {
        ECPVehicleValue requestValue_t;
        requestValue_t.length = 0;
        requestValue_t.MsgType = MTYPE_ACTION;
        requestValue_t.MsgID = MTYPE_ACTION_MSGID_CAR_LIGHT;

        ptrECP->setValue(requestValue_t, ECP_TYPE_MCU);
    }
}

void CoreMsgHandler::ActionMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen)
{
    if (ucMsgData == NULL || iDataLen == 0)
    {
        return;
    }

    if (uiMsgId != 0x01)
    {
        InfoPrint("Now Process Data Message. MsgId:[0x%02x], DataLen:[%d]\n", uiMsgId, iDataLen);
        DebugPrintMsg(ucMsgData, iDataLen);
    }

    switch (uiMsgId)
    {
    /* ACC状态 */
    case MTYPE_ACTION_MSGID_KEY:
        break;
    /* 车灯信号 */
    case MTYPE_ACTION_MSGID_CAR_LIGHT:
    {
        std::string strTelltaleTurnSingal;

        int iValueLow = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucLightSignalValueLow = iValueLow & 0xFF;
        memcpy(&m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t,
               &ucLightSignalValueLow,
               sizeof(m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t));

        char chMsgByte1Buff[16] = {0};
        HexToStr(chMsgByte1Buff, &ucLightSignalValueLow, 8);
        strTelltaleTurnSingal += std::string(chMsgByte1Buff);

        if (iDataLen > 1)
        {
            int iValueHigh = std::strtol(HexToStr(ucMsgData[1]).c_str(), 0, 16);
            unsigned char ucLightSignalValueHigh = iValueHigh & 0xFF;
            memcpy(&m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte2_t,
                   &ucLightSignalValueHigh,
                   sizeof(m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte2_t));

            char chMsgByte2Buff[16] = {0};
            HexToStr(chMsgByte2Buff, &ucLightSignalValueHigh, 8);
            strTelltaleTurnSingal += std::string(chMsgByte2Buff);
        }

        // m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t.turnLeftLight = GET_BIT(iLaneDepartureStatus & 0x01, 0);
        // m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t.turnRightLight = GET_BIT(iLaneDepartureStatus & 0x02, 1);
        // m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t.positionLight = GET_BIT(iLaneDepartureStatus & 0x04, 2);
        // m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t.positionLight = GET_BIT(iLaneDepartureStatus & 0x08, 3);
        // m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t.positionLight = GET_BIT(iLaneDepartureStatus & 0x10, 4);

        /* 指示灯 03 01 02 FF01 */
        std::string strMsgType = HexToStr(HMI_MSG_TYPE_TELLTALE);          //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_TELLTALE_TURN_SIGNAL);     //B3CC MsgId(左右转向灯)
        std::string strLen = HexToStr(strTelltaleTurnSingal.length() / 2); //B3CC DataLength
        std::string strMsgData = strTelltaleTurnSingal;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 车门状态 */
    case MTYPE_ACTION_MSGID_DOOR_WARNING:
    {
        int iValue = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucDoorOpenInfo = iValue & 0xFF;
        m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte3_t.doorOpenLight = ucDoorOpenInfo & 0x3F ? 0x01 : 0x00;

        char chMsgBuff[128] = {0};
        memcpy(chMsgBuff,
               &m_vehicleWorkData_t.telltaleData_t.warn_info_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.warn_info_t));

        std::string strTelltaleWarnInfo = chMsgBuff;
        /* 指示灯 03 01 02 FF01 */
        std::string strMsgType = HexToStr(HMI_MSG_TYPE_TELLTALE);        //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_TELLTALE_WARNING);       //B3CC MsgId(报警信息)
        std::string strLen = HexToStr(strTelltaleWarnInfo.length() / 2); //B3CC DataLength
        std::string strMsgData = strTelltaleWarnInfo;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 故障信息(发动机故障指示灯、发动机排放故障、蓄电池故障、制动液位低) */
    case MTYPE_ACTION_MSGID_FAULT:
    {
        int iValueByte1 = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucDVehicleFaultInfoByte1 = iValueByte1 & 0xFF;

        m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte6_t.svsLight = GET_BIT(ucDVehicleFaultInfoByte1, 0);
        m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte6_t.engineEmissionFailure = GET_BIT(ucDVehicleFaultInfoByte1, 1);

        if (iDataLen >= 2)
        {
            int iValueByte2 = std::strtol(HexToStr(ucMsgData[1]).c_str(), 0, 16);
            unsigned char ucDVehicleFaultInfoByte2 = iValueByte2 & 0xFF;

            m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte5_t.chargeFaultLight = GET_BIT(ucDVehicleFaultInfoByte2, 1);
            m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte5_t.ebdFaultLight = GET_BIT(ucDVehicleFaultInfoByte2, 3);
        }

        char chMsgBuff[128] = {0};
        memcpy(chMsgBuff,
               &m_vehicleWorkData_t.telltaleData_t.warn_info_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.warn_info_t));

        std::string strTelltaleWarnInfo = chMsgBuff;
        /* 指示灯 03 01 02 FF01 */
        std::string strMsgType = HexToStr(HMI_MSG_TYPE_TELLTALE);        //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_TELLTALE_WARNING);       //B3CC MsgId(报警信息)
        std::string strLen = HexToStr(strTelltaleWarnInfo.length() / 2); //B3CC DataLength
        std::string strMsgData = strTelltaleWarnInfo;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 报警信息(主/副安全带、机油压力、燃油低、水温高、ABS、小灯未关) */
    case MTYPE_ACTION_MSGID_ALARM:
    {
        int iValueByte1 = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucWarnInfoValueByte1 = iValueByte1 & 0xFF;

        m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte3_t.driverBeltLight = GET_BIT(ucWarnInfoValueByte1, 0);
        m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte3_t.passengerBeltLight = GET_BIT(ucWarnInfoValueByte1, 1);

        m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte6_t.oilPressureLow = GET_BIT(ucWarnInfoValueByte1, 3);

        if (iDataLen >= 2)
        {
            int iValueByte2 = std::strtol(HexToStr(ucMsgData[1]).c_str(), 0, 16);
            unsigned char ucWarnInfoValueByte2 = iValueByte2 & 0xFF;
            m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte5_t.oilFuelLowLight = GET_BIT(ucWarnInfoValueByte2, 0);
            m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte5_t.waterTempHigh = GET_BIT(ucWarnInfoValueByte2, 2);

            if (iDataLen >= 3)
            {
                int iValueByte3 = std::strtol(HexToStr(ucMsgData[2]).c_str(), 0, 16);
                unsigned char ucWarnInfoValueByte3 = iValueByte2 & 0xFF;
                m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte3_t.absLight = GET_BIT(ucWarnInfoValueByte3, 2);
            }
        }

        char chMsgBuff[128] = {0};
        memcpy(chMsgBuff,
               &m_vehicleWorkData_t.telltaleData_t.warn_info_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.warn_info_t));

        std::string strTelltaleWarnInfo = chMsgBuff;
        /* 指示灯 03 01 02 FF01 */
        std::string strMsgType = HexToStr(HMI_MSG_TYPE_TELLTALE);        //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_TELLTALE_WARNING);       //B3CC MsgId(报警信息)
        std::string strLen = HexToStr(strTelltaleWarnInfo.length() / 2); //B3CC DataLength
        std::string strMsgData = strTelltaleWarnInfo;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 电子手刹系统(EPB) */
    case MTYPE_ACTION_MSGID_EPB:
    {
        int iValueLow = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucEpbLightInfoLow = iValueLow & 0xFF;
        m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte4_t.epbRedLight = GET_BIT(ucEpbLightInfoLow, 1);
        m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte4_t.epbGreenLight = GET_BIT(ucEpbLightInfoLow, 3);
        m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte4_t.avhRedLight = GET_BIT(ucEpbLightInfoLow, 5);
        m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte4_t.avhGreenLight = GET_BIT(ucEpbLightInfoLow, 7);

        if (iDataLen > 1)
        {
            int iValueHigh = std::strtol(HexToStr(ucMsgData[1]).c_str(), 0, 16);
            unsigned char ucEpbLightInfoHigh = iValueHigh & 0xFF;
            m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte7_t.aebWorkLight = ucEpbLightInfoHigh & 0x02 ? 0x01 : 0x00;
            m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte7_t.aebFaultLight = ucEpbLightInfoHigh & 0x04 ? 0x01 : 0x00;
        }

        char chMsgBuff[128] = {0};
        memcpy(chMsgBuff,
               &m_vehicleWorkData_t.telltaleData_t.warn_info_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.warn_info_t));

        std::string strTelltaleWarnInfo = chMsgBuff;
        /* 指示灯 03 01 02 FF01 */
        std::string strMsgType = HexToStr(HMI_MSG_TYPE_TELLTALE);        //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_TELLTALE_WARNING);       //B3CC MsgId(报警信息)
        std::string strLen = HexToStr(strTelltaleWarnInfo.length() / 2); //B3CC DataLength
        std::string strMsgData = strTelltaleWarnInfo;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 安全气囊 */
    case MTYPE_ACTION_MSGID_AIRBAG:
    {
        int iValue = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucAirBagInfo = iValue & 0xFF;
        m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte4_t.airBagFaultLight = ucAirBagInfo & 0x01;

        char chMsgBuff[128] = {0};
        memcpy(chMsgBuff,
               &m_vehicleWorkData_t.telltaleData_t.warn_info_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.warn_info_t));

        std::string strTelltaleWarnInfo = chMsgBuff;
        /* 指示灯 03 01 02 FF01 */
        std::string strMsgType = HexToStr(HMI_MSG_TYPE_TELLTALE);        //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_TELLTALE_WARNING);       //B3CC MsgId(报警信息)
        std::string strLen = HexToStr(strTelltaleWarnInfo.length() / 2); //B3CC DataLength
        std::string strMsgData = strTelltaleWarnInfo;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    case MTYPE_ACTION_MSGID_PEPS:
        break;
    /* 车道偏离指示灯 */
    case MTYPE_ACTION_MSGID_LANE_DEPATURE:
    {
        int iValue = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucLdwLightInfo = iValue & 0xFF;

        m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte7_t.ldwLightGreen = ucLdwLightInfo & 0x02 ? 0x01 : 0x00;
        m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte7_t.ldwLightRed = ucLdwLightInfo & 0x05 ? 0x01 : 0x00;

        char chMsgBuff[128] = {0};
        memcpy(chMsgBuff,
               &m_vehicleWorkData_t.telltaleData_t.warn_info_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.warn_info_t));

        std::string strTelltaleWarnInfo = chMsgBuff;
        /* 指示灯 03 01 02 FF01 */
        std::string strMsgType = HexToStr(HMI_MSG_TYPE_TELLTALE);        //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_TELLTALE_WARNING);       //B3CC MsgId(报警信息)
        std::string strLen = HexToStr(strTelltaleWarnInfo.length() / 2); //B3CC DataLength
        std::string strMsgData = strTelltaleWarnInfo;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 胎压报警指示灯 */
    case MTYPE_ACTION_MSGID_ALARM_TIRE:
    {
        int iValue = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucLdwLightInfo = iValue & 0xFF;

        m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte3_t.tirePressureAbnormalLight = ucLdwLightInfo & 0x01;

        char chMsgBuff[128] = {0};
        memcpy(chMsgBuff,
               &m_vehicleWorkData_t.telltaleData_t.warn_info_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.warn_info_t));

        std::string strTelltaleWarnInfo = chMsgBuff;
        /* 指示灯 03 01 02 FF01 */
        std::string strMsgType = HexToStr(HMI_MSG_TYPE_TELLTALE);        //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_TELLTALE_WARNING);       //B3CC MsgId(报警信息)
        std::string strLen = HexToStr(strTelltaleWarnInfo.length() / 2); //B3CC DataLength
        std::string strMsgData = strTelltaleWarnInfo;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    case MTYPE_ACTION_MSGID_OILLOW:
    {
        int iValue = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucOilLightInfo = iValue & 0xFF;

        m_vehicleWorkData_t.telltaleData_t.warn_info_t.byte6_t.oilPressureLow = ucOilLightInfo & 0x01;

        char chMsgBuff[128] = {0};
        memcpy(chMsgBuff,
               &m_vehicleWorkData_t.telltaleData_t.warn_info_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.warn_info_t));

        std::string strTelltaleWarnInfo = chMsgBuff;
        /* 指示灯 03 01 02 FF01 */
        std::string strMsgType = HexToStr(HMI_MSG_TYPE_TELLTALE);        //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_TELLTALE_WARNING);       //B3CC MsgId(报警信息)
        std::string strLen = HexToStr(strTelltaleWarnInfo.length() / 2); //B3CC DataLength
        std::string strMsgData = strTelltaleWarnInfo;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
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
        std::string strVehicleSpeedValue = FloatToStr(IEE754_HexToFloat(&ucMsgData[4])); //MsgByte[4] ~ MsgByte[7]
        InfoPrint("VehicleSpeed:");
        DebugPrintMsg(&ucMsgData[4], 4);
        std::string strEngineSpeedValue = FloatToStr(IEE754_HexToFloat(ucMsgData)); //MsgByte[0] ~ MsgByte[3]
        InfoPrint("EngineSpeed:");
        DebugPrintMsg(ucMsgData, 4);

        InfoPrint("Float Vehicle Speed:%s", strVehicleSpeedValue.c_str());
        InfoPrint("Float  Engine Speed:%s", strEngineSpeedValue.c_str());

        {
            /* 车速 02 01 04 000006F1 */
            std::string strMsgType = HexToStr(HMI_MGS_TYPE_GUAGE);            //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_GUAGE_ID_SPEED);          //B3CC MsgId(车速)
            std::string strLen = UIntToHexStr(strVehicleSpeedValue.length()); //B3CC DataLength
            std::string strMsgData = strVehicleSpeedValue;

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }

        {
            /* 转速 02 02 04 00000FA0 */
            std::string strMsgType = HexToStr(HMI_MGS_TYPE_GUAGE);           //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_GUAGE_ID_RPM);           //B3CC MsgId(转速)
            std::string strLen = UIntToHexStr(strEngineSpeedValue.length()); //B3CC DataLength
            std::string strMsgData = strEngineSpeedValue;

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }
    }
    break;
    /* 燃油量 */
    case MTYPE_DATA_MSGID_FULE_TRAVLE:
    {
        unsigned int iFuelVolume = ucMsgData[0];
        InfoPrint("Fuel Volume:"); //燃油量
        DebugPrintMsg(ucMsgData, iDataLen);

        /* 
            燃油量: 02 04 02 XXYY
            XX：燃油量(0%~100%)
            YY：显示段(仪表格段显示，目前最多支持8段)
        */
        static unsigned char iOilVolumeValue = 0x00;
        int iValumePercentValue = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        InfoPrint("Valume Percent Value:[%d]\n", iValumePercentValue);
        if (0 == iValumePercentValue)
        {
            iOilVolumeValue = 0x00;
        }
        else if (1 <= iValumePercentValue && iValumePercentValue < 13)
        {
            iOilVolumeValue = 0x01;
        }
        else if (13 <= iValumePercentValue && iValumePercentValue < 25)
        {
            iOilVolumeValue = 0x02;
        }
        else if (25 <= iValumePercentValue && iValumePercentValue < 37)
        {
            iOilVolumeValue = 0x03;
        }
        else if (37 <= iValumePercentValue && iValumePercentValue < 49)
        {
            iOilVolumeValue = 0x04;
        }
        else if (49 <= iValumePercentValue && iValumePercentValue < 61)
        {
            iOilVolumeValue = 0x05;
        }
        else if (61 <= iValumePercentValue && iValumePercentValue < 73)
        {
            iOilVolumeValue = 0x06;
        }
        else if (73 <= iValumePercentValue && iValumePercentValue < 85)
        {
            iOilVolumeValue = 0x07;
        }
        else if (85 <= iValumePercentValue && iValumePercentValue <= 100)
        {
            iOilVolumeValue = 0x08;
        }

        std::string strMsgType = HexToStr(HMI_MGS_TYPE_GUAGE);  //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_GUAGE_ID_FUEL); //B3CC MsgId(燃油量)
        std::string strLen = HexToStr(0x01);                    //B3CC DataLength
        std::string strMsgData = HexToStr(iOilVolumeValue);

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
        static unsigned char iCoolantTempVolumeValue = 0x00;
        if (fCoolantTemp < 0.0002)
        {
            iCoolantTempVolumeValue = 0x00;
        }
#endif

        /* 冷却液温度 02 03 04 000006F1 */
        std::string strMsgType = HexToStr(HMI_MGS_TYPE_GUAGE);                 //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_GUAGE_ID_COOLANT_TEMPERATURE); //B3CC MsgId(冷却液温度)
        std::string strLen = HexToStr(0x01);                                   //B3CC DataLength
        std::string strMsgData = HexToStr(ucMsgData[2]);

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 总里程 */
    case MTYPE_DATA_MSGID_CAN_TOTAL_MILEAGE:
    {
        if (iDataLen < 3)
        {
            ErrPrint("CAN Total Mileage Data From MCU Data!");
        }

        unsigned int ucData = 0x000000;
        memcpy(&ucData, ucMsgData, 3);
        std::string strMsgTmp = UIntToHexStr(ucData);

        std::string strMsgType = HexToStr(HMI_MGS_TYPE_ODO);   //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_ODO_ID_ODO);   //B3CC MsgId
        std::string strLen = HexToStr(strMsgTmp.length() / 2); //B3CC DataLength
        std::string strMsgData = strMsgTmp;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 小计里程 */
    case MTYPE_DATA_MSGID_TOTAL_MILEAGE:
    {
        if (iDataLen < 4)
        {
            ErrPrint("Total Mileage Data From MCU Data!");
        }

        unsigned int ucData = 0x000000;
        memcpy(&ucData, &ucMsgData[1], 3);
        std::string strMsgTmp = UIntToHexStr(ucData);

        std::string strMsgType = HexToStr(HMI_MGS_TYPE_ODO);    //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_ODO_ID_TRIP_A); //B3CC MsgId
        std::string strLen = HexToStr(strMsgTmp.length() / 2);  //B3CC DataLength
        std::string strMsgData = strMsgTmp;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 续航里程 */
    case MTYPE_DATA_MSGID_ENDURANCE_MILEAGE:
    {
        if (iDataLen < 2)
        {
            ErrPrint("Endurance Mileage Data From MCU Data!");
        }

        unsigned int ucData = 0x000000;
        memcpy(&ucData, ucMsgData, 2);
        std::string strMsgTmp = UIntToHexStr(ucData);

        std::string strMsgType = HexToStr(HMI_MSG_TYPE_ECU);       //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_ECU_ID_ENDURANCE); //B3CC MsgId
        std::string strLen = HexToStr(strMsgTmp.length() / 2);     //B3CC DataLength
        std::string strMsgData = strMsgTmp;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 平均油耗 */
    case MTYPE_DATA_MSGID_AVERAGE_FUEL:
    {
        if (iDataLen < 2)
        {
            ErrPrint("Average Fuel Data From MCU Data!");
        }

        unsigned int ucData = 0x000000;
        memcpy(&ucData, ucMsgData, 2);
        std::string strMsgTmp = UIntToHexStr(ucData);

        std::string strMsgType = HexToStr(HMI_MSG_TYPE_ECU);      //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_ECU_ID_AVE_FUEL); //B3CC MsgId
        std::string strLen = HexToStr(strMsgTmp.length() / 2);    //B3CC DataLength
        std::string strMsgData = strMsgTmp;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 瞬时油耗 */
    case MTYPE_DATA_MSGID_INTANT_FUEL:
    {
        if (iDataLen < 2)
        {
            ErrPrint("Intant Fuel Data From MCU Data!");
        }

        unsigned int ucData = 0x000000;
        memcpy(&ucData, ucMsgData, 2);
        std::string strMsgTmp = UIntToHexStr(ucData);

        std::string strMsgType = HexToStr(HMI_MSG_TYPE_ECU);      //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_ECU_ID_INS_FUEL); //B3CC MsgId
        std::string strLen = HexToStr(strMsgTmp.length() / 2);    //B3CC DataLength
        std::string strMsgData = strMsgTmp;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 环境温度 */
    case MTYPE_DATA_MSGID_EXTERNEL_TEMPERATURE:
    {
        if (iDataLen < 5)
        {
            ErrPrint("Temperature Data From MCU Data!");
        }

        unsigned int ucTempInside = 0x00000000;
        unsigned int ucTempOutside = 0x00000000;
        memcpy(&ucTempInside, ucMsgData, 2);
        memcpy(&ucTempOutside, &ucMsgData[2], 2);

        std::string strMsgTempInside = UIntToHexStr(ucTempInside);
        std::string strMsgTempOutside = UIntToHexStr(ucTempOutside);
        std::string strMsgTmp = strMsgTempInside + strMsgTempOutside;

        std::string strMsgType = HexToStr(HMI_MGS_TYPE_ODO);                 //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_ODO_ID_OUTSIDE_TEMPERATURE); //B3CC MsgId
        std::string strLen = HexToStr(strMsgTmp.length() / 2);               //B3CC DataLength
        std::string strMsgData = strMsgTmp;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    default:
        break;
    }
}

void CoreMsgHandler::NodeAliveMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen)
{
    InfoPrint("Now Process Data Message. MsgId:[0x%02x], DataLen:[%d]\n", uiMsgId, iDataLen);
    DebugPrintMsg(ucMsgData, iDataLen);
}

void CoreMsgHandler::InteractiveMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen)
{
    InfoPrint("Now Process Data Message. MsgId:[0x%02x], DataLen:[%d]\n", uiMsgId, iDataLen);
    DebugPrintMsg(ucMsgData, iDataLen);
}

void CoreMsgHandler::SetupMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen)
{
    InfoPrint("Now Process Data Message. MsgId:[0x%02x], DataLen:[%d]\n", uiMsgId, iDataLen);
    DebugPrintMsg(ucMsgData, iDataLen);
}

void CoreMsgHandler::KeyMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen)
{
    InfoPrint("Now Process Data Message. MsgId:[0x%02x], DataLen:[%d]\n", uiMsgId, iDataLen);
    DebugPrintMsg(ucMsgData, iDataLen);
}

void CoreMsgHandler::EOLMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen)
{
    InfoPrint("Now Process Data Message. MsgId:[0x%02x], DataLen:[%d]\n", uiMsgId, iDataLen);
    DebugPrintMsg(ucMsgData, iDataLen);
}

static void ReadCallBackFunc(const ECPVehicleValue &rData)
{
    CoreMsgHandler::GetInstance()->E02_MsgReciever(rData);
}