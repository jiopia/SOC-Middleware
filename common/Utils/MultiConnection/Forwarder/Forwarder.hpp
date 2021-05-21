#ifndef _MESSAGE_FORWARDER_H_
#define _MESSAGE_FORWARDER_H_

#include "Util.h"
#include "Singleton.hpp"

class MsgData
{
public:
    MsgData() {}
    MsgData(std::string topic, std::string msg) : strTopic(topic), strMsg(msg) {}

    ~MsgData() {}

    // MsgData(const MsgData &msgData)
    // {
    //     this->strTopic = msgData.strTopic;
    //     this->strMsg = msgData.strMsg;
    // }

    std::string strTopic;
    std::string strMsg;

    bool Empty()
    {
        return (strTopic.empty() && strMsg.empty());
    }
};

class Forwarder : public Singleton<Forwarder>
{
public:
    Forwarder();
    ~Forwarder();

    void MsgPush(MsgData msgData);
    MsgData MsgPop();

private:
    std::mutex m_mtx;
    std::queue<MsgData> m_msgQueue;
};

#endif // !_MESSAGE_FORWARDER_H_
