#include <cstdlib>
#include <signal.h>
#include "Util.h"
#include "Actuator.h"

std::string strComponentName = "ViewService";
bool g_debugFlag = true;

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		return EXIT_FAILURE;
	}

	std::string fileName(argv[1]);
	XmlManager::GetInstance()->XmlParse(fileName);
	Actuator::GetInstance()->Start();

	while (1)
	{
		usleep(-1);
	}

	return EXIT_SUCCESS;
}
