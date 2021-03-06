#include "MsgHandler.h"
#include "Actuator.h"
#include <map>
#include <string>
#include <mutex>
#include <memory>
#include <vector>
#include <algorithm>

static bool ViewListSortFunction(ViewNode viewNode1, ViewNode viewNode2)
{
    return (viewNode1 < viewNode2);
}

/* 告警是否完成最小显示时间显示的标志 */
static std::mutex g_mtxTimeDone;
static bool g_isAtLeastTimeDone = true;

MsgHandler::MsgHandler() : m_audioControl(AudioControl::GetInstance()),
                           m_xmlManager(XmlManager::GetInstance()),
                           m_jsonHandler(JsonHandler::GetInstance()),
                           m_warnPageControl(WarnPageControl::GetInstance())
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
    if (strViewName.compare("ACC") == 0)
    {
        std::lock_guard<std::mutex> lockGuard(m_mtxAccStatus);
        VehicleAccStatus tempStatus = (viewStatus == VIEW_OFF) ? VEHICLE_OFF : VEHICLE_ON;
        if (m_vehicleStatus != tempStatus)
        {
            m_vehicleStatus = tempStatus;
        }
    }
    else if (strViewName.compare("vehicle_speed") == 0)
    {
        m_strVehicleSpeed = strExtraInfo;
    }
    else if (strViewName.compare("warn_hiden") == 0)
    {
        SetAllWarnHidenStatus(true);
    }
    else if (strViewName.compare("get_curr_warn") == 0)
    {
        if (GetAllWarnHidenStatus())
        {
            return;
        }

        std::lock_guard<std::mutex> lockGuard(m_mtxCurrWarn);
        ViewInfoPtr pViewInfo = m_xmlManager->GetViewInfo(m_currViewNode.strViewName);
        if (pViewInfo == NULL)
        {
            return;
        }

        ResetAtLeastFlag();
        AtLeastTimerStart(); //1.5秒最短显示计时开始
        Actuator::GetInstance()->WarnShowTimerStop();
        int iLoopTime = pViewInfo->GetLoop();
        Actuator::GetInstance()->WarnShowTimerStart(iLoopTime);
        Notify(m_currViewNode);
    }
    else
    {
        ViewInfoPtr viewInfo = m_xmlManager->GetViewInfo(strViewName);
        if (viewInfo == NULL)
        {
            DIAG_ERR("Can NOT find ViewInfo which viewname is [%s]!\n", strViewName.c_str());
            return;
        }

        DIAG_INFO("DEBUG -- View[%s], Extra[%s], Status[%s].\n",
                  strViewName.c_str(), strExtraInfo.c_str(), viewStatus == VIEW_ON ? "ON" : "OFF");

        ViewNode viewNode(strViewName, strExtraInfo, viewStatus);
        UpdateWarnViewNode(viewNode);
    }
}

/** 
 * @brief   删除当前正在显示的告警信息
 */
void MsgHandler::RemoveCurrentWarn()
{
    DIAG_WARN("Remove Current Warning View Info.\n");
    ViewNode tempViewNode;
    {
        std::lock_guard<std::mutex> lockGuard(m_mtxCurrWarn);
        tempViewNode = m_currViewNode;
    }

    DIAG_INFO("WarnView:[%s]-[%s]\n", tempViewNode.strViewName.c_str(), tempViewNode.strExtraInfo.c_str());

    EraseWarnViewNode(tempViewNode);

    /* 对于上次显示的告警信息，如果需要轮询且未被执行过，则更新至轮训告警列表 */
    if (CheckNeedPushLoopAgain(tempViewNode))
    {
        std::lock_guard<std::mutex> lockGuard(m_mtxLoop);
        if (find(m_loopViewList.begin(), m_loopViewList.end(), tempViewNode) == m_loopViewList.end())
        {
            DIAG_WARN("WarnView:[%s] Need Push Loop List.\n", tempViewNode.strViewName.c_str());
            m_loopViewList.emplace_back(tempViewNode);
        }

        DIAG_INFO("m_loopViewList Size:[%d]\n", (int)m_loopViewList.size());
        for (auto iter : m_loopViewList)
        {
            ViewNode warnNode = iter;
            DIAG_INFO("LoopWarnList -- ViewName:[%s], ExtraInfo:[%s], OnOff[%s]\n",
                      warnNode.strViewName.c_str(),
                      warnNode.strExtraInfo.c_str(),
                      warnNode.viewStatus == VIEW_ON ? "ON" : "OFF");

            for (auto iterExtraInfo : warnNode.extraInfos)
            {
                ViewExtraInfo extraInfo = iterExtraInfo;
                DIAG_INFO("----------> ExtraInfos -- ExtraInfo:[%s], OnOff[%s]\n",
                          extraInfo.strExtraInfo.c_str(),
                          extraInfo.viewStatus == VIEW_ON ? "ON" : "OFF");
            }
        }
    }
}

/** 
 * @brief   MsgHandler类的主要处理函数
 */
void MsgHandler::Run()
{
    while (1)
    {
        if (IsAtLeastTimeDone())
        {
            HandleWarnViews();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); //防止CPU过载
    }
}

/** 
 * @brief   告警信息的主要处理函数
 */
void MsgHandler::HandleWarnViews()
{
    if (GetAllWarnHidenStatus())
    {
        DIAG_INFO("All warning message is Hidden now.\n");
        return;
    }

    ViewNode nextWarnViewNode;
    GetNextWarnViewNode(nextWarnViewNode);
    if (nextWarnViewNode.Empty())
    {
        return;
    }

    std::shared_ptr<ViewInfo> pNextViewInfo = m_xmlManager->GetViewInfo(nextWarnViewNode.GetKeyName());
    if (pNextViewInfo == NULL || m_currViewNode == nextWarnViewNode)
    {
        if (pNextViewInfo == NULL)
        {
            EraseWarnViewNode(nextWarnViewNode);
        }

        if (m_currViewNode == nextWarnViewNode)
        {
            if (m_currViewNode.strViewName != "doorwarn" && m_currViewNode.strViewName != "doorinfo")
            {
                if (m_currViewNode.viewStatus == nextWarnViewNode.viewStatus)
                {
                    return;
                }
            }
            else if ((m_currViewNode.viewStatus == nextWarnViewNode.viewStatus) &&
                     (GetDoorInfoFlagStr(m_currViewNode) == GetDoorInfoFlagStr(nextWarnViewNode)))
            {
                return;
            }
        }
    }

    ViewNode viewNeedExected;
    switch (nextWarnViewNode.viewStatus)
    {
    case VIEW_OFF:
    {
        DIAG_INFO("---------> Debug 1\n");
        EraseWarnViewNode(nextWarnViewNode);
        viewNeedExected = nextWarnViewNode;
        Actuator::GetInstance()->WarnShowTimerStop();

        std::lock_guard<std::mutex> lockGuard(m_mtxCurrWarn);
        m_currViewNode.SetEmpty();
    }
    break;
    case VIEW_ON:
    case VIEW_FLICKER:
    {
        ResetAtLeastFlag();
        AtLeastTimerStart(); //1.5秒最短显示计时开始
        Actuator::GetInstance()->WarnShowTimerStop();
        int iLoopTime = pNextViewInfo->GetLoop();
        DIAG_INFO("LoopTime:%d seconds.\n", iLoopTime);
        Actuator::GetInstance()->WarnShowTimerStart(iLoopTime);
        viewNeedExected = nextWarnViewNode;

        std::lock_guard<std::mutex> lockGuard(m_mtxCurrWarn);
        m_currViewNode = nextWarnViewNode;
    }
    break;
    default:
        break;
    }

    if (!viewNeedExected.Empty())
    {
        Notify(viewNeedExected);
    }
}

/** 
 * @brief   隐藏当前列表中的所有的告警信息
 *          (当这个信息来到的时候，说明已经进入轮询的显示了)
 */
void MsgHandler::HideAllWarn()
{
    DIAG_WARN("Hide All Warning View Info.\n");
    if (IsAtLeastTimeDone())
    {
        SetAllWarnHidenStatus(true);
    }
}

/** 
 * @brief   告警信息发送(通知)函数
 */
void MsgHandler::Notify(ViewNode &viewNode)
{
    DIAG_INFO("Handling -- View[%s], Extra[%s], Status[%s].\n",
              viewNode.strViewName.c_str(), viewNode.strExtraInfo.c_str(),
              (viewNode.viewStatus == VIEW_ON || viewNode.viewStatus == VIEW_OFF)
                  ? (viewNode.viewStatus == VIEW_ON ? "ON" : "OFF")
                  : "DEFAULT");

    std::shared_ptr<ViewInfo> pViewInfo = m_xmlManager->GetViewInfo(viewNode.GetKeyName());
    if (pViewInfo == NULL)
    {
        return;
    }

    if (!CheckAccStatusConform(pViewInfo) || GetAllWarnHidenStatus())
    {
        return;
    }

    if (!pViewInfo->GetAudioBindInfo().empty())
    {
        m_audioControl->SendAudio("warning",
                                  pViewInfo->GetAudioBindInfo(),
                                  m_currViewNode.GetViewStatusStr(m_currViewNode.viewStatus));
    }

    std::string strMsgSend = m_warnPageControl->GetWarnPageDataToHMI(viewNode);
    if (!strMsgSend.empty())
    {
        Actuator::GetInstance()->MsgSend(std::string(MQTT_TOPIC_WARN_MW_TO_HMI), strMsgSend);
    }
}

/** 
 * @brief   更新待显示列表的告警信息(更新/插入)
 */
void MsgHandler::UpdateWarnViewNode(ViewNode viewNode)
{
    std::shared_ptr<ViewInfo> pViewInfo = m_xmlManager->GetViewInfo(viewNode.GetKeyName());
    if (pViewInfo == NULL)
    {
        DIAG_WARN("ViewName:[%s], ExtraName:[%s] is NOT supported now......\n",
                  viewNode.strViewName.c_str(), viewNode.strExtraInfo.c_str());
        return;
    }

    if (GetAllWarnHidenStatus() &&
        viewNode.viewStatus == VIEW_ON)
    {
        //新报警触发，默认重置“隐藏所有报警”的标志位
        SetAllWarnHidenStatus(false);
    }

    if (IsSeriousWarn(viewNode))
    {
        UpdateSeriousWarnList(viewNode);
    }
    else
    {
        UpdateFrashWarnList(viewNode);
    }

#if 1 /* -- Debug for Printing WarnInfo -- */
    {
        DIAG_INFO("m_seriousViewList Size:[%d]\n", (int)m_seriousViewList.size());
        for (auto iter : m_seriousViewList)
        {
            ViewNode warnNode = iter;
            DIAG_INFO("SeriousWarnList -- ViewName:[%s], ExtraInfo:[%s], OnOff[%s]\n",
                      warnNode.strViewName.c_str(),
                      warnNode.strExtraInfo.c_str(),
                      warnNode.viewStatus == VIEW_ON ? "ON" : "OFF");

            for (auto iterExtraInfo : warnNode.extraInfos)
            {
                ViewExtraInfo extraInfo = iterExtraInfo;
                DIAG_INFO("----------> ExtraInfos -- ExtraInfo:[%s], OnOff[%s]\n",
                          extraInfo.strExtraInfo.c_str(),
                          extraInfo.viewStatus == VIEW_ON ? "ON" : "OFF");
            }
        }

        DIAG_INFO("m_freshViewList Size:[%d]\n", (int)m_freshViewList.size());
        for (auto iter : m_freshViewList)
        {
            ViewNode warnNode = iter;
            DIAG_INFO("FreshWarnList -- ViewName:[%s], ExtraInfo:[%s], OnOff[%s]\n",
                      warnNode.strViewName.c_str(),
                      warnNode.strExtraInfo.c_str(),
                      warnNode.viewStatus == VIEW_ON ? "ON" : "OFF");

            for (auto iterExtraInfo : warnNode.extraInfos)
            {
                ViewExtraInfo extraInfo = iterExtraInfo;
                DIAG_INFO("----------> ExtraInfos -- ExtraInfo:[%s], OnOff[%s]\n",
                          extraInfo.strExtraInfo.c_str(),
                          extraInfo.viewStatus == VIEW_ON ? "ON" : "OFF");
            }
        }

        DIAG_INFO("m_loopViewList Size:[%d]\n", (int)m_loopViewList.size());
        for (auto iter : m_loopViewList)
        {
            ViewNode warnNode = iter;
            DIAG_INFO("LoopWarnList -- ViewName:[%s], ExtraInfo:[%s], OnOff[%s]\n",
                      warnNode.strViewName.c_str(),
                      warnNode.strExtraInfo.c_str(),
                      warnNode.viewStatus == VIEW_ON ? "ON" : "OFF");

            for (auto iterExtraInfo : warnNode.extraInfos)
            {
                ViewExtraInfo extraInfo = iterExtraInfo;
                DIAG_INFO("----------> ExtraInfos -- ExtraInfo:[%s], OnOff[%s]\n",
                          extraInfo.strExtraInfo.c_str(),
                          extraInfo.viewStatus == VIEW_ON ? "ON" : "OFF");
            }
        }

        DIAG_INFO("CurrentWarn -- ViewName:[%s], ExtraInfo:[%s], OnOff[%s]\n",
                  m_currViewNode.strViewName.c_str(),
                  m_currViewNode.strExtraInfo.c_str(),
                  m_currViewNode.viewStatus == VIEW_ON ? "ON" : "OFF");
    }
#endif
}

/** 
 * @brief   更新严重告警列表的告警信息(更新/插入)
 */
void MsgHandler::UpdateSeriousWarnList(ViewNode viewNode)
{
    {
        bool isAlreadyExist = false;
        ViewNode *tempNode = NULL;
        std::lock_guard<std::mutex> lockGuard(m_mtxSerious);
        for (auto iter = m_seriousViewList.begin(); iter != m_seriousViewList.end(); iter++)
        {
            tempNode = &(*iter);
            if (tempNode->strViewName == viewNode.strViewName)
            {
                isAlreadyExist = true;
                break;
            }
        }

        /* 已经存在的报警信息(更新) */
        if (isAlreadyExist && tempNode != NULL)
        {
            if (tempNode->strViewName != "doorwarn")
            {
                tempNode->viewStatus = viewNode.viewStatus;
            }
            else
            {
                bool isExtraInfoExist = false;
                for (auto iter = tempNode->extraInfos.begin(); iter != tempNode->extraInfos.end(); iter++)
                {
                    ViewExtraInfo *tempExtraInfo = &(*iter);
                    if (tempExtraInfo->strExtraInfo == viewNode.strExtraInfo)
                    {
                        isExtraInfoExist = true;
                        tempExtraInfo->viewStatus = viewNode.viewStatus;
                        break;
                    }
                }

                if (!isExtraInfoExist)
                {
                    ViewExtraInfo viewExtraInfo(viewNode.strExtraInfo, viewNode.viewStatus);
                    tempNode->extraInfos.emplace_back(viewExtraInfo);
                }

                tempNode->UpdateViewStatus();
            }
        }
        else if (viewNode.viewStatus == VIEW_ON)
        {
            /* 新报警信息(插入) */
            if (viewNode.strViewName == "doorwarn")
            {
                ViewExtraInfo viewExtraInfo(viewNode.strExtraInfo, viewNode.viewStatus);
                viewNode.strExtraInfo = viewNode.strViewName;
                viewNode.extraInfos.emplace_back(viewExtraInfo);
                viewNode.UpdateViewStatus();
            }

            m_seriousViewList.emplace_back(viewNode);
        }

        std::sort(m_seriousViewList.begin(), m_seriousViewList.end(), ViewListSortFunction);
    }

    {
        /* 在处理非轮询类告警时，将轮询列表排序，为了保证之后再次处理轮询列表时是按照优先级处理的 */
        std::lock_guard<std::mutex> lockGuard(m_mtxLoop);
        std::sort(m_loopViewList.begin(), m_loopViewList.end(), ViewListSortFunction);
    }
}

/** 
 * @brief   更新尚未显示过的告警列表的告警信息(更新/插入)
 */
void MsgHandler::UpdateFrashWarnList(ViewNode viewNode)
{
    if (viewNode.strViewName == "doorinfo")
    {
        ViewNode tmpWarnOnNode(viewNode.strViewName, viewNode.strViewName, viewNode.viewStatus);
        if (IsNodeExistInLoopList(tmpWarnOnNode))
        {
            ViewNode loopDoorInfoNode;
            GetLoopDoorInfoNode(loopDoorInfoNode);
            EraseLoopWarnListNode(tmpWarnOnNode);

            bool isExtraInfoExist = false;
            for (auto iter = loopDoorInfoNode.extraInfos.begin(); iter != loopDoorInfoNode.extraInfos.end(); iter++)
            {
                ViewExtraInfo *tempExtraInfo = &(*iter);
                if (tempExtraInfo->strExtraInfo == viewNode.strExtraInfo)
                {
                    isExtraInfoExist = true;
                    tempExtraInfo->viewStatus = viewNode.viewStatus;
                    break;
                }
            }

            if (!isExtraInfoExist)
            {
                ViewExtraInfo viewExtraInfo(viewNode.strExtraInfo, viewNode.viewStatus);
                loopDoorInfoNode.extraInfos.emplace_back(viewExtraInfo);
            }

            loopDoorInfoNode.UpdateViewStatus();

            std::lock_guard<std::mutex> lockGuard(m_mtxFresh);
            m_freshViewList.emplace_back(loopDoorInfoNode);
            return;
        }
        else if (IsNodeExistInFreshList(tmpWarnOnNode))
        {
            ViewNode *tempNode = NULL;
            std::lock_guard<std::mutex> lockGuard(m_mtxFresh);
            for (auto iter = m_freshViewList.begin(); iter != m_freshViewList.end(); iter++)
            {
                tempNode = &(*iter);
                if (tempNode->strViewName == viewNode.strViewName)
                {
                    bool isExtraInfoExist = false;
                    for (auto iter = tempNode->extraInfos.begin(); iter != tempNode->extraInfos.end(); iter++)
                    {
                        ViewExtraInfo *tempExtraInfo = &(*iter);
                        if (tempExtraInfo->strExtraInfo == viewNode.strExtraInfo)
                        {
                            isExtraInfoExist = true;
                            tempExtraInfo->viewStatus = viewNode.viewStatus;
                            break;
                        }
                    }

                    if (!isExtraInfoExist)
                    {
                        ViewExtraInfo viewExtraInfo(viewNode.strExtraInfo, viewNode.viewStatus);
                        tempNode->extraInfos.emplace_back(viewExtraInfo);
                    }

                    tempNode->UpdateViewStatus();
                    break;
                }
            }
        }
        else if (viewNode.viewStatus == VIEW_ON)
        {
            /* 新报警信息(插入) */
            ViewExtraInfo viewExtraInfo(viewNode.strExtraInfo, viewNode.viewStatus);
            viewNode.strExtraInfo = viewNode.strViewName;
            viewNode.extraInfos.emplace_back(viewExtraInfo);
            viewNode.UpdateViewStatus();

            std::lock_guard<std::mutex> lockGuard(m_mtxFresh);
            m_freshViewList.emplace_back(viewNode);
        }
    }
    else
    {
        if (IsNodeExistInLoopList(viewNode))
        {
            EraseLoopWarnListNode(viewNode);

            std::lock_guard<std::mutex> lockGuard(m_mtxFresh);
            m_freshViewList.emplace_back(viewNode);
        }
        else if (IsNodeExistInFreshList(viewNode))
        {
            ViewNode *tempNode = NULL;
            std::lock_guard<std::mutex> lockGuard(m_mtxFresh);
            for (auto iter = m_freshViewList.begin(); iter != m_freshViewList.end(); iter++)
            {
                tempNode = &(*iter);
                if (tempNode->strViewName == viewNode.strViewName)
                {
                    tempNode->viewStatus = viewNode.viewStatus;
                    break;
                }
            }
        }
        else
        {
            std::lock_guard<std::mutex> lockGuard(m_mtxFresh);
            m_freshViewList.emplace_back(viewNode);
        }
    }

    {
        /* 在处理非轮询类告警时，将轮询列表排序，为了保证之后再次处理轮询列表时是按照优先级处理的 */
        std::lock_guard<std::mutex> lockGuard(m_mtxLoop);
        std::sort(m_loopViewList.begin(), m_loopViewList.end(), ViewListSortFunction);
    }
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

void MsgHandler::GetLoopDoorInfoNode(ViewNode &viewNode)
{
    std::lock_guard<std::mutex> lockGuard(m_mtxLoop);
    for (auto iter : m_loopViewList)
    {
        ViewNode warnNode = iter;
        if (warnNode.strViewName == "doorinfo")
        {
            viewNode = warnNode;
            break;
        }
    }
}

/** 
 * @brief   依次从严重告警队列(优先)、待显示队列(次优先)、轮询队列(当待显示队列为空时)获取下一个告警信息
 * 
 * @param   viewNode    获取到的ViewNode信息
 */
void MsgHandler::GetNextWarnViewNode(ViewNode &viewNode)
{
    /* 1、严重报警 */
    GetSeriousWarnListNode(viewNode);
    if (!viewNode.Empty())
    {
        return;
    }

    if (m_freshViewList.empty() && m_loopViewList.empty())
    {
        return;
    }

    if (!m_freshViewList.empty())
    {
        /* 2、尚未被执行过的报警 */
        GetFrashWarnListNode(viewNode);
    }
    else if (!m_loopViewList.empty())
    {
        /* 3、执行过的需要轮训的报警 */
        GetLoopWarnListNode(viewNode);
    }
}

void MsgHandler::EraseWarnViewNode(ViewNode viewNode)
{
    //利用了逻辑“且”的短路评估原则[严重报警->尚未被执行过的报警->轮训类报警]
    if (!EraseSeriousWarnListNode(viewNode) &&
        !EraseFreshWarnListNode(viewNode))
    {
        EraseLoopWarnListNode(viewNode);
    }
}

bool MsgHandler::EraseSeriousWarnListNode(ViewNode viewNode)
{
    /* 严重报警 */
    DIAG_INFO("---------> Debug 2\n");
    bool isExist = false;
    std::lock_guard<std::mutex> lockGuard(m_mtxSerious);
    for (auto iter = m_seriousViewList.begin(); iter != m_seriousViewList.end(); iter++)
    {
        ViewNode tempNode = (*iter);
        if (tempNode == viewNode)
        {
            DIAG_INFO("---------> Debug 3\n");
            m_seriousViewList.erase(iter);
            isExist = true;
            break;
        }
    }

    DIAG_INFO("---------> Debug 4 [%s]\n", isExist ? "Exist" : "NOT Exist");
    return isExist;
}

bool MsgHandler::EraseFreshWarnListNode(ViewNode viewNode)
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

bool MsgHandler::EraseLoopWarnListNode(ViewNode viewNode)
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

bool MsgHandler::IsNodeExistInFreshList(ViewNode viewNode)
{
    bool isAlreadyExist = false;
    std::lock_guard<std::mutex> lockGuard(m_mtxFresh);
    auto iterFind = find(m_freshViewList.begin(), m_freshViewList.end(), viewNode);
    if (iterFind != m_freshViewList.end())
    {
        isAlreadyExist = true;
    }

    return isAlreadyExist;
}

bool MsgHandler::IsNodeExistInLoopList(ViewNode viewNode)
{
    bool isAlreadyExist = false;
    std::lock_guard<std::mutex> lockGuard(m_mtxLoop);
    auto iterFind = find(m_loopViewList.begin(), m_loopViewList.end(), viewNode);
    if (iterFind != m_loopViewList.end())
    {
        isAlreadyExist = true;
    }

    return isAlreadyExist;
}

/** 
 * @brief   获取告警的优先级
 * 
 * @param   viewNode    告警类对象的引用
 */
int MsgHandler::GetViewPriority(ViewNode &viewNode)
{
    int priority = -1;
    std::shared_ptr<ViewInfo> pViewInfo = m_xmlManager->GetViewInfo(viewNode.GetKeyName());
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
bool MsgHandler::CheckNeedPushLoopAgain(ViewNode &viewNode)
{
    std::shared_ptr<ViewInfo> pViewInfo = m_xmlManager->GetViewInfo(viewNode.GetKeyName());
    if (pViewInfo != NULL &&
        viewNode.viewStatus == VIEW_ON &&
        pViewInfo->GetRepeatStatus().compare("ON") == 0 &&
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
    std::shared_ptr<ViewInfo> viewInfo = m_xmlManager->GetViewInfo(viewNode.GetKeyName());
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
    return true;
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
        DIAG_ERR("Failed to Create AtLeast Timer. ErrCode:[%s].\n", strerror(retCode));
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
        DIAG_ERR("Failed to Set AtLeast Time. ErrCode:[%s].\n", strerror(retCode));
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
        DIAG_ERR("Failed to Set AtLeast Timer. ErrCode:[%s].\n", strerror(retCode));
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
        DIAG_ERR("Failed to Delete AtLeast Timer. ErrCode:[%s].\n", strerror(retCode));
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

bool MsgHandler::IsAtLeastTimeDone()
{
    std::lock_guard<std::mutex> lockGuard(g_mtxTimeDone);
    return g_isAtLeastTimeDone;
}

std::string MsgHandler::GetDoorInfoFlagStr(ViewNode viewNode)
{
    std::string strDoorFlags = "000000";
    std::string strFlag;

    for (auto iter = viewNode.extraInfos.begin(); iter != viewNode.extraInfos.end(); iter++)
    {
        ViewExtraInfo *tempExtraInfo = &(*iter);
        strFlag = (tempExtraInfo->viewStatus == VIEW_ON) ? "1" : "0";

        if (tempExtraInfo->strExtraInfo == "DRIVER_DOOR")
        {
            strDoorFlags = strFlag + strDoorFlags.substr(1);
        }
        else if (tempExtraInfo->strExtraInfo == "PASSENGER_DOOR")
        {
            strDoorFlags = strDoorFlags.substr(0, 1) + strFlag + strDoorFlags.substr(2);
        }
        else if (tempExtraInfo->strExtraInfo == "REAR_RIGHT_DOOR")
        {
            strDoorFlags = strDoorFlags.substr(0, 2) + strFlag + strDoorFlags.substr(3);
        }
        else if (tempExtraInfo->strExtraInfo == "REAR_LEFT_DOOR")
        {
            strDoorFlags = strDoorFlags.substr(0, 3) + strFlag + strDoorFlags.substr(4);
        }
        else if (tempExtraInfo->strExtraInfo == "HOOD")
        {
            strDoorFlags = strDoorFlags.substr(0, 4) + strFlag + strDoorFlags.substr(5);
        }
        else if (tempExtraInfo->strExtraInfo == "BACK_DOOR")
        {
            strDoorFlags = strDoorFlags.substr(0, 5) + strFlag;
        }
    }

    return strDoorFlags;
}

bool MsgHandler::CheckAccStatusConform(std::shared_ptr<ViewInfo> pViewInfo)
{
    bool isConform = false;
    std::string strPower = pViewInfo->GetPower();
    std::lock_guard<std::mutex> lockGuard(m_mtxAccStatus);
    if (m_vehicleStatus == VEHICLE_ON)
    {
        if (strPower.compare("wm1") == 0 || strPower.compare("wm12") == 0)
        {
            isConform = true;
        }
    }
    else if (m_vehicleStatus == VEHICLE_OFF)
    {
        if (strPower.compare("wm2") == 0 || strPower.compare("wm12") == 0)
        {
            isConform = true;
        }
    }

    return isConform;
}

bool MsgHandler::GetAllWarnHidenStatus()
{
    std::lock_guard<std::mutex> lockGuard(m_mtxHideAll);
    return m_isAllWarnHidden;
}

void MsgHandler::SetAllWarnHidenStatus(bool status)
{
    std::lock_guard<std::mutex> lockGuard(m_mtxHideAll);
    m_isAllWarnHidden = status;
}
