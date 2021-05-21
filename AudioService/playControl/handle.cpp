#include "handle.h"

handle::handle() : m_pJsonHandler(JsonHandler::GetInstance())
{
	std::vector<std::string> strTopicList;
	strTopicList.emplace_back(std::string(MQTT_TOPIC_WARN_AUDIO));

	m_mqttClient = std::make_shared<MqttConnection>(MQTT_HOST, MQTT_PORT, BDSTAR_SOCKET_MQTT);
	m_mqttClient->Subscribe(strTopicList);
	m_mqttClient->Run();

	m_pPlayControl = new PlayControl();
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
			InfoPrint("Recieve Warning Message From Mqtt:[%s]\r\n", msgData.strMsg.c_str());
			m_pJsonHandler->WarnMsgParse(msgData.strMsg, strWarn, strWarnName, strStatus);
			DebugPrint("KeyName:[%s], OnOff:[%s]\r\n", strWarnName.c_str(), strStatus.c_str());

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
