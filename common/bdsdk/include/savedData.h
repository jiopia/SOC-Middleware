/*
 * savedData.h
 *
 *  Created on: 2017-7-17
 *      Author: Administrator
 */

#ifndef SAVEDDATA_H_
#define SAVEDDATA_H_
#include <string.h>
#include <pthread.h>
#define TELLTALE_LENGTH 5

#pragma pack(4)
typedef struct mcuStatusData
{
	unsigned char left_lamp :1;                       //左转向灯
	unsigned char right_lamp :1;                      //右转向灯
	unsigned char mil_fault :1;                       //发动机排放系统
	unsigned char epc_fault :1;                       //发动机故障
	unsigned char coolant_temperature_high :2;        //水温高
	unsigned char fuel_level :1;                      //燃油不足
	unsigned char abs_fault :2;                       //防抱死系统ABS故障
	/*************************1byte*****************************************/

	unsigned char coolant_temperature_low :2;         //水温低
	unsigned char hand_brake :2;                      //手刹
	unsigned char charging_system_fault :1;           //充电系统故障
	unsigned char airbag_fault :2;                    //安全气囊故障
	unsigned char gear_fault :1;                      //档位故障
	/*************************1byte*****************************************/
	unsigned char gearbox_fault :3;                   //变速箱故障
	unsigned char oil_pressure_alarm :2;              //机油压力报警
	unsigned char gearbox_heat :1;                    //变速箱过热
	unsigned char body_security :2;                   //车身防盗
	unsigned char fl_door :1;                         //左前车门
	unsigned char fr_door :1;                         //右前车门
	/*************************1byte*****************************************/
	unsigned char rl_door :1;                         //左后车门
	unsigned char rr_door :1;                         //右后车门
	unsigned char hood :1;                           //引擎盖
	unsigned char back_door :1;                        //后备箱
	unsigned char door_alarm :2;                       //门开报警
	unsigned char high_beam :1;                       //远光指示灯
	unsigned char low_beam :1;                        //近光指示灯
	/*************************1byte*****************************************/
	unsigned char over_speed_alarm :2;                  //超速报警
	unsigned char back_fog_lamp :1;                   //后雾灯
	unsigned char front_fog_lamp :1;                  //前雾灯
	unsigned char curise_lamp :1;                     //巡航灯
	unsigned char contour_lamp :1;                    //轮廓灯
	unsigned char eps :2;                             //EPS（电子控制动力转向系统故障）
	/*************************1byte*****************************************/
	unsigned char epb_fault :2;                       //EPB故障
	unsigned char avh_fault :2;                       //AVH
	unsigned char esc_lamp :1;                        //开机自检点亮
	unsigned char escoff_lamp :1;                     //ESCoff
	unsigned char drl_lamp :1;                        //DRL指示灯
	unsigned char tpms :1;                            //TPMS
	/*************************1byte*****************************************/
	unsigned char als_lamp :2;                        //ALS小灯
	unsigned char start_stop_no_function :2;          //启停故障
	unsigned char four_whell_drive :1;                //四轮驱动
	unsigned char ep_lamp :1;                         //紧急刹车灯
	unsigned char lane_depature_switch :1;            //车道偏离开关
	unsigned char brake_fluid :1;                     //制动液位低
	/*************************1byte*****************************************/
	unsigned char hazzard_light :1;                   //双闪灯
	unsigned char parking_light :1;                   //停车灯(位置灯)
	unsigned char reverse_light :1;                   //倒车灯
	unsigned char ebd_fult :1;                        //电子制动力分配系统故障
	unsigned char drive_belt :2;                      //主驾驶位安全带
	unsigned char headlight_automatic_open_fault :1;      //大灯自动开启故障
	unsigned char cooling :1;                         //冷却液位低
	/*************************1byte*****************************************/
	unsigned char copilot_belt :2;                    //副驾驶位安全带
	unsigned char accumulator_discharge :2;            //蓄电池充放电
	unsigned char epb_actionstatus :2;                 //电子手刹动作状态
	unsigned char esp_active_status :2;               //esp激活状态
	/*************************1byte*****************************************/
	unsigned char rain_sensor_fault :1;                //雨量传感器故障
	unsigned char esp_active :2;                      //esp激活
	unsigned char epb_status :2;                      //电子手刹状态
	unsigned char esp_onoff :1;                       //esp开关
	unsigned char esp_fault :1;                       //esp故障
	unsigned char automatic_vehicle_fixed_fault :1;    //自动车辆固定故障
	/*************************1byte*****************************************/
	unsigned char tire_alarm :2;                       //门开报警
	unsigned char ign_offshow :2;
	unsigned char over_speed_status :2;                  //超速提醒状态
	unsigned char over_speed_onoff :1;                  //超速报警开关
	unsigned char vehicle_mainteance_onoff :1;            //整车保养开关
	/*************************1byte*****************************************/
	unsigned char hcd_status :2;                         //hdc状态指示
	unsigned char hdc_fault :2;                          //hdc 故障
	unsigned char lane_depature_status :3;               //车道偏离状态
	unsigned char acc_pcw_active :1;                        //自适应巡航pcw激活状态
	/*************************1byte*****************************************/
	unsigned char vehicle_mainteance_notice :2;            //整车保养提示
	unsigned char acc_pcw_warning :1;                       //自适应巡航pcw警告状态
	unsigned char acc_pcw_safedistance :1;                  //自适应巡航安全距离报警
	unsigned char start_and_function :2;                     //启停系统状态
	unsigned char electric_seat :2;                		     //电动座椅
	/*************************1byte*****************************************/
	unsigned char aeb_status :1;                          //aeb(自动紧急制动系统)
	unsigned char lane_depature_warning :1;                 //车道偏离报警
	unsigned char navi_active_onoff :1;                     //导航激活状态
	unsigned char dg_type :4;                               //导航引导类型
	unsigned char intersection_type :1;                     //路口类型
//	unsigned char reverse :2;                              //EOL开关
	/*************************1byte*****************************************/
	unsigned char eol_onoff :2;                              //EOL开关
	unsigned char brakefluid_low_view:1;                   //制动液位低报警界面
	unsigned char lane_depature_fault :1;               //车道偏离状
	unsigned char accTakeover :1;
	unsigned char coolant_temperature_lost :1;
	unsigned char insulationFault :2;
	/*************************1byte*****************************************/
	unsigned char systemFault :1;
	unsigned char powerbattFault :1;
	unsigned char electricDriveFault :1;
	unsigned char powerbattDisconnectFault :1;
	unsigned char ready :1;
	unsigned char motorinvertorOvertemp :1;
	unsigned char socLow :1;
	unsigned char powerLimit :1;
	/*************************1byte -1*****************************************/
	unsigned char chgwireConnect :1;
	unsigned char coolantWarmup :2;
	unsigned char keyReminder :1;
	unsigned char epbWarning :2;
	/*************************1byte*****************************************/
	unsigned char cdp :1;
	unsigned char avmSysFault :1;
	unsigned char avmFault :4;
	unsigned char avhFail :1;
	unsigned char avhFault :1;
	/*************************1byte-1*****************************************/
	unsigned char phone :1;
	unsigned char battTempWarn :1;
	unsigned char threeGFail :1;
	unsigned char reverseGear :1;
	unsigned char reserved :3;
	/*******************************状态值*************************************************************/
	unsigned char turnbyturn;                          //TurnByTurn信息
	unsigned char eol_show;                            //eol显示
	unsigned char eol_horn;                            //eol喇叭
	int steering_distance;                          //转向距离
	int destination_distance;                          //目的地距离
	unsigned int clock_intersection_type;               //时钟路口类型
	unsigned char turn_by_turn;                        //转向信息
	unsigned char compass;                        // 指南针
	unsigned char acc_status;                     //自适应巡航状态
	unsigned char peps_alarm;                       //PEPS警告
	unsigned char peps_note;                       //PEPS提示信息
	unsigned char over_speed;
	unsigned char clock_intersection_guide;               //时钟路口引导方向
	char drive_mode;                         //驾驶模式
	char back_light_onoff;                   //背光开关
	char back_light;                         //背光亮度
	char pdc_onoff;                         //泊车辅助系统开关
	char pdc_status;                         //泊车辅助系统状态
	char curise_control_status;              //定速巡航状态
	float curise_control_speed;               //定速巡航速度
	char curise_control_work_status;         //定速巡航工作状态
	char key_status;                         //按键状态
	char gear;                                //档位
	char alarm[3];                            //警报类型
	char hdc;                                  //陡坡缓行
	char ignkey_status;                                  //ACC状态
	char door;                                        //车门
	char epb[2];                                       //电子刹车
	char media_type;                                 //多媒体源
	char tire_system_warning;               //胎压系统警告
	unsigned char radar[9];                           //雷达距离
	int tire_pressure_tenfold[4];                   //胎压
	unsigned char tire_pressure_warning[4];           //胎压警报
	float tire_temperature[4];                //轮胎温度
	unsigned char tire_temperature_warning[4];        //轮胎温度警报
	char car_light[2];                           //车灯
	char fuleTravle[2];                         //油量位置
	char coolandTravle[2];                         //水温表位置
	char fault[3];                                  //故障状态
	char recbuf[20][200];                         //msg queue 名字
	int recnum;                                   //msg queue 数量
	unsigned char mediasource;                    //多媒体源
	char musicName[60];                           //音乐名
	char authorName[60];                          //歌手名
	char albumName[60];                            //专辑名
	float speed;
	int rpm;
	int can_totle_meliage;                          //总里程
	int totle_meliage_tenfold_a;                      //里程小计
	int totle_meliage_tenfold_b;                      //里程小计
	float endurance_meliage;                          //续航里程
	int external_temperature;                       //外部温度
	float cooland_temperature;
	float average_fule;
	float instant_fule;
	float battery_volage;
	int maintenance_kilemeters;
	int maintenance_remaindays;
	char viewActualMqbuf[20][64];
	int viewActualMq_num;
	char audioServiceMqbuf[20][64];
	int audioServiceMq_num;
	char eolMqbuf[20][64];
	int eolMq_num;
	char ACC_ACCMode_HMI;
	char ACC_dxTarObj;
	char ACC_TauGapSet;
	char ACC_ObjValid;
	char ACC_Textinfo;
	char ACC_AEB_Request;
	char ACC_vSetDis;
	char speedload;
	char temp_unit;
	char airconditionstatus;
	char ACC_PCW_prewarning;
	int current;
	int voltage;
	int powermeter;
	char limitSpeedStatus;
	float restTime;
	short followme;
	char dtc[2];
	char gearInd;
	unsigned char WrongGearOp;						//挡位误操作
	unsigned char LDW_lamp;							//车道偏离指示灯
	unsigned char LDW_warning;						//车道偏离报警
} typemcustatusData;

typedef struct responseStatusData
{
	unsigned char left_lamp_response :1;                       //左转向灯
	unsigned char right_lamp_response :1;                      //右转向灯
	unsigned char mil_fault_response :1;                       //发动机排放系统
	unsigned char epc_fault_response :1;                       //发动机故障
	unsigned char coolant_temperature_high_response :2;        //水温高状态
	unsigned char fuel_level_response :2;                      //燃油不足
	/*************************1byte*****************************************/
	unsigned char abs_fault_response :2;                       //防抱死系统ABS故障
	unsigned char coolant_temperature_low_response :2;         //水温低状态
	unsigned char epb_lamp_response :2;                      //epb手刹灯
	unsigned char hand_brake_response :2;                      //手刹
	unsigned char charging_system_fault_response :1;           //充电系统故障
	/*************************1byte*****************************************/
	unsigned char airbag_fault_response :2;                    //安全气囊故障
	unsigned char gearbox_fault_response :3;                   //变速箱故障
	unsigned char oil_pressure_alarm_response :2;              //机油压力
	unsigned char gearbox_heat_response :2;             //变速箱过热
	/*************************1byte*****************************************/
	unsigned char gear_fault_response :1;                      //档位故障
	unsigned char body_security_response :1;                   //车身防盗
	unsigned char fl_door_response :1;                         //左前车门
	unsigned char fr_door_response :1;                         //右前车门
	unsigned char rl_door_response :1;                         //左后车门
	unsigned char rr_door_response :1;                         //右后车门
	unsigned char hood_response :1;                           //引擎盖
	unsigned char back_door_response :1;                        //后备箱
	/*************************1byte*****************************************/
	unsigned char high_beam_response :1;                       //远光指示灯
	unsigned char low_beam_response :1;                        //近光指示灯
	unsigned char door_alarm_response :2;               //门开报警状态
	unsigned char over_speed_alarm_response :2;                  //超速报警
	unsigned char back_fog_lamp_response :1;                   //后雾灯
	unsigned char front_fog_lamp_response :1;                  //前雾灯
	/*************************1byte*****************************************/
	unsigned char curise_lamp_response :1;                     //巡航灯
	unsigned char contour_lamp_response :1;                    //轮廓灯
	unsigned char eps_response :2;                           //EPS（电子控制动力转向系统故障）
	unsigned char epb_fault_response :2;                       //EPB故障
	unsigned char esc_lamp_response :1;                        //开机自检点亮
	unsigned char escoff_lamp_response :1;                     //ESCoff
	/*************************1byte*****************************************/
	unsigned char avh_fault_response :2;                       //AVH
	unsigned char drl_lamp_response :1;                        //DRL指示灯
	unsigned char tpms_response :1;                            //TPMS
	unsigned char als_lamp_response :3;                        //ALS小灯
	unsigned char ep_lamp_response :1;                         //弃用
	/*************************1byte*****************************************/
	unsigned char start_stop_no_function_response :2;          //启停故障
	unsigned char lane_depature_switch_response :1;            //车道偏离声音开关
	unsigned char brake_fluid_response :1;                     //制动液位低
	unsigned char hazzard_light_response :1;                   //双闪灯
	unsigned char parking_light_response :1;                   //停车灯(位置灯)
	unsigned char drive_belt_response :2;               //主驾驶位安全带响应状态
	/*************************1byte*****************************************/
	unsigned char ebd_fult_response :1;                        //电子制动力分配系统故障
	unsigned char copilot_belt_response :2;                    //副驾驶位安全带
	unsigned char cooling_response :1;                         //冷却液位低
	unsigned char eplamp_response :2;                         //紧急刹车灯
	unsigned char apa_error_sound_response :1;
	unsigned char apa_information_sound_response :1;
	/*************************1byte*****************************************/
	unsigned char headlight_automatic_open_fault_response :4;  //大灯自动开启故障状态
	unsigned char rain_sensor_fault_response :2;       //雨量传感器故障状态
	unsigned char epb_actionstatus_response :2;                 //电子手刹动作状态
	/*************************1byte*****************************************/
	unsigned char accumulator_discharge_response :2;            //蓄电池充放电
	unsigned char esp_active_response :2;               //esp激活状态
	unsigned char epb_status_response :2;                      //电子手刹状态
	unsigned char esp_onoff_response :1;                       //esp开关
	unsigned char esp_fault_response :1;                       //esp故障
	unsigned char automatic_vehicle_fixed_fault_response :1;    //自动车辆固定故障
	/*************************1byte*****************************************/
	unsigned char tire_alarm_response :2;                       //门开报警状态
	unsigned char over_speed_status_response :2;                  //超速提醒状态
	unsigned char hcd_status_response :2;                         //hdc状态指示
	unsigned char hdc_fault_response :2;                          //hdc 故障
	/*************************1byte*****************************************/
	unsigned char over_speed_onoff_response :1;                  //超速报警开关
	unsigned char tire_system_warning_response :2;               //胎压系统警告
	unsigned char ign_offshow_response :2;
	unsigned char lane_depature_status_response :2;               //车道偏离界面状态
	unsigned char vehicle_mainteance_onoff_response :1;            //整车保养开关
	/*************************1byte*****************************************/
	unsigned char vehicle_mainteance_notice_response :2;            //整车保养提示
	unsigned char acc_accmode_yellow_response :2;                    //自适应巡航状态黄灯
	unsigned char acc_accmode_green_response :2;                    //自适应巡航状态 绿灯
	unsigned char acc_accmode_fault_response :2;                     //自适应巡航状态故障
	/*************************1byte*****************************************/
	unsigned char acc_pcwcative_green_response :2;                     //自适应预警绿灯
	unsigned char acc_pcwcative_red_response :2;                       //自适应预警红灯
	unsigned char acc_pcwcative_latentwarning_response :2;          //自适应安全距离指示灯
	unsigned char start_and_function_fault_response :2;                 //启停系统故障
	/*************************1byte*****************************************/
	unsigned char start_and_function_work_response :2;               //启停系统工作指示灯
	unsigned char electric_seat_response :2;                		     //电动座椅
	unsigned char aeb_status_response :1;                        //aeb(自动紧急制动系统)
	unsigned char lane_depature_warning_response :1;                 //车道偏离报警
	unsigned char navi_active_onoff_response :1;                     //导航激活状态
	unsigned char intersection_type_response :1;                     //路口类型
	/*******************************1byte*************************************************************/
	unsigned char dg_type_response :4;                               //导航引导类型
	unsigned char eol_onoff_response :1;                              //EOL开关
	unsigned char belt1_sound_response :1;                             //belt1声音
	unsigned char belt2_sound_response :1;                             //belt2声音
	unsigned char oilepressurelow_sound_response :1;         //oilepressurelow声音
	/*******************************1byte*************************************************************/
	unsigned char handbrake_sound_response :1;         //handbrake声音
	unsigned char avh_sound_response :1;         //avh声音
	unsigned char dooropen_sound_response :1;         //dooropen声音
	unsigned char overspeed_sound_response :1;         //overspeed声音
	unsigned char tire_sound_response :1;         //tire声音
	unsigned char lanedepature_sound_response :1;         //lanedepature声音
	unsigned char ACC_PCW_latentWarning1_sound_response :1; //ACC_PCW_latentWarning1声音
	unsigned char ACC_TakeOverReq_sound_response :1; //ACC_TakeOverReq声音
	/*******************************1byte*************************************************************/
	unsigned char elec_sit_sound_response :1; //elec_sit声音
	unsigned char coolandtemperaturehigh_sound_response :1; //coolandtemperaturehigh声音
	unsigned char elec_sit2_sound_response :1;
	unsigned char brakefluid_low_view_response :1;

	unsigned char radar_failure_sound_response :1;
	unsigned char radar_front_on_sound_response :1;			//20190102 修改人杜源  因audioservice启动较慢，无法直接收到消息队列的中消息，需要通过共享内存来获取消息
	unsigned char radar_front_1hz_sound_response :1;
	unsigned char radar_front_2hz_sound_response :1;
	/*******************************1byte*************************************************************/
	unsigned char radar_front_4hz_sound_response :1;
	unsigned char radar_front_8hz_sound_response :1;
	unsigned char radar_rear_on_sound_response :1;
	unsigned char radar_rear_1hz_sound_response :1;
	unsigned char radar_rear_2hz_sound_response :1;
	unsigned char radar_rear_4hz_sound_response :1;
	unsigned char radar_rear_8hz_sound_response :1;
	unsigned char acc_takeover_response :1;
	/*******************************1byte -1*************************************************************/
	unsigned char insulationFault_response :2;
	unsigned char coolantWarmup_response :2;
	unsigned char keyReminder_response :1;
	unsigned char restOnoff_response :1;
	/*******************************1byte-1*************************************************************/
	unsigned char avmSysFault_response :1;
	unsigned char avmFault_response :4;
	unsigned char phone_response :1;
	unsigned char reserved_response :1;
	/*******************************状态值*************************************************************/
	unsigned char eol_show_response;                            //eol显示
	unsigned char eol_horn_response;                            //eol喇叭
	unsigned char turnbyturn_response;                          //TurnByTurn信息
	unsigned char compass_response;                        // 指南针
	unsigned int clock_intersection_type_response;               //时钟路口类型
	int steering_distance_response;                          //转向距离
	int destination_distance_response;                          //目的地距离
	char mcu_version_response[20];                        //mcu版本
	char hardwar_version_response[20];
	char cfg_version_response[20];
	char navi_switch_response;
	char fuleTravle_response[2];                         //油量位置
	char coolandTravle_response[2];                         //水温表位置
	unsigned char tire_pressure_warning_response[4];           //胎压警报
	float tire_pressure_response[4];                   //胎压
	float tire_temperature_response[4];                //轮胎温度
	unsigned char tire_temperature_warning_response[4];        //轮胎温度警报
	unsigned char radar_response[9];                           //雷达距离
	unsigned char peps_alarm_response;                       //PEPS警告
	unsigned char peps_note_response;                       //PEPS提示信息
	unsigned char over_speed_response;
	char drive_mode_response;                         //驾驶模式
	char back_light_onoff_response;                   //背光开关
	char back_light_response;                         //背光亮度
	char pdc_onoff_response;                         //泊车辅助系统开关
	char pdc_status_response;                         //泊车辅助系统状态
	char curise_control_status_response;              //定速巡航状态
	float curise_control_speed_response;               //定速巡航速度
	char curise_control_work_status_response;         //定速巡航工作状态
	char key_status_response;                         //按键状态
	char gear_response;                                //档位
	unsigned char mediasource_response;                    //多媒体源
	char musicName_response[60];                           //音乐名
	char authorName_response[60];                          //歌手名
	char albumName_response[60];                            //专辑名
	char fault_response[40][100];
	char fault_num_response;
	float speed_response;
	int rpm_response;
	int can_totle_meliage_response;                          //总里程
	float totle_meliage_a_response;                      //里程小计
	float totle_meliage_b_response;                      //里程小计
	float endurance_meliage_response;                          //续航里程
	float external_temperature_response;                       //外部温度
	float cooland_temperature_response;
	float average_fule_response;
	float instant_fule_response;
	float battery_volage_response;
	char ignkey_status_response;                                  //钥匙状态
	int maintenance_kilemeters_response;
	int maintenance_remaindays_response;
	float ACC_speed_response;
	char ACC_dxTarObj_response;
	char ACC_tauGapSet_response;
	char ACC_objvalid_response;
	char ACC_aeb_response;
	char ACC_notice_response;
	char ACC_fault_response;
	char speedload_response;
	unsigned char clock_intersection_guide_response;               //时钟路口引导方向
	char temp_unit_response;
	char temp_show_response;
	char ACC_PCW_lantentwarning2;

	char carLight_response[2];                           //车灯
	char alarm1hz_response[TELLTALE_LENGTH]; //flick data ref the enum define of flick0 and flick1
	char alarm0hz_response[TELLTALE_LENGTH]; //常亮
	int current_response;
	int voltage_response;
	int powermeter_response;
	char limitSpeedStatus_response;
	char upgradeProgress_response;
	float restTime_response;
	short followme_response;
	char dtc_response[2];
	unsigned char WrongGearO_sound : 1;			//挡位误操作声音
	unsigned char WrongGearO_view : 7;			//挡位误操作信息
} typeresponsestatusData;

typedef struct debugData
{
	char buf[512];
} typeDebugData;

typedef struct
{
	typeresponsestatusData response_status_data;
	typemcustatusData mcu_status_data;
	typeDebugData debug_data;
	pthread_mutex_t myshmemmutex;
	bool status;
} shem_t;

typedef struct versionData
{
	char name[32];
	char version[32];
} typeversionData;

typedef struct systemSharedData
{
	char language;
	char hmi_status;
	char navi_switch;
	char upgradeProgress;
	char modename[32];

} typessystemSharedData;

typedef struct
{
	int version_num;
	int updateversion_num;
	typeversionData version[5];
	typeversionData updateversion[5];
	typessystemSharedData system_data;
	pthread_mutex_t systemshmemmutex;
} systemsharedshem_t;

#pragma pack()
#endif /* SAVEDDATA_H_ */
