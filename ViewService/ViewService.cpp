#include <cstdlib>
#include <signal.h>
#include "Util.h"
#include "Actuator.h"

char MODULE_TAG[16] = {0};
std::string strComponentName = "ViewService";
bool g_debugFlag = true;

int main(int argc, char *argv[])
{
	memset(MODULE_TAG, 0, sizeof(MODULE_TAG));
	sprintf(MODULE_TAG, "%s", strComponentName.c_str());

	DIAG_OPENLOG(strComponentName.c_str(), LOG_CONS | LOG_PID, LOG_USER);
	DIAG_INFO("Module[%s] compiled time:%s %s\n", strComponentName.c_str(), __DATE__, __TIME__);

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
