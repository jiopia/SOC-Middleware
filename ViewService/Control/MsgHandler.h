#ifndef _MSG_HANDLER_H_
#define _MSG_HANDLER_H_

#include <pthread.h>
#include "savedData.h"
#include "Thread.h"
#include "recvDataDefine.h"
#include "systemdefine.h"
#include "Util.h"
#include "Singleton.hpp"
#include "AudioControl.h"
#include "ViewNode.hpp"
#include "PriorityQueue.hpp"

#define CURR_HMI_STATUS_MATCH_FAIL_MAX_TIMES 10

enum ViewOperationAction
{
    VIEW_ACTION_DEFAULT = -1,
    VIEW_ACTION_REMOVE_CUR,
    VIEW_ACTION_HIDE_ALL_WARN, //长按OK键隐藏
    VIEW_ACTION_INIT,
    VIEW_ACTION_IGNOFF
};

typedef struct
{
    char warnData[2048];
    pthread_mutex_t mtxSharedMemViewData;
} ViewSharedMem;

typedef std::multimap<ViewNode, std::shared_ptr<ViewInfo>> WarnInfoMultiMap;

class MsgHandler : public Singleton<MsgHandler>, public Thread
{
    friend class Singleton<MsgHandler>;

public:
    MsgHandler();
    ~MsgHandler();

    void SharedMemInit();
    void SetWarnView(std::string strViewName, std::string strExtraInfo, VIEW_STATUS viewStatus);
    void TouchOptionFlag(ViewOperationAction flag);
    void RemoveCurrentWarn();

    int AtLeastTimerStart();
    int AtLeastTimerStop();

private:
    int AtLeastTimerInit();
    int AtLeastTimerDelete();
    void ResetAtLeastFlag();

    void Run();
    void HandleWarnViews();
    void Notify(ViewNode &viewNode);

    void InsertWarnViewNode(ViewNode &viewNode);
    void GetNextWarnViewNode(ViewNode &viewNode);
    void DeleteWarnViewNode();

    void HideAllWarn();

    void InsertWarnInfoMap(ViewNode &viewNode, std::shared_ptr<ViewInfo> viewInfo);
    std::shared_ptr<ViewInfo> GetWarnInfoFromMap(ViewNode &viewNode);
    void DeleteWarnInfoInMap(ViewNode &viewNode);

    bool IsWarnClosedSilently(ViewNode &viewNode);
    bool CheckNeedPushLoopAgain(ViewNode &viewNode, std::shared_ptr<ViewInfo> viewInfo);

    bool IsSeriousWarn(ViewNode &viewNode);

    bool IsCurrHmiStatusMatchedSuccess();

    timer_t m_timerId;

    int m_fdShareMemViewData;
    ViewSharedMem *m_ptrShareMemViewData;

    AudioControl *m_audioControl;
    XmlManager *m_xmlManager;
    JsonHandler *m_jsonHandler;

    HmiStatus m_curHmiStatus = HMI_STATUS_DEFAULT;

    std::mutex m_mtxForAction;
    ViewOperationAction m_viewOperationAct = VIEW_ACTION_DEFAULT;

    std::mutex m_mtxServerCoId;
    std::set<int> m_serverCoIdList;

    ViewNode m_currViewNode;

    PriorityQueue<ViewNode> m_freshViewQueue; //尚未显示过的报警列表(按照优先级排序，自带锁)

    PriorityQueue<ViewNode> m_loopShowQueue; //轮询显示的队列(按照优先级排序，自带锁)

    std::mutex m_mtxWarnMap;
    WarnInfoMultiMap m_warnInfoMap; //当前所有的报警信息Map
};

#endif // !_MSG_HANDLER_H_
