/*
    根据E02_BD_B3CC协议5.5编写的头文件
    中间件发送给HMI的信息定义
*/
#ifndef _DATA_TO_HMI_DEFINE_H_
#define _DATA_TO_HMI_DEFINE_H_

#include <string.h>

#define SPI_DATA_MAX_LENGTH 128

const float g_fSppedDoorOpenWarn = 9.50;
const float g_fSpeedOverLimitAlarm = 119.50;

struct IPCData
{
    unsigned char m_msgType;
    unsigned char m_msgID;
    unsigned char m_msgLength;
    unsigned char m_msgData[SPI_DATA_MAX_LENGTH];

    IPCData()
    {
        m_msgType = 0x00;
        m_msgID = 0x00;
        m_msgLength = 0x00;
        memset(m_msgData, 0, sizeof(m_msgData));
    }
};

enum HMI_MSG_TYPE
{
    HMI_MGS_TYPE_ODO = 0x01,
    HMI_MGS_TYPE_GUAGE = 0x02,
    HMI_MSG_TYPE_TELLTALE = 0x03,
    HMI_MSG_TYPE_WARNING = 0x04,
    HMI_MSG_TYPE_NAVI = 0x05,
    HMI_MSG_TYPE_SETTING = 0x06,
    HMI_MSG_TYPE_ECU = 0x07,
    HMI_MSG_TYPE_SETUP = 0x08,
    HMI_MSG_TYPE_KEY = 0x09,
    HMI_MSG_TYPE_DEBUG = 0x0A,
    HMI_MSG_TYPE_FAULT = 0x0B,
    HMI_MSG_TYPE_EOL = 0x0C
};

enum HMI_MGS_ODO_ID
{
    HMI_MGS_ODO_ID_GEAR = 0x01,
    HMI_MGS_ODO_ID_DRIVER_MODE = 0x02,
    HMI_MGS_ODO_ID_ODO = 0x03,
    HMI_MGS_ODO_ID_TRIP_A = 0x04,
    HMI_MGS_ODO_ID_TRIP_B = 0x5,
    HMI_MGS_ODO_ID_TIME = 0x06,
    HMI_MGS_ODO_ID_OUTSIDE_TEMPERATURE = 0x07,
    HMI_MGS_ODO_ID_GEAR_IND = 0x08
};

enum HMI_MGS_GUAGE_ID
{
    HMI_MGS_GUAGE_ID_SPEED = 0x01,        //车速
    HMI_MGS_GUAGE_ID_RPM,                 //转速
    HMI_MGS_GUAGE_ID_COOLANT_TEMPERATURE, //冷却液温度
    HMI_MGS_GUAGE_ID_FUEL,                //燃油量
    HMI_MGS_GUAGE_ID_VOLTAMETER,          //
    HMI_MGS_GUAGE_ID_POWERMETER,          //
    HMI_MGS_GUAGE_ID_CURISE_STATUS,       //
    HMI_MGS_GUAGE_ID_CURISE_LIMIT,        //
    HMI_MGS_GUAGE_ID_CURISE_SPEED         //
};

enum HMI_MGS_TELLTALE_ID
{
    HMI_MGS_TELLTALE_WARNING = 0x01, //报警信息
    HMI_MGS_TELLTALE_TURN_SIGNAL     //左右转向灯
};

enum HMI_MGS_SETUP_ID
{
    HMI_MGS_SETUP_ID_HMI_ADDR = 0x01,
    HMI_MGS_SETUP_ID_IGN_STATUS,
    HMI_MGS_SETUP_ID_HARDWARE_VERSION,
    HMI_MGS_SETUP_ID_SOFT_VERSION,
    HMI_MGS_SETUP_ID_MCU_VERSION,
    HMI_MGS_SETUP_ID_CFG_INFO,
    HMI_MGS_SETUP_ID_UNIT
};
enum HMI_MGS_NAVI_ID
{
    HMI_MGS_NAVI_ID_MUSIC_CLEAR = 0x05,
    HMI_MGS_NAVI_ID_MUSIC_POSITION,
    HMI_MGS_NAVI_ID_MUSIC_ALBUM,
    HMI_MGS_NAVI_ID_MUSIC_TITLE,
    HMI_MGS_NAVI_ID_MUSIC_ARTIST,
    HMI_MGS_NAVI_ID_PHONE_CLEAR,
    HMI_MGS_NAVI_ID_PHONE_STATUS,
    HMI_MGS_NAVI_ID_PHONE_NAME,
    HMI_MGS_NAVI_ID_PHONE_NUMBER,
    HMI_MGS_NAVI_ID_PHONE_ELAPSEDTIME,
    HMI_MGS_NAVI_ID_RADIO
};
enum HMI_MGS_SETTING_ID
{
    HMI_MGS_SET_ID_LANGUAGE = 0x01,
    HMI_MGS_SET_ID_HMISTATUS,
    HMI_MGS_SET_ID_RESET,
    HMI_MGS_SET_ID_THEME,
    HMI_MGS_SET_ID_OVERSPEED,
    HMI_MGS_SET_ID_BACKLIGHT,
    HMI_MGS_SET_ID_REST,
    HMI_MGS_SET_ID_RESETALL, //
    HMI_MGS_SET_ID_TIME
};
enum HMI_MGS_ECU_ID
{
    HMI_MGS_ECU_ID_INS_FUEL = 0x01,
    HMI_MGS_ECU_ID_AVE_FUEL,
    HMI_MGS_ECU_ID_ENDURANCE,
    HMI_MGS_ECU_ID_INS_ENERGYCONSUME,
    HMI_MGS_ECU_ID_AVE_ENERGYCONSUME,
    HMI_MGS_ECU_ID_CURRENT,
    HMI_MGS_ECU_ID_VOLTAGE
};
enum HMI_MGS_DEBUG_ID
{
    HMI_MGS_DEBUG_ID_SCREEN_TEMPER = 0x01,
    HMI_MGS_DEBUG_ID_FUEL
};
enum HMI_MGS_KEY_ID
{
    HMI_MGS_KEY_ID_KEY = 0x01,
    HMI_MGS_KEY_ID_REG
};
enum HMI_MGS_WARNING_ID
{
    HMI_MGS_WARNING_ID_OVERSPEED = 0x01,
    HMI_MGS_WARNING_ID_DOOR,
    HMI_MGS_WARNING_ID_NOWARN,
    HMI_MGS_WARNING_ID_BELT,
    HMI_MGS_WARNING_ID_RADOR_STATUS,
    HMI_MGS_WARNING_ID_RADOR_DATA,
    HMI_MGS_WARNING_ID_TIRE_PRESS,
    HMI_MGS_WARNING_ID_TIRE_PRESS_WARN,
    HMI_MGS_WARNING_ID_TIRE_TEMP,
    HMI_MGS_WARNING_ID_TIRE_TEMP_WARN,
    HMI_MGS_WARNING_ID_TIRE_SYS_WARN, //11
    HMI_MGS_WARNING_ID_COOLANT,
    HMI_MGS_WARNING_ID_WARMUP,
    HMI_MGS_WARNING_ID_FOLLOWME,
    HMI_MGS_WARNING_ID_OILPRESSLOW,
    HMI_MGS_WARNING_ID_AIRBAG,
    HMI_MGS_WARNING_ID_REST,
    HMI_MGS_WARNING_ID_MAINTENANCE,
    HMI_MGS_WARNING_ID_OILLOW,
    HMI_MGS_WARNING_ID_EPS_CHECK,
    HMI_MGS_WARNING_ID_EPS_INIT,
    HMI_MGS_WARNING_ID_GEARBOX_TEMP,
    HMI_MGS_WARNING_ID_GEARBOX_OVERHEAT,
    HMI_MGS_WARNING_ID_GEARBOX_CHECK,
    HMI_MGS_WARNING_ID_ESP,
    HMI_MGS_WARNING_ID_ABS,
    HMI_MGS_WARNING_ID_BRAKEFLUIDLOW,
    HMI_MGS_WARNING_ID_PEPS1,
    HMI_MGS_WARNING_ID_PEPS2,
    HMI_MGS_WARNING_ID_PEPS3,
    HMI_MGS_WARNING_ID_PEPS4,
    HMI_MGS_WARNING_ID_PEPS5,
    HMI_MGS_WARNING_ID_PEPS6,
    HMI_MGS_WARNING_ID_PEPS7,
    HMI_MGS_WARNING_ID_PEPS8,
    HMI_MGS_WARNING_ID_PEPS9,
    HMI_MGS_WARNING_ID_PEPS10,
    HMI_MGS_WARNING_ID_DOORINFO,
    HMI_MGS_WARNING_ID_BELT2,
    HMI_MGS_WARNING_ID_AVH1, //40
    HMI_MGS_WARNING_ID_AVH2,
    HMI_MGS_WARNING_ID_AVH3,
    HMI_MGS_WARNING_ID_CDP,
    HMI_MGS_WARNING_ID_EPB1,
    HMI_MGS_WARNING_ID_EPB2,
    HMI_MGS_WARNING_ID_EPB3,
    HMI_MGS_WARNING_ID_AVMSYS,
    HMI_MGS_WARNING_ID_AVM,
    HMI_MGS_WARNING_ID_DTC1,
    HMI_MGS_WARNING_ID_DTC2,
    HMI_MGS_WARNING_ID_DTC3,
    HMI_MGS_WARNING_ID_DTC4,
    HMI_MGS_WARNING_ID_DTC5,
    HMI_MGS_WARNING_ID_DTC6,
    HMI_MGS_WARNING_ID_DTC7,
    HMI_MGS_WARNING_ID_DTC8,
    HMI_MGS_WARNING_ID_DTC9,
    HMI_MGS_WARNING_ID_DTC10,
    HMI_MGS_WARNING_ID_DTC11,
    HMI_MGS_WARNING_ID_DTC12,
    HMI_MGS_WARNING_ID_DTC13,
    HMI_MGS_WARNING_ID_HDC1,
    HMI_MGS_WARNING_ID_HDC2,
    HMI_MGS_WARNING_ID_SPORT,
    HMI_MGS_WARNING_ID_PHONE,
    HMI_MGS_WARNING_ID_SYSFAULT,
    HMI_MGS_WARNING_ID_BATTFAULT,
    HMI_MGS_WARNING_ID_DRIVEFAULT,
    HMI_MGS_WARNING_ID_BATTDISCONNECT,
    HMI_MGS_WARNING_ID_SOCLOW,
    HMI_MGS_WARNING_ID_PARKING = 80,
    HMI_MGS_WARNING_ID_CHARGEFAULT = 83,
    HMI_MGS_WARNING_ID_GEARFAULT = 84,
    HMI_MGS_WARNING_ID_VACYYMPRESWARN = 85,
    HMI_MGS_WARNING_ID_VECHICLEFAULT = 86,
    HMI_MGS_WARNING_ID_HMIWARNING = 87,
    HMI_MGS_WARNING_ID_CHARGEDISPLAY,
};
enum HMI_MGS_EOL_ID
{
    HMI_MGS_EOL_ID_MODE = 0x01,
    HMI_MGS_EOL_ID_SHOW
};

#pragma pack(4)
typedef struct ODOData
{
    /*************  档位  *************/
    unsigned char gear : 8;
    /************* 驾驶模式 ************/
    unsigned char driveMode : 8;
    /************* 里程累计 ************/
    int totalMileage; // 0~999999km
    /************ 里程小计A ************/
    float mileageSubtotalA; // 0.0～9999.9km
    /************ 里程小计B ***********/
    float mileageSubtotalB; // 0.0～9999.9km
    /************ 环境温度 ************/
    // float temperatureInside;
    // float temperatureOutside;
    char temperatureInside[4];
    char temperatureOutside[4];
    /************ 换挡提醒 ************/
    unsigned char gearChangeRemind;
    /*********************************/
} ODOData_t;

typedef struct
{
    /*************  车速  *************/
    float vehicleSpeed; //0~260km/h
    /***********  发动机转速  **********/
    int engineSpeed; //0~8000r/min
    /********  发动机冷却液温度  ********/
    float coolantTemprature;
    /************  燃油量  *************/
    float oilFuelValue;
    unsigned char oilFuelVolumeValue;
    /*********************************/
} GaugeData_t;

typedef struct TelltaleData
{
    /* 报警信息 */
    struct WarnInfo
    {
        /************  Byte1 *************/
        struct Byte1
        {
            unsigned char reserved : 8;
        } byte1_t;
        /************  Byte2 *************/
        struct Byte2
        {
            unsigned char reserved : 8;
        } byte2_t;
        /************  Byte3 *************/
        struct Byte3
        {
            unsigned char driverBeltLight : 1;           //
            unsigned char passengerBeltLight : 1;        //
            unsigned char espLight : 1;                  //ESP
            unsigned char espOffLight : 1;               //ESP
            unsigned char absLight : 1;                  //ABS
            unsigned char doorOpenLight : 1;             //
            unsigned char epsLight : 1;                  //EPS
            unsigned char tirePressureAbnormalLight : 1; //TIRE
        } byte3_t;
        /************  Byte4 *************/
        struct Byte4
        {
            unsigned char gearBoxFaultLight : 1; //
            unsigned char epbRedLight : 1;       //EPB Green
            unsigned char epbGreenLight : 1;     //EPB Red
            unsigned char airBagFaultLight : 1;  //
            unsigned char hdcControlLight : 1;   //
            unsigned char avhRedLight : 1;       //AVH(AUTO HOLD) Red
            unsigned char avhGreenLight : 1;     //AVH(AUTO HOLD) Green
            unsigned char engineAntiTheft : 1;   //
        } byte4_t;
        /************  Byte5 *************/
        struct Byte5
        {
            unsigned char chargeFaultLight : 1;     //蓄电池充电故障
            unsigned char maintenanceIndicator : 1; //保养指示灯
            unsigned char ebdFaultLight : 1;        //EBD制动液位低
            unsigned char cruiseControlLight : 1;   //定速巡航
            unsigned char waterTempHigh : 1;        //水温高
            unsigned char oilFuelLowLight : 1;      //燃油低
            unsigned char reserved : 1;             //
            unsigned char gpfLight : 1;             //GPF
        } byte5_t;
        /************  Byte6 *************/
        struct Byte6
        {
            unsigned char hdcFailLight : 1;          //HDC故障
            unsigned char svsLight : 1;              //SVS发动机故障
            unsigned char oilPressureLow : 1;        //机油压力低
            unsigned char blindSpotMonitorNomal : 1; //盲区监测正常
            unsigned char blindSpotMonitorFault : 1; //盲区监测故障
            unsigned char reserved : 1;              //
            unsigned char epcLight : 1;              //EPC
            unsigned char engineEmissionFailure : 1; //发动机排放故障
        } byte6_t;
        /************  Byte7 *************/
        struct Byte7
        {
            unsigned char aebWorkLight : 1;   //AEB防撞系统工作指示灯
            unsigned char aebFaultLight : 1;  //AEB防撞系统故障
            unsigned char warmupLight : 1;    //暖机
            unsigned char speedLimit : 1;     //限速
            unsigned char ldwLightYellow : 1; //LDW车道偏离指示灯(黄)
            unsigned char ldwLightRed : 1;    //LDW车道偏离指示灯(红)
            unsigned char ldwLightGreen : 1;  //LDW车道偏离指示灯(绿)
            unsigned char reserved : 1;       //
        } byte7_t;
        /*********************************/
    } warn_info_t;

    /* 左右转向灯 */
    struct TurnLight
    {
        /************  Byte1 *************/
        struct Byte1
        {
            unsigned char turnLeftLight : 1;
            unsigned char turnRightLight : 1;
            unsigned char dangerLight : 1;
            unsigned char positionLight : 1;
            unsigned char highBeamLight : 1;
            unsigned char lowBeamLight : 1;
            unsigned char frontFogLight : 1;
            unsigned char backFogLight : 1;
        } byte1_t;
        /************  Byte2 *************/
        struct Byte2
        {
            unsigned char daytimeRunningLight : 1;
            unsigned char reserved : 7;
        } byte2_t;
        /*********************************/
    } turn_light_t;

} TelltaleData_t;

typedef struct ECUData
{
    /*************  瞬时油耗  *************/
    float instantFuelConsumption; //0.0 - 99.9 L/100km
    /************* 平均油耗 ************/
    float averageFuelConsumption; //0.0 - 99.9 L/100km
    /************* 续航里程 ************/
    float rechargeMileage; //0~999.9km
    /************ 瞬时电耗 ************/
    float instantPowerConsumption; // 0 ~ 49.8 KWH/100km
    /************ 平均电耗 ***********/
    float averagePowerConsumption; // 0 ~ 49.8 KWH/100km
    /*********************************/
} ECUData_t;

typedef struct VehicleWorkData
{
    ODOData_t odoData_t;
    GaugeData_t gaugeData_t;
    TelltaleData_t telltaleData_t;
} VehicleWorkData_t;

#pragma pack()

#endif //!_DATA_TO_HMI_DEFINE_H_