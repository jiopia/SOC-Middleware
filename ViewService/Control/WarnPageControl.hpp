#ifndef _WARN_PAGE_CONTROL_H
#define _WARN_PAGE_CONTROL_H

#include "Util.h"
#include "Singleton.hpp"
#include "ViewNode.hpp"
#include "MqttConnection.hpp"

class WarnPageControl : public Singleton<WarnPageControl>
{
    friend class Singleton<WarnPageControl>;

public:
    WarnPageControl();
    ~WarnPageControl();

    std::string GetWarnPageDataToHMI(ViewNode viewNode);

private:
};

#endif