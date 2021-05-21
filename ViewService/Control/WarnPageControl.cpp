#include "WarnPageControl.hpp"
#include "DataToHMIDefine.h"

WarnPageControl::WarnPageControl()
{
}

WarnPageControl::~WarnPageControl()
{
}

std::string WarnPageControl::GetWarnPageDataToHMI(ViewNode viewNode)
{
    std::string strMsgSend;

    IPCData pIPCData;
    memset(&pIPCData, 0, sizeof(IPCData));

    if (viewNode.strViewName.compare("pepsswitchtoPStart") == 0)
    {
        pIPCData.m_msgType = HMI_MSG_TYPE_WARNING;
        pIPCData.m_msgID = HMI_MGS_WARNING_ID_PEPS9;
        pIPCData.m_msgLength = 0x01;
        pIPCData.m_msgData[0] = viewNode.viewStatus == VIEW_ON ? 0x01 : 0x00;
    }
    else if (viewNode.strViewName.compare("parking") == 0)
    {
        pIPCData.m_msgType = HMI_MSG_TYPE_WARNING;
        pIPCData.m_msgID = HMI_MGS_WARNING_ID_PARKING;
        pIPCData.m_msgLength = 0x01;
        pIPCData.m_msgData[0] = viewNode.viewStatus == VIEW_ON ? 0x01 : 0x00;
    }
    else if (viewNode.strViewName.compare("doorinfo") == 0 || viewNode.strViewName.compare("doorwarn") == 0)
    {
        pIPCData.m_msgType = HMI_MSG_TYPE_WARNING;
        pIPCData.m_msgID = viewNode.strViewName.compare("doorinfo") == 0 ? HMI_MGS_WARNING_ID_DOORINFO : HMI_MGS_WARNING_ID_DOOR;
        pIPCData.m_msgLength = 0x06;

        for (auto iter = viewNode.extraInfos.begin(); iter != viewNode.extraInfos.end(); iter++)
        {
            ViewExtraInfo *tempExtraInfo = &(*iter);
            if (tempExtraInfo->strExtraInfo == "DRIVER_DOOR")
            {
                pIPCData.m_msgData[0] = tempExtraInfo->viewStatus == VIEW_ON ? 0x01 : 0x00;
            }
            else if (tempExtraInfo->strExtraInfo == "PASSENGER_DOOR")
            {
                pIPCData.m_msgData[1] = tempExtraInfo->viewStatus == VIEW_ON ? 0x01 : 0x00;
            }
            else if (tempExtraInfo->strExtraInfo == "REAR_RIGHT_DOOR")
            {
                pIPCData.m_msgData[2] = tempExtraInfo->viewStatus == VIEW_ON ? 0x01 : 0x00;
            }
            else if (tempExtraInfo->strExtraInfo == "REAR_LEFT_DOOR")
            {
                pIPCData.m_msgData[3] = tempExtraInfo->viewStatus == VIEW_ON ? 0x01 : 0x00;
            }
            else if (tempExtraInfo->strExtraInfo == "HOOD")
            {
                pIPCData.m_msgData[4] = tempExtraInfo->viewStatus == VIEW_ON ? 0x01 : 0x00;
            }
            else if (tempExtraInfo->strExtraInfo == "BACK_DOOR")
            {
                pIPCData.m_msgData[5] = tempExtraInfo->viewStatus == VIEW_ON ? 0x01 : 0x00;
            }
        }
    }
    else if (viewNode.strViewName.compare("speedlimit") == 0)
    {
        pIPCData.m_msgType = HMI_MSG_TYPE_WARNING;
        pIPCData.m_msgID = HMI_MGS_WARNING_ID_OVERSPEED;
        pIPCData.m_msgLength = 1;
        pIPCData.m_msgData[0] = viewNode.viewStatus == VIEW_ON ? 0x01 : 0x00;
    }
    else if (viewNode.strViewName.compare("oillow") == 0)
    {
        pIPCData.m_msgType = HMI_MSG_TYPE_WARNING;
        pIPCData.m_msgID = HMI_MGS_WARNING_ID_OILLOW;
        pIPCData.m_msgLength = 1;
        pIPCData.m_msgData[0] = viewNode.viewStatus == VIEW_ON ? 0x01 : 0x00;
    }
    else if (viewNode.strViewName.compare("pepsbatterylow") == 0)
    {
        pIPCData.m_msgType = HMI_MSG_TYPE_WARNING;
        pIPCData.m_msgID = HMI_MGS_WARNING_ID_PEPS6;
        pIPCData.m_msgLength = 1;
        pIPCData.m_msgData[0] = viewNode.viewStatus == VIEW_ON ? 0x01 : 0x00;
    }
    else if (viewNode.strViewName.compare("pepsnotfindb") == 0)
    {
        pIPCData.m_msgType = HMI_MSG_TYPE_WARNING;
        pIPCData.m_msgID = HMI_MGS_WARNING_ID_PEPS2;
        pIPCData.m_msgLength = 1;
        pIPCData.m_msgData[0] = viewNode.viewStatus == VIEW_ON ? 0x01 : 0x00;
    }
    else if (viewNode.strViewName.compare("tiresystemfault") == 0)
    {
        pIPCData.m_msgType = HMI_MSG_TYPE_WARNING;
        pIPCData.m_msgID = HMI_MGS_WARNING_ID_TIRE_SYS_WARN;
        pIPCData.m_msgLength = 1;
        pIPCData.m_msgData[0] = viewNode.viewStatus == VIEW_ON ? 0x01 : 0x00;
    }
    else if (viewNode.strViewName.compare("WrongGear_P") == 0)
    {
        pIPCData.m_msgType = HMI_MSG_TYPE_WARNING;
        pIPCData.m_msgID = HMI_MGS_WARNING_ID_WARONG_GEAR;
        pIPCData.m_msgLength = 1;
        pIPCData.m_msgData[0] = viewNode.viewStatus == VIEW_ON ? 0x01 : 0x00;
    }

    if (pIPCData.m_msgType != 0x00)
    {
        std::string strMsgType = HexToStr(pIPCData.m_msgType);
        std::string strMsgId = HexToStr(pIPCData.m_msgID);
        std::string strMsgLen = HexToStr(pIPCData.m_msgLength);

        strMsgSend = strMsgType + strMsgId + strMsgLen + UCharArrToStr(pIPCData.m_msgData, pIPCData.m_msgLength);
    }

    return strMsgSend;
}