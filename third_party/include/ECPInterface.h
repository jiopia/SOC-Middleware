#include <memory>
#include <thread>
#include <vector>
#include <linux/input.h>

using namespace std;

/*length = sizeof(MsgType) + sizeof(MsgID) + RawData.size()*/
struct ECPVehicleValue
{
    short length;
    uint32_t MsgType;
    uint32_t MsgID;
    vector<uint8_t> RawData; //data
};

enum ECPCallBackType
{
    ECP_TYPE_MCU,
    ECP_TYPE_ANDROID,
    ECP_TYPE_MAX
};

//MCU定义的协议：
#define GET_PROPDATA 0X21411030
#define SET_REPORT_PERIOD 0X21411031

typedef void (*ReadDataCallBack)(const ECPVehicleValue &rData);

class ECPInterface
{
public:
    ECPInterface(std::unique_ptr<ECPComm> comm)
        : mExit(false),
          mComm(comm.release()),
          mThread{&ECPInterface::rxThread, this}
    {
        /*set get all data from mcu*/
        //vector<uint8_t> nVals(0);
        //ECPVehicleValue InitData = {.length = (short)(sizeof(InitData.MsgType) + sizeof(InitData.MsgID) + nVals.size()), .MsgType = GET_PROPDATA, .MsgID = 0, .RawData = nVals} ;
        //setValue(InitData, ECP_TYPE_MCU);
    }
    virtual ~ECPInterface();
    void setMcuValue(const ECPVehicleValue &Value); //write data.....
    void setAndroidValue(const ECPVehicleValue &Value);
    void rxThread();
    void registerGetDataCallBackFunc(ReadDataCallBack CallBackFunc);

private:
    void hex_array_printf(int loglevel, const char *func, const char *prefix, unsigned char *value, uint32_t size);
    void parseRxECPBuf(std::vector<uint8_t> &msg);
    void parseMcuData(unsigned char *buff, uint32_t size);
    void parseAndroidData(unsigned char *buff, uint32_t size);

private:
    std::atomic<bool> mExit{false};
    std::unique_ptr<ECPComm> mComm;
    std::atomic<bool> isEcpSupport{true};
    ReadDataCallBack ECPGetDataCallBackFunction; //callback function
    std::thread mThread;
};
