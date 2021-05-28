#include <iostream>
#include "Util.h"
#include "CoreMsgHandler.hpp"

std::string strComponentName = "CoreProcessor";
bool g_debugFlag = true;

int main(int argc, char *argv[])
{
	InfoPrint("Module[%s] compiled time:%s %s\n", strComponentName.c_str(), __DATE__, __TIME__);

	CoreMsgHandler::GetInstance()->Start();

	while (1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return 0;
}
