#include <iostream>
#include "Util.h"
#include "CoreMsgHandler.hpp"

char MODULE_TAG[16] = {0};
std::string strComponentName = "CoreProcessor";
bool g_debugFlag = true;

int main(int argc, char *argv[])
{
	memset(MODULE_TAG, 0, sizeof(MODULE_TAG));
	sprintf(MODULE_TAG, "%s", strComponentName.c_str());

	DIAG_OPENLOG(strComponentName.c_str(), LOG_CONS | LOG_PID, LOG_USER);
	InfoPrint("Module[%s] compiled time:%s %s\n", strComponentName.c_str(), __DATE__, __TIME__);

	CoreMsgHandler::GetInstance()->Start();

	while (1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return 0;
}
