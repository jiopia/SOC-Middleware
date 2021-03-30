#ifndef _VIEW_INFO_HPP_
#define _VIEW_INFO_HPP_

#include "Util.h"

enum VIEW_STATUS
{
    VIEW_DEFAULT = -1,
    VIEW_OFF,
    VIEW_ON,
    VIEW_FLICKER
};

#define POWER_MODE_WM1 "wm1"
#define POWER_MODE_WM2 "wm2"
#define POWER_MODE_WM1_WM2 "wm12"

#define MAX_VIEWINFO_NUM 100

class ViewInfo
{
public:
    ViewInfo() {}
    ~ViewInfo() {}

    bool operator<(const ViewInfo &viewInfo)
    {
        if (this->GetPriority() < viewInfo.GetPriority())
        {
            return true;
        }
        return false;
    }

    std::string GetName();
    std::string GetPid();
    std::string GetExtraInfo();
    std::string GetRepeatStatus();
    int GetLoop();
    int GetLeast();
    std::string GetType();
    int GetPriority() const;
    std::string GetShowView();
    std::string GetKey();
    std::string GetNotifyType();
    std::string GetMutexStatus();
    std::string GetAudioBindInfo();
    std::string GetPower();
    
    bool HasAlreadyExecuted();

    void SetKey(const char *key);
    void SetShowView(const char *showview);
    void SetName(const char *name);
    void SetPid(const char *pid);
    void SetExtraInfo(const char *name);
    void SetRepeatStatus(const char *repeatStatus);
    void SetLoop(int loop);
    void SetPriority(int priority);
    void SetType(const char *type);
    void SetNotifyType(const char *notifyType);
    void SetMutexStatus(const char *mutexStatus);
    void SetPower(const char *power);
    void SetAudioBindInfo(const char *audioBindInfo);
    void SetLeast(int least);

    void SetAlreadyExecutedFlag();

private:
    /* data */
    std::string m_strKey;
    std::string m_strShowview;
    std::string m_strName;
    std::string m_strPid;
    std::string m_strExtraInfo;
    std::string m_strRepeatStatus;
    int m_iLoop = -1;
    int m_iPriority = -1;
    std::string m_strType;
    std::string m_strNotifyType;
    std::string m_strMutexStatus;
    std::string m_strPower;
    std::string m_strAudioBindInfo;
    int m_iLeast = -1;

    bool isAlreadyExecuted = false;
};

#endif // !_VIEW_INFO_HPP_