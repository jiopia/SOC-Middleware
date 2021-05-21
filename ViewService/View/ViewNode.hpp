#ifndef _VIEW_NODE_HPP_
#define _VIEW_NODE_HPP_

#include "ViewInfo.hpp"
#include "XmlManager.h"

class ViewExtraInfo
{
public:
    ViewExtraInfo() {}
    ViewExtraInfo(std::string extraInfo, VIEW_STATUS status)
        : strExtraInfo(extraInfo), viewStatus(status) {}
    ~ViewExtraInfo() {}

    bool operator==(const ViewExtraInfo &info) const
    {
        return ((this->strExtraInfo.compare(info.strExtraInfo) == 0) &&
                (this->viewStatus == info.viewStatus));
    }

    ViewExtraInfo(const ViewExtraInfo &info)
    {
        this->strExtraInfo = info.strExtraInfo;
        this->viewStatus = info.viewStatus;
    }

    ViewExtraInfo &operator=(const ViewExtraInfo &info)
    {
        this->strExtraInfo = info.strExtraInfo;
        this->viewStatus = info.viewStatus;

        return *this;
    }

    std::string strExtraInfo;
    VIEW_STATUS viewStatus = VIEW_DEFAULT;
};

class ViewNode
{
public:
    ViewNode() {}
    ViewNode(std::string viewName, std::string extraInfo, VIEW_STATUS status)
        : strViewName(viewName), strExtraInfo(extraInfo), viewStatus(status) {}
    ~ViewNode() {}

    bool operator==(const ViewNode &node) const
    {
        return ((this->strViewName.compare(node.strViewName) == 0) &&
                (this->strExtraInfo.compare(node.strExtraInfo) == 0));
    }

    /* For std::priority_queue<T, std::vector<T>, std::greater<T>> */
    bool operator>(const ViewNode &viewNode) const
    {
        return (XmlManager::GetInstance()->GetViewInfo(this->GetKeyName())->GetPriority() >
                XmlManager::GetInstance()->GetViewInfo(viewNode.GetKeyName())->GetPriority());
    }

    /* For std::multimap<ViewNode, std::shared_ptr<ViewInfo>> */
    bool operator<(const ViewNode &viewNode) const
    {
        return (XmlManager::GetInstance()->GetViewInfo(this->GetKeyName())->GetPriority() <
                XmlManager::GetInstance()->GetViewInfo(viewNode.GetKeyName())->GetPriority());
    }

    ViewNode(const ViewNode &viewNode)
    {
        this->strViewName = viewNode.strViewName;
        this->strExtraInfo = viewNode.strExtraInfo;
        this->viewStatus = viewNode.viewStatus;
        this->extraInfos = viewNode.extraInfos;
    }

    ViewNode &operator=(const ViewNode &viewNode)
    {
        this->strViewName = viewNode.strViewName;
        this->strExtraInfo = viewNode.strExtraInfo;
        this->viewStatus = viewNode.viewStatus;
        this->extraInfos = viewNode.extraInfos;

        return *this;
    }

    bool Empty()
    {
        return this->strViewName.empty() && this->strExtraInfo.empty();
    }

    std::string GetKeyName() const
    {
        return this->strViewName;
    }

    void SetEmpty()
    {
        this->strViewName.clear();
        this->strExtraInfo.clear();
        this->extraInfos.clear();
        this->viewStatus = VIEW_DEFAULT;
    }

    static std::string GetViewStatusStr(VIEW_STATUS viewStatus)
    {
        switch (viewStatus)
        {
        case VIEW_ON:
            return std::string("ON");
        default:
            return std::string("OFF");
        }
    }

    void UpdateViewStatus()
    {
        VIEW_STATUS finalStatus;
        if (extraInfos.size() != 0)
        {
            finalStatus = extraInfos[0].viewStatus;
        }
        else
        {
            finalStatus = this->viewStatus;
        }

        for (auto iter = extraInfos.begin(); iter != extraInfos.end(); iter++)
        {
            if (iter->viewStatus == VIEW_ON)
            {
                finalStatus = VIEW_ON;
                break;
            }
        }

        this->viewStatus = finalStatus;
    }

    std::string strViewName;
    std::string strExtraInfo;
    VIEW_STATUS viewStatus = VIEW_DEFAULT;

    std::vector<ViewExtraInfo> extraInfos;
};

#endif //!_VIEW_NODE_HPP_
