#ifndef _CORE_MSG_HANDLER_H_
#define _CORE_MSG_HANDLER_H_

//Utils
#include "Util.h"
#include "Thread.h"
#include "Singleton.hpp"
#include "PriorityQueue.hpp"
#include "mosquitto.h"
#include "MqttConnection.hpp"

//bdsdk
#include "savedData.h"
#include "recvDataDefine.h"
#include "systemdefine.h"

#define E02

#ifdef E02

#include <sys/time.h>
#include <algorithm>
#include <string>
#include <unistd.h>
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

#define CODE_GET_PRODATA 0X21411030
#define CODE_SET_REPORT_PERIOD 0X21411031

#endif //!E02

class CoreMsgHandler : public Singleton<CoreMsgHandler>, public Thread
{
public:
    CoreMsgHandler();
    ~CoreMsgHandler();

    void E02_MsgReciever(ECPVehicleValue rData);

private:
    void Run();

    void MsgReciever();

    void MsgProcessor(std::string strMsg);

    void ActionMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData);

    void DataMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData, int iDataLen);

    void NodeAliveMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData);

    void InteractiveMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData);

    void SetupMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData);

    void KeyMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData);

    void EOLMsgHandler(uint32_t uiMsgId, const unsigned char *ucMsgData);

    std::shared_ptr<BaseConnection> m_mqttClient = NULL;
    ECPInterface *ptrECP = NULL;
};

#endif //!_CORE_MSG_HANDLER_H_