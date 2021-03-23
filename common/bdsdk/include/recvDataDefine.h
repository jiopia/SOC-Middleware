/*
 * recvDataDefine.h
 *
 *  Created on: 2017-9-8
 *      Author: Administrator
 */

#ifndef RECVDATADEFINE_H_
#define RECVDATADEFINE_H_
#define MCU_RECV_NAME "MCU_DATA"
#define I2C_RECV_NAME "I2C_DATA"
#define SYSTEM_RECV_NAME "SYSTEM_DATA"
#define VIEW_AUTUATOR_NAME "VIEW_AUTUATOR_DATA"
#define AUDIO_NAME "AUDIO_NAME"
#define HMI_NAME "HMI_NAME"

#define SYSTEM_SHARED "/jsbdsystemshared"
#define MCU_DATA_SHARED "/myshareddata"
#define VIEWACTUAL_SHARED "/mysharedviewdata"
#define VERSION_SHARED "/myversionshared"

enum MsgType
{
	MCU_VEHICLE_SPEED = 1,                  //
	MCU_SPEED_RPM,                       //
	MCU_FULE_TRAVLE,                    //燃油表位置
	MCU_COOLAND_TEMPERATURE_TRAVLE,     //水温表位置
	MCU_COOLAND_TEMPERATURE,            //水温
	MCU_CAN_TIME,                       //总线时间
	MCU_CAN_TOTAL_MILEAGE,              //总线上的总里程
	MCU_TOTAL_MILEAGE_A,
	MCU_TOTAL_MILEAGE_B,
	MCU_INSTANT_FUEL,                   //瞬时油耗
	MCU_AVERAGE_FUEL,                   //平均油耗
	MCU_ENDURANCE_MILEAGE,              //续航里程
	MCU_LANE_DEPATURE,           		//车道偏离状态
	MCU_PROMPT_AND_ALARM,               //警报
	MCU_CURISE_CONTROL_STATUS,          //定速巡航状态
	MCU_DOOR_WARNING,                   //门开警报
	MCU_PEPS_ICU_ALARM,                 //无钥匙警报状态信息
	MCU_PEPS_ICU_NOTE,                  //无钥匙提示
	MCU_TIRE_PRESSURE,                  //胎压
	MCU_TIRE_PRESSURE_WARNING,          //胎压报警
	MCU_TIRE_TEMPERATURE,               //轮胎温度
	MCU_TIRE_TEMPERATURE_WARNING,       //轮胎温度警报
	MCU_GEAR,
	MCU_WHELL_KEY,
	MCU_EXTERNEL_TEMPERATURE,          //外部温度
	MCU_ROUTE,                         //路径规划
	MCU_NEXT_TURNING_NAME,             //下一个路口名称
	MCU_NEXT_TURNING_DISTANCE,         //下一个路口距离
	MCU_DESTINATION_DISTANCE,          //目的地距离
	MCU_DESTINATION_TIME,              //显示目的地时间
	MCU_TURN_BY_TURN,                  //转向信息
	MCU_RADIO,                         //收音机信息
	MCU_TIRE_SYSTEM_FAULT,                         //多媒体信息
	MCU_BT_STATUS,                     //蓝牙连接状态
	MCU_BT_PHONE_CALL_STATUS,          //来电状态
	MCU_BT_PHONE_CALL_INFO,            //来电信息
	MCU_PHONE_INFO,                    //手机信息
	MCU_EPB_ACTION_STATUS,             //电子手刹动作状态
	MCU_EPB_STATUS,                    //电子手刹状态
	MCU_MODE,                          //模式
	MCU_PDC_STATUS,                    //泊车辅助系统状态
	MCU_CURISE_CONTROL_SPEED,         //定速巡航速度
	MCU_RADAR_RANGE,                   //雷达距离
	MCU_BATERRY_VOLTAGE,               //电池电压
	MCU_HDC,                           //陡坡缓降
	MCU_ACC_STATUS,                    //ACC状态
	MCU_EPB_FAULT,
	MCU_BACK_LIGHT,
	MCU_OVER_SPEED,
	MCU_DEBUG,
	MCU_NAVI,
	MCU_ADAPTIVE_CRUISE,
	MCU_MAINTENCE,
	MCU_FAULT_LIST,
	MCU_CFG_INFO,
	MCU_DEBUG_SCREEN,
	MCU_DEBUG_FUEL,
	MCU_CURRENT,
	MCU_VOLTAGE,
	MCU_POWERMETER,
	MCU_LIMIT_SPEED,
	IVI_UPGRADE,
	UPGRADE_IVI,
	MCU_CFG_THEME,
	MCU_CFG_LANG,
	MCU_REST,
	MCU_RESET_ACK,
	MCU_TIME,
	MCU_RADAR_STATUS,
	MCU_EOL,
	MCU_EOL_SHOW,
	MCU_FOLLOWME,
	MCU_AVM,
	MCU_GEAR_IND,
	MCU_NAVI_PHONE_STATUS,
	MCU_NAVI_MUSIC_TITLE
};
enum sendToMcu
{
	MCU_SEND_NAME = 0,
	MCU_SEND_OVERSPEED_SWITCH,
	MCU_SEND_BACK_LIGHT,
	MCU_SEND_OVERSPEED_SPEED,
	MCU_SEND_RESET,
	MCU_SEND_WATCH_DOG,
	MCU_SEND_TIME,
	MCU_SEND_SELF_CHECK,
	MCU_SEND_REST,
	HMI_SEND_AUDIO
};
enum sendToUpgrade
{
	UPGRADE_SEND_NAME = 0x10,
	UPGRADE_SEND_REQ
};
enum sendToIVI
{
	IVI_UPGRADE_FTP = 0x20,
	IVI_UPGRADE_PROGRESS
};
enum sendToViewAutuator
{
	VIEW_AUTUATOR_SEND_NAME, VIEW_AUTUATOR_BACK, VIEW_AUTUATOR_REMOVEALL
};
enum viewAutuatorType
{
	VIEW_AUTUATOR_REC_JSON,
};
enum systemEnum
{
	SYSTEM_UPDATE_SEND_NAME,
	SYSTEM_UPDATE_LOAD_FILE,
	SYSTEM_UPDATE_START_UPDATE,
	SYSTEM_UPDATE_STOP_UPDATE,
	SYSTEM_UPDATE_MCU_VERSION_CHANGE,
	SYSTEM_USB_STATUS_REQUEST,
	SYSTEM_DATA_STORE_SAVE
};
enum systemClienEnum
{

	SYSTEM_UPDATE_CLIENT_UPDATING,
	SYSTEM_UPDATE_CLIENT_UPDATE_SUCCESS,
	SYSTEM_UPDATE_CLIENT_ERROR_NO_FROM_PATH,
	SYSTEM_UPDATE_CLIENT_ERROR_NO_TO_PATH,
	SYSTEM_UPDATE_CLIENT_ERROR_UNKNOW_INTTERRUPT,
	SYSTEM_USB_STATUS
};
enum i2cMsg
{
	I2C_ROUTE,                         //路径规划
	I2C_NEXT_TURNING_NAME,             //下一个路口名称
	I2C_NEXT_TURNING_DISTANCE,         //下一个路口距离
	I2C_DESTINATION_DISTANCE,          //目的地距离
	I2C_DESTINATION_TIME,              //显示目的地时间
	I2C_TURN_BY_TURN,                  //转向信息
	I2C_RADIO,                         //收音机信息
	I2C_MEDIA,                         //多媒体信息
	I2C_BT_STATUS,                     //蓝牙连接状态
	I2C_BT_PHONE_CALL_STATUS,          //来电状态
	I2C_BT_PHONE_CALL_INFO,            //来电信息
	I2C_PHONE_INFO,              //手机信息
	I2C_MEDIA_SOURCE,                         //多媒体source
	I2C_SEND_MSG_NAME,
	I2C_SEND_MSG_CMD,

};
enum AudioType
{
	AUDIO_TYPE_TURN, AUDIO_TYPE_WARNING, AUDIO_TYPE_ACC
};
enum usbStatus
{
	MOUNT, UNMOUNT, UNKNOW
};
typedef enum ParaMeter
{
	INVALID_PARAMETER, C_DATA, I_DATA, F_DATA, L_DATA, BUF1, BUF2, BUF3
} ParaMeter;
//指示灯类型
enum class TELLTALE_TYPE
{
	carlight0,
	carlight1,
	flick0, //闪烁
	flick1,
	flick2,
	flick3,
	flick4,
	flick5,
	flick6,
	on0, //常亮
	on1,
	on2,
	on3,
	on4,
	on5,
	on6
};

enum class TELLTALES
{
	drive_belt,
	copilot_belt,
	esp,
	espoff,
	abs,
	door,
	eps,
	tire,

	gearbox,
	epb,
	epb_fault,
	airbag,
	hdc_ctrl,
	reserve1,
	reserve2,
	guard,

	discharge,
	mainteance,
	ebd,
	cruise,
	coolant,
	fuel,
	spt,
	gpf,

	hdc_fail,
	avh,
	oil_lowpressure,
	bsd_normal,
	bsd_fault,
	eco,
	epc,
	mil_fault,

	cas,
	cas_fault,
	warmup,
	limitspeed,
	ldw_yellow,
	ldw_grey,
	ldw_green,
	reserve6,

	powerbatt_fault,
	system_fault,
	powerbatt_disconnect,
	chgwire_connect,
	ready,
	motorinvertor_overtemp,
	soc_low,
	power_limit,

	electricdrive_fault,
	batt_isolation,
	battTempWarn,
	threeGFail,
};

#pragma pack(4)
typedef struct Data
{
	char parameterNum;
	char c_data[2];
	char reverse;
	int i_data;
	float f_data;
	long l_data;
	ParaMeter parameterType[8];
	char buf1[64];
	char buf2[64];
	char buf3[64];
} typeData;

typedef struct MsgData
{
	int type;
	int reverse;
	typeData data;
} typeMsgData;

typedef struct systemData
{
	int type;
	int reverse;
	char buf[128];
} typeSystemData;

typedef struct systemClientData
{
	int _type;
	int reverse;
	int i_data[2];
	char type[32];
	char name[32];
	char version[32];
	char author[32];
}typeSystemClientData;
#pragma pack()

#endif /* RECVDATADEFINE_H_ */
