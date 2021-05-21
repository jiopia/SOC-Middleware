#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <mutex>
#include <atomic>
#include <vector>
using namespace std;

#define ONE_FRAME_BUFFER_SIZE 2 * 1024

class ECPComm
{
public:
    ECPComm();
    virtual ~ECPComm();

    std::vector<uint8_t> read();

    int poll();

    void stop();

    int write(const std::vector<uint8_t> &data);

private:
    std::vector<uint8_t> rx_msg;
    std::atomic<int> mExit;
    std::mutex mMutex;
    int mSockFd;
    int mECPFd;
};
