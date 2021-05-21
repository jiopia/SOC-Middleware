#include <iostream>
#include "handle.h"
#include "Util.h"

std::string strComponentName = "AudioService";
bool g_debugFlag = true;

int main(int argc, char *argv[])
{
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
