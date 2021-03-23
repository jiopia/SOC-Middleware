#include <cstdlib>
#include <iostream>
#include "Util.h"
#include "CoreMsgHandler.hpp"

std::string strComponentName = "CoreProcessor";

int main(int argc, char *argv[])
{
	CoreMsgHandler::GetInstance()->start();

	return EXIT_SUCCESS;
}
