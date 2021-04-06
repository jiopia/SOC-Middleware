#include "MsgHandler.h"
#include "Actuator.h"
#include <map>
#include <string>
#include <mutex>
#include <memory>
#include <vector>

static bool ViewListSortFunction(ViewNode viewNode1, ViewNode viewNode2)
{
    return (viewNode1 < viewNode2);
}

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
    AtLeastTimerDelete();
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
    UpdateWarnViewNode(viewNode);
}

/** 
 * @brief   删除当前正在显示的告警信息
 */
void MsgHandler::RemoveCurrentWarn()
{
    WarnPrint("Remove Current Warning View Info.\n");
    std::lock_guard<std::mutex> lockGuard(m_mtxCurrWarn);
    if (!IsSeriousWarn(m_currViewNode) || m_currViewNode.viewStatus == VIEW_OFF)
    {
        m_currViewNode.SetEmpty();
    }
}

/** 
 * @brief   MsgHandler类的主要处理函数
 */
void MsgHandler::Run()
{
    while (1)
    {
        HandleWarnViews();
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); //防止CPU过载
    }
}

/** 
 * @brief   告警信息的主要处理函数
 */
void MsgHandler::HandleWarnViews()
{
    if (g_isAllWarnHidden)
    {
        InfoPrint("All warning message is Hidden now.\n");
        return;
    }

    ViewNode nextWarnViewNode;
    GetNextWarnViewNode(nextWarnViewNode);

    std::lock_guard<std::mutex> lockGuard(m_mtxCurrWarn);
    std::shared_ptr<ViewInfo> pNextViewInfo = GetWarnInfoFromMap(nextWarnViewNode);
    if (nextWarnViewNode.Empty() ||
        pNextViewInfo == NULL ||
        m_currViewNode == nextWarnViewNode)
    {
        if (!nextWarnViewNode.Empty() && pNextViewInfo == NULL)
        {
            EraseWarnViewNode(nextWarnViewNode);
        }
        return;
    }

    std::shared_ptr<ViewInfo> pCurrViewInfo = GetWarnInfoFromMap(m_currViewNode);
    if (pCurrViewInfo != NULL && //当前存在报警信息
        !g_isAtLeastTimeDone)    //最小计时还没有结束
    {
        DebugPrint("At Least the Time Required to Display is NOT Over.\n");
        return;
    }

    ViewNode viewNeedExected;
    switch (nextWarnViewNode.viewStatus)
    {
    case VIEW_OFF:
    {
        /* code */
        Actuator::GetInstance()->WarnShowTimerStop();
        EraseWarnViewNode(nextWarnViewNode);
        EraseWarnNodeFromInfoMap(nextWarnViewNode);

        viewNeedExected = nextWarnViewNode;
    }
    break;
    case VIEW_ON:
    case VIEW_FLICKER:
    {
        /* 下一个告警是：优先级更高的报警信息 */
        if (pCurrViewInfo != NULL &&
            pNextViewInfo->GetPriority() < pCurrViewInfo->GetPriority())
        {
            /* 对于上次显示的告警信息，如果需要轮询且未被执行过，则更新至轮训告警列表 */
            if (CheckNeedPushLoopAgain(m_currViewNode, pCurrViewInfo) &&
                !pCurrViewInfo->HasAlreadyExecuted())
            {
                pCurrViewInfo->SetAlreadyExecutedFlag();
                UpdateWarnInfoMap(m_currViewNode, *(pCurrViewInfo.get()));

                std::lock_guard<std::mutex> lockGuard(m_mtxLoop);
                m_loopViewList.emplace_back(m_currViewNode);
            }

            AtLeastTimerStart(); //1.5秒最短显示计时开始
            Actuator::GetInstance()->WarnShowTimerStop();
            Actuator::GetInstance()->WarnShowTimerStart(pViewInfo->GetLoop());

            viewNeedExected = nextWarnViewNode;
        }
    }
    break;
    default:
        break;
    }

    m_currViewNode = nextWarnViewNode;
    Notify(viewNeedExected);
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
 * @brief   告警信息发送(通知)函数
 */
void MsgHandler::Notify(ViewNode &viewNode)
{
    InfoPrint("Handling -- View[%s], Extra[%s], Status[%s].\n",
              viewNode.strViewName.c_str(), viewNode.strExtraInfo.c_str(),
              viewNode.viewStatus == VIEW_ON ? "ON" : "OFF");

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

    // MsgSend((*iter), &msgJustWithType, sizeof(msgJustWithType), NULL, 0);

    BDSTAR_FREE(warnData);
}

/** 
 * @brief   更新待显示列表的告警信息(更新/插入)
 */
void MsgHandler::UpdateWarnViewNode(ViewNode &viewNode)
{
    std::shared_ptr<ViewInfo> pViewInfo = m_xmlManager->GetViewInfo(viewNode.strViewName);
    if (pViewInfo == NULL)
    {
        WarnPrint("ViewName:[%s], ExtraName:[%s] is NOT supported now......\n",
                  viewNode.strViewName.c_str(), viewNode.strExtraInfo.c_str());
        return;
    }

    UpdateWarnInfoMap(viewNode, *(pViewInfo.get()));

    if (g_isAllWarnHidden &&
        viewNode.viewStatus == VIEW_ON)
    {
        //新报警触发，默认重置“隐藏所有报警”的标志位
        std::lock_guard<std::mutex> lockGuard(g_mtxHideAll);
        g_isAllWarnHidden = false;
    }

    if (IsSeriousWarn(viewNode))
    {
        UpdateSeriousWarnList(viewNode);
    }
    else
    {
        UpdateFrashWarnList(viewNode);
    }
}

/** 
 * @brief   更新严重告警列表的告警信息(更新/插入)
 */
void MsgHandler::UpdateSeriousWarnList(ViewNode &viewNode)
{
    bool isAlreadyExist = false;
    std::lock_guard<std::mutex> lockGuard(m_mtxSerious);
    for (auto iter = m_seriousViewList.begin(); iter != m_seriousViewList.end(); iter++)
    {
        ViewNode *tempNode = &(*iter);
        if (tempNode->strViewName == viewNode.strViewName &&
            tempNode->strExtraInfo == viewNode.strExtraInfo)
        {
            tempNode->viewStatus = viewNode.viewStatus;
            isAlreadyExist = true;
        }
    }

    if (!isAlreadyExist)
    {
        m_seriousViewList.emplace_back(viewNode);
        std::sort(m_seriousViewList.begin(), m_seriousViewList.end(), ViewListSortFunction);
    }

    /* 在处理非轮询类告警时，将轮询列表排序，为了保证之后再次处理轮询列表时是按照优先级处理的 */
    std::sort(m_loopViewList.begin(), m_loopViewList.end(), ViewListSortFunction);
}

/** 
 * @brief   更新尚未显示过的告警列表的告警信息(更新/插入)
 */
void MsgHandler::UpdateFrashWarnList(ViewNode &viewNode)
{
    bool isAlreadyExist = false;
    std::lock_guard<std::mutex> lockGuard(m_mtxFresh);
    for (auto iter = m_freshViewList.begin(); iter != m_freshViewList.end(); iter++)
    {
        ViewNode *tempNode = &(*iter);
        if (tempNode->strViewName == viewNode.strViewName &&
            tempNode->strExtraInfo == viewNode.strExtraInfo)
        {
            tempNode->viewStatus = viewNode.viewStatus;
            isAlreadyExist = true;
        }
    }

    if (!isAlreadyExist)
    {
        m_freshViewList.emplace_back(viewNode);
    }

    /* 在处理非轮询类告警时，将轮询列表排序，为了保证之后再次处理轮询列表时是按照优先级处理的 */
    std::sort(m_loopViewList.begin(), m_loopViewList.end(), ViewListSortFunction);
}

/** 
 * @brief   更新轮询显示的告警列表的告警信息(更新/插入)
 */
void MsgHandler::GetSeriousWarnListNode(ViewNode &viewNode)
{
    std::lock_guard<std::mutex> lockGuard(m_mtxSerious);
    if (!m_seriousViewList.empty())
    {
        viewNode = m_seriousViewList[0];
    }
}

void MsgHandler::GetFrashWarnListNode(ViewNode &viewNode)
{
    std::lock_guard<std::mutex> lockGuard(m_mtxFresh);
    if (!m_freshViewList.empty())
    {
        viewNode = m_freshViewList[0];
    }
}

void MsgHandler::GetLoopWarnListNode(ViewNode &viewNode)
{
    std::lock_guard<std::mutex> lockGuard(m_mtxLoop);
    if (!m_loopViewList.empty())
    {
        viewNode = m_loopViewList[0];
    }
}

/** 
 * @brief   依次从严重告警队列(优先)、待显示队列(次优先)、轮询队列(当待显示队列为空时)获取下一个告警信息
 * 
 * @param   viewNode    获取到的ViewNode信息
 */
void MsgHandler::GetNextWarnViewNode(ViewNode &viewNode)
{
    GetSeriousWarnListNode(viewNode);
    if (!viewNode.Empty())
    {
        return;
    }

    if (m_freshViewList.empty() && m_loopViewList.empty())
    {
        std::lock_guard<std::mutex> lockGuard(m_mtxWarnMap);
        m_warnInfoMap.clear(); //为了确保同步
        return;
    }

    if (!m_freshViewList.empty())
    {
        GetFrashWarnListNode(viewNode);
        EraseFreshWarnListNode(viewNode);
    }
    else if (!m_loopViewList.empty())
    {
        GetLoopWarnListNode(viewNode);
        EraseFreshWarnListNode(viewNode);
    }
}

void MsgHandler::EraseWarnViewNode(ViewNode viewNode)
{
    //利用了逻辑“且”的短路评估原则
    if (!EraseSeriousWarnListNode(viewNode) &&
        !EraseFreshWarnListNode(viewNode))
    {
        EraseLoopWarnListNode(viewNode);
    }

    EraseWarnNodeFromInfoMap(viewNode);
}

void MsgHandler::EraseSeriousWarnListNode(ViewNode viewNode)
{
    /* 严重报警 */
    bool isExist = false;
    std::lock_guard<std::mutex> lockGuard(m_mtxSerious);
    for (auto iter = m_seriousViewList.begin(); iter != m_seriousViewList.end(); iter++)
    {
        ViewNode tempNode = (*iter);
        if (tempNode == viewNode)
        {
            m_seriousViewList.erase(iter);
            isExist = true;
            break;
        }
    }

    return isExist;
}

void MsgHandler::EraseFreshWarnListNode(ViewNode viewNode)
{
    /* 首轮显示的报警 */
    bool isExist = false;
    std::lock_guard<std::mutex> lockGuard(m_mtxFresh);
    for (auto iter = m_freshViewList.begin(); iter != m_freshViewList.end(); iter++)
    {
        ViewNode tempNode = (*iter);
        if (tempNode == viewNode)
        {
            m_freshViewList.erase(iter);
            isExist = true;
            break;
        }
    }

    return isExist;
}

void MsgHandler::EraseLoopWarnListNode(ViewNode viewNode)
{
    /* 轮询显示的报警 */
    bool isExist = false;
    std::lock_guard<std::mutex> lockGuard(m_mtxLoop);
    for (auto iter = m_loopViewList.begin(); iter != m_loopViewList.end(); iter++)
    {
        ViewNode tempNode = (*iter);
        if (tempNode == viewNode)
        {
            m_loopViewList.erase(iter);
            isExist = true;
            break;
        }
    }

    return isExist;
}

/** 
 * @brief   从 m_mtxWarnMap 中获取指定Node的ViewInfo信息
 * 
 * @param   viewNode    告警信息的一些重要信息类对象的引用
 * 
 * @return  获取的ViewInfo信息
 *          NULL    没有对应的信息
 *          非NULL  获取到的ViewInfo信息
 */
std::shared_ptr<ViewInfo> MsgHandler::GetWarnInfoFromMap(ViewNode &viewNode)
{
    std::lock_guard<std::mutex> lockGuard(m_mtxWarnMap);
    std::shared_ptr<ViewInfo> pViewInfo = NULL;
    for (auto iter : m_warnInfoMap)
    {
        if (iter.first == viewNode)
        {
            ViewInfo tmpViewInfo = iter.second;
            pViewInfo = std::make_shared<ViewInfo>(tmpViewInfo);
            break;
        }
    }

    return pViewInfo;
}

/** 
 * @brief   更新/插入 m_mtxWarnMap 中指定的Node信息
 * 
 * @param   viewNode    告警信息的一些重要信息类对象的引用
 */
void MsgHandler::UpdateWarnInfoMap(ViewNode &viewNode, ViewInfo viewInfo)
{
    std::lock_guard<std::mutex> lockGuard(m_mtxWarnMap);
    for (auto iter : m_warnInfoMap)
    {
        ViewNode warnNode = iter.first;
        if (warnNode.strViewName == viewNode.strViewName &&
            warnNode.strExtraInfo == viewNode.strExtraInfo)
        {
            m_warnInfoMap.erase(iter);
            break;
        }
    }

    m_warnInfoMap.insert(std::make_pair<ViewNode, ViewInfo>(viewNode, viewInfo));
}

/** 
 * @brief   删除 m_mtxWarnMap 中指定的Node信息
 * 
 * @param   viewNode    告警信息的一些重要信息类对象的引用
 */
void MsgHandler::EraseWarnNodeFromInfoMap(ViewNode &viewNode)
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
 * @brief   获取告警的优先级
 * 
 * @param   viewNode    告警类对象的引用
 */
int MsgHandler::GetViewPriority(ViewNode &viewNode)
{
    int priority = -1;
    std::shared_ptr<ViewInfo> pViewInfo = m_xmlManager->GetViewInfo(viewNode.strViewName);
    if (pViewInfo != NULL)
    {
        priority = pViewInfo->GetPriority();
    }

    return priority;
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

#if 0
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