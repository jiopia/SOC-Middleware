#ifndef SOUNDPLAY_H_
#define SOUNDPLAY_H_

#include "Util.h"
#include "MsgDataDefine.h"

#define MEDIA_PATH "/etc/media/"

enum AudioPlayStatus
{
	AUDIO_STOP = 0,
	AUDIO_START = 1
};

class SoudPlay
{
public:
	SoudPlay();
	virtual ~SoudPlay();

	bool init();
	void playWarning(std::string strNewChimeId, int iLoop, std::string strFreq);
	void AudioPlay(std::string audioFileName);
	void stopPlay();
	int getCurLeftNum();

	AudioPlayStatus GetAudioPlayThreadStatus();
	void SetAudioPlayThreadStatus(AudioPlayStatus status);

	void UpdataVehicalAccStatus(SoundSwitch status);
	VehicleAccStatus GetVehicalAccStatus();

private:
	void AudioPlayThread(std::string filename, int iLoop, std::string strFreq);

	int m_iTimercount = 0;

	bool m_isAudioPlayThExit = true;

	std::mutex m_audioPlayStatusMtx;
	AudioPlayStatus m_audioPlayStatus = AUDIO_STOP;

	std::mutex m_mtxAccStatus;
	VehicleAccStatus m_vehicleStatus = VEHICLE_DEFAULT;
};

#endif /* SOUNDPLAY_H_ */
