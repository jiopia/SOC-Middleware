#include <cstdlib>
#include <signal.h>
#include "Util.h"
#include "Actuator.h"

std::string strComponentName = "ViewService";

void CatchSignal(int signal)
{
	switch (signal)
	{
	case SIGABRT:
		DebugPrint("catch ViewService signal SIGABRT\n");
		break;
	case SIGALRM:
		DebugPrint("catch ViewService signal SIGALRM\n");
		break;
	case SIGFPE:
		DebugPrint("catch ViewService signal SIGFPE\n");
		break;
	case SIGPIPE:
		DebugPrint("catch ViewService signal SIGPIPE\n");
		break;
	case SIGINT:
		DebugPrint("catch ViewService signal SIGINT\n");
		break;
	case SIGHUP:
		DebugPrint("catch ViewService signal SIGHUP\n");
		break;
	case SIGILL:
		DebugPrint("catch ViewService signal SIGILL\n");
		break;
	case SIGQUIT:
		DebugPrint("catch ViewService signal SIGQUIT\n");
		break;
	case SIGSEGV:
		DebugPrint("catch ViewService signal SIGSEGV\n");
		break;
	case SIGTERM:
		DebugPrint("catch ViewService signal SIGTERM\n");
		break;
	case SIGUSR1:
		DebugPrint("catch ViewService signal SIGUSR1\n");
		break;
	case SIGUSR2:
		DebugPrint("catch ViewService signal SIGUSR2\n");
		break;
	}
}

void SignalInit()
{
	signal(SIGABRT, CatchSignal);
	signal(SIGALRM, CatchSignal);
	signal(SIGFPE, CatchSignal);
	signal(SIGPIPE, CatchSignal);
	signal(SIGINT, CatchSignal);
	signal(SIGHUP, CatchSignal);
	signal(SIGILL, CatchSignal);
	signal(SIGQUIT, CatchSignal);
	signal(SIGSEGV, CatchSignal);
	signal(SIGUSR1, CatchSignal);
	signal(SIGUSR2, CatchSignal);
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		return EXIT_FAILURE;
	}
	
	// SignalInit();
	setvbuf(stdout, NULL, _IOLBF, 0);
	std::string fileName(argv[1]);
	XmlManager::GetInstance()->XmlParse(fileName);
	Actuator::GetInstance()->start();

	while (1)
	{
		usleep(-1);
	}

	return EXIT_SUCCESS;
}
