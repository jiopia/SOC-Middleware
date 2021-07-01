#ifndef PLAYCONTROL_H_
#define PLAYCONTROL_H_

#include <string>
#include <map>
#include <list>
#include "readXmlFile.h"
#include "soundplay.h"
#include "MqttConnection.hpp"
#include "MsgDataDefine.h"
#include "Util.h"
#include "Thread.h"

//
class PlayControl : public Thread
{
public:
	PlayControl();
	virtual ~PlayControl();

	void init();

	void loadFile(std::string strFilePath);

	void pushSound(std::string strName, SoundSwitch iOnOff);

	void getOneSound(Sound *&pSound);

	void removeOneSound(Sound *pSound);

	void printAll(std::string strLog);

	void handleNode();

	void Run();

	AudioCtrlAdscription GetAudioCtrlAdscription();

private:

	SoudPlay *m_pSoudPlay = NULL;

	readXmlFile *m_pReadFile = NULL;

	map<std::string, Sound *> m_mapConfig;

	pthread_mutex_t m_listSoundLock;
	list<Sound *> m_listSound;
	Sound *m_pCurSound = NULL;

	std::shared_ptr<BaseConnection> m_mqttClient = NULL;
};

#endif /* PLAYCONTROL_H_ */
