/**
 * @file CoreMsgHandler.hpp
 * @author wangqijin (wangqijin@bdstar.com)
 * @brief 
 * @version 0.1
 * @date 2021-06-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _CORE_MSG_HANDLER_H_
#define _CORE_MSG_HANDLER_H_

//Utils
#include "Util.h"
#include "Thread.h"
#include "Singleton.hpp"
#include "PriorityQueue.hpp"
#include "mosquitto.h"
#include "MqttConnection.hpp"
#include "JsonHandler.h"

#include "MsgDataDefine.h"
#include "DataToHMIDefine.h"

#ifdef E02

extern "C"
{
#include "powerct.h"
#include "ipo_blank.h"
}

#include <sys/time.h>
#include <algorithm>
#include <utility>
#include "ECPComm.h"
#include "ECPInterface.h"

/**数据类型转换使用**/
typedef union
{
#define CHANGE_INT_NUMS 3
    uint32_t int32Val[CHANGE_INT_NUMS];
    uint8_t uint8Val[CHANGE_INT_NUMS * sizeof(int32Val)];
} typeInt2Chars;

int acquire_event(void); //ACC_ON占用锁资源
int release_event(void); //ACC_OFF释放锁资源

int set_powernormal_mode(void); //电源唤醒接口
int set_poweroff_mode(void);    //电源睡眠接口

#endif //!E02

class CoreMsgHandler : public Singleton<CoreMsgHandler>, public Thread
{
public:
    CoreMsgHandler();
    ~CoreMsgHandler();

    void E02_MsgReciever(ECPVehicleValue rData);

    void E02_Request(uint32_t uiMsgType, uint32_t uiMsgId,
                     uint8_t uchValue = 0x00,
                     ECPCallBackType ecpCbType = ECP_TYPE_MCU, uint32_t ecpProtocal = GET_PROPDATA);

private:
    void Run();

    void MsgReciever();

    void MqttMsgProcessor(MsgData msgData);

    void ProcessHMIRequest(uint32_t uiMsgType, uint32_t uiMsgId, uint8_t *uiMsgData, int iDataLen);

    void ActionMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen);

    void DataMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen);

    void NodeAliveMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen);

    void InteractiveMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen);

    void SetupMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen);

    void KeyAndAudioCtrlMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen);

    void EOLMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen);

    std::shared_ptr<BaseConnection> m_connClient = NULL;

    ECPInterface *ptrECP = NULL;

    JsonHandler *m_jsonHandler = NULL;

    VehicleWorkData_t m_vehicleWorkData_t;

    VehicleAccStatus m_vehicleStatus = VEHICLE_DEFAULT;

    std::mutex m_mtxAudioCtrl;
    AudioCtrlAdscription m_audioCtrlAdscription = AUDIO_CTRL_BELONG_TO_MCU;

    std::mutex m_mtxPowerOff;
    bool m_isPowerOffNeedExec = false;

private:
    void SendViewPageInfo(std::string strViewName, std::string strExtraInfo, std::string strStatus);
    void SendAudioWarnInfo(std::string strAudioName, std::string strStatus);
    void SendFaultInfo();

    void MWBroadcastAccStatus(VehicleAccStatus accStatus);
    void ExecPowerEvent(VehicleAccStatus accStatus);

    void SetPowerOffExecValid(bool isNeedExec);
    bool IsPowerOffNeedExec();
    void PowerOffTimerThread();

    void McuHeartBeatThread();
};

#endif //!_CORE_MSG_HANDLER_H_