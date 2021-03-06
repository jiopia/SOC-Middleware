#ifndef _MSG_HANDLER_H_
#define _MSG_HANDLER_H_

#include <pthread.h>
#include "Thread.h"
#include "Util.h"
#include "Singleton.hpp"
#include "AudioControl.h"
#include "ViewNode.hpp"
#include "PriorityQueue.hpp"
#include "WarnPageControl.hpp"
#include "MsgDataDefine.h"

#define CURR_HMI_STATUS_MATCH_FAIL_MAX_TIMES 10

enum ViewOperationAction
{
    VIEW_ACTION_DEFAULT = -1,
    VIEW_ACTION_REMOVE_CUR,
    VIEW_ACTION_HIDE_ALL_WARN, //长按OK键隐藏
    VIEW_ACTION_INIT,
    VIEW_ACTION_IGNOFF
};

typedef std::multimap<ViewNode, ViewInfo> WarnInfoMultiMap;

class MsgHandler : public Singleton<MsgHandler>, public Thread
{
public:
    MsgHandler();
    ~MsgHandler();

    void SetWarnView(std::string strViewName, std::string strExtraInfo, VIEW_STATUS viewStatus);
    void RemoveCurrentWarn();

private:
    void Run();
    void HandleWarnViews();
    void HideAllWarn();
    void Notify(ViewNode &viewNode);

    /* 更新/插入 告警列表List */
    void UpdateWarnViewNode(ViewNode viewNode);
    void UpdateSeriousWarnList(ViewNode viewNode);
    void UpdateFrashWarnList(ViewNode viewNode);

    /* 获取 告警列表List */
    void GetSeriousWarnListNode(ViewNode &viewNode);
    void GetFrashWarnListNode(ViewNode &viewNode);
    void GetLoopWarnListNode(ViewNode &viewNode);
    void GetLoopDoorInfoNode(ViewNode &viewNode);

    void GetNextWarnViewNode(ViewNode &viewNode);

    /* 删除 告警列表List */
    void EraseWarnViewNode(ViewNode viewNode);
    bool EraseSeriousWarnListNode(ViewNode viewNode);
    bool EraseFreshWarnListNode(ViewNode viewNode);
    bool EraseLoopWarnListNode(ViewNode viewNode);

    bool IsNodeExistInFreshList(ViewNode viewNode);
    bool IsNodeExistInLoopList(ViewNode viewNode);

    int GetViewPriority(ViewNode &viewNode);
    bool CheckNeedPushLoopAgain(ViewNode &viewNode);
    bool IsSeriousWarn(ViewNode &viewNode);
    bool IsCurrHmiStatusMatchedSuccess();

    int AtLeastTimerInit();
    int AtLeastTimerStart();
    int AtLeastTimerStop();
    int AtLeastTimerDelete();
    void ResetAtLeastFlag();
    bool IsAtLeastTimeDone();

    std::string GetDoorInfoFlagStr(ViewNode viewNode);
    bool CheckAccStatusConform(std::shared_ptr<ViewInfo> pViewInfo);

    bool GetAllWarnHidenStatus();
    void SetAllWarnHidenStatus(bool status);

    timer_t m_timerId;

    AudioControl *m_audioControl;
    XmlManager *m_xmlManager;
    JsonHandler *m_jsonHandler;
    WarnPageControl *m_warnPageControl;

    std::mutex m_mtxCurrWarn;
    ViewNode m_currViewNode; //当前正在显示的告警

    std::mutex m_mtxSerious;
    std::vector<ViewNode> m_seriousViewList; //严重告警列表
    std::mutex m_mtxFresh;
    std::vector<ViewNode> m_freshViewList; //尚未显示过的非严重告警列表
    std::mutex m_mtxLoop;
    std::vector<ViewNode> m_loopViewList; //循环显示的非严重告警列表

    std::mutex m_mtxAccStatus;
    VehicleAccStatus m_vehicleStatus = VEHICLE_DEFAULT;

    /* 告警信息是否全部隐藏的标志 */
    std::mutex m_mtxHideAll;
    bool m_isAllWarnHidden = false;

    std::string m_strVehicleSpeed;
};

#endif // !_MSG_HANDLER_H_
