#include "Forwarder.hpp"

Forwarder::Forwarder()
{
}

Forwarder::~Forwarder()
{
}

void Forwarder::MsgPush(std::string strMsg)
{
    std::lock_guard<std::mutex> lockGuard(m_mtx);
    if (!strMsg.empty())
    {
        this->m_msgQueue.push(strMsg);
    }
}

std::string Forwarder::MsgPop()
{
    std::string strMsg;
    std::lock_guard<std::mutex> lockGuard(m_mtx);
    if (!m_msgQueue.empty())
    {
        strMsg = this->m_msgQueue.front();
        this->m_msgQueue.pop();
    }

    return strMsg;
}