#include "ViewInfo.hpp"

ViewInfo::ViewInfo()
{
}

ViewInfo::ViewInfo(const ViewInfo &viewInfo)
{
    this->m_strKey = viewInfo.GetKey();
    this->m_strShowview = viewInfo.GetShowView();
    this->m_strName = viewInfo.GetName();
    this->m_strPid = viewInfo.GetPid();
    this->m_strExtraInfo = viewInfo.GetExtraInfo();
    this->m_strRepeatStatus = viewInfo.GetRepeatStatus();
    this->m_iLoop = viewInfo.GetLoop();
    this->m_iPriority = viewInfo.GetPriority();
    this->m_strType = viewInfo.GetType();
    this->m_strNotifyType = viewInfo.GetNotifyType();
    this->m_strMutexStatus = viewInfo.GetMutexStatus();
    this->m_strPower = viewInfo.GetPower();
    this->m_strAudioBindInfo = viewInfo.GetAudioBindInfo();
    this->m_iLeast = viewInfo.GetLeast();

    this->isAlreadyExecuted = viewInfo.HasAlreadyExecuted();
}

ViewInfo::~ViewInfo()
{
}

std::string ViewInfo::GetName() const
{
    return this->m_strName;
}
std::string ViewInfo::GetPid() const
{
    return this->m_strPid;
}

std::string ViewInfo::GetExtraInfo() const
{
    return this->m_strExtraInfo;
}

int ViewInfo::GetLoop() const
{
    return this->m_iLoop;
}
int ViewInfo::GetLeast() const
{
    return this->m_iLeast;
}
std::string ViewInfo::GetType() const
{
    return this->m_strType;
}
int ViewInfo::GetPriority() const
{
    return this->m_iPriority;
}
std::string ViewInfo::GetShowView() const
{
    return this->m_strShowview;
}
std::string ViewInfo::GetKey() const
{
    return this->m_strKey;
}
std::string ViewInfo::GetNotifyType() const
{
    return this->m_strNotifyType;
}
std::string ViewInfo::GetMutexStatus() const
{
    return this->m_strMutexStatus;
}
std::string ViewInfo::GetRepeatStatus() const
{
    return this->m_strRepeatStatus;
}
std::string ViewInfo::GetAudioBindInfo() const
{
    return this->m_strAudioBindInfo;
}
std::string ViewInfo::GetPower() const
{
    return this->m_strPower;
}

bool ViewInfo::HasAlreadyExecuted() const
{
    return this->isAlreadyExecuted;
}

void ViewInfo::SetKey(const char *key)
{
    this->m_strKey = key;
}
void ViewInfo::SetShowView(const char *showview)
{
    this->m_strShowview = showview;
}
void ViewInfo::SetName(const char *name)
{
    this->m_strName = name;
}
void ViewInfo::SetPid(const char *pid)
{
    this->m_strPid = pid;
}

void ViewInfo::SetExtraInfo(const char *name)
{
    this->m_strExtraInfo = name;
}

void ViewInfo::SetRepeatStatus(const char *repeatStatus)
{
    this->m_strRepeatStatus = repeatStatus;
}
void ViewInfo::SetLoop(int loop)
{
    this->m_iLoop = loop;
}
void ViewInfo::SetPriority(int priority)
{
    this->m_iPriority = priority;
}
void ViewInfo::SetType(const char *type)
{
    this->m_strType = type;
}
void ViewInfo::SetNotifyType(const char *notifyType)
{
    this->m_strNotifyType = notifyType;
}
void ViewInfo::SetMutexStatus(const char *mutexStatus)
{
    this->m_strMutexStatus = mutexStatus;
}
void ViewInfo::SetPower(const char *power)
{
    this->m_strPower = power;
}
void ViewInfo::SetAudioBindInfo(const char *audioBindInfo)
{
    this->m_strAudioBindInfo = audioBindInfo;
}
void ViewInfo::SetLeast(int least)
{
    this->m_iLeast = least;
}
void ViewInfo::SetAlreadyExecutedFlag()
{
    this->isAlreadyExecuted = true;
}

void ViewInfo::PushChildViewName(std::string childviewName)
{
    this->childViewNames.emplace_back(childviewName);
}

std::vector<std::string> ViewInfo::GetChildViewNames()
{
    return this->childViewNames;
}
