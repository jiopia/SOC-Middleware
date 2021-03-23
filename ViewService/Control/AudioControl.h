#ifndef _AUDIO_CONTROL_H
#define _AUDIO_CONTROL_H

#include "Util.h"
#include "Singleton.hpp"
#include "JsonHandler.h"

class AudioControl : public Singleton<AudioControl>
{
    friend class Singleton<AudioControl>;

public:
    AudioControl();
    ~AudioControl();

    void SendAudio(std::string strWarn, std::string strExtraInfo, VIEW_STATUS viewStatus);

private:
    JsonHandler *m_jsonHandler;
};

#endif