#include "sound.h"

Sound::Sound()
{
}

Sound::~Sound()
{
}

void Sound::setName(std::string &strName)
{
	m_strName = strName;
}

std::string Sound::getName() const
{
	return m_strName;
}

void Sound::setChimeId(std::string &strChimeId)
{
	m_strChimeId = strChimeId;
}

std::string Sound::getChimeId() const
{
	return m_strChimeId;
}

void Sound::setFreq(std::string &strFreq)
{
	m_strFreq = strFreq;
}

std::string Sound::getFreq() const
{
	return m_strFreq;
}

void Sound::setPriority(int iPriority)
{
	m_iPriority = iPriority;
}

int Sound::getPriority() const
{
	return m_iPriority;
}

void Sound::setPowerMode(std::string &strPowerMode)
{
	m_strPowerMode = strPowerMode;
}

std::string Sound::getPowerMode() const
{
	return m_strPowerMode;
}

void Sound::setLoop(int iLoop)
{
	m_iLoop = iLoop;
}

int Sound::getLoop() const
{
	return m_iLoop;
}

void Sound::setPlayMode(std::string &strPlayMode)
{
	m_strPlayMode = strPlayMode;
}

std::string Sound::getPlayMode() const
{
	return m_strPlayMode;
}

void Sound::setStopMode(std::string &strStopMode)
{
	m_strStopMode = strStopMode;
}

std::string Sound::getStopMode() const
{
	return m_strStopMode;
}

void Sound::setSelfCheck(bool &bSelfCheck)
{
	m_bSelfCheck = bSelfCheck;
}

bool Sound::getSelfCheck() const
{
	return m_bSelfCheck;
}

void Sound::setOnTime(int iOnTime)
{
	m_iOnTime = iOnTime;
}
int Sound::getOnTime() const
{
	return m_iOnTime;
}
void Sound::setOnInterrupt(bool &bOnInterrupt)
{
	m_bOnInterrupt = bOnInterrupt;
}
bool Sound::getOnInterrupt() const
{
	return m_bOnInterrupt;
}

void Sound::setSoundSwitch(SoundSwitch soundSwitch)
{
	m_soundSwitch = soundSwitch;
}

SoundSwitch Sound::getSoundSwitch() const
{
	return m_soundSwitch;
}

void Sound::setLeftPlayNum(int iLeftPlayNum)
{
	m_iLeftPlayNum = iLeftPlayNum;
}

int Sound::getLeftPlayNum() const
{
	return m_iLeftPlayNum;
}
