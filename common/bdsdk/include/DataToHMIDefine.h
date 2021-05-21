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
const int g_iFullCoolandTemp = 80; //0,10 10,20 20,30 30,40 40,50 50,60 60,70 70,80

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
    HMI_MGS_NAVI_ID_NAVIGATION_STATUS = 0x01, //导航激活状态
    HMI_MGS_NAVI_ID_NAVIGATION_INFO = 0x02,   //导航信息
    HMI_MGS_NAVI_ID_CROSSING_INFO = 0x03,     //路口信息
};

enum HMI_MGS_INTERACTIVE_ID
{
    HMI_MGS_INTER_ID_MUSIC_CLEAR = 0x05, //多媒体音乐关闭
    HMI_MGS_INTER_ID_MUSIC_POSITION,     //多媒体音乐进度
    HMI_MGS_INTER_ID_MUSIC_ALBUM,        //多媒体音乐专辑
    HMI_MGS_INTER_ID_MUSIC_TITLE,        //多媒体音乐名称
    HMI_MGS_INTER_ID_MUSIC_ARTIST,       //多媒体音乐歌手
    HMI_MGS_INTER_ID_PHONE_CLEAR,
    HMI_MGS_INTER_ID_PHONE_STATUS,
    HMI_MGS_INTER_ID_PHONE_NAME,
    HMI_MGS_INTER_ID_PHONE_NUMBER,
    HMI_MGS_INTER_ID_PHONE_ELAPSEDTIME,
    HMI_MGS_INTER_ID_RADIO
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
    HMI_MGS_WARNING_ID_OVERSPEED = 0x01,       //超速报警
    HMI_MGS_WARNING_ID_DOOR = 0x02,            //门开警告
    HMI_MGS_WARNING_ID_NOWARN = 0x03,          //
    HMI_MGS_WARNING_ID_BELT = 0x04,            //安全带未系报警
    HMI_MGS_WARNING_ID_RADOR_STATUS = 0x05,    //
    HMI_MGS_WARNING_ID_RADOR_DATA = 0x06,      //
    HMI_MGS_WARNING_ID_TIRE_PRESS = 0x07,      //
    HMI_MGS_WARNING_ID_TIRE_PRESS_WARN = 0x08, //
    HMI_MGS_WARNING_ID_TIRE_TEMP = 0x09,       //
    HMI_MGS_WARNING_ID_TIRE_TEMP_WARN = 0x0A,  //
    HMI_MGS_WARNING_ID_TIRE_SYS_WARN = 0x0B,   //胎压系统故障报警
    HMI_MGS_WARNING_ID_COOLANT = 0x0C,         //
    HMI_MGS_WARNING_ID_WARMUP = 0x0D,          //
    HMI_MGS_WARNING_ID_FOLLOWME,               //
    HMI_MGS_WARNING_ID_OILPRESSLOW,            //
    HMI_MGS_WARNING_ID_AIRBAG,                 //
    HMI_MGS_WARNING_ID_REST,                   //
    HMI_MGS_WARNING_ID_MAINTENANCE,            //
    HMI_MGS_WARNING_ID_OILLOW = 0x13,          //燃油低报警
    HMI_MGS_WARNING_ID_EPS_CHECK,              //
    HMI_MGS_WARNING_ID_EPS_INIT,               //
    HMI_MGS_WARNING_ID_GEARBOX_TEMP,           //
    HMI_MGS_WARNING_ID_GEARBOX_OVERHEAT,       //
    HMI_MGS_WARNING_ID_GEARBOX_CHECK,          //
    HMI_MGS_WARNING_ID_ESP,                    //
    HMI_MGS_WARNING_ID_ABS,                    //
    HMI_MGS_WARNING_ID_BRAKEFLUIDLOW,          //
    HMI_MGS_WARNING_ID_PEPS1 = 0x1C,           //
    HMI_MGS_WARNING_ID_PEPS2 = 0x1D,           //
    HMI_MGS_WARNING_ID_PEPS3 = 0x1E,           //
    HMI_MGS_WARNING_ID_PEPS4 = 0x1F,           ////
    HMI_MGS_WARNING_ID_PEPS5 = 0x20,           ////
    HMI_MGS_WARNING_ID_PEPS6 = 0x21,           //
    HMI_MGS_WARNING_ID_PEPS7 = 0x22,           //
    HMI_MGS_WARNING_ID_PEPS8 = 0x23,           //
    HMI_MGS_WARNING_ID_PEPS9 = 0x24,           //
    HMI_MGS_WARNING_ID_PEPS10 = 0x25,          //
    HMI_MGS_WARNING_ID_DOORINFO = 0x26,        //
    HMI_MGS_WARNING_ID_BELT2,                  //
    HMI_MGS_WARNING_ID_AVH1,                   //40
    HMI_MGS_WARNING_ID_AVH2,                   //
    HMI_MGS_WARNING_ID_AVH3,                   //
    HMI_MGS_WARNING_ID_CDP,                    //
    HMI_MGS_WARNING_ID_EPB1,                   //
    HMI_MGS_WARNING_ID_EPB2,                   //
    HMI_MGS_WARNING_ID_EPB3,                   //
    HMI_MGS_WARNING_ID_AVMSYS,                 //
    HMI_MGS_WARNING_ID_AVM,                    //
    HMI_MGS_WARNING_ID_DTC1,                   //
    HMI_MGS_WARNING_ID_DTC2,                   //
    HMI_MGS_WARNING_ID_DTC3,                   //
    HMI_MGS_WARNING_ID_DTC4,                   //
    HMI_MGS_WARNING_ID_DTC5,                   //
    HMI_MGS_WARNING_ID_DTC6,                   //
    HMI_MGS_WARNING_ID_DTC7,                   //
    HMI_MGS_WARNING_ID_DTC8,                   //
    HMI_MGS_WARNING_ID_DTC9,                   //
    HMI_MGS_WARNING_ID_DTC10,                  //
    HMI_MGS_WARNING_ID_DTC11,                  //
    HMI_MGS_WARNING_ID_DTC12,                  //
    HMI_MGS_WARNING_ID_DTC13,                  //
    HMI_MGS_WARNING_ID_HDC1,                   //
    HMI_MGS_WARNING_ID_HDC2,                   //
    HMI_MGS_WARNING_ID_SPORT,                  //
    HMI_MGS_WARNING_ID_PHONE,                  //
    HMI_MGS_WARNING_ID_SYSFAULT,               //
    HMI_MGS_WARNING_ID_BATTFAULT,              //
    HMI_MGS_WARNING_ID_DRIVEFAULT,             //
    HMI_MGS_WARNING_ID_BATTDISCONNECT,         //
    HMI_MGS_WARNING_ID_SOCLOW,                 //
    HMI_MGS_WARNING_ID_PARKING = 80,           //
    HMI_MGS_WARNING_ID_WARONG_GEAR = 82,       //挡位误操作
    HMI_MGS_WARNING_ID_CHARGEFAULT = 83,       //
    HMI_MGS_WARNING_ID_GEARFAULT = 84,         //
    HMI_MGS_WARNING_ID_VACYYMPRESWARN = 85,    //
    HMI_MGS_WARNING_ID_VECHICLEFAULT = 86,     //
    HMI_MGS_WARNING_ID_HMIWARNING = 87,        //
    HMI_MGS_WARNING_ID_CHARGEDISPLAY,          //
};

enum HMI_MGS_EOL_ID
{
    HMI_MGS_EOL_ID_MODE = 0x01,
    HMI_MGS_EOL_ID_SHOW
};

enum HMI_MGS_FAULT_ID
{
    HMI_MGS_FAULT_ID_INFO = 0x01
};

enum DoorDirection
{
    DRIVER_DOOR = 0x00,     //主驾驶门
    PASSENGER_DOOR = 0x01,  //副驾驶门
    REAR_RIGHT_DOOR = 0x02, //后右车门
    REAR_LEFT_DOOR = 0x03,  //后左车门
    HOOD = 0x04,            //引擎盖
    TRUNK = 0x04            //后备箱
};

enum TirePosition
{
    TIRE_FRONT_LEFT = 0x00,  //左前轮胎
    TIRE_FRONT_RIGHT = 0x01, //右前轮胎
    TIRE_REAR_LEFT = 0x02,   //左后轮胎
    TIRE_REAR_RIGHT = 0x03   //右后轮胎
};

enum TireWarn
{
    BDHMI_TIRE_NORMAL_PRESSURE = 0x00,
    BDHMI_TIRE_RESERVED = 0x01,
    BDHMI_TIRE_LOW_PRESSURE = 0x02,
    BDHMI_TIRE_HIGH_PRESSURE = 0x03
};

#pragma pack(4)
typedef struct ODOData
{
    /*************  档位  *************/
    unsigned char gear : 8;
    /************* 驾驶模式 ************/
    unsigned char driveMode : 8;
    /************* 里程累计 ************/
    float totalMileage; // 0~999999km
    /************ 里程小计A ************/
    float mileageSubtotalA; // 0.0～9999.9km
    /************ 里程小计B ***********/
    float mileageSubtotalB; // 0.0～9999.9km
    /************ 环境温度 ************/
    // float temperatureInside;
    // float temperatureOutside;
    char temperatureInside[8];
    char temperatureOutside[8];
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
    /* 报警灯信息 */
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
    } warn_light_t;

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

typedef struct WarnInfo
{
    /************  ParkLight *************/
    struct ParkLight
    {
        unsigned char parkingLight : 1; //小灯
        unsigned char reserved : 7;
    } parking_light_t;
    /************  PepsInfo *************/
    struct PepsInfo
    {
        /************ Byte1 *************/
        unsigned char peps1_PowerOff : 1;
        unsigned char peps2_NoKeyB : 1;
        unsigned char peps3_KeyInCar : 1;
        unsigned char peps4_NoKeyC : 1;
        unsigned char peps5_NoKeyA : 1;
        unsigned char peps6_KeyBattLow : 1;
        unsigned char peps7_Switch2NP : 1;
        unsigned char peps8_StepBreak : 1;
        /************ Byte2 *************/
        unsigned char peps9_Switch2P : 1;
        unsigned char peps9_NoComplete : 1;
        unsigned char reserved : 6;
        /*********************************/
    } peps_info_t;
    /************  DTCInfo *************/
    struct DTCInfo
    {
        unsigned char switchToP : 1; //请挂入 P 挡防止溜车
        unsigned char reserved : 7;
    } dtc_info_t;
    /************  OverSpeedInfo *************/
    struct OverSpeed
    {
        unsigned char overSpeed : 1; //超速报警
        unsigned char reserved : 7;
    } over_speed_t;
    /************  TireInfo *************/
    struct TireInfo
    {
        /****************** 胎压 ********************/
        float fFrontLeftPressure;  //左前轮胎胎压
        float fFrontRightPressure; //右前轮胎胎压
        float fRearLeftPressure;   //左后轮胎胎压
        float fRearRightPressure;  //右后轮胎胎压
        /****************** 胎温 ********************/
        float fFrontLeftTemperature;  //左前轮胎温度
        float fFrontRightTemperature; //右前轮胎温度
        float fRearLeftTemperature;   //左后轮胎温度
        float fRearRightTemperature;  //右后轮胎温度
        /**************** 胎压系统警告 ***************/
        unsigned char tirePressureWarn; //胎压系统警告
        /****************** 轮胎故障 *****************/
        unsigned char frontLeftTireFault;  //左前轮胎故障
        unsigned char frontRightTireFault; //右前轮胎故障
        unsigned char rearLeftTireFault;   //左后轮胎故障
        unsigned char rearRightTireFault;  //右后轮胎故障
        /********************************************/
    } tire_info_t;
    /************  OilLow *************/
    struct OilLow
    {
        unsigned char oilLowWarn; //燃油低
    } oil_low_t;
    struct DoorInfo
    {
        /******* 门开状态(0x01: 开; 0x00: 关) ********/
        unsigned char frontLeftDoor;  //左前门
        unsigned char frontRightDoor; //右前门
        unsigned char rearRightDoor;  //右后门
        unsigned char rearLeftDoor;   //左后门
        unsigned char hood;           //引擎盖
        unsigned char trunk;          //后备箱
        /********************************************/
    } door_info_t;
    /*********************************/
} WarnInfo_t;

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

typedef struct SetupInfo
{
    unsigned char ignStatus;
} SetupInfo_t;

typedef struct NavigationInfo
{
    char musicName[60];  //音乐名
    char authorName[60]; //歌手名
    char albumName[60];  //专辑名

    unsigned char phoneStatus; //电话状态
    char phoneCallName[60];    //来电名称
    char phoneCallNumber[60];  //来电号码
    char phoneCallTime[60];    //通话时间
} NavigationInfo_t;

typedef struct SettingInfo
{
    unsigned char theme;    //主题
    unsigned char language; //语言
} SettingInfo_t;

/* 故障列表信息 */
typedef struct FaultInfo
{
    unsigned char faultDoorOpenWarn;    //门开报警(车速>10Km/h)
    unsigned char faultOverSpeed;       //超速报警
    unsigned char faultWrongGear;       //档位操作
    unsigned char faultOilLow;          //燃油低报警
    unsigned char faultPeps2NoKey;      //PEPS2 未检测到智能钥匙
    unsigned char faultPeps6KeyBattLow; //PEPS6智能钥匙电量低
    unsigned char faultDoorOpenNotice;  //门开报警(车速<=10Km/h)
    unsigned char faultTireSysWarn;     //胎压系统故障
} FaultInfo_t;

typedef struct VehicleWorkData
{
    ODOData_t odoData_t;
    GaugeData_t gaugeData_t;
    TelltaleData_t telltaleData_t;
    WarnInfo_t warnInfo_t;
    NavigationInfo_t navigationInfo_t;
    SettingInfo_t settingInfo_t;
    ECUData_t ecuData_t;
    SetupInfo_t setupInfo_t;
    FaultInfo_t faultInfo_t;
} VehicleWorkData_t;

#pragma pack()

#endif //!_DATA_TO_HMI_DEFINE_H_