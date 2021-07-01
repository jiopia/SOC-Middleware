/*
    根据E02_SPI协议_V0.2编写的头文件
    MCU发送给中间件的信息定义
*/
#ifndef _MSG_DATA_DEFINE_H_
#define _MSG_DATA_DEFINE_H_

enum VehicleAccStatus
{
    VEHICLE_DEFAULT = -1,
    VEHICLE_OFF = 0,
    VEHICLE_ACC = 1,
    VEHICLE_ON = 2,
    VEHICLE_START = 3
};

enum MType
{
    MTYPE_DUMMY = 0x00,             /* 未知消息类型 */
    MTYPE_ACTION = 0x01,            /* Action类消息 */
    MTYPE_DATA = 0x02,              /* DATA类消息 */
    MTYPE_NODE_ALIVE = 0x03,        /* NodeAlive类消息 */
    MTYPE_INTERACTIVE = 0x04,       /* 交互类消息 */
    MTYPE_SETUP = 0x05,             /* Setup类消息 */
    MTYPE_KEY_AND_AUDIO_CTL = 0x06, /* Key类/音频控制权类消息 */
    MTYPE_EOL = 0x07,               /* 诊断类消息 */
};

enum ActionMsgID
{
    MTYPE_ACTION_MSGID_KEY = 0x01,              //钥匙
    MTYPE_ACTION_MSGID_CAR_LIGHT = 0x02,        //车灯信息
    MTYPE_ACTION_MSGID_DOOR_WARNING = 0x03,     //车门警报
    MTYPE_ACTION_MSGID_FAULT = 0x04,            //故障警报
    MTYPE_ACTION_MSGID_ALARM = 0x05,            //报警信息
    MTYPE_ACTION_MSGID_EPB = 0x06,              //电子手刹
    MTYPE_ACTION_MSGID_ESP = 0x07,              //电子稳定系统
    MTYPE_ACTION_MSGID_EPS = 0x08,              //电子转向助力
    MTYPE_ACTION_MSGID_PEPS = 0x09,             //无钥匙进入及启动系统
    MTYPE_ACTION_MSGID_GEAR_BOX = 0x0A,         //变速箱挡位(泊车辅助系统A)
    MTYPE_ACTION_MSGID_GEAR_BOX_FAULT = 0x0B,   //变速箱故障
    MTYPE_ACTION_MSGID_ACC = 0x0C,              //
    MTYPE_ACTION_MSGID_COURISE_CONTROL = 0x0D,  //定速巡航
    MTYPE_ACTION_MSGID_HDC = 0x0E,              //坡道缓降
    MTYPE_ACTION_MSGID_AIRBAG = 0x0F,           //
    MTYPE_ACTION_MSGID_RETIRE = 0x10,           //保养休息 0x10
    MTYPE_ACTION_MSGID_LANE_DEPATURE = 0x11,    //车道偏离指示灯
    MTYPE_ACTION_MSGID_ALARM_TIRE = 0x12,       //
    MTYPE_ACTION_MSGID_STARTANDSTOP = 0x13,     //启停系统
    MTYPE_ACTION_MSGID_ELECTRICSEAT = 0x14,     //电动座椅
    MTYPE_ACTION_MSGID_AIRCONDITION = 0x15,     //空调
    MTYPE_ACTION_MSGID_DISCHARGE = 0x16,        //
    MTYPE_ACTION_MSGID_INSULATION_FAULT = 0x17, //
    MTYPE_ACTION_MSGID_GUARD = 0x18,            //制动液位低提示
    MTYPE_ACTION_MSGID_FOLLOWME = 0x19,         //
    MTYPE_ACTION_MSGID_AVM = 0x1A,              //
    MTYPE_ACTION_MSGID_DTC = 0x1B,              //
    MTYPE_ACTION_MSGID_GEARIND = 0x1C,          //挡位提醒
    MTYPE_ACTION_MSGID_OILLOW = 0x1D,           //
    MTYPE_ACTION_MSGID_WrongGear = 0x1E,        //挡位误操作
    MTYPE_ACTION_MSGID_LDW = 0x1F,              //车道偏离报警
    MTYPE_ACTION_MSGID_PHONE_CHARGING = 0x20,   //
    MTYPE_ACTION_MSGID_APA = 0x21,              //
    MTYPE_ACTION_MSGID_GPF = 0x22,              //
    MTYPE_ACTION_MSGID_NVS = 0x23               //
};

enum DataMsgID
{
    MTYPE_DATA_MSGID_SPEED_AND_RPM = 0x01,             //车速和转速
    MTYPE_DATA_MSGID_FULE_TRAVLE = 0x02,               //油量
    MTYPE_DATA_MSGID_COOLAND_TEMPERATURE = 0x03,       //水温
    MTYPE_DATA_MSGID_TOTAL_MILEAGE = 0x04,             //总里程
    MTYPE_DATA_MSGID_SUBTOTAL_MILEAGE = 0x05,          //小计里程
    MTYPE_DATA_MSGID_ENDURANCE_MILEAGE = 0x06,         //续航里程
    MTYPE_DATA_MSGID_AVERAGE_FUEL = 0x07,              //平均油耗
    MTYPE_DATA_MSGID_INTANT_FUEL = 0x08,               //瞬时油耗
    MTYPE_DATA_MSGID_EXTERNEL_TEMPERATURE = 0x09,      //环境温度
    MTYPE_DATA_MSGID_EXTERNEL_BATTERY_VOLTAGE = 0x0A,  //电池电压
    MTYPE_DATA_MSGID_EXTERNEL_RADAR = 0x0B,            //雷达
    MTYPE_DATA_MSGID_EXTERNEL_TIRE_PRESSURE = 0x0C,    //胎压
    MTYPE_DATA_MSGID_EXTERNEL_TIRE_TEMPERATURE = 0x0D, //胎温
    MTYPE_DATA_MSGID_EXTERNEL_TIRE_FAULT = 0x0E,       //轮胎故障
    MTYPE_DATA_MSGID_STORE = 0x0F,                     //
    MTYPE_DATA_MSGID_POWERMETER = 0x10,                //
    MTYPE_DATA_MSGID_CURRENT_VOL = 0x11,               //
    MTYPE_DATA_MSGID_IFE = 0x12,                       //喷油嘴
    MTYPE_DATA_MSGID_AFE = 0x13,                       //AFE
    MTYPE_DATA_MSGID_DTE1 = 0x14,                      //DTE1
    MTYPE_DATA_MSGID_DTE2 = 0x15                       //DTE2
};

enum NodeAliveMsgID
{
    MTYPE_NODEALIVE_MSGID_ACTIVE = 0x01,    //激活节点
    MTYPE_NODEALIVE_MSGID_WATCH_DOG = 0x02, //心跳
};

enum InteractiveMsgID
{
    MTYPE_INTERACTIVE_MSGID_MUSIC = 0x01, //多媒体
    MTYPE_INTERACTIVE_MSGID_PHONE = 0x02, //电话
    MTYPE_INTERACTIVE_MSGID_RADIO,        //收音机
    MTYPE_INTERACTIVE_MSGID_BT,           //蓝牙
};

enum NavigationId
{
    NAVIGATION_STATUS = 0x05,
    NAVIGATION_CROSSING_INDEX = 0x08,
    NAVIGATION_NEXT_CROSSING_NAME = 0x0A,
    NAVIGATION_CAST_SCREEN_REQUEST = 0x0B, //投屏请求(MW->MCU)
    NAVIGATION_CAST_SCREEN_RESPONSE = 0x0C //投屏响应(MCU->MW)
};

enum MusicSubId
{
    MUSIC_CLOSE = 0x00,
    MUSIC_PROCESS = 0x01,
    MUSIC_ALBUM = 0x02,
    MUSIC_NAME = 0x03,
    MUSIC_AUTHOR = 0x04
};

enum PhoneSubId
{
    PHONE_CLEAR = 0x00,
    PHONE_STATUS = 0x01,
    PHONE_CALL_NAME = 0x02,
    PHONE_CALL_NUMBER = 0x03,
    PHONE_CALL_TIME = 0x04
};

enum SetUpMsgID
{
    MTYPE_SETUP_MSGID_SYSTEMTIME = 0x01,  //系统时间
    MTYPE_SETUP_MSGID_VERSION = 0x02,     //mcu版本
    MTYPE_SETUP_MSGID_MODE = 0x03,        //驱动模式
    MTYPE_SETUP_MSGID_BACKLIGHT = 0x04,   //背光调节
    MTYPE_SETUP_MSGID_HW_VERSION = 0x05,  //硬件版本号
    MTYPE_SETUP_MSGID_OVERSPEED = 0x06,   //超速报警
    MTYPE_SETUP_MSGID_DATARESET = 0x07,   //数据重置
    MTYPE_SETUP_MSGID_MAINTENCE = 0x08,   //保养里程
    MTYPE_SETUP_MSGID_REST = 0x09,        //
    MTYPE_SETUP_MSGID_CFG_VERSION = 0x0A, //配置信息
    MTYPE_SETUP_MSGID_CFG_THEME = 0x0B,   //
    MTYPE_SETUP_MSGID_CFG_LANG = 0x0C     //
};

enum KeyAndAudioCtrlMsgID
{
    MTYPE_KEY_MSGID_KEY = 0x01,               //按键
    MTYPE_AUDIO_CTRL_REQUEST_MSGID_KEY = 0x02, //音频控制权申请
    MTYPE_AUDIO_CTRL_REPORT_MSGID_KEY = 0x03   //音频控制权状态
};

enum AudioCtrlModeReq
{
    AUDIO_CTRL_REQUEST = 0x01,
    AUDIO_CTRL_RELEASE = 0x02
};

enum AudioCtrlAdscription
{
    AUDIO_CTRL_BELONG_TO_MCU = 0x01,
    AUDIO_CTRL_BELONG_TO_DHU = 0x02
};

enum KeyStatus
{
    KEY_TYPE_IDLE = 0x00,
    KEY_TYPE_PRESS = 0x01,
    KEY_TYPE_LONG_PRESS = 0x02,
    KEY_TYPE_LONG_HOLD = 0x04,
    KEY_TYPE_RELEASE = 0x08,
    KEY_TYPE_LONG_RELEASE = 0x10,
    KEY_TYPE_ERROR = 0x20,
};

enum EolMsgID
{
    MTYPE_EOL_MSGID_EOLONOFF = 0x01,
    MTYPE_EOL_MSGID_SHOW = 0x02,
    MTYPE_EOL_MSGID_HORN = 0x03,
    MTYPE_EOL_MSGID_DIGAGNOSE = 0x06,
    MTYPE_EOL_MSGID_DOT
};

enum UpgradeStatus
{
    UPGRADE_REQUEST = 0xF0,
    UPGRADE_FTP_READY,
    UPGRADE_FILE_READY,
    UPGRADE_PROGRESS,
    UPGRADE_MDFAILED
};

enum DoorOpenStatus
{
    DOOR_OPEN_FRONT_LEFT = 0x01,
    DOOR_OPEN_FRONT_RIGHT = 0x02,
    DOOR_OPEN_REAR_RIGHT = 0x04,
    DOOR_OPEN_REAR_LEFT = 0x08,
    DOOR_OPEN_HOOD = 0x10,
    DOOR_OPEN_TRUNK = 0x20
};

#endif //!_MSG_DATA_DEFINE_H_