#ifndef _MESSAGE_FORWARDER_H_
#define _MESSAGE_FORWARDER_H_

#include "Util.h"
#include "Singleton.hpp"

class Forwarder : public Singleton<Forwarder>
{
public:
    Forwarder();
    ~Forwarder();

    void MsgPush(std::string strMsg);
    std::string MsgPop();

private:
    std::mutex m_mtx;
    std::queue<std::string> m_msgQueue;
};

#endif // !_MESSAGE_FORWARDER_H_
