#include "soundplay.h"
#include "Util.h"

extern "C"
{
#include "dhu_audio.h"
}

SoudPlay::SoudPlay()
{

}

SoudPlay::~SoudPlay()
{
	stopPlayMedia();
}

void SoudPlay::playWarning(std::string filename, int iLoop, std::string strFreq)
{
	DIAG_INFO("AudioName:[%s], iLoop:[%d], Freq:[%s]", filename.c_str(), iLoop, strFreq.c_str());
	if (GetAudioPlayThreadStatus() == AUDIO_STOP)
	{
		while (!m_isAudioPlayThExit)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		SetAudioPlayThreadStatus(AUDIO_START);
		m_isAudioPlayThExit = false;
		std::thread audioPlayTh(std::bind(&SoudPlay::AudioPlayThread, this, filename, iLoop, strFreq));
		audioPlayTh.detach();
	}
}

void SoudPlay::AudioPlay(std::string audioFileName)
{
	audioFileName = std::string(MEDIA_PATH) + audioFileName;

	// DIAG_INFO("AudioName:%s", audioFileName.c_str());

	// std::string strAudioPlayCmd = std::string("/bin/audioplay ") + audioFileName;
	// system(strAudioPlayCmd.c_str());

	if (getPlayerState() != 0)
	{
		stopPlayMedia();
	}

	char fileName[64] = {0};
	sprintf(fileName, "%s", audioFileName.c_str());
	DIAG_INFO("fileName:%s", fileName);
	startPlayMedia(fileName);
}

void SoudPlay::stopPlay()
{
	SetAudioPlayThreadStatus(AUDIO_STOP);
}

int SoudPlay::getCurLeftNum()
{
	return 0;
}

AudioPlayStatus SoudPlay::GetAudioPlayThreadStatus()
{
	std::lock_guard<std::mutex> lockGuard(m_audioPlayStatusMtx);
	return m_audioPlayStatus;
}

void SoudPlay::SetAudioPlayThreadStatus(AudioPlayStatus status)
{
	std::lock_guard<std::mutex> lockGuard(m_audioPlayStatusMtx);
	m_audioPlayStatus = status;
}

void SoudPlay::UpdataVehicalAccStatus(SoundSwitch status)
{
	std::lock_guard<std::mutex> lockGuard(m_mtxAccStatus);
	if (status == SOUND_ON)
	{
		setVolume(4);
	}

	m_vehicleStatus = (status == SOUND_OFF) ? VEHICLE_OFF : VEHICLE_ON;
}

VehicleAccStatus SoudPlay::GetVehicalAccStatus()
{
	std::lock_guard<std::mutex> lockGuard(m_mtxAccStatus);
	return m_vehicleStatus;
}

void SoudPlay::UpdataAudioCtrlAdscription(SoundSwitch status)
{
	std::lock_guard<std::mutex> lockGuard(m_mtxAudioCtrl);
	if (status == SOUND_ON)
	{
		m_audioCtrlAdscription = AUDIO_CTRL_BELONG_TO_DHU;
	}
	else if (status == SOUND_OFF)
	{
		m_audioCtrlAdscription = AUDIO_CTRL_BELONG_TO_MCU;
	}
}

AudioCtrlAdscription SoudPlay::GetAudioCtrlAdscription()
{
	std::lock_guard<std::mutex> lockGuard(m_mtxAudioCtrl);
	return m_audioCtrlAdscription;
}

void SoudPlay::AudioPlayThread(std::string filename, int iLoop, std::string strFreq)
{
	DIAG_INFO("Thread Id:[%lu]\n", std::hash<std::thread::id>()(std::this_thread::get_id()));

	int iDelayMs = 1000;
	int iToleranceMs = 0;
	uint32_t iIntervalTime;
	if ("1HZ" == strFreq)
	{
		iDelayMs = 1000 - iToleranceMs;
	}
	else if ("2HZ" == strFreq)
	{
		iDelayMs = 500 - iToleranceMs;
	}
	else if ("2.5HZ" == strFreq)
	{
		iDelayMs = 400 - iToleranceMs;
	}
	else if ("4HZ" == strFreq)
	{
		iDelayMs = 250 - iToleranceMs;
	}
	else if ("8HZ" == strFreq)
	{
		iDelayMs = 125 - iToleranceMs;
	}

	while (iLoop-- &&
		   GetAudioPlayThreadStatus() == AUDIO_START &&
		   GetAudioCtrlAdscription() == AUDIO_CTRL_BELONG_TO_DHU)
	{
		AudioPlay(filename);
		std::this_thread::sleep_for(std::chrono::milliseconds(iDelayMs));
	}

	m_isAudioPlayThExit = true;
	SetAudioPlayThreadStatus(AUDIO_STOP);
}
