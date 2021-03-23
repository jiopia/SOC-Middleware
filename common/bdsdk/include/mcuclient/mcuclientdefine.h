#ifndef MCUCLIENTDEFINE_H
#define MCUCLIENTDEFINE_H

#define AVERAGE_FUEL_DEFAULT -1
#define INSTANT_FUEL_DEFAULT -1
#define EXTERNAL_TEMPERATURE_DEFAULT -10000
#define TIRE_PRESSURE_DEFAULT -10000
#define TIRE_TEMPERATURE_DEFAULT -10000
#define ACC_SPEED_DEFAULT -10000
#define ENDURANCE_DEFAULT -1
enum AlarmAndWarning
{
	MIL_FAULT,                          //发动机排放系统及
	EPC_FAULT,                          //发动机故障
	DRIVER_BELT_ALARM,                  //驾驶位安全带
	COPILOT_BELT_ALARM,                 //副驾驶安全带
	HEADLIGHT_AUTOMATIC_OPENING_FAULT,  //大灯自动开启故障
	FUEL_LEVEL_LOW_ALARM,               //燃油低位报警
	BRAKE_FLUID_LEVEL_LOW_ALARM,        //制动液低位报警
	COOLING_LEVEL_LOW_ALARM,            //冷却液低位报警
	COOLANT_TEMPERATURE_HIGH,           //水温高报警
	ABS_FAULT,                          //ABS故障
	COOLANT_TEMPERATURE_LOW,            //水温低报警
	EBD_FAULT,                          //刹车故障(电子制动力分配系统故障)
	AIRBAG_FAULT,                       //安全气囊故障
	OIL_PRESSURE_ALARM,                 //机油压力报警
	GEARBOX_FAULT_LAMP,                 //变速箱故障指示灯
	GEAR_FAULT_LAMP,                    //档位故障指示灯
	GEAR_HEAT,                          //变速箱过热
	BODY_SECURITY,                      //车身防盗
	VEHICLE_MAINTENANCE,                //整车保养
	EPS_FAULT,                          //EPS报警（电子控制动力转向系统故障）
	IMMOBILIZER,                        //发动机防盗报警
	HAND_BRAKE,                         //手刹
	CHARGING_SYSTEM_FAULT,              //充电系统故障
	FOUR_WHEEL_DRIVE,                   //四轮驱动
	OVER_SPEED,                         //超速报警
	LEFT_TURN_LIGHT,                    //左转向灯
	RIGHT_TURN_LIGHT,                   //右转向灯
	HIGH_BEAM,                          //远光指示灯
	LOW_BEAM,                           //近光指示灯
	FRONT_FOG_LAMP,                     //前雾灯
	BACK_FOG_LAMP,                      //后雾灯
	DRL_LAMP,                           //DRL指示灯(日间行车灯)
	ALS_LAMP,                           //ALS小灯
	ESC_LAMP,                           //开机自检点亮
	ACCUMULATOR_DISCHARGE_LAMP,         //蓄电池充放电指示灯
	PDC_ONOFF,                          //泊车辅助系统开关
	EP_LAMP,                            //紧急刹车灯
	CONTOUR_LAMP,                       //轮廓灯
	EPB_FAULT,                          //epb报警(电子驻车系统警报)
	ESCOFF_LAMP,                        //ESCoff指示
	TPMS,                               //TPMS
	HAZZARD_LIGHT,                      //双闪灯
	PARKING_LIGHT,                      //停车灯
	EPB_LAMP,                           //电子手刹灯
	ESP_ACTIVE,                         //电子稳定系统激活
	ESP_ONOFF,                          //电子稳定系统开关
	ESP_FAULT,                          //电子稳定系统故障
	AUTOMATIC_VEHICLE_FIXED_FAULT,      //自动车辆固定故障
	RAIN_SENSOR_FAULT,                  //雨量传感器故障
	DOOR_ALARM,                         //门开报警
	AVH_ALARM,                          //AVH报警
	TIRE_ALARM,                         //胎压报警
	HDC_FAULT,                          //坡道缓降故障
	HDC_STATUS,                         //坡道缓降故障状态
	ACC_ACCMODE_YELLOW_LAMP,            //自适应巡航指示灯黄色
	ACC_ACCMODE_GREEN_LAMP,             //自适应巡航指示灯绿色
	ACC_ACCMODE_FAULT_LAMP,             //自适应巡航故障
	ACC_PCW_ACTIVE_GREEN_LAMP,          //自适应预警绿灯
	ACC_PCW_ACTIVE_RED_LAMP,            //自适应预警红灯
	ACC_PCW_ACTIVE_LATENTWARNING,       //自适应安全距离指示灯
	AEB_SYSTEM,
	START_AND_FUNCTION_FAULT,           //启停系统故障
	START_AND_FUNCTION_WORK,            //启停系统工作指示灯
	INSULATION,
	ALARM_WARNING_MAX
};
enum CuriseControlStatus
{
	CC_DEFAULT = -1, CC_OFF,                 //巡航关闭
	CC_ACTIVE,                     //正在巡航
	CC_STANDBY,             //巡航开启，但未巡航
	CC_INVALID,
	CC_MAX
};
enum DoorWarning
{
	DRIVER_DOOR = 0,                 //主驾驶门
	PASSENGER_DOOR,                //副驾驶门
	REAR_RIGHT_DOOR,               //后右车门
	REAR_LEFT_DOOR,                //后左车门
	HOOD,                          //引擎盖
	BACK_DOOR,                     //后备箱
	DOOR_MAX
};
enum Tire
{
	FL = 0, FR, RL, RR, TIR_MAX

};
enum TireWarninig
{
	NO_WARNING = 0x00,                        //没有警报
	HIGH_PRESSURE_WARNING = 0x01,             //高压警报
	LOW_PRESSURE_WARNING = 0x02,              //低压警报
	QUIK_LEAKAGE = 0x03,                      //快速泄漏
	LOST_SENSOR = 0x04,                       //传感器丢失
	SENSOR_BATTERY_LOW = 0x05,                //传感器电量低
	SENSOR_FAILURE = 0x06,                    //传感器错误
	RESERVED = 0x07,
	TIER_WARNING_MAX,
};
enum ONOFF
{
	OFF = 0, ON = 1, FLICKER = 2,
	DEFAULT = 0,//闪烁（默认频率）
	FLICKER_1HZ = 3,                  //闪烁1HZ
	FLICKER_2HZ = 4,                  //闪烁2HZ
	FLICKER_2point5HZ = 5             //闪烁2.5HZ
};
enum Unit
{
	CENTIGRADE,   //摄氏度
	FAHERENHEIT,  //华氏度
};
enum Gear
{
	GEAR_DEFAULT = 0,
	GEAR_1,
	GEAR_2,
	GEAR_3,
	GEAR_4,
	GEAR_5,
	GEAR_6,
	GEAR_P,
	GEAR_D,
	GEAR_N,
	GEAR_R,
	GEAR_L,
	GEAR_MAX
};
enum KeyCode     //按键类型
{
	CONFIRM,      //确定
	UP,    //上
	DOWN,    //左
	CANCEL,   //取消
};
enum KeyType
{
	CLICK = 0x1,        //短按
	LONG_CLICK,   //长按
	LONG_CLICK_INTR,
	LONG_RELEASE
};
enum EpbActionStatus
{
	EPB_ACTION_DEFAULT,
	EPB_ACTION_DURING_PARKING,
	EPB_ACTION_DURING_RELEASING,
	EPB_ACTION_REVERSED,
	EBP_ACTION_MAX
};
enum EpbStatus
{
	EPB_UNKNOW, EPB_PARKING, EPB_RELEASING, EPB_REVERSED,
};
enum Mode
{
	ECONOMY_MODE = 0x00,
	MANUAL = 0x01,
	SPORTS_MODE = 0x02,
	WINTER = 0x03,
	AUTOMIC = 0x04,
	MODE_ERROR = 0x05,
};
enum PepsAlarmStatus
{
	PEPS_DEFAULT = 0, ESCL_FAULT = 1,                         //ESCL故障
	PEPS_FAULT = 2,                         //PEPS故障
	KEYNOTFIND_WHEN_ON_LEVEL_DOOR_CLOSED,       //未搜索到钥匙，门关
	KEYNOTFIND_WHEN_ON_LEVEL_INDICATION_ON,     //未搜索到钥匙指示灯
	KEY_BATTERY_LOW,                            //电池电量低
	PEPS_ALARM_MAX
};
enum PepsNote
{
	PEPS_NOTE_DEFAULT = 0,                            //default
	PEPS_NOTE_1 = 1,                         //显示“钥匙尾端抵住杯托底部”
	PEPS_NOTE_2 = 2,                         //显示“切换至P或者N档启动”
	PEPS_NOTE_3 = 3,                            //“踩离合器按启动按钮”
	PEPS_NOTE_4 = 4,                            //“踩刹车按启动按钮”
	PEPS_NOTE_5,
	PEPS_NOTE_6,
	PEPS_NOTE_7,
	PEPS_NOTE_8,
	PEPS_NOTE_MAX
};
enum PdcStatus
{
	PDC_DEFAULT = -1, PDC_NOACTIVE = 1,          //泊车辅助系统未激活
	PDC_ACTIVE = 2,            //泊车辅助系统激活
	PDC_FAULT = 3,          //泊车辅助系统故障
	PDC_MAX
};

enum RadarProb
{
	FRONT_LEFT,
	FRONT_LEFT_MID,
	FRONT_RIGHT_MID,
	FRONT_RIGHT,
	BACK_LEFT,
	BACK_LEFT_MID,
	BACK_RIGHT_MID,
	BACK_RIGHT,
	RADAR_MAX
};
enum AccStatus
{
	ACC_DEFAULT = -1, ACC_OFF, ACC, ACC_ON, START
};
enum ResetType
{
	RESET_TOTAL_MILEAGE, RESET_AVERAGE_FUEL, RESET_MAINTENANCE_MILEAGE, RESET_ALL
};
enum ContolLimit
{
	LIMIT_DEFAULT = -1, DECREASE, INCREASE,
};
enum LaneDepartureStatus
{
	LDWS_DEFAULT = -1, LDWS_A, LDWS_B, LDWS_C, LDWS_D
};
enum ACC_Distance
{
	DISTANCE_DEFAULT = -1, DISTANCE_0, DISTANCE_1, DISTANCE_2, DISTANCE_3
};
enum ACC_CarShow
{
	ACC_CARSHOW_DEFAULT = -1,
	ACC_CARSHOW_HIDE,             //不显示小车
	ACC_CARSHOW_1,                //显示红色小车和1格距离
	ACC_CARSHOW_2,                //显示红色小车和2格距离
	ACC_CARSHOW_3,                //显示灰色小车和3格距离
};

enum ACC_Notice
{
	ACC_NOTICE_DEFAULT = -1,
	ACC_NOTICE_1, //显示中文：ACC功能无法开启；英文：Failed to Activiate ACC;
	ACC_NOTICE_2,                //ACC功能取消；英文：ACC Cancelled；
	ACC_NOTICE_3,                //ACC功能激活;英文：ACC Active
	ACC_NOTICE_4,                //请检查雷达；英文：Please Check Radar
	ACC_NOTICE_5,                //ACC功能开启；英文：ACC Switched On
	ACC_NOTICE_6,                //ACC功能关闭；英文：ACC Switched Off
	ACC_NOTICE_7,                //手刹拉起，ACC已退出；EPB Active， ACC Exited
	ACC_NOTICE_8,    //显示中文：车辆倒档， ACC禁止激活;英文：In Reverse Gear, ACC Not Applicable
	ACC_NOTICE_9,        //显示中文：请系好安全带，并激活ACC; Fastern Seatbelt and Activate ACC
	ACC_NOTICE_10,               //自适应巡航已取消；英文：ACC Cancelled
	ACC_NOTICE_11, //按Resume或踩油门激活ACC；英文：Press Resume or Depress Acceleration Pedal to Activate ACC;
	ACC_NOTICE_12, //显示中文：自适应巡航已退出;英文：ACC Exited
	ACC_NOTICE_13, //ACC/AEB系统故障; 英文：ACC/AEB Unavailable
	ACC_NOTICE_14  //ACC/AEB系统故障；英文：ACC/AEB Error
};
enum UPGRADE_2_IVI
{
	UPGRADE_2_IVI_FTP_READY,
	UPGRADE_2_IVI_BUTT
};
enum NAVI_MEDIA
{
	NAVI_MUSIC_NOSHOW = 0X5,
	NAVI_MUSIC_POSITION,
	NAVI_MUSIC_ALBUM,
	NAVI_MUSIC_TITLE,
	NAVI_MUSIC_ARTIST,
	NAVI_PHONE_CLEAR,
	NAVI_PHONE_STATUS,
	NAVI_PHONE_NAME,
	NAVI_PHONE_NUMBER,
	NAVI_PHONE_ELAPSEDTIME
};

#endif // MCUCLIENTDEFINE_H
