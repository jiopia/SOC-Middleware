#include "playControl.h"

extern "C"
{
#include "dhu_audio.h"
}

PlayControl::PlayControl()
{
	pthread_mutex_init(&m_listSoundLock, NULL);
}

PlayControl::~PlayControl()
{
	pthread_mutex_destroy(&m_listSoundLock);
	if (m_pReadFile != NULL)
	{
		delete m_pReadFile;
		m_pReadFile = NULL;
	}
}

void PlayControl::init()
{
}

void PlayControl::loadFile(std::string strFilePath)
{
	m_pReadFile = new readXmlFile(strFilePath, m_mapConfig);
}

void PlayControl::pushSound(std::string strName, SoundSwitch iOnOff)
{
	pthread_mutex_lock(&m_listSoundLock);
	list<Sound *>::iterator iter;
	for (iter = m_listSound.begin(); iter != m_listSound.end(); iter++)
	{
		if ((*iter)->m_strName == strName)
		{
			{
				(*iter)->m_soundSwitch = iOnOff;
			}

			break;
		}
	}
	if (iter == m_listSound.end())
	{
		if (SOUND_OFF == iOnOff)
		{
			pthread_mutex_unlock(&m_listSoundLock);
			return;
		}

		Sound *pSound(NULL);
		map<std::string, Sound *>::iterator iterMap = m_mapConfig.find(strName);
		if (iterMap != m_mapConfig.end())
		{
			pSound = iterMap->second;
		}
		if (pSound != NULL)
		{
			pSound->m_soundSwitch = iOnOff;

			if (m_pCurSound != NULL && (pSound->getPriority() > m_pCurSound->getPriority()) && ("turn_on" == strName || "turn_off" == strName))
			{
			}
			else
			{
				m_listSound.push_back(pSound);
			}
		}
	}

	m_listSound.sort(my_sort_sound);
	pthread_mutex_unlock(&m_listSoundLock);
}

void PlayControl::getOneSound(Sound *&pSound)
{
	pSound = NULL;
	pthread_mutex_lock(&m_listSoundLock);
	if (!m_listSound.empty())
	{

		// for (list<Sound *>::iterator iter = m_listSound.begin(); iter != m_listSound.end(); iter++)
		// {
		// 	if ("wm1" == (*iter)->getPowerMode() ||
		// 		"wm2" == (*iter)->getPowerMode() ||
		// 		"wm12" == (*iter)->getPowerMode())
		// 	{
		// 		pSound = (*iter);
		// 		break;
		// 	}
		// }
		pSound = m_listSound.front();
	}
	pthread_mutex_unlock(&m_listSoundLock);
}

void PlayControl::removeOneSound(Sound *pSound)
{
	if (pSound == NULL)
	{
		return;
	}

	pthread_mutex_lock(&m_listSoundLock);
	m_listSound.remove(pSound);
	pthread_mutex_unlock(&m_listSoundLock);
}

void PlayControl::printAll(std::string strLog)
{
	pthread_mutex_lock(&m_listSoundLock);
	std::string strAllDialogInfo(strLog);
	for (list<Sound *>::iterator iter = m_listSound.begin(); iter != m_listSound.end(); iter++)
	{
		strAllDialogInfo += " name:";
		strAllDialogInfo += (*iter)->m_strName;
		strAllDialogInfo += " priority:";
		char tmp[10];
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%d", (*iter)->m_iPriority);
		strAllDialogInfo += tmp;
		strAllDialogInfo += " status:";
		strAllDialogInfo += (*iter)->m_soundSwitch == 0 ? "OFF" : "ON";
	}

	InfoPrint("%s\n", strAllDialogInfo.c_str());
	pthread_mutex_unlock(&m_listSoundLock);
}

void PlayControl::handleNode()
{
	Sound *pSound(NULL);
	this->getOneSound(pSound);
	if (pSound != NULL)
	{
		if (m_pCurSound == NULL)
		{
			if (pSound->getSoundSwitch() == SOUND_OFF)
			{
				this->removeOneSound(pSound);
				m_pCurSound = NULL;
				m_SoudPlay.stopPlay();
			}
			else if (pSound->getSoundSwitch() == SOUND_ON)
			{
				if (pSound->getLeftPlayNum() > 0 && pSound->getLeftPlayNum() != 0xFF && pSound->getPlayMode() == "continue")
				{
					pSound->setLoop(pSound->getLeftPlayNum());
				}
				m_SoudPlay.stopPlay();
				m_SoudPlay.playWarning(pSound->getChimeId(), pSound->getLoop(), pSound->getFreq());
				m_pCurSound = pSound;
			}
		}
		else
		{
			if (pSound->getName() == m_pCurSound->getName())
			{
				if (pSound->getSoundSwitch() == SOUND_OFF)
				{
					this->removeOneSound(pSound);
					m_pCurSound = NULL;
					m_SoudPlay.stopPlay();
				}
				else if (pSound->getSoundSwitch() == SOUND_ON)
				{
					if (m_SoudPlay.GetAudioPlayThreadStatus() == AUDIO_STOP)
					{
						m_SoudPlay.playWarning(pSound->getChimeId(), pSound->getLoop(), pSound->getFreq());
						m_pCurSound = pSound;
					}
				}
			}
			else
			{
				if (m_pCurSound->getPlayMode() == "replay")
				{
					this->removeOneSound(m_pCurSound);
					m_pCurSound = NULL;
					m_SoudPlay.stopPlay();
				}
				else if (m_pCurSound->getPlayMode() == "continue")
				{
					m_pCurSound->setLeftPlayNum(m_SoudPlay.getCurLeftNum());
				}
				m_SoudPlay.stopPlay();
				m_SoudPlay.playWarning(pSound->getChimeId(), pSound->getLoop(), pSound->getFreq());
				m_pCurSound = pSound;
			}
		}
	}
}

void PlayControl::Run()
{
	while (1)
	{
		handleNode();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		// m_SoudPlay.playWarning("warning.wav", 0, "", "");
		// std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}
