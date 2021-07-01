#include "playControl.h"

extern "C"
{
#include "dhu_audio.h"
}

PlayControl::PlayControl()
{
	m_pSoudPlay = new SoudPlay();
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

	if (m_pSoudPlay != NULL)
	{
		delete m_pSoudPlay;
		m_pSoudPlay = NULL;
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
	if (strName == "ACC")
	{
		m_pSoudPlay->UpdataVehicalAccStatus(iOnOff);
		return;
	}
	else if (strName == "audio_belong2dhu")
	{
		m_pSoudPlay->UpdataVehicalAccStatus(iOnOff);
		return;
	}

	auto iterFind = m_mapConfig.find(strName);
	if (iterFind == m_mapConfig.end() ||
		iterFind->second == NULL)
	{
		return;
	}

	pthread_mutex_lock(&m_listSoundLock);

	/* 对转向灯逻辑进行特殊处理 */
	for (auto iterList = m_listSound.begin(); iterList != m_listSound.end(); iterList++)
	{
		if ("turn_on" == strName || "turn_off" == strName)
		{
			if (((*iterList)->m_strName != strName) &&
				((*iterList)->m_strName == "turn_on" || (*iterList)->m_strName == "turn_off"))
			{
				(*iterList)->m_soundSwitch = SOUND_OFF;
			}
		}
		else if ((*iterList)->m_strName == "turn_on" || (*iterList)->m_strName == "turn_off")
		{
			Sound *pSound = iterFind->second;
			if (pSound != NULL && pSound->getPriority() < (*iterList)->m_iPriority)
			{
				(*iterList)->m_soundSwitch = SOUND_OFF;
			}
		}
	}

	list<Sound *>::iterator iter;
	for (iter = m_listSound.begin(); iter != m_listSound.end(); iter++)
	{
		if ((*iter)->m_strName == strName)
		{
			(*iter)->m_soundSwitch = iOnOff;
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
		for (list<Sound *>::iterator iter = m_listSound.begin(); iter != m_listSound.end(); iter++)
		{
			if (m_pSoudPlay->GetVehicalAccStatus() == VEHICLE_ON)
			{
				if ("wm1" == (*iter)->getPowerMode() || "wm12" == (*iter)->getPowerMode())
				{
					pSound = (*iter);
					break;
				}
			}
			else if (m_pSoudPlay->GetVehicalAccStatus() == VEHICLE_OFF)
			{
				if ("wm2" == (*iter)->getPowerMode() || "wm12" == (*iter)->getPowerMode())
				{
					pSound = (*iter);
					break;
				}
			}
		}
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

	DIAG_INFO("%s\n", strAllDialogInfo.c_str());
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
				m_pSoudPlay->stopPlay();
			}
			else if (pSound->getSoundSwitch() == SOUND_ON)
			{
				if (pSound->getLeftPlayNum() > 0 && pSound->getLeftPlayNum() != 0xFF && pSound->getPlayMode() == "continue")
				{
					pSound->setLoop(pSound->getLeftPlayNum());
				}
				m_pSoudPlay->stopPlay();
				m_pSoudPlay->playWarning(pSound->getChimeId(), pSound->getLoop(), pSound->getFreq());
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
					m_pSoudPlay->stopPlay();
				}
				else if (pSound->getSoundSwitch() == SOUND_ON)
				{
					if (m_pSoudPlay->GetAudioPlayThreadStatus() == AUDIO_STOP)
					{
						this->removeOneSound(pSound);
						m_pCurSound = NULL;
					}
				}
			}
			else
			{
				if (m_pCurSound->getPlayMode() == "replay")
				{
					this->removeOneSound(m_pCurSound);
					m_pCurSound = NULL;
					m_pSoudPlay->stopPlay();
				}
				else if (m_pCurSound->getPlayMode() == "continue")
				{
					m_pCurSound->setLeftPlayNum(m_pSoudPlay->getCurLeftNum());
				}
				m_pSoudPlay->stopPlay();
				m_pSoudPlay->playWarning(pSound->getChimeId(), pSound->getLoop(), pSound->getFreq());
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
		// m_pSoudPlay->playWarning("warning.wav", 0, "", "");
		// std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}

AudioCtrlAdscription PlayControl::GetAudioCtrlAdscription()
{
	return m_pSoudPlay->GetAudioCtrlAdscription();
}