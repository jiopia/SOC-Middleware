#include "handle.h"

handle::handle() : m_pJsonHandler(JsonHandler::GetInstance())
{
	std::vector<std::string> strTopicList;
	strTopicList.emplace_back(std::string(MQTT_TOPIC_WARN_AUDIO));

	m_mqttClient = std::make_shared<MqttConnection>(MQTT_HOST, MQTT_PORT, BDSTAR_SOCKET_MQTT);
	m_mqttClient->Subscribe(strTopicList);
	m_mqttClient->Run();

	m_pPlayControl = new PlayControl();

	std::thread audioCtrlCheckTh(std::bind(&handle::AudioCtrlCheckThread, this));
	audioCtrlCheckTh.detach();
}

handle::~handle()
{
	if (m_pPlayControl != NULL)
	{
		delete m_pPlayControl;
		m_pPlayControl = NULL;
	}
}

void handle::loadFile(std::string strFilePath)
{
	if (m_pPlayControl != NULL)
	{
		m_pPlayControl->loadFile(strFilePath.c_str());
	}
}

void handle::Run()
{
	m_pPlayControl->Start();

	std::string strWarn, strWarnName, strStatus;
	while (1)
	{
		MsgData msgData = Forwarder::GetInstance()->MsgPop();
		if (!msgData.Empty())
		{
			DIAG_INFO("Recieve Warning Message From Mqtt:[%s]\n", msgData.strMsg.c_str());
			m_pJsonHandler->WarnMsgParse(msgData.strMsg, strWarn, strWarnName, strStatus);
			DIAG_INFO("KeyName:[%s], OnOff:[%s]\n", strWarnName.c_str(), strStatus.c_str());

			handleLogic(strWarnName, strStatus);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void handle::handleLogic(std::string strName, std::string strOnOff)
{
	SoundSwitch soundSwitch;
	if ("ON" == strOnOff)
	{
		soundSwitch = SOUND_ON;
	}
	else if ("OFF" == strOnOff)
	{
		soundSwitch = SOUND_OFF;
	}
	else
	{
		return;
	}

	if (m_pPlayControl != NULL)
	{
		m_pPlayControl->pushSound(strName, soundSwitch);
	}
}

void handle::AudioCtrlCheckThread()
{
	while (1)
	{
		if (m_pPlayControl->GetAudioCtrlAdscription() != AUDIO_CTRL_BELONG_TO_DHU)
		{
			m_mqttClient->MsgSend(std::string(MQTT_TOPIC_WARN_AUDIO_TO_MW), "audio_ctrl_mode_req");
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}