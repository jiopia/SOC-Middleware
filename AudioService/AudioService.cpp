#include <iostream>
#include "handle.h"
#include "Util.h"

char MODULE_TAG[16] = {0};
std::string strComponentName = "AudioService";
bool g_debugFlag = true;

int main(int argc, char *argv[])
{
	memset(MODULE_TAG, 0, sizeof(MODULE_TAG));
	sprintf(MODULE_TAG, "%s", strComponentName.c_str());

	DIAG_OPENLOG(strComponentName.c_str(), LOG_CONS | LOG_PID, LOG_USER);
	DIAG_INFO("Module[%s] compiled time:%s %s\n", strComponentName.c_str(), __DATE__, __TIME__);

	if (argc != 2)
	{
		return -1;
	}

	handle *pHandle = new handle();
	std::string strFilePath = argv[1];
	pHandle->loadFile(strFilePath);
	pHandle->Start();

	while (1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}

	return 0;
}
