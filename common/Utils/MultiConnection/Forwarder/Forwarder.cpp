#include "Forwarder.hpp"

Forwarder::Forwarder()
{
}

Forwarder::~Forwarder()
{
}

void Forwarder::MsgPush(MsgData msgData)
{
    std::lock_guard<std::mutex> lockGuard(m_mtx);
    if (!msgData.Empty())
    {
        this->m_msgQueue.push(msgData);
    }
}

MsgData Forwarder::MsgPop()
{
    MsgData msgData;
    std::lock_guard<std::mutex> lockGuard(m_mtx);
    if (!m_msgQueue.empty())
    {
        msgData = this->m_msgQueue.front();
        this->m_msgQueue.pop();
    }

    return msgData;
}