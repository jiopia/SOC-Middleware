#ifndef SOUND_H_
#define SOUND_H_
#include <string>
#include <iostream>

using namespace std;
enum SoundSwitch
{
	SOUND_OFF,
	SOUND_ON
};

class Sound
{
public:
	Sound();
	virtual ~Sound();

	void setName(std::string &strName);
	std::string getName() const;
	void setChimeId(std::string &strChimeId);
	std::string getChimeId() const;
	void setFreq(std::string &strFreq);
	std::string getFreq() const;
	void setPriority(int iPriority);
	int getPriority() const;
	void setPowerMode(std::string &strPowerMode);
	std::string getPowerMode() const;
	void setLoop(int iLoop);
	int getLoop() const;
	void setPlayMode(std::string &strPlayMode);
	std::string getPlayMode() const;
	void setStopMode(std::string &strStopMode);
	std::string getStopMode() const;
	void setSelfCheck(bool &bSelfCheck);
	bool getSelfCheck() const;
	void setOnTime(int iOnTime);
	int getOnTime() const;
	void setOnInterrupt(bool &bOnInterrupt);
	bool getOnInterrupt() const;

	void setSoundSwitch(SoundSwitch soundSwitch);
	SoundSwitch getSoundSwitch() const;
	void setLeftPlayNum(int iLeftPlayNum);
	int getLeftPlayNum() const;

	bool operator<(const Sound &sound) const
	{
		return m_iPriority < sound.m_iPriority;
	}

	std::string m_strName = "";
	std::string m_strChimeId = "";
	std::string m_strFreq = "";
	int m_iPriority = -1;
	std::string m_strPowerMode = "";
	int m_iLoop = -1;
	std::string m_strPlayMode = "";
	std::string m_strStopMode = "";
	bool m_bSelfCheck = false;
	int m_iOnTime = -1;
	bool m_bOnInterrupt = true;
	std::string m_strRemark = "";

	SoundSwitch m_soundSwitch = SOUND_OFF;
	int m_iLeftPlayNum = 0xFF;
};

inline bool my_sort_sound(const Sound *a1, const Sound *a2)
{
	return a1->m_iPriority < a2->m_iPriority;
}

#endif /* SOUND_H_ */
