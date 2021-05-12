#include "CoreMsgHandler.hpp"
#include "Forwarder.hpp"

static void ReadCallBackFunc(const ECPVehicleValue &rData);

CoreMsgHandler::CoreMsgHandler() : m_jsonHandler(JsonHandler::GetInstance())
{
    memset(&m_vehicleWorkData_t, 0, sizeof(m_vehicleWorkData_t));

    std::unique_ptr<ECPComm> comm = std::make_unique<ECPComm>(); //实例化一个Comm设备
    ptrECP = new ECPInterface(std::move(comm));                  //实例化ECP对象

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

    if (ucMsgData == NULL || iDataLen == 0)
    {
        return;
    }

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
    if (!strMsg.empty() && strMsg.length() == 4)
    {
        typeInt2Chars nValChanges;
        nValChanges.int32Val[0] = strtol(strMsg.substr(0, 2).c_str(), 0, 10);    //B3CC MsgType
        nValChanges.int32Val[1] = strtol(strMsg.substr(2, 2).c_str(), 0, 10);    //B3CC MsgID
        vector<uint8_t> getVals(nValChanges.uint8Val, nValChanges.uint8Val + 8); //value data

        ECPVehicleValue requestValue_t;
        requestValue_t.MsgType = GET_PROPDATA; //E02_Get_MsgType
        requestValue_t.MsgID = 0x00;           //E02_Get_MsgID
        requestValue_t.RawData = getVals;      //E02_Get_MsgRawData
        requestValue_t.length = sizeof(requestValue_t.MsgType) +
                                sizeof(requestValue_t.MsgID) +
                                requestValue_t.RawData.size(); //E02_Get_DataLen

        ptrECP->setMcuValue(requestValue_t);
    }
}

void CoreMsgHandler::ActionMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen)
{
    InfoPrint("Now Process Data Message. MsgId:[0x%02x], DataLen:[%d]\n", uiMsgId, iDataLen);
    DebugPrintMsg(ucMsgData, iDataLen);

    IPCData ipcData;
    switch (uiMsgId)
    {
    /* ACC状态 */
    case MTYPE_ACTION_MSGID_KEY:
    {
        if (iDataLen != 1)
        {
            return;
        }

        switch (std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16))
        {
        case 0x00:
            m_vehicleStatus = VEHICLE_OFF;
            break;
        case 0x01:
            m_vehicleStatus = VEHICLE_ACC;
            break;
        case 0x02:
            m_vehicleStatus = VEHICLE_ON;
            break;
        case 0x03:
            m_vehicleStatus = VEHICLE_START;
            break;
        default:
            break;
        }
    }
    break;
    /* 车灯信号 */
    case MTYPE_ACTION_MSGID_CAR_LIGHT:
    {
        int iValueLow = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucLightSignalValueLow = iValueLow & 0xFF;
        memcpy(&m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t,
               &ucLightSignalValueLow,
               sizeof(m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t));

        m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t.dangerLight =
            (m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t.turnLeftLight & 0x01) &&
            (m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t.turnRightLight & 0x01);

        char chMsgByte1Buff[4] = {0};
        HexToStr(chMsgByte1Buff, &ucLightSignalValueLow, sizeof(ucLightSignalValueLow));

        if (iDataLen > 1)
        {
            int iValueHigh = std::strtol(HexToStr(ucMsgData[1]).c_str(), 0, 16);
            unsigned char ucLightSignalValueHigh = iValueHigh & 0xFF;
            memcpy(&m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte2_t,
                   &ucLightSignalValueHigh,
                   sizeof(m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte2_t));
        }

        /* 左右转向灯 03 02 02 0101 */
        ipcData.m_msgType = HMI_MSG_TYPE_TELLTALE;      //B3CC MsgType
        ipcData.m_msgID = HMI_MGS_TELLTALE_TURN_SIGNAL; //B3CC MsgId
        ipcData.m_msgLength = 0x02;                     //B3CC DataLength
        memcpy(ipcData.m_msgData,
               &m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t)); //B3CC MsgData Byte[1]
        memcpy(&ipcData.m_msgData[1],
               &m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte2_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte2_t)); //B3CC MsgData Byte[2]

        if (m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t.dangerLight == 0x00)
        {
            if (m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t.turnLeftLight & 0x01 ||
                m_vehicleWorkData_t.telltaleData_t.turn_light_t.byte1_t.turnRightLight & 0x01)
            {
                this->SendAudioWarnInfo("turn_on", "ON");
            }
            else
            {
                /* 会有BUG:任意触发车灯状态都会报 */
                this->SendAudioWarnInfo("turn_off", "ON");
            }
        }
    }
    break;
    /* 车门状态 */
    case MTYPE_ACTION_MSGID_DOOR_WARNING:
    {
        int iValue = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucDoorOpenInfo = iValue & 0xFF;
        unsigned char uchDoorOpenLightStatus = (ucDoorOpenInfo & 0x3F) ? 0x01 : 0x00;

        std::string strViewName;
        std::string strExtraInfo;
        std::string strStatus;

        if (m_vehicleWorkData_t.gaugeData_t.vehicleSpeed > g_fSppedDoorOpenWarn)
        {
            /* 门开报警 */
            strViewName = "doorwarn";
        }
        else
        {
            /* 门开提示 */
            strViewName = "doorinfo";
        }

        /* 仅以后备箱门作为测试 */
        if ((m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte3_t.doorOpenLight == 0x01) &&
            (uchDoorOpenLightStatus == 0x00))
        {
            m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte3_t.doorOpenLight = 0x00;

            if (m_vehicleWorkData_t.warnInfo_t.door_info_t.frontLeftDoor)
            {
                this->SendViewPageInfo(strViewName, "DRIVER_DOOR", "OFF");
            }

            if (m_vehicleWorkData_t.warnInfo_t.door_info_t.frontRightDoor)
            {
                this->SendViewPageInfo(strViewName, "PASSENGER_DOOR", "OFF");
            }

            if (m_vehicleWorkData_t.warnInfo_t.door_info_t.rearRightDoor)
            {
                this->SendViewPageInfo(strViewName, "REAR_RIGHT_DOOR", "OFF");
            }

            if (m_vehicleWorkData_t.warnInfo_t.door_info_t.rearLeftDoor)
            {
                this->SendViewPageInfo(strViewName, "REAR_LEFT_DOOR", "OFF");
            }

            if (m_vehicleWorkData_t.warnInfo_t.door_info_t.hood)
            {
                this->SendViewPageInfo(strViewName, "HOOD", "OFF");
            }

            if (m_vehicleWorkData_t.warnInfo_t.door_info_t.trunk)
            {
                this->SendViewPageInfo(strViewName, "BACK_DOOR", "OFF");
            }

            memset(&m_vehicleWorkData_t.warnInfo_t.door_info_t, 0, sizeof(m_vehicleWorkData_t.warnInfo_t.door_info_t));
        }

        if (uchDoorOpenLightStatus)
        {
            unsigned char uchFrontLeftDoorStatus = ((ucDoorOpenInfo & DOOR_OPEN_FRONT_LEFT) == DOOR_OPEN_FRONT_LEFT) ? 0x01 : 0x00;
            if (m_vehicleWorkData_t.warnInfo_t.door_info_t.frontLeftDoor != uchFrontLeftDoorStatus)
            {
                m_vehicleWorkData_t.warnInfo_t.door_info_t.frontLeftDoor = uchFrontLeftDoorStatus;
                strStatus = uchFrontLeftDoorStatus == 0x01 ? "ON" : "OFF";
                this->SendViewPageInfo(strViewName, "DRIVER_DOOR", strStatus);
            }

            unsigned char uchFrontRightDoorStatus = ((ucDoorOpenInfo & DOOR_OPEN_FRONT_RIGHT) == DOOR_OPEN_FRONT_RIGHT) ? 0x01 : 0x00;
            if (m_vehicleWorkData_t.warnInfo_t.door_info_t.frontRightDoor != uchFrontRightDoorStatus)
            {
                m_vehicleWorkData_t.warnInfo_t.door_info_t.frontRightDoor = uchFrontRightDoorStatus;
                strStatus = uchFrontRightDoorStatus == 0x01 ? "ON" : "OFF";
                this->SendViewPageInfo(strViewName, "PASSENGER_DOOR", strStatus);
            }

            unsigned char uchRearRightDoorStatus = ((ucDoorOpenInfo & DOOR_OPEN_REAR_RIGHT) == DOOR_OPEN_REAR_RIGHT) ? 0x01 : 0x00;
            if (m_vehicleWorkData_t.warnInfo_t.door_info_t.rearRightDoor != uchRearRightDoorStatus)
            {
                m_vehicleWorkData_t.warnInfo_t.door_info_t.rearRightDoor = uchRearRightDoorStatus;
                strStatus = uchRearRightDoorStatus == 0x01 ? "ON" : "OFF";
                this->SendViewPageInfo(strViewName, "REAR_RIGHT_DOOR", strStatus);
            }

            unsigned char uchRearLeftDoorStatus = ((ucDoorOpenInfo & DOOR_OPEN_REAR_LEFT) == DOOR_OPEN_REAR_LEFT) ? 0x01 : 0x00;
            if (m_vehicleWorkData_t.warnInfo_t.door_info_t.rearLeftDoor != uchRearLeftDoorStatus)
            {
                m_vehicleWorkData_t.warnInfo_t.door_info_t.rearLeftDoor = uchRearLeftDoorStatus;
                strStatus = uchRearLeftDoorStatus == 0x01 ? "ON" : "OFF";
                this->SendViewPageInfo(strViewName, "REAR_LEFT_DOOR", strStatus);
            }

            unsigned char uchHoodStatus = ((ucDoorOpenInfo & DOOR_OPEN_HOOD) == DOOR_OPEN_HOOD) ? 0x01 : 0x00;
            if (m_vehicleWorkData_t.warnInfo_t.door_info_t.hood != uchHoodStatus)
            {
                m_vehicleWorkData_t.warnInfo_t.door_info_t.hood = uchHoodStatus;
                strStatus = uchHoodStatus == 0x01 ? "ON" : "OFF";
                this->SendViewPageInfo(strViewName, "HOOD", strStatus);
            }

            unsigned char uchTrunkStatus = ((ucDoorOpenInfo & DOOR_OPEN_TRUNK) == DOOR_OPEN_TRUNK) ? 0x01 : 0x00;
            if (m_vehicleWorkData_t.warnInfo_t.door_info_t.trunk != uchTrunkStatus)
            {
                m_vehicleWorkData_t.warnInfo_t.door_info_t.trunk = uchTrunkStatus;
                strStatus = uchTrunkStatus == 0x01 ? "ON" : "OFF";
                this->SendViewPageInfo(strViewName, "BACK_DOOR", strStatus);
            }
        }

        if (m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte3_t.doorOpenLight != uchDoorOpenLightStatus)
        {
            m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte3_t.doorOpenLight = uchDoorOpenLightStatus;

            /* 报警指示灯 03 01 07 XX...XX */
            ipcData.m_msgType = HMI_MSG_TYPE_TELLTALE;  //B3CC MsgType
            ipcData.m_msgID = HMI_MGS_TELLTALE_WARNING; //B3CC MsgId
            ipcData.m_msgLength = 0x07;                 //B3CC DataLength
            memcpy(ipcData.m_msgData,
                   &m_vehicleWorkData_t.telltaleData_t.warn_light_t,
                   sizeof(m_vehicleWorkData_t.telltaleData_t.warn_light_t)); //B3CC MsgData
        }
    }
    break;
    /* 故障信息(ucMsgData[1]:发动机故障指示灯、发动机排放故障; ucMsgData[0]:蓄电池故障、制动液位低) */
    case MTYPE_ACTION_MSGID_FAULT:
    {
        if (iDataLen != 2)
        {
            return;
        }

        int iValueByte1 = std::strtol(HexToStr(ucMsgData[1]).c_str(), 0, 16);
        unsigned char ucDVehicleFaultInfoByte1 = iValueByte1 & 0xFF;

        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte6_t.engineEmissionFailure = GET_BIT(ucDVehicleFaultInfoByte1, 0); //00 01
        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte6_t.svsLight = GET_BIT(ucDVehicleFaultInfoByte1, 1);              //00 02

        int iValueByte2 = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucDVehicleFaultInfoByte2 = iValueByte2 & 0xFF;

        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte5_t.chargeFaultLight = GET_BIT(ucDVehicleFaultInfoByte2, 1); //01 00
        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte5_t.ebdFaultLight = GET_BIT(ucDVehicleFaultInfoByte2, 3);    //08 00

        /* 报警指示灯 03 01 07 XX...XX */
        ipcData.m_msgType = HMI_MSG_TYPE_TELLTALE;  //B3CC MsgType
        ipcData.m_msgID = HMI_MGS_TELLTALE_WARNING; //B3CC MsgId
        ipcData.m_msgLength = 0x07;                 //B3CC DataLength
        memcpy(ipcData.m_msgData,
               &m_vehicleWorkData_t.telltaleData_t.warn_light_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.warn_light_t)); //B3CC MsgData
    }
    break;
    /* 报警信息(主/副安全带、机油压力、燃油低、水温高、ABS、小灯未关) */
    case MTYPE_ACTION_MSGID_ALARM:
    {
        if (iDataLen != 3)
        {
            return;
        }

        int iValueByte1 = std::strtol(HexToStr(ucMsgData[2]).c_str(), 0, 16);
        unsigned char ucWarnInfoValueByte1 = iValueByte1 & 0xFF;

        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte3_t.driverBeltLight = GET_BIT(ucWarnInfoValueByte1, 0);
        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte3_t.passengerBeltLight = GET_BIT(ucWarnInfoValueByte1, 1);
        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte6_t.oilPressureLow = GET_BIT(ucWarnInfoValueByte1, 3);

        int iValueByte2 = std::strtol(HexToStr(ucMsgData[1]).c_str(), 0, 16);
        unsigned char ucWarnInfoValueByte2 = iValueByte2 & 0xFF;
        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte5_t.oilFuelLowLight = GET_BIT(ucWarnInfoValueByte2, 0);
        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte5_t.waterTempHigh = GET_BIT(ucWarnInfoValueByte2, 2);

        int iValueByte3 = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucWarnInfoValueByte3 = iValueByte3 & 0xFF;
        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte3_t.absLight = GET_BIT(ucWarnInfoValueByte3, 2);

        if (m_vehicleWorkData_t.warnInfo_t.parking_light_t.parkingLight != GET_BIT(ucWarnInfoValueByte3, 4))
        {
            m_vehicleWorkData_t.warnInfo_t.parking_light_t.parkingLight = GET_BIT(ucWarnInfoValueByte3, 4);
            std::string strStatus = m_vehicleWorkData_t.warnInfo_t.parking_light_t.parkingLight ? "ON" : "OFF";
            this->SendViewPageInfo("parking", "parking", strStatus);
        }

        /* 报警指示灯 03 01 07 XX...XX */
        ipcData.m_msgType = HMI_MSG_TYPE_TELLTALE;  //B3CC MsgType
        ipcData.m_msgID = HMI_MGS_TELLTALE_WARNING; //B3CC MsgId
        ipcData.m_msgLength = 0x07;                 //B3CC DataLength
        memcpy(ipcData.m_msgData,
               &m_vehicleWorkData_t.telltaleData_t.warn_light_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.warn_light_t)); //B3CC MsgData
    }
    break;
    /* 电子手刹系统(EPB) */
    case MTYPE_ACTION_MSGID_EPB:
    {
        if (iDataLen != 2)
        {
            return;
        }

        int iValueLow = std::strtol(HexToStr(ucMsgData[1]).c_str(), 0, 16);
        unsigned char ucEpbLightInfoLow = iValueLow & 0xFF;
        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte4_t.epbRedLight = GET_BIT(ucEpbLightInfoLow, 0);
        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte4_t.epbGreenLight = GET_BIT(ucEpbLightInfoLow, 2);
        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte4_t.avhRedLight = GET_BIT(ucEpbLightInfoLow, 4);
        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte4_t.avhGreenLight = GET_BIT(ucEpbLightInfoLow, 6);

        int iValueHigh = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucEpbLightInfoHigh = iValueHigh & 0xFF;
        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte7_t.aebWorkLight = GET_BIT(ucEpbLightInfoHigh, 0);
        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte7_t.aebFaultLight = GET_BIT(ucEpbLightInfoHigh, 2);

        /* 报警指示灯 03 01 07 XX...XX */
        ipcData.m_msgType = HMI_MSG_TYPE_TELLTALE;  //B3CC MsgType
        ipcData.m_msgID = HMI_MGS_TELLTALE_WARNING; //B3CC MsgId
        ipcData.m_msgLength = 0x07;                 //B3CC DataLength
        memcpy(ipcData.m_msgData,
               &m_vehicleWorkData_t.telltaleData_t.warn_light_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.warn_light_t)); //B3CC MsgData
    }
    break;
    /* 安全气囊 */
    case MTYPE_ACTION_MSGID_AIRBAG:
    {
        int iValue = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucAirBagInfo = iValue & 0xFF;
        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte4_t.airBagFaultLight = ucAirBagInfo & 0x01;

        ipcData.m_msgType = HMI_MSG_TYPE_TELLTALE;  //B3CC MsgType
        ipcData.m_msgID = HMI_MGS_TELLTALE_WARNING; //B3CC MsgId
        ipcData.m_msgLength = 0x07;                 //B3CC DataLength
        memcpy(ipcData.m_msgData,
               &m_vehicleWorkData_t.telltaleData_t.warn_light_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.warn_light_t)); //B3CC MsgData
    }
    break;
    /* 无钥匙进入及启动系统 */
    case MTYPE_ACTION_MSGID_PEPS:
    {
        int iValue = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucPEPSInfo = iValue & 0xFF;

        if (m_vehicleWorkData_t.warnInfo_t.peps_info_t.peps4_NoKeyC != GET_BIT(ucPEPSInfo, 4))
        {
            m_vehicleWorkData_t.warnInfo_t.peps_info_t.peps4_NoKeyC = GET_BIT(ucPEPSInfo, 4);
            std::string strStatus = m_vehicleWorkData_t.warnInfo_t.peps_info_t.peps4_NoKeyC ? "ON" : "OFF";
            this->SendViewPageInfo("pepsnotfindb", "pepsnotfindb", strStatus);
        }

        if (m_vehicleWorkData_t.warnInfo_t.peps_info_t.peps6_KeyBattLow != GET_BIT(ucPEPSInfo, 5))
        {
            m_vehicleWorkData_t.warnInfo_t.peps_info_t.peps6_KeyBattLow = GET_BIT(ucPEPSInfo, 5);
            std::string strStatus = m_vehicleWorkData_t.warnInfo_t.peps_info_t.peps6_KeyBattLow ? "ON" : "OFF";
            this->SendViewPageInfo("pepsbatterylow", "pepsbatterylow", strStatus);
        }
    }
    break;
    /* 变速箱挡位 */
    case MTYPE_ACTION_MSGID_GEAR_BOX:
    {
        int iValue = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        m_vehicleWorkData_t.odoData_t.gear = iValue & 0xFF;

        ipcData.m_msgType = HMI_MGS_TYPE_ODO;                      //B3CC MsgType
        ipcData.m_msgID = HMI_MGS_ODO_ID_GEAR;                     //B3CC MsgId
        ipcData.m_msgLength = 0x01;                                //B3CC DataLength
        ipcData.m_msgData[0] = m_vehicleWorkData_t.odoData_t.gear; //B3CC MsgData
    }
    break;
    /* 车道偏离指示灯 */
    case MTYPE_ACTION_MSGID_LANE_DEPATURE:
    {
        int iValue = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucLdwLightInfo = iValue & 0xFF;

        if (ucLdwLightInfo == 0x05)
        {
            m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte7_t.ldwLightRed = 0x01;
        }
        else if (ucLdwLightInfo == 0x02)
        {
            m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte7_t.ldwLightGreen = 0x01;
        }
        else
        {
            m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte7_t.ldwLightRed = 0x00;
            m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte7_t.ldwLightGreen = 0x00;
        }

        /* 报警指示灯 03 01 07 XX...XX */
        ipcData.m_msgType = HMI_MSG_TYPE_TELLTALE;  //B3CC MsgType
        ipcData.m_msgID = HMI_MGS_TELLTALE_WARNING; //B3CC MsgId
        ipcData.m_msgLength = 0x07;                 //B3CC DataLength
        memcpy(ipcData.m_msgData,
               &m_vehicleWorkData_t.telltaleData_t.warn_light_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.warn_light_t)); //B3CC MsgData
    }
    break;
    /* 胎压报警指示灯 */
    case MTYPE_ACTION_MSGID_ALARM_TIRE:
    {
        int iValue = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucLdwLightInfo = iValue & 0xFF;

        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte3_t.tirePressureAbnormalLight = ucLdwLightInfo & 0x01;

        /* 报警指示灯 03 01 07 XX...XX */
        ipcData.m_msgType = HMI_MSG_TYPE_TELLTALE;  //B3CC MsgType
        ipcData.m_msgID = HMI_MGS_TELLTALE_WARNING; //B3CC MsgId
        ipcData.m_msgLength = 0x07;                 //B3CC DataLength
        memcpy(ipcData.m_msgData,
               &m_vehicleWorkData_t.telltaleData_t.warn_light_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.warn_light_t)); //B3CC MsgData
    }
    break;
    /* DTC文字提示(请挂入P档防止溜车) */
    case MTYPE_ACTION_MSGID_DTC:
    {
        if (iDataLen != 2)
        {
            return;
        }

        int iValue = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucDTCInfo = iValue & 0xFF;

        if (m_vehicleWorkData_t.warnInfo_t.dtc_info_t.switchToP != GET_BIT(ucDTCInfo, 6))
        {
            m_vehicleWorkData_t.warnInfo_t.dtc_info_t.switchToP = GET_BIT(ucDTCInfo, 6);
            std::string strStatus = m_vehicleWorkData_t.warnInfo_t.dtc_info_t.switchToP ? "ON" : "OFF";
            this->SendViewPageInfo("WrongGear_P", "WrongGear_P", strStatus);
        }
    }
    break;
    /* 燃油液位低 */
    case MTYPE_ACTION_MSGID_OILLOW:
    {
        int iValue = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        unsigned char ucOilLightInfo = iValue & 0xFF;

        m_vehicleWorkData_t.telltaleData_t.warn_light_t.byte6_t.oilPressureLow = ucOilLightInfo & 0x01;

        /* 报警指示灯 03 01 07 XX...XX */
        ipcData.m_msgType = HMI_MSG_TYPE_TELLTALE;  //B3CC MsgType
        ipcData.m_msgID = HMI_MGS_TELLTALE_WARNING; //B3CC MsgId
        ipcData.m_msgLength = 0x07;                 //B3CC DataLength
        memcpy(ipcData.m_msgData,
               &m_vehicleWorkData_t.telltaleData_t.warn_light_t,
               sizeof(m_vehicleWorkData_t.telltaleData_t.warn_light_t)); //B3CC MsgData
    }
    break;
    default:
        break;
    }

    if (ipcData.m_msgType != 0x00)
    {
        std::string strMsgType = HexToStr(ipcData.m_msgType);
        std::string strMsgId = HexToStr(ipcData.m_msgID);
        std::string strMsgLen = HexToStr(ipcData.m_msgLength);

        std::string strMsgSend = strMsgType + strMsgId + strMsgLen + UCharArrToStr(ipcData.m_msgData, ipcData.m_msgLength);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
}

/* 车速、发动机转速、燃油量、水温、里程 */
void CoreMsgHandler::DataMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen)
{
    DebugPrint("Now Process Data Message. MsgId:[0x%02x], DataLen:[%d]\n", uiMsgId, iDataLen);
    DebugPrintMsg(ucMsgData, iDataLen);

    switch (uiMsgId)
    {
    /* 车速&转速 */
    case MTYPE_DATA_MSGID_SPEED_AND_RPM:
    {
        if (iDataLen != 8)
        {
            break;
        }

        float fVehicleSpeed = IEE754_HexToFloat(&ucMsgData[4]);
        std::string strVehicleSpeedValue = FloatToStr(fVehicleSpeed); //MsgByte[4] ~ MsgByte[7]
        InfoPrint("VehicleSpeed:");
        DebugPrintMsg(&ucMsgData[4], 4);
        m_vehicleWorkData_t.gaugeData_t.vehicleSpeed = fVehicleSpeed;

        float fEngineSpeed = IEE754_HexToFloat(&ucMsgData[0]);
        std::string strEngineSpeedValue = FloatToStr(fEngineSpeed); //MsgByte[0] ~ MsgByte[3]
        InfoPrint("EngineSpeed:");
        DebugPrintMsg(ucMsgData, 4);
        m_vehicleWorkData_t.gaugeData_t.engineSpeed = fEngineSpeed;

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

        char *pFloat;
        float fSpeed = strtof(strVehicleSpeedValue.c_str(), &pFloat);

        if (fSpeed > g_fSpeedOverLimitAlarm)
        {
            if (m_vehicleWorkData_t.warnInfo_t.over_speed_t.overSpeed == 0x00)
            {
                /* 超速报警 */
                this->SendViewPageInfo("speedlimit", "speedlimit", "ON");
                m_vehicleWorkData_t.warnInfo_t.over_speed_t.overSpeed = 0x01;
            }
        }
        else
        {
            if (m_vehicleWorkData_t.warnInfo_t.over_speed_t.overSpeed == 0x01)
            {
                /* 超速报警 */
                this->SendViewPageInfo("speedlimit", "speedlimit", "OFF");
                m_vehicleWorkData_t.warnInfo_t.over_speed_t.overSpeed = 0x00;
            }
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
        static unsigned char uchOilVolumeValue = 0x00;
        int iValumePercentValue = std::strtol(HexToStr(ucMsgData[0]).c_str(), 0, 16);
        InfoPrint("Valume Percent Value:[%d]\n", iValumePercentValue);
        if (0 == iValumePercentValue)
        {
            uchOilVolumeValue = 0x00;
        }
        else if (1 <= iValumePercentValue && iValumePercentValue < 13)
        {
            uchOilVolumeValue = 0x01;
        }
        else if (13 <= iValumePercentValue && iValumePercentValue < 25)
        {
            uchOilVolumeValue = 0x02;
        }
        else if (25 <= iValumePercentValue && iValumePercentValue < 37)
        {
            uchOilVolumeValue = 0x03;
        }
        else if (37 <= iValumePercentValue && iValumePercentValue < 49)
        {
            uchOilVolumeValue = 0x04;
        }
        else if (49 <= iValumePercentValue && iValumePercentValue < 61)
        {
            uchOilVolumeValue = 0x05;
        }
        else if (61 <= iValumePercentValue && iValumePercentValue < 73)
        {
            uchOilVolumeValue = 0x06;
        }
        else if (73 <= iValumePercentValue && iValumePercentValue < 85)
        {
            uchOilVolumeValue = 0x07;
        }
        else if (85 <= iValumePercentValue && iValumePercentValue <= 100)
        {
            uchOilVolumeValue = 0x08;
        }

        m_vehicleWorkData_t.gaugeData_t.oilFuelVolumeValue = uchOilVolumeValue;

        {
            std::string strMsgType = HexToStr(HMI_MGS_TYPE_GUAGE);  //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_GUAGE_ID_FUEL); //B3CC MsgId(燃油量)
            std::string strLen = HexToStr(0x01);                    //B3CC DataLength
            std::string strMsgData = HexToStr(uchOilVolumeValue);

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }

        if (m_vehicleWorkData_t.gaugeData_t.oilFuelVolumeValue <= 0x02)
        {
            if (m_vehicleWorkData_t.warnInfo_t.oil_low_t.oilLowWarn != 0x01)
            {
                m_vehicleWorkData_t.warnInfo_t.oil_low_t.oilLowWarn = 0x01;
                this->SendViewPageInfo("oillow", "oillow", "ON");
            }
        }
        else if (m_vehicleWorkData_t.warnInfo_t.oil_low_t.oilLowWarn != 0x00)
        {
            m_vehicleWorkData_t.warnInfo_t.oil_low_t.oilLowWarn = 0x00;
            this->SendViewPageInfo("oillow", "oillow", "OFF");
        }
    }
    break;
    /* 水温(发动机冷却液温度) 00 00 00 EF 41 => 41 EF 00 00 => 30.875 ℃ */
    case MTYPE_DATA_MSGID_COOLAND_TEMPERATURE:
    {
        if (iDataLen != 5)
        {
            break;
        }

        float fCoolantTemp = IEE754_HexToFloat(&ucMsgData[1]);
        std::string strCoolantTemp = FloatToStr(fCoolantTemp); //MsgByte[4] ~ MsgByte[1]
        InfoPrint("CoolantTemp:[%.4f]\n", fCoolantTemp);
        InfoPrint("CoolantTemp HexData:");
        DebugPrintMsg(&ucMsgData[0], iDataLen);
        m_vehicleWorkData_t.gaugeData_t.coolantTemprature = fCoolantTemp;

        /* 冷却液温度 02 03 04 000006F1 */
        std::string strMsgType = HexToStr(HMI_MGS_TYPE_GUAGE);                 //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_GUAGE_ID_COOLANT_TEMPERATURE); //B3CC MsgId(冷却液温度)
        std::string strLen = HexToStr(strCoolantTemp.length());                //B3CC DataLength
        std::string strMsgData = strCoolantTemp;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 总里程 */
    case MTYPE_DATA_MSGID_TOTAL_MILEAGE:
    {
        if (iDataLen != 4)
        {
            ErrPrint("Total Mileage Data From MCU Data!");
            break;
        }

        float fTotalMileage = IEE754_HexToFloat(&ucMsgData[0]);
        std::string strTotalMileage = FloatToStr(fTotalMileage); //MsgByte[0] ~ MsgByte[3]
        InfoPrint("TotalMileage:[%.4f]\n", fTotalMileage);
        InfoPrint("TotalMileage HexData:");
        DebugPrintMsg(&ucMsgData[0], iDataLen);
        m_vehicleWorkData_t.odoData_t.totalMileage = fTotalMileage;

        std::string strMsgType = HexToStr(HMI_MGS_TYPE_ODO);     //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_ODO_ID_ODO);     //B3CC MsgId
        std::string strLen = HexToStr(strTotalMileage.length()); //B3CC DataLength
        std::string strMsgData = strTotalMileage;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 小计里程 */
    case MTYPE_DATA_MSGID_SUBTOTAL_MILEAGE:
    {
        if (iDataLen != 4)
        {
            ErrPrint("SubTotal Mileage Data From MCU Data!");
            break;
        }

        float fSubTotalMileage = IEE754_HexToFloat(&ucMsgData[0]);
        std::string strSubTotalMileage = FloatToStr(fSubTotalMileage); //MsgByte[0] ~ MsgByte[3]
        InfoPrint("SubTotalMileage:[%.4f]\n", fSubTotalMileage);
        InfoPrint("SubTotalMileage HexData:");

        m_vehicleWorkData_t.odoData_t.mileageSubtotalA = fSubTotalMileage;

        std::string strMsgType = HexToStr(HMI_MGS_TYPE_ODO);            //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_ODO_ID_TRIP_A);         //B3CC MsgId
        std::string strLen = HexToStr(strSubTotalMileage.length() / 2); //B3CC DataLength
        std::string strMsgData = strSubTotalMileage;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 续航里程 */
    case MTYPE_DATA_MSGID_ENDURANCE_MILEAGE:
    {
        if (iDataLen != 2)
        {
            ErrPrint("Endurance Mileage Data From MCU Data!");
            break;
        }

        std::string strData = HexToStr(ucMsgData[1]) + HexToStr(ucMsgData[0]);
        float fData = strtol(strData.c_str(), 0, 16) * 1.0;
        std::string strMsgTmp = FloatToStr(fData);

        std::string strMsgType = HexToStr(HMI_MSG_TYPE_ECU);       //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_ECU_ID_ENDURANCE); //B3CC MsgId
        std::string strLen = HexToStr(strMsgTmp.length());         //B3CC DataLength
        std::string strMsgData = strMsgTmp;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 平均油耗 */
    case MTYPE_DATA_MSGID_AVERAGE_FUEL:
    {
        if (iDataLen != 4)
        {
            ErrPrint("Average Fuel Data From MCU Data!");
            break;
        }

        float fAverageFuel = IEE754_HexToFloat(&ucMsgData[0]);
        std::string strAverageFuel = FloatToStr(fAverageFuel); //MsgByte[0] ~ MsgByte[3]
        InfoPrint("AverageFuel:[%.4f]\n", fAverageFuel);
        InfoPrint("AverageFuel HexData:");
        m_vehicleWorkData_t.ecuData_t.averageFuelConsumption = fAverageFuel;

        std::string strMsgType = HexToStr(HMI_MSG_TYPE_ECU);      //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_ECU_ID_AVE_FUEL); //B3CC MsgId
        std::string strLen = HexToStr(strAverageFuel.length());   //B3CC DataLength
        std::string strMsgData = strAverageFuel;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 瞬时油耗 */
    case MTYPE_DATA_MSGID_INTANT_FUEL:
    {
        if (iDataLen != 4)
        {
            ErrPrint("Intant Fuel Data From MCU Data!");
            break;
        }

        float fIntantFuel = IEE754_HexToFloat(&ucMsgData[0]);
        std::string strIntantFuel = FloatToStr(fIntantFuel); //MsgByte[0] ~ MsgByte[3]
        InfoPrint("IntantFuel:[%.4f]\n", fIntantFuel);
        InfoPrint("IntantFuel HexData:");
        m_vehicleWorkData_t.ecuData_t.instantFuelConsumption = fIntantFuel;

        std::string strMsgType = HexToStr(HMI_MSG_TYPE_ECU);      //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_ECU_ID_INS_FUEL); //B3CC MsgId
        std::string strLen = HexToStr(strIntantFuel.length());    //B3CC DataLength
        std::string strMsgData = strIntantFuel;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 环境温度 */
    case MTYPE_DATA_MSGID_EXTERNEL_TEMPERATURE:
    {
        if (iDataLen != 9)
        {
            ErrPrint("Temperature Data From MCU Data!");
            break;
        }

        /*
            [0x00] [0x00] [0x00] [0x40] [0x40] [0x00] [0x00] [0x40] [0x40] 
            => 00(温度单位0:℃,1:℉) 00004040(0x40400000=>3.0C) 00004040(0x40400000=>3.0C)
         */
        float fInsideTemp = IEE754_HexToFloat(&ucMsgData[1]);
        float fOutsideTemp = IEE754_HexToFloat(&ucMsgData[5]);
        printf("fInsideTemp:%f\n", fInsideTemp);
        printf("fOutsideTemp:%f\n", fOutsideTemp);

        char strInsideBuff[4] = {0};
        sprintf(strInsideBuff, "%.1f", fInsideTemp);
        char strOutsideBuff[4] = {0};
        sprintf(strOutsideBuff, "%.1f", fOutsideTemp);

        std::string strInsideTemp = strInsideBuff;
        std::string strOutsideTemp = strOutsideBuff;

        InfoPrint("InsideTemp:%s\n", strInsideTemp.c_str());
        InfoPrint("InsideTemp:%s\n", strInsideTemp.c_str());

        memset(m_vehicleWorkData_t.odoData_t.temperatureInside, 0, sizeof(m_vehicleWorkData_t.odoData_t.temperatureInside));
        memset(m_vehicleWorkData_t.odoData_t.temperatureOutside, 0, sizeof(m_vehicleWorkData_t.odoData_t.temperatureOutside));

        strncpy(m_vehicleWorkData_t.odoData_t.temperatureInside, strInsideTemp.c_str(), strInsideTemp.length());
        strncpy(m_vehicleWorkData_t.odoData_t.temperatureOutside, strOutsideTemp.c_str(), strOutsideTemp.length());

        std::string strMsgType = HexToStr(HMI_MGS_TYPE_ODO);                 //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_ODO_ID_OUTSIDE_TEMPERATURE); //B3CC MsgId
        std::string strLen = HexToStr(strInsideTemp.length());               //B3CC DataLength
        std::string strMsgData = strInsideTemp;

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    /* 胎压、胎压系统警告 */
    case MTYPE_DATA_MSGID_EXTERNEL_TIRE_PRESSURE:
    {
        if (iDataLen != 9)
        {
            ErrPrint("Tire Pressure Data From MCU Data!");
            break;
        }

        unsigned short int uiData;
        {
            uiData = 0;
            memcpy(&uiData, &ucMsgData[7], 2);
            m_vehicleWorkData_t.warnInfo_t.tire_info_t.fFrontLeftPressure = uiData * 1.0;

            std::string strMsgData = HexToStr(TIRE_FRONT_LEFT) +
                                     FloatToStr(m_vehicleWorkData_t.warnInfo_t.tire_info_t.fFrontLeftPressure);

            std::string strMsgType = HexToStr(HMI_MSG_TYPE_WARNING);        //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_WARNING_ID_TIRE_PRESS); //B3CC MsgId
            std::string strLen = HexToStr(strMsgData.length());             //B3CC DataLength

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }

        {
            uiData = 0;
            memcpy(&uiData, &ucMsgData[5], 2);
            m_vehicleWorkData_t.warnInfo_t.tire_info_t.fFrontRightPressure = uiData * 1.0;

            std::string strMsgData = HexToStr(TIRE_FRONT_RIGHT) +
                                     FloatToStr(m_vehicleWorkData_t.warnInfo_t.tire_info_t.fFrontRightPressure);

            std::string strMsgType = HexToStr(HMI_MSG_TYPE_WARNING);        //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_WARNING_ID_TIRE_PRESS); //B3CC MsgId
            std::string strLen = HexToStr(strMsgData.length());             //B3CC DataLength

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }

        {
            uiData = 0;
            memcpy(&uiData, &ucMsgData[3], 2);
            m_vehicleWorkData_t.warnInfo_t.tire_info_t.fRearLeftPressure = uiData * 1.0;

            std::string strMsgData = HexToStr(TIRE_REAR_LEFT) +
                                     FloatToStr(m_vehicleWorkData_t.warnInfo_t.tire_info_t.fRearLeftPressure);

            std::string strMsgType = HexToStr(HMI_MSG_TYPE_WARNING);        //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_WARNING_ID_TIRE_PRESS); //B3CC MsgId
            std::string strLen = HexToStr(strMsgData.length());             //B3CC DataLength

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }

        {
            uiData = 0;
            memcpy(&uiData, &ucMsgData[1], 2);
            m_vehicleWorkData_t.warnInfo_t.tire_info_t.fRearRightPressure = uiData * 1.0;

            std::string strMsgData = HexToStr(TIRE_REAR_RIGHT) +
                                     FloatToStr(m_vehicleWorkData_t.warnInfo_t.tire_info_t.fRearRightPressure);

            std::string strMsgType = HexToStr(HMI_MSG_TYPE_WARNING);        //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_WARNING_ID_TIRE_PRESS); //B3CC MsgId
            std::string strLen = HexToStr(strMsgData.length());             //B3CC DataLength

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }

        if (m_vehicleWorkData_t.warnInfo_t.tire_info_t.tirePressureWarn != ucMsgData[0] & 0x01)
        {
            m_vehicleWorkData_t.warnInfo_t.tire_info_t.tirePressureWarn = ucMsgData[0] & 0x01;
            std::string strStatus = m_vehicleWorkData_t.warnInfo_t.tire_info_t.tirePressureWarn ? "ON" : "OFF";
            this->SendViewPageInfo("tiresystemfault", "tiresystemfault", strStatus);
        }
    }
    break;
    /* 胎温 */
    case MTYPE_DATA_MSGID_EXTERNEL_TIRE_TEMPERATURE:
    {
        if (iDataLen != 8)
        {
            ErrPrint("Tire Temperature Data From MCU Data!");
            break;
        }

        short int iData = 0;
        memcpy(&iData, &ucMsgData[6], 2);
        m_vehicleWorkData_t.warnInfo_t.tire_info_t.fFrontLeftTemperature = iData * 1.0;

        {
            std::string strMsgData = HexToStr(TIRE_FRONT_LEFT) +
                                     FloatToStr(m_vehicleWorkData_t.warnInfo_t.tire_info_t.fFrontLeftTemperature);

            std::string strMsgType = HexToStr(HMI_MSG_TYPE_WARNING);       //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_WARNING_ID_TIRE_TEMP); //B3CC MsgId
            std::string strLen = HexToStr(strMsgData.length());            //B3CC DataLength

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }

        memcpy(&iData, &ucMsgData[4], 2);
        m_vehicleWorkData_t.warnInfo_t.tire_info_t.fFrontRightTemperature = iData * 1.0;

        {
            std::string strMsgData = HexToStr(TIRE_FRONT_RIGHT) +
                                     FloatToStr(m_vehicleWorkData_t.warnInfo_t.tire_info_t.fFrontRightTemperature);

            std::string strMsgType = HexToStr(HMI_MSG_TYPE_WARNING);       //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_WARNING_ID_TIRE_TEMP); //B3CC MsgId
            std::string strLen = HexToStr(strMsgData.length());            //B3CC DataLength

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }

        memcpy(&iData, &ucMsgData[2], 2);
        m_vehicleWorkData_t.warnInfo_t.tire_info_t.fRearLeftTemperature = iData * 1.0;

        {
            std::string strMsgData = HexToStr(TIRE_REAR_LEFT) +
                                     FloatToStr(m_vehicleWorkData_t.warnInfo_t.tire_info_t.fRearLeftTemperature);

            std::string strMsgType = HexToStr(HMI_MSG_TYPE_WARNING);       //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_WARNING_ID_TIRE_TEMP); //B3CC MsgId
            std::string strLen = HexToStr(strMsgData.length());            //B3CC DataLength

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }

        memcpy(&iData, &ucMsgData[0], 2);
        m_vehicleWorkData_t.warnInfo_t.tire_info_t.fRearRightTemperature = iData * 1.0;

        {
            std::string strMsgData = HexToStr(TIRE_REAR_RIGHT) +
                                     FloatToStr(m_vehicleWorkData_t.warnInfo_t.tire_info_t.fRearRightTemperature);

            std::string strMsgType = HexToStr(HMI_MSG_TYPE_WARNING);       //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_WARNING_ID_TIRE_TEMP); //B3CC MsgId
            std::string strLen = HexToStr(strMsgData.length());            //B3CC DataLength

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }
    }
    break;
    /* 轮胎故障 */
    case MTYPE_DATA_MSGID_EXTERNEL_TIRE_FAULT:
    {
        if (m_vehicleWorkData_t.warnInfo_t.tire_info_t.frontLeftTireFault != GET_BIT(ucMsgData[0], 7))
        {
            m_vehicleWorkData_t.warnInfo_t.tire_info_t.frontLeftTireFault = GET_BIT(ucMsgData[0], 7);
            std::string strMsgData = HexToStr(TIRE_FRONT_LEFT) +
                                     HexToStr(m_vehicleWorkData_t.warnInfo_t.tire_info_t.frontLeftTireFault);

            std::string strMsgType = HexToStr(HMI_MSG_TYPE_WARNING);             //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_WARNING_ID_TIRE_PRESS_WARN); //B3CC MsgId
            std::string strLen = HexToStr(strMsgData.length());                  //B3CC DataLength

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }

        if (m_vehicleWorkData_t.warnInfo_t.tire_info_t.frontRightTireFault != GET_BIT(ucMsgData[0], 6))
        {
            m_vehicleWorkData_t.warnInfo_t.tire_info_t.frontRightTireFault = GET_BIT(ucMsgData[0], 6);
            std::string strMsgData = HexToStr(TIRE_FRONT_RIGHT) +
                                     HexToStr(m_vehicleWorkData_t.warnInfo_t.tire_info_t.frontRightTireFault);

            std::string strMsgType = HexToStr(HMI_MSG_TYPE_WARNING);             //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_WARNING_ID_TIRE_PRESS_WARN); //B3CC MsgId
            std::string strLen = HexToStr(strMsgData.length());                  //B3CC DataLength

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }

        if (m_vehicleWorkData_t.warnInfo_t.tire_info_t.rearLeftTireFault != GET_BIT(ucMsgData[0], 5))
        {
            m_vehicleWorkData_t.warnInfo_t.tire_info_t.rearLeftTireFault = GET_BIT(ucMsgData[0], 5);
            std::string strMsgData = HexToStr(TIRE_REAR_LEFT) +
                                     HexToStr(m_vehicleWorkData_t.warnInfo_t.tire_info_t.rearLeftTireFault);

            std::string strMsgType = HexToStr(HMI_MSG_TYPE_WARNING);             //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_WARNING_ID_TIRE_PRESS_WARN); //B3CC MsgId
            std::string strLen = HexToStr(strMsgData.length());                  //B3CC DataLength

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }

        if (m_vehicleWorkData_t.warnInfo_t.tire_info_t.rearRightTireFault != GET_BIT(ucMsgData[0], 4))
        {
            m_vehicleWorkData_t.warnInfo_t.tire_info_t.rearRightTireFault = GET_BIT(ucMsgData[0], 4);
            std::string strMsgData = HexToStr(TIRE_REAR_RIGHT) +
                                     HexToStr(m_vehicleWorkData_t.warnInfo_t.tire_info_t.rearRightTireFault);

            std::string strMsgType = HexToStr(HMI_MSG_TYPE_WARNING);             //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_WARNING_ID_TIRE_PRESS_WARN); //B3CC MsgId
            std::string strLen = HexToStr(strMsgData.length());                  //B3CC DataLength

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }
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

    IPCData ipcData;
    switch (uiMsgId)
    {
    /* 激活节点 */
    case MTYPE_NODEALIVE_MSGID_ACTIVE:
    {
    }
    break;
    /* 心跳 */
    case MTYPE_NODEALIVE_MSGID_WATCH_DOG:
    {
    }
    break;
    default:
        break;
    }
}

void CoreMsgHandler::InteractiveMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen)
{
    InfoPrint("Now Process Data Message. MsgId:[0x%02x], DataLen:[%d]\n", uiMsgId, iDataLen);
    DebugPrintMsg(ucMsgData, iDataLen);

    switch (uiMsgId)
    {
    /* 多媒体 */
    case MTYPE_INTERACTIVE_MSGID_MUSIC:
    {
        switch (ucMsgData[0])
        {
        case MUSIC_CLOSE:
        {
        }
        break;
        case MUSIC_PROCESS:
        {
        }
        break;
        case MUSIC_ALBUM:
        {
        }
        break;
        case MUSIC_NAME:
        {
        }
        break;
        case MUSIC_AUTHOR:
        {
        }
        break;
        default:
            break;
        }
    }
    break;
    /* 电话 */
    case MTYPE_INTERACTIVE_MSGID_PHONE:
    {
        switch (ucMsgData[0])
        {
        case PHONE_CLEAR:
        {
        }
        break;
        case PHONE_STATUS:
        {
        }
        break;
        case PHONE_CALL_NAME:
        {
        }
        break;
        case PHONE_CALL_NUMBER:
        {
        }
        break;
        case PHONE_CALL_TIME:
        {
        }
        break;
        default:
            break;
        }
    }
    break;
    default:
        break;
    }
}

void CoreMsgHandler::SetupMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen)
{
    InfoPrint("Now Process Data Message. MsgId:[0x%02x], DataLen:[%d]\n", uiMsgId, iDataLen);
    DebugPrintMsg(ucMsgData, iDataLen);

    switch (uiMsgId)
    {
    /* 主题 */
    case MTYPE_SETUP_MSGID_CFG_THEME:
    {
        if (m_vehicleWorkData_t.settingInfo_t.theme != ucMsgData[0] & 0xFF)
        {
            m_vehicleWorkData_t.settingInfo_t.theme = ucMsgData[0] & 0xFF;

            std::string strMsgType = HexToStr(HMI_MSG_TYPE_SETTING); //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_SET_ID_THEME);   //B3CC MsgId
            std::string strLen = HexToStr(0x01);                     //B3CC DataLength
            std::string strMsgData = HexToStr(m_vehicleWorkData_t.settingInfo_t.theme);

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }
    }
    break;
    /* 语言 */
    case MTYPE_SETUP_MSGID_CFG_LANG:
    {
        if (m_vehicleWorkData_t.settingInfo_t.language != ucMsgData[0] & 0xFF)
        {
            m_vehicleWorkData_t.settingInfo_t.language = ucMsgData[0] & 0xFF;

            std::string strMsgType = HexToStr(HMI_MSG_TYPE_SETTING);  //B3CC MsgType
            std::string strMsgId = HexToStr(HMI_MGS_SET_ID_LANGUAGE); //B3CC MsgId
            std::string strLen = HexToStr(0x01);                      //B3CC DataLength
            std::string strMsgData = HexToStr(m_vehicleWorkData_t.settingInfo_t.language);

            std::string strMsgSend;
            strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
            m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
        }
    }
    break;
    default:
        break;
    }
}

void CoreMsgHandler::KeyMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen)
{
    InfoPrint("Now Process Data Message. MsgId:[0x%02x], DataLen:[%d]\n", uiMsgId, iDataLen);
    DebugPrintMsg(ucMsgData, iDataLen);

    switch (uiMsgId)
    {
    /* 按键 */
    case MTYPE_KEY_MSGID_KEY:
    {
        if (iDataLen != 2)
        {
            ErrPrint("Key Data From MCU Data!");
            break;
        }

        std::string strMsgType = HexToStr(HMI_MSG_TYPE_KEY); //B3CC MsgType
        std::string strMsgId = HexToStr(HMI_MGS_KEY_ID_KEY); //B3CC MsgId
        std::string strLen = HexToStr(0x01);                 //B3CC DataLength
        std::string strMsgData = HexToStr(ucMsgData[1]);

        std::string strMsgSend;
        strMsgSend.append(strMsgType).append(strMsgId).append(strLen).append(strMsgData);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
    break;
    default:
        break;
    }
}

void CoreMsgHandler::EOLMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen)
{
    InfoPrint("Now Process Data Message. MsgId:[0x%02x], DataLen:[%d]\n", uiMsgId, iDataLen);
    DebugPrintMsg(ucMsgData, iDataLen);
}

void CoreMsgHandler::SendViewPageInfo(std::string strViewName, std::string strExtraInfo, std::string strStatus)
{
    if (m_connClient->m_isConnected)
    {
        std::string strMsgSend = m_jsonHandler->GetWarnSendData(strViewName, strExtraInfo, strStatus);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_VIEW), strMsgSend);
    }
}

void CoreMsgHandler::SendAudioWarnInfo(std::string strAudioName, std::string strStatus)
{
    if (m_connClient->m_isConnected)
    {
        std::string strMsgSend = m_jsonHandler->GetWarnSendData(strAudioName, strAudioName, strStatus);
        m_connClient->MsgSend(std::string(MQTT_TOPIC_WARN_AUDIO), strMsgSend);
    }
}

static void ReadCallBackFunc(const ECPVehicleValue &rData)
{
    CoreMsgHandler::GetInstance()->E02_MsgReciever(rData);
}
