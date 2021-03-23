#include "MsgHandler.h"
#include "Actuator.h"
#include <map>
#include <string>
#include <mutex>
#include <memory>
#include <vector>

/* 告警是否完成最小显示时间显示的标志 */
static std::mutex g_mtxTimeDone;
static bool g_isAtLeastTimeDone = true;

/* 告警信息是否全部隐藏的标志 */
static std::mutex g_mtxHideAll;
static bool g_isAllWarnHidden = false;

MsgHandler::MsgHandler() : m_audioControl(AudioControl::GetInstance()),
                           m_xmlManager(XmlManager::GetInstance()),
                           m_jsonHandler(JsonHandler::GetInstance())
{
    AtLeastTimerInit();
}

MsgHandler::~MsgHandler()
{
    if (this->m_ptrShareMemViewData != NULL)
    {
        pthread_mutex_unlock(&m_ptrShareMemViewData->mtxSharedMemViewData);
        close(m_fdShareMemViewData);
        munmap(m_ptrShareMemViewData, sizeof(ViewSharedMem));
        shm_unlink(VIEWACTUAL_SHARED);
        AtLeastTimerDelete();
    }
}

/** 
 * @brief   对用于发送告警信息给ipcService的共享内存进行初始化
 *          此处的共享内存VIEWACTUAL_SHARED，用于对Notify时告警信息的存储
 */
void MsgHandler::SharedMemInit()
{
    pthread_mutexattr_t mtxViewDataAttr;
    this->m_fdShareMemViewData = shm_open(VIEWACTUAL_SHARED, O_RDWR | O_CREAT | O_EXCL, S_IRWXU);

    if (this->m_fdShareMemViewData < 0)
    {
        if (this->m_fdShareMemViewData = shm_open(VIEWACTUAL_SHARED, O_RDWR | O_CREAT, S_IRWXU) < 0)
        {
            ErrPrint("Failed to open shared data [%s]!\n", VIEWACTUAL_SHARED);
            exit(1);
        }
    }

    ftruncate(m_fdShareMemViewData, sizeof(ViewSharedMem));
    this->m_ptrShareMemViewData = (ViewSharedMem *)mmap(0, sizeof(ViewSharedMem), PROT_READ | PROT_WRITE,
                                                        MAP_SHARED, m_fdShareMemViewData, 0);

    pthread_mutexattr_init(&mtxViewDataAttr);
    pthread_mutexattr_setpshared(&mtxViewDataAttr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&m_ptrShareMemViewData->mtxSharedMemViewData, &mtxViewDataAttr);
}

/** 
 * @brief   对到来的告警信息进行第一步处理的函数
 * 
 * @param   strViewName     告警信息的名称
 *          strExtraInfo    告警信息额外的信息(比如：对于门开类告警/提示，此字段用于标识当前哪个门打开。
 *                                          对于多个门打开的时候，会多次进行SetWarnView处理)
 *          viewStatus      当前告警的动作：VIEW_ON/VIEW_OFF/...
 */
void MsgHandler::SetWarnView(std::string strViewName, std::string strExtraInfo, VIEW_STATUS viewStatus)
{
    ViewInfoPtr viewInfo = m_xmlManager->GetViewInfo(strViewName);
    if (viewInfo == NULL)
    {
        ErrPrint("Can NOT find ViewInfo which viewname is [%s]!\n", strViewName.c_str());
        return;
    }
    viewInfo->SetExtraInfo(strExtraInfo.c_str());

    InfoPrint("DEBUG -- View[%s], Extra[%s], Status[%s].\n",
              strViewName.c_str(), strExtraInfo.c_str(), viewStatus == VIEW_ON ? "ON" : "OFF");

    ViewNode viewNode(strViewName, strExtraInfo, viewStatus);
    InsertWarnViewNode(viewNode);
}

/** 
 * @brief   此函数用于对告警信息列表的处理
 *          
 * @param   flag    具体处理的动作
 *          VIEW_ACTION_REMOVE_CUR      删除当前显示的告警信息
 *          VIEW_ACTION_HIDE_ALL_WARN   隐藏所有告警信息
 *          VIEW_ACTION_INIT            初始化告警信息
 */
void MsgHandler::TouchOptionFlag(ViewOperationAction flag)
{
    std::lock_guard<std::mutex> lockGuard(m_mtxForAction);
    this->m_viewOperationAct = flag;
}

/** 
 * @brief   删除当前正在显示的告警信息
 */
void MsgHandler::RemoveCurrentWarn()
{
    WarnPrint("Remove Current Warning View Info.\n");
    ViewNode viewNode = this->m_freshViewQueue.Top();
    if (!IsSeriousWarn(viewNode))
    {
        this->m_freshViewQueue.Pop();
        DeleteWarnInfoInMap(viewNode);
    }
}

/** 
 * @brief   MsgHandler类的主要处理函数
 */
void MsgHandler::run()
{
    SharedMemInit();

    while (1)
    {
        if (!g_isAtLeastTimeDone)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); //In fact, 10ms. Here is just for debugging.
            HandleWarnViews();
        }
    }
}

/** 
 * @brief   告警信息的主要处理函数
 */
void MsgHandler::HandleWarnViews()
{
    if (g_isAllWarnHidden)
    {
        InfoPrint("All warning message is hidden now.\n");
        return;
    }

#if 0
    if (!m_freshViewQueue.Empty())
    {
        m_currViewNode = this->m_freshViewQueue.Top();
        InfoPrint("Handling -- View[%s], Extra[%s], Status[%s].\n",
                  m_currViewNode.strViewName.c_str(), m_currViewNode.strExtraInfo.c_str(),
                  m_currViewNode.viewStatus == VIEW_ON ? "ON" : "OFF");
        std::shared_ptr<ViewInfo> pViewInfo = GetWarnInfoFromMap(m_currViewNode);
        if (pViewInfo == NULL)
        {
            ErrPrint("ViewNode was Not found in WarInfoMap!\n");
            return;
        }

        if (!pViewInfo->HasAlreadyExecuted())
        {
            Notify(m_currViewNode);
            pViewInfo->SetAlreadyExecutedFlag();
        }
    }
    else
    {
        WarnPrint("No ViewNode in FreshViewNodeQueue...\n");
    }
#endif

#if 1
    /* 如果当前报警为空（尚未有报警执行过），或者不为空但也不是严重告警，则需要切换至下一个报警 */
    if (m_currViewNode.Empty() || !IsSeriousWarn(m_currViewNode))
    {
        ViewNode nextWarnViewNode;
        GetNextWarnViewNode(nextWarnViewNode);
        /* 说明当前告警仍然在显示中 */
        if (m_currViewNode == nextWarnViewNode)
        {
            return;
        }

        /* 说明有优先级高的新的告警进入队列 */
        Actuator::GetInstance()->WarnShowTimerStop();
        m_currViewNode = nextWarnViewNode;
    }

    if (m_currViewNode.Empty())
    {
        return;
    }

    InfoPrint("Handling -- View[%s], Extra[%s], Status[%s].\n",
              m_currViewNode.strViewName.c_str(), m_currViewNode.strExtraInfo.c_str(),
              m_currViewNode.viewStatus == VIEW_ON ? "ON" : "OFF");

    std::shared_ptr<ViewInfo> pViewInfo = GetWarnInfoFromMap(m_currViewNode);
    if (pViewInfo == NULL)
    {
        ErrPrint("ViewInfo is NULL!\n");
        return;
    }

    // Actuator::GetInstance()->WarnShowTimerStart(pViewInfo->GetLoop());

    if (IsSeriousWarn(m_currViewNode))
    {
        //To do......
        Actuator::GetInstance()->WarnShowTimerStop();
        if (pViewInfo->HasAlreadyExecuted())
        {
            return;
        }
    }

    ResetAtLeastFlag();
    pViewInfo->SetAlreadyExecutedFlag();

    if (CheckNeedPushLoopAgain(m_currViewNode, pViewInfo))
    {
        this->m_loopShowQueue.Push(m_currViewNode);
    }

    bool isIgnOff = Actuator::GetInstance()->IsIgnOFFCheck();
    if (isIgnOff &&
        !pViewInfo->GetPower().empty() &&
        pViewInfo->GetPower().compare("wm1") == 0)
    {
        WarnPrint("ViewInfo [%s]:[%s] Power information does not match!\n",
                  m_currViewNode.strViewName.c_str(), m_currViewNode.strExtraInfo.c_str());
        return;
    }
    else if (!pViewInfo->GetPower().empty() &&
             pViewInfo->GetPower().compare("wm2") == 0)
    {
        WarnPrint("ViewInfo [%s]:[%s] Power information does not match!\n",
                  m_currViewNode.strViewName.c_str(), m_currViewNode.strExtraInfo.c_str());
        return;
    }

    AtLeastTimerStart();
    Actuator::GetInstance()->WarnShowTimerStart(pViewInfo->GetLoop());

    Notify(m_currViewNode);
#endif
}

/** 
 * @brief   告警信息发送(通知)函数
 */
void MsgHandler::Notify(ViewNode &viewNode)
{
    std::shared_ptr<ViewInfo> pViewInfo = m_xmlManager->GetViewInfo(viewNode.strViewName);
    if (pViewInfo == NULL)
    {
        return;
    }

    if (!pViewInfo->GetAudioBindInfo().empty())
    {
        m_audioControl->SendAudio("warning", pViewInfo->GetAudioBindInfo(), m_currViewNode.viewStatus);
    }

    char *warnData = m_jsonHandler->GetWarnJsonInfo(m_currViewNode.strViewName, m_currViewNode.strExtraInfo,
                                                    m_currViewNode.viewStatus);
    pthread_mutex_lock(&this->m_ptrShareMemViewData->mtxSharedMemViewData);
    memset(this->m_ptrShareMemViewData->warnData, 0, sizeof(this->m_ptrShareMemViewData->warnData));
    strcpy(this->m_ptrShareMemViewData->warnData, warnData);
    pthread_mutex_unlock(&this->m_ptrShareMemViewData->mtxSharedMemViewData);

    typeMsgData msgJustWithType;
    msgJustWithType.type = VIEW_AUTUATOR_REC_JSON;

    std::lock_guard<std::mutex> lockGuard(m_mtxServerCoId);
    for (auto iter = this->m_serverCoIdList.begin(); iter != this->m_serverCoIdList.end(); iter++)
    {
        WarnPrint("Notify MsgSending serverCoId:[%d], warnData:[%s]......\n", (*iter), warnData);
        // MsgSend((*iter), &msgJustWithType, sizeof(msgJustWithType), NULL, 0);
    }

    BDSTAR_FREE(warnData);
}

/** 
 * @brief   将新的告警信息插入待显示队列
 */
void MsgHandler::InsertWarnViewNode(ViewNode &viewNode)
{
    /* 判断是否有应该静默删除的报警 */
    if (IsWarnClosedSilently(viewNode))
    {
        return;
    }

    if (viewNode.viewStatus == VIEW_ON)
    {
        g_isAllWarnHidden = false;
    }

    this->m_freshViewQueue.Push(viewNode);

    ViewInfoPtr pViewInfo = m_xmlManager->GetViewInfo(viewNode.strViewName);
    if (pViewInfo != NULL)
    {
        InfoPrint("Here Insert to WarnInfoMap......\n");
        InsertWarnInfoMap(viewNode, pViewInfo);
    }
}

/** 
 * @brief   从待显示队列(优先)或者轮询队列(当待显示队列为空时)获取下一个告警信息
 * 
 * @param   viewNode    获取到的ViewNode信息
 */
void MsgHandler::GetNextWarnViewNode(ViewNode &viewNode)
{
    if (m_freshViewQueue.Empty() && m_loopShowQueue.Empty())
    {
        std::lock_guard<std::mutex> lockGuard(m_mtxWarnMap);
        m_warnInfoMap.clear(); //为了确保同步
        return;
    }

    if (!m_freshViewQueue.Empty())
    {
        viewNode = this->m_freshViewQueue.Top();
        // if (!IsSeriousWarn(viewNode))
        // {
        //     this->m_freshViewQueue.Pop();
        //     DeleteWarnInfoInMap(viewNode);
        // }
    }
    else if (!m_loopShowQueue.Empty())
    {
        viewNode = this->m_loopShowQueue.Top();
        this->m_loopShowQueue.Pop();
        // DeleteWarnInfoInMap(viewNode);
    }
}

void MsgHandler::DeleteWarnViewNode()
{
    if (!m_freshViewQueue.Empty())
    {
        this->m_freshViewQueue.Pop();
    }
    else if (!m_loopShowQueue.Empty())
    {
        this->m_loopShowQueue.Pop();
    }
}

/** 
 * @brief   隐藏当前列表中的所有的告警信息
 *          (当这个信息来到的时候，说明已经进入轮询的显示了)
 */
void MsgHandler::HideAllWarn()
{
    WarnPrint("Hide All Warning View Info.\n");
    std::lock_guard<std::mutex> lockGuard(g_mtxHideAll);
    g_isAllWarnHidden = true;
}

/** 
 * @brief   将view相关信息插入 m_mtxWarnMap 中
 * 
 * @param   viewNode    告警信息的一些重要信息类对象
 *          viewInfo    告警viewInfo详细信息
 */
void MsgHandler::InsertWarnInfoMap(ViewNode &viewNode, std::shared_ptr<ViewInfo> viewInfo)
{
    std::lock_guard<std::mutex> lockGuard(m_mtxWarnMap);
    bool isWarnAlreadyExist = false;
    for (auto iter : this->m_warnInfoMap)
    {
        if (iter.first == viewNode)
        {
            isWarnAlreadyExist = true;
            break;
        }
    }

    if (!isWarnAlreadyExist)
    {
        WarnPrint("New Warn ViewInfo[%s][%s] comming......\n",
                  viewNode.strViewName.c_str(), viewNode.strExtraInfo.c_str());
        this->m_warnInfoMap.insert(std::make_pair(viewNode, viewInfo));
    }
}

/** 
 * @brief   从 m_mtxWarnMap 中获取指定Node的ViewInfo信息
 * 
 * @param   viewNode    告警信息的一些重要信息类对象
 * 
 * @return  获取的ViewInfo信息
 *          NULL    没有对应的信息
 *          非NULL  获取到的ViewInfo信息
 */
std::shared_ptr<ViewInfo> MsgHandler::GetWarnInfoFromMap(ViewNode &viewNode)
{
    std::lock_guard<std::mutex> lockGuard(m_mtxWarnMap);
    std::shared_ptr<ViewInfo> pViewInfo = std::make_shared<ViewInfo>();
    for (auto iter : m_warnInfoMap)
    {
        if (iter.first == viewNode)
        {
            pViewInfo = iter.second;
            return pViewInfo;
        }
    }

    return NULL;
}

/** 
 * @brief   删除 m_mtxWarnMap 中指定的Node信息
 * 
 * @param   viewNode    告警信息的一些重要信息类对象
 */
void MsgHandler::DeleteWarnInfoInMap(ViewNode &viewNode)
{
    std::lock_guard<std::mutex> lockGuard(m_mtxWarnMap);
    for (auto iter = this->m_warnInfoMap.begin(); iter != this->m_warnInfoMap.end(); iter++)
    {
        if (iter->first == viewNode)
        {
            m_warnInfoMap.erase(iter);
            return;
        }
    }
}

/** 
 * @brief   对于“在尚未执行的报警”队列m_freshViewQueue中等待执行的ON类告警
 *              如果在显示之前条件已经不满足了，则应该等到其最短时间显示完之
 *          后静默地自动从m_warnInfoMap中删除.
 * 
 * @param   viewNode    告警信息的一些重要信息类对象
 *          
 * @return  返回是否应该静默关闭
 *          true    应该(并且删除m_warnInfoMap中对应的报警节点)
 *          false   不应该
 */
bool MsgHandler::IsWarnClosedSilently(ViewNode &viewNode)
{
    if (viewNode.viewStatus != VIEW_OFF || viewNode.strExtraInfo.compare("nowarning") != 0)
    {
        return false;
    }

    if (viewNode.viewStatus == VIEW_OFF)
    {
        ViewNode viewNodeWait(viewNode.strViewName, viewNode.strExtraInfo, VIEW_ON);
        std::lock_guard<std::mutex> lockGuard(m_mtxWarnMap);
        for (auto iter = this->m_warnInfoMap.begin(); iter != this->m_warnInfoMap.end(); ++iter)
        {
            if (iter->first == viewNodeWait)
            {
                if (viewNodeWait == m_currViewNode && iter->second->HasAlreadyExecuted())
                {
                    /* 对于当前正在显示中的报警，最短显示时间1.5s过去之后即关闭之 */
                    while (!g_isAtLeastTimeDone)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    ResetAtLeastFlag();
                    Notify(viewNode);
                }
                this->m_warnInfoMap.erase(viewNodeWait);
                return true;
            }
        }
    }

    if (viewNode.strExtraInfo.compare("nowarning") == 0)
    {
        bool isMatchedOne = false;
        std::lock_guard<std::mutex> lockGuard(m_mtxWarnMap);
        std::vector<ViewNode> viewNodeDeleteList;
        for (auto iter : this->m_warnInfoMap)
        {
            if (iter.first.strViewName == viewNode.strViewName)
            {
                /* 对于利用nowarning来关闭报警的消息，通过匹配viewName来关闭之 */
                ViewNode viewNodeClose(iter.first.strViewName, iter.first.strExtraInfo, VIEW_OFF);
                Notify(viewNodeClose);
                viewNodeDeleteList.emplace_back(iter.first);
                isMatchedOne = true;
            }
        }

        for (auto iter : viewNodeDeleteList)
        {
            for (auto iterMap = this->m_warnInfoMap.begin(); iterMap != this->m_warnInfoMap.end(); iterMap++)
            {
                if (iterMap->first == iter)
                {
                    this->m_warnInfoMap.erase(iterMap);
                    break;
                }
            }
        }

        return isMatchedOne;
    }

    return false;
}

/** 
 * @brief   是否需要再次插入到轮询队列中
 * 
 * @param   viewNode    告警信息的一些重要信息类对象
 *          viewInfo    告警详细信息类对象
 *          
 * @return  返回是否需要的结果
 *          true    需要(为轮询类的VIEW_ON类告警)
 *          false   不需要
 */
bool MsgHandler::CheckNeedPushLoopAgain(ViewNode &viewNode, std::shared_ptr<ViewInfo> viewInfo)
{
    if (viewNode.viewStatus == VIEW_ON &&
        viewInfo->GetRepeatStatus().compare("ON") == 0 &&
        IsSeriousWarn(viewNode) == false)
    {
        return true;
    }

    return false;
}

/** 
 * @brief   是否为严重告警
 * 
 * @param   viewNode    告警信息的一些重要信息类对象
 *          
 * @return  返回是否为严重告警
 *          true    是
 *          false   不是
 */
bool MsgHandler::IsSeriousWarn(ViewNode &viewNode)
{
    std::shared_ptr<ViewInfo> viewInfo = GetWarnInfoFromMap(viewNode);
    if (viewInfo != NULL && viewInfo->GetLoop() == 0)
    {
        return true;
    }

    return false;
}

/** 
 * @brief   对于当前的仪表状态进行匹配判断
 *          先按照IGN的状态将HMI状态分两大类进行分析：
 *          IGN_OFF（即ACC_OFF）：
 *              如果满足此状态，则HMI的状态应该是 HMI_STATUS_ENTER_IGNOFF_ANIMATION 或者 HMI_STATUS_ENTER_IGNOFF，
 *              否则，状态匹配失败；
 * 
 *          非IGN_OFF（即ACC/ACC_ON/START）：
 *              如果满足此状态，则HMI的状态应该是 HMI_STATUS_EXIT_IGNON_ANIMATION 或者 HMI_STATUS_EXIT_SELFCHECK，
 *              否则，状态匹配失败；
 * 
 *          【特别】
 *              如果匹配失败次数超过 CURR_HMI_STATUS_MATCH_FAIL_MAX_TIMES，则按照匹配成功处理。
 * 
 * @return  返回匹配的结果
 *          true    匹配成功
 *          false   匹配失败
 */
bool MsgHandler::IsCurrHmiStatusMatchedSuccess()
{
    static int iStatusMatchFailedTimes = 0;

    HmiStatus currHmiStatus = HMI_STATUS_EXIT_IGNON_ANIMATION;
    if (m_curHmiStatus != currHmiStatus)
    {
        InfoPrint("HMI Status Update Old[%d] -> New[%d].\n", (int)m_curHmiStatus, (int)currHmiStatus);
        m_curHmiStatus = currHmiStatus;
    }

    /* IGN_OFF */
    if (Actuator::GetInstance()->IsIgnOFFCheck())
    {
        if (HMI_STATUS_ENTER_IGNOFF_ANIMATION == m_curHmiStatus || HMI_STATUS_ENTER_IGNOFF == m_curHmiStatus)
        {
            iStatusMatchFailedTimes = 0;
            return true;
        }
        /* 既不是自检结束也不是进入关机动画 [Current status is neither selfcheck end entering IGN_OFF animation nor entering IGN_OFF.] */
        else if (iStatusMatchFailedTimes > CURR_HMI_STATUS_MATCH_FAIL_MAX_TIMES)
        {
            WarnPrint("HMI Status has already matched failed [%d] times.\n", iStatusMatchFailedTimes);
            iStatusMatchFailedTimes = 0;
            return true;
        }
        else
        {
            iStatusMatchFailedTimes++;
            return false;
        }
    }
    /* NOT IGN_OFF */
    else
    {
        if (HMI_STATUS_EXIT_IGNON_ANIMATION == m_curHmiStatus || HMI_STATUS_EXIT_SELFCHECK == m_curHmiStatus)
        {
            return true;
        }
        /* 当前状态既不是进入开机动画也不是自检状态[Current status is neither IGN_ON animation over nor selfcheck.] */
        else
        {
            return false;
        }
    }
}

/** 
 * @brief   最小显示时间定时器关联的处理函数
 *          (当定时时间到了之后被调用的函数)
 */
static void AtLeastTimerThreadFun(union sigval arg)
{
    std::lock_guard<std::mutex> lockGuard(g_mtxTimeDone);
    g_isAtLeastTimeDone = true;
}

/** 
 * @brief   最小显示时间定时器初始化
 */
int MsgHandler::AtLeastTimerInit()
{
    struct sigevent event;
    int retCode;

    event.sigev_notify = SIGEV_THREAD;
    event.sigev_value.sival_ptr = NULL;
    event.sigev_notify_function = AtLeastTimerThreadFun;
    event.sigev_notify_attributes = NULL;

    retCode = timer_create(CLOCK_MONOTONIC, &event, &this->m_timerId);
    if (retCode != EOK)
    {
        ErrPrint("Failed to Create AtLeast Timer. ErrCode:[%s].\n", strerror(retCode));
        exit(1);
    }

    return retCode;
}

/** 
 * @brief   启动报警信息的最小显示时间（1.5秒）定时器
 * 
 * @return  定时器开始执行结果
 *          0   启动成功
 *          -1  启动失败
 */
int MsgHandler::AtLeastTimerStart()
{
    struct itimerspec iTimer;
    int retCode;

    /* The time wait for first start. */
    iTimer.it_value.tv_sec = 1;
    iTimer.it_value.tv_nsec = 500000;

    /* the interval after start. */
    iTimer.it_interval.tv_sec = 0;
    iTimer.it_interval.tv_nsec = 0;

    retCode = timer_settime(this->m_timerId, 0, &iTimer, NULL);
    if (retCode != EOK)
    {
        ErrPrint("Failed to Set AtLeast Time. ErrCode:[%s].\n", strerror(retCode));
    }

    return retCode;
}

/** 
 * @brief   停止报警信息的最小显示时间定时器
 * 
 * @return  定时器开始执行结果
 *          0   停止成功
 *          -1  停止失败
 */
int MsgHandler::AtLeastTimerStop()
{
    struct itimerspec iTimer;
    int retCode;

    /* The time wait for first start.When value is 0, mean stop it. */
    iTimer.it_value.tv_sec = 0;
    iTimer.it_value.tv_nsec = 0;

    /* the interval after start. */
    iTimer.it_interval.tv_sec = 0;
    iTimer.it_interval.tv_nsec = 0;

    retCode = timer_settime(this->m_timerId, 0, &iTimer, NULL);
    if (retCode != EOK)
    {
        ErrPrint("Failed to Set AtLeast Timer. ErrCode:[%s].\n", strerror(retCode));
    }

    ResetAtLeastFlag();

    return retCode;
}

/** 
 * @brief   删除最小显示时间定时器
 */
int MsgHandler::AtLeastTimerDelete()
{
    int retCode = timer_delete(this->m_timerId);
    if (retCode != EOK)
    {
        ErrPrint("Failed to Delete AtLeast Timer. ErrCode:[%s].\n", strerror(retCode));
    }

    return retCode;
}

/** 
 * @brief   重置告警最小显示时间标志
 */
void MsgHandler::ResetAtLeastFlag()
{
    std::lock_guard<std::mutex> lockGuard(g_mtxTimeDone);
    g_isAtLeastTimeDone = false;
}
