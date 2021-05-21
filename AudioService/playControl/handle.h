#ifndef HANDLE_H_
#define HANDLE_H_

#include "Thread.h"
#include "playControl.h"
#include "JsonHandler.h"

class handle : public Thread
{
public:
	handle();

	~handle();

	void loadFile(std::string strFilePath);

	void Run();

private:
	void handleLogic(std::string strName, std::string strOnOff);

	JsonHandler *m_pJsonHandler = NULL;

	PlayControl *m_pPlayControl = NULL;

	std::shared_ptr<BaseConnection> m_mqttClient = NULL;
};

#endif /* HANDLE_H_ */
