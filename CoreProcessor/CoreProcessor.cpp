#include <iostream>
#include "Util.h"
#include "CoreMsgHandler.hpp"

std::string strComponentName = "CoreProcessor";
bool g_debugFlag = true;

int main(int argc, char *argv[])
{
	CoreMsgHandler::GetInstance()->Start();

	while (1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return 0;
}
