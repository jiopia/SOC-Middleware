#ifndef _MSG_DATA_DEFINE_H_
#define _MSG_DATA_DEFINE_H_

inline void HexToStr(char *dstStr, const unsigned char *srcUch, const int len)
{
    for (int i = 0; i < len; i++)
    {
        sprintf(dstStr, "%02X", srcUch[i]);
        dstStr += 2;
    }
}

inline std::string HexToStr(const unsigned char uchData)
{
    char buff[4] = {0};
    HexToStr(buff, &uchData, 1);
    return std::string(buff);
}

enum MType
{
    MTYPE_DUMMY = 0x00,       /* 未知消息类型 */
    MTYPE_ACTION = 0x01,      /* Action类消息 */
    MTYPE_DATA = 0x02,        /* DATA类消息 */
    MTYPE_NODE_ALIVE = 0x03,  /* NodeAlive类消息 */
    MTYPE_INTERACTIVE = 0x04, /* 交互类消息 */
    MTYPE_SETUP = 0x05,       /* Setup类消息 */
    MTYPE_KEY = 0x06,         /* Key类消息 */
    MTYPE_EOL = 0x07,         /* 诊断类消息 */
};

enum DataMsgID
{
    MTYPE_DATA_MSGID_SPEED_AND_RPM = 0x01,            //车速和转速
    MTYPE_DATA_MSGID_FULE_TRAVLE = 0x02,              //油量
    MTYPE_DATA_MSGID_COOLAND_TEMPERATURE = 0x03,      //水温
    MTYPE_DATA_MSGID_CAN_TOTAL_MILEAGE = 0x04,        //总里程
    MTYPE_DATA_MSGID_TOTAL_MILEAGE = 0x05,            //小计里程
    MTYPE_DATA_MSGID_ENDURANCE_MILEAGE = 0x06,        //续航里程
    MTYPE_DATA_MSGID_AVERAGE_FUEL = 0x07,             //平均油耗
    MTYPE_DATA_MSGID_INTANT_FUEL = 0x08,              //瞬时油耗
    MTYPE_DATA_MSGID_EXTERNEL_TEMPERATURE = 0x09,     //环境温度
    MTYPE_DATA_MSGID_EXTERNEL_BATTERY_VOLTAGE = 0x0A, //电池电压
    MTYPE_DATA_MSGID_EXTERNEL_RADAR = 0x0B,           //雷达
    MTYPE_DATA_MSGID_EXTERNEL_TIREPRESSURE = 0x0C,    //胎压
    MTYPE_DATA_MSGID_DEBUG_TEST = 0x0D,               //debug输入
    MTYPE_DATA_MSGID_SCREEN_TEMP = 0x0E,              //屏幕温度
    MTYPE_DATA_MSGID_STORE = 0x0F,
    //#ifdef FX11E
    MTYPE_DATA_MSGID_POWERMETER = 0x10,
    MTYPE_DATA_MSGID_CURRENT_VOL,
    //#else
    MTYPE_DATA_MSGID_IFE,  //喷油嘴
    MTYPE_DATA_MSGID_AFE,  //AFE
    MTYPE_DATA_MSGID_DTE1, //DTE1
    MTYPE_DATA_MSGID_DTE2, //DTE2
    //#endif

};

enum ActionMsgID
{
    MTYPE_ACTION_MSGID_KEY = 0x01,          //钥匙
    MTYPE_ACTION_MSGID_CAR_LIGHT = 0x02,    //车灯信息
    MTYPE_ACTION_MSGID_DOOR_WARNING = 0x03, //车门警报
    MTYPE_ACTION_MSGID_FAULT = 0x04,        //故障警报
    MTYPE_ACTION_MSGID_ALARM = 0x05,        //报警信息
    MTYPE_ACTION_MSGID_EPB = 0x06,          //电子手刹
    MTYPE_ACTION_MSGID_ESP = 0x07,          //电子稳定系统
    MTYPE_ACTION_MSGID_EPS,                 //电子转向助力
    MTYPE_ACTION_MSGID_PEPS,                //无钥匙进入及启动系统
    MTYPE_ACTION_MSGID_GEAR_BOX,            //泊车辅助系统A
    MTYPE_ACTION_MSGID_GEAR_BOX_FAULT,      //变速箱故障
    MTYPE_ACTION_MSGID_ACC,
    MTYPE_ACTION_MSGID_COURISE_CONTROL, //定速巡航
    MTYPE_ACTION_MSGID_HDC,             //坡道缓降
    MTYPE_ACTION_MSGID_AIRBAG,
    MTYPE_ACTION_MSGID_RETIRE,        //保养休息 0x10
    MTYPE_ACTION_MSGID_LANE_DEPATURE, //车道偏离指示灯
    MTYPE_ACTION_MSGID_ALARM_TIRE,
    MTYPE_ACTION_MSGID_STARTANDSTOP, //启停系统
    MTYPE_ACTION_MSGID_ELECTRICSEAT, //电动座椅
    MTYPE_ACTION_MSGID_AIRCONDITION, //空调
    MTYPE_ACTION_MSGID_DISCHARGE = 0x16,
    MTYPE_ACTION_MSGID_INSULATION_FAULT,
    MTYPE_ACTION_MSGID_GUARD, //制动液位低提示
    MTYPE_ACTION_MSGID_FOLLOWME,
    MTYPE_ACTION_MSGID_AVM,
    MTYPE_ACTION_MSGID_DTC,
    MTYPE_ACTION_MSGID_GEARIND, //挡位提醒0x1c
    MTYPE_ACTION_MSGID_OILLOW,
    MTYPE_ACTION_MSGID_WrongGear, //挡位误操作0x1e
    MTYPE_ACTION_MSGID_LDW,       //车道偏离报警
    MTYPE_ACTION_MSGID_PHONE_CHARGING,
    MTYPE_ACTION_MSGID_APA,
    MTYPE_ACTION_MSGID_GPF,
    MTYPE_ACTION_MSGID_NVS
};

enum NodeAliveMsgID
{
    MTYPE_NODEALIVE_MSGID_ACTIVE = 0x01,    //激活节点
    MTYPE_NODEALIVE_MSGID_WATCH_DOG = 0x02, //心跳
};

enum InteractiveMsgID
{
    MTYPE_INTERACTIVE_MSGID_MUSIC = 0x01,        //导航信息
    MTYPE_INTERACTIVE_MSGID_PHONE = 0x02,        //多媒体
    MTYPE_INTERACTIVE_MSGID_RADIO,               //收音机
    MTYPE_INTERACTIVE_MSGID_BT,                  //蓝牙
    MTYPE_INTERACTIVE_MSGID_INTERACTIVE1 = 0x05, //导航信息1
    MTYPE_INTERACTIVE_MSGID_INTERACTIVE2 = 0x06, //导航信息2
    MTYPE_INTERACTIVE_MSGID_INTERACTIVE3 = 0x07, //导航信息3
    MTYPE_INTERACTIVE_MSGID_INTERACTIVE4 = 0x08, //导航信息4
    MTYPE_INTERACTIVE_MSGID_INTERACTIVE5 = 0x09, //导航信息5
};

enum SetUpMsgID
{
    MTYPE_SETUP_MSGID_SYSTEMTIME = 0x01, //系统时间
    MTYPE_SETUP_MSGID_VERSION = 0x02,    //mcu版本
    MTYPE_SETUP_MSGID_MODE = 0x03,       //驱动模式
    MTYPE_SETUP_MSGID_BACKLIGHT = 0x04,  //背光调节
    MTYPE_SETUP_MSGID_HW_VERSION = 0x05, //硬件版本号
    MTYPE_SETUP_MSGID_OVERSPEED = 0x06,  //超速报警
    MTYPE_SETUP_MSGID_DATARESET = 0x07,  //数据重置
    MTYPE_SETUP_MSGID_MAINTENCE = 0x08,  //保养里程
    MTYPE_SETUP_MSGID_REST,
    MTYPE_SETUP_MSGID_CFG_VERSION = 0x0A, //配置信息
    MTYPE_SETUP_MSGID_CFG_THEME,
    MTYPE_SETUP_MSGID_CFG_LANG
};

enum EolMsgID
{
    MTYPE_EOL_MSGID_EOLONOFF = 0x01,
    MTYPE_EOL_MSGID_SHOW = 0x02,
    MTYPE_EOL_MSGID_HORN = 0x03,
    MTYPE_EOL_MSGID_DIGAGNOSE = 0x06,
    MTYPE_EOL_MSGID_DOT
};

enum KeyMsgID
{
    MTYPE_KEY_MSGID_KEY = 0x01, //按键
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

enum UpgradeStatus
{
    UPGRADE_REQUEST = 0xF0,
    UPGRADE_FTP_READY,
    UPGRADE_FILE_READY,
    UPGRADE_PROGRESS,
    UPGRADE_MDFAILED
};

#endif //!_MSG_DATA_DEFINE_H_