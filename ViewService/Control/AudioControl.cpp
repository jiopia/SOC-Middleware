#include "AudioControl.h"
#include "Actuator.h"

AudioControl::AudioControl() : m_jsonHandler(JsonHandler::GetInstance())
{
}

AudioControl::~AudioControl()
{
}

void AudioControl::SendAudio(std::string strWarn, std::string strAudioInfo, std::string strViewStatus)
{
    std::string strDataSend = m_jsonHandler->GetWarnSendData(strWarn, strAudioInfo, strViewStatus);

    DIAG_INFO("AudioControl SendAudio:[%s]\n", strDataSend.c_str());
    Actuator::GetInstance()->SendAudioWarnInfo(strDataSend);
}
