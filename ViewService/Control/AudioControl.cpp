#include "AudioControl.h"
#include "Actuator.h"

AudioControl::AudioControl() : m_jsonHandler(JsonHandler::GetInstance())
{
}

AudioControl::~AudioControl()
{
}

void AudioControl::SendAudio(std::string strWarn, std::string strAudioInfo, VIEW_STATUS viewStatus)
{
    char *sendData = m_jsonHandler->GetAudioSendData(strWarn, strAudioInfo, viewStatus);

    InfoPrint("AudioControl SendAudio:[%s]\r\n", sendData);

    BDSTAR_FREE(sendData);
}
