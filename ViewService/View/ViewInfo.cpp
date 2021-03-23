#include "ViewInfo.hpp"

std::string ViewInfo::GetName()
{
    return this->m_strName;
}
std::string ViewInfo::GetPid()
{
    return this->m_strPid;
}

std::string ViewInfo::GetExtraInfo()
{
    return this->m_strExtraInfo;
}

int ViewInfo::GetLoop()
{
    return this->m_iLoop;
}
int ViewInfo::GetLeast()
{
    return this->m_iLeast;
}
std::string ViewInfo::GetType()
{
    return this->m_strType;
}
int ViewInfo::GetPriority() const
{
    return this->m_iPriority;
}
std::string ViewInfo::GetShowView()
{
    return this->m_strShowview;
}
std::string ViewInfo::GetKey()
{
    return this->m_strKey;
}
std::string ViewInfo::GetNotifyType()
{
    return this->m_strNotifyType;
}
std::string ViewInfo::GetMutexStatus()
{
    return this->m_strMutexStatus;
}
std::string ViewInfo::GetRepeatStatus()
{
    return this->m_strRepeatStatus;
}
std::string ViewInfo::GetAudioBindInfo()
{
    return this->m_strAudioBindInfo;
}
std::string ViewInfo::GetPower()
{
    return this->m_strPower;
}

bool ViewInfo::HasAlreadyExecuted()
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
