#ifndef _VIEW_NODE_HPP_
#define _VIEW_NODE_HPP_

#include "ViewInfo.hpp"
#include "XmlManager.h"

class ViewNode
{
public:
    ViewNode() {}
    ViewNode(std::string viewName, std::string extraInfo, VIEW_STATUS status)
        : strViewName(viewName), strExtraInfo(extraInfo), viewStatus(status) {}
    ~ViewNode() {}

    bool operator==(ViewNode &node) const
    {
        return ((this->strViewName.compare(node.strViewName) == 0) &&
                (this->strExtraInfo.compare(node.strExtraInfo) == 0) &&
                (this->viewStatus == node.viewStatus));
    }

    /* For std::priority_queue<T, std::vector<T>, std::greater<T>> */
    bool operator>(const ViewNode &viewNode) const
    {
        return (XmlManager::GetInstance()->GetViewInfo(this->strViewName)->GetPriority() >
                XmlManager::GetInstance()->GetViewInfo(viewNode.strViewName)->GetPriority());
    }

    /* For std::multimap<ViewNode, std::shared_ptr<ViewInfo>> */
    bool operator<(const ViewNode &viewNode) const
    {
        return (XmlManager::GetInstance()->GetViewInfo(this->strViewName)->GetPriority() <
                XmlManager::GetInstance()->GetViewInfo(viewNode.strViewName)->GetPriority());
    }

    ViewNode(const ViewNode &viewNode)
    {
        this->strViewName = viewNode.strViewName;
        this->strExtraInfo = viewNode.strExtraInfo;
        this->viewStatus = viewNode.viewStatus;
    }

    ViewNode &operator=(const ViewNode &viewNode)
    {
        this->strViewName = viewNode.strViewName;
        this->strExtraInfo = viewNode.strExtraInfo;
        this->viewStatus = viewNode.viewStatus;

        return *this;
    }

    bool Empty()
    {
        return this->strViewName.empty() && this->strExtraInfo.empty();
    }

    std::string strViewName;
    std::string strExtraInfo;
    VIEW_STATUS viewStatus = VIEW_DEFAULT;
};

#endif //!_VIEW_NODE_HPP_