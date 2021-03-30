#include "Actuator.h"
#include "AudioControl.h"
#include "JsonHandler.h"
#include "Forwarder.hpp"

sem_t g_warnShowSem;

MsgHandler *Actuator::m_msgHandler = MsgHandler::GetInstance();
std::mutex Actuator::m_mtxMsgListen;

Actuator::Actuator() : m_xmlManager(XmlManager::GetInstance())
{
    /*while*/ if ((this->m_fdShareMemMcuData = shm_open(MCU_DATA_SHARED, O_RDWR, S_IRWXU)) == -1)
    {
        ErrPrint("Failed to open shared data [%s]!\n", MCU_DATA_SHARED);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    this->m_pShareMemMcuData = (shem_t *)mmap(0, sizeof(shem_t), PROT_READ | PROT_WRITE,
                                              MAP_SHARED, m_fdShareMemMcuData, 0);

    m_mqttClient = std::make_shared<MqttConnection>(MQTT_HOST, MQTT_PORT, BDSTAR_MOSQUITTP);

    std::vector<std::string> strTopicList;
    strTopicList.emplace_back(std::string(MQTT_TOPIC_WARN_VIEW));
    m_mqttClient->Subscribe(strTopicList);
    m_mqttClient->Run();
}

Actuator::~Actuator()
{
    if (this->m_pShareMemMcuData != NULL)
    {
        munmap(m_pShareMemMcuData, sizeof(shem_t));
        close(m_fdShareMemMcuData);
        shm_unlink(MCU_DATA_SHARED);
        WarnShowTimerDelete();
    }
}

void Actuator::Run()
{
    std::thread msgRecieveTh(std::bind(&Actuator::MsgReciever, this));
    msgRecieveTh.detach();
    // m_msgHandler->start();

    // pthread_mutex_lock(&m_pShareMemMcuData->myshmemmutex);
    // bool isIgnOFF = IsIgnOFFCheck();
    // InitAllWarnInfo(isIgnOFF);
    // pthread_mutex_unlock(&m_pShareMemMcuData->myshmemmutex);

    while (1)
    {
        InfoPrint("RUN......\n");
        // sem_wait(&g_warnShowSem);
        // m_msgHandler->RemoveCurrentWarn();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}


void Actuator::MsgReciever()
{
    while (1)
    {
        std::string strMsg = Forwarder::GetInstance()->MsgPop();
        if (!strMsg.empty())
        {
            MsgProcessor(strMsg);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Actuator::MsgProcessor(std::string strMsg)
{
    InfoPrint("Message:[%s] Comming\r\n", strMsg.c_str());
}

void Actuator::InitAllWarnInfo(bool isIgnOFF)
{
    InitPEPS(isIgnOFF);
    InitDoor(isIgnOFF);
    InitEPB(isIgnOFF);
    InitPhoneForget();

    if (!isIgnOFF)
    {
        InitSomeSeriousWarn();
        InitOverSpeed();
        InitFollowMeHome();
        InitTPMS();
        InitBrakeFluid();

        InitAVH();
        InitCDP();
        InitSeatBelt();
        InitAirBag();
        InitABS();
        InitESP();
        InitHDC();
        InitEPS();
        InitGearBox();
        InitAVM();
        InitSportMode();
        InitFatigueDriving();
        InitFuelLow();
        InitWarmUp();
        InitLDW();
        InitWrongGear();
        InitDTC();
        InitSocLow();
        InitParkingLight();
    }
}

/** 
 * @brief 部分严重报警信息初始化
 */
void Actuator::InitSomeSeriousWarn()
{
    std::string seriousWarnNameList[] = {
        "oilpressurelow",  //机油压力低
        "sys_fault",       //整车系统故障
        "batt_fault",      //动力电池故障
        "drive_fault",     //电驱动系统故障
        "battdisconnect"}; //动力电池断开

    switch (this->m_pShareMemMcuData->response_status_data.oil_pressure_alarm_response)
    {
    case 0:
        break;
    default:
        this->m_msgHandler->SetWarnView(seriousWarnNameList[0].c_str(),
                                        seriousWarnNameList[0].c_str(), VIEW_ON);
        break;
    }

    if (1 == this->m_pShareMemMcuData->mcu_status_data.systemFault)
    {
        this->m_msgHandler->SetWarnView(seriousWarnNameList[1].c_str(),
                                        seriousWarnNameList[1].c_str(), VIEW_ON);
    }

    if (1 == this->m_pShareMemMcuData->mcu_status_data.powerbattFault)
    {
        this->m_msgHandler->SetWarnView(seriousWarnNameList[2].c_str(),
                                        seriousWarnNameList[2].c_str(), VIEW_ON);
    }

    if (1 == this->m_pShareMemMcuData->mcu_status_data.electricDriveFault)
    {
        this->m_msgHandler->SetWarnView(seriousWarnNameList[3].c_str(),
                                        seriousWarnNameList[3].c_str(), VIEW_ON);
    }

    if (1 == this->m_pShareMemMcuData->mcu_status_data.powerbattDisconnectFault)
    {
        this->m_msgHandler->SetWarnView(seriousWarnNameList[4].c_str(),
                                        seriousWarnNameList[4].c_str(), VIEW_ON);
    }
}

/** 
 * @brief PEPS警告/提示信息初始化
 * @param isIgnOFF      ACC_OFF是否成立
 *        关于ignore    见view.xml中对于power的支持，如果与当前IGN状态不匹配的话就不会触发
 *                      IGN_OFF: wm2
 *                      非IGN_OFF: wm1
 */
void Actuator::InitPEPS(bool isIgnOFF)
{
    std::string pepsAlarmNameList[] = {
        "pepspoweroff",   //请关闭电源
        "pepsnotfindb",   //未检测到智能钥匙B
        "pepsincar",      //请注意车内有智能钥匙
        "pepsnotfindc",   //未检测到智能钥匙C
        "pepsnotfinda",   //未检测到智能钥匙A（严重报警）
        "pepsbatterylow", //智能钥匙电量低
        "pepsswitchtoNP", //请挂入P挡或N挡启动
        "pepsstepbreak"}; //启动请踩制动踏板

    if (isIgnOFF)
    {
        pepsAlarmNameList[6] = "ignore";
    }
    else
    {
        pepsAlarmNameList[0] = "ignore";
    }

    char alarmBits = this->m_pShareMemMcuData->response_status_data.peps_alarm_response;
    for (size_t index = 0; index < GET_ARRAY_LEN(pepsAlarmNameList); ++index)
    {
        if (GET_BIT(alarmBits, index) &&
            0 != strcmp(pepsAlarmNameList[index].c_str(), "ignore"))
        {
            this->m_msgHandler->SetWarnView(pepsAlarmNameList[index].c_str(),
                                            pepsAlarmNameList[index].c_str(), VIEW_ON);
        }
    }

    std::string pepsNoteNameList[] = {
        "pepsswitchtoPStart", //请挂入P挡
        "pepsnotcomplete"};   //无钥匙启动系统在线配置未完成

    if (!isIgnOFF)
    {
        pepsNoteNameList[0] = "ignore";
    }
    char noteBits = this->m_pShareMemMcuData->response_status_data.peps_note_response;
    for (size_t index = 0; index < GET_ARRAY_LEN(pepsNoteNameList); ++index)
    {
        if (GET_BIT(noteBits, index) &&
            0 != strcmp(pepsNoteNameList[index].c_str(), "ignore"))
        {
            this->m_msgHandler->SetWarnView(pepsNoteNameList[index].c_str(),
                                            pepsNoteNameList[index].c_str(), VIEW_ON);
        }
    }
}

/** 
 * @brief 超速报警信息初始化
 */
void Actuator::InitOverSpeed()
{
    switch (this->m_pShareMemMcuData->response_status_data.over_speed_alarm_response)
    {
    case 1:
        this->m_msgHandler->SetWarnView("speedlimit", "speedlimit", VIEW_ON);
        break;
    default:
        break;
    }
}

/** 
 * @brief 伴我回家提示信息初始化
 */
void Actuator::InitFollowMeHome()
{
    if (0 < this->m_pShareMemMcuData->response_status_data.followme_response)
    {
        this->m_msgHandler->SetWarnView("followme", "followme", VIEW_ON);
    }
}

/** 
 * @brief 胎压故障报警信息初始化
 */
void Actuator::InitTPMS()
{
    switch (this->m_pShareMemMcuData->response_status_data.tire_system_warning_response)
    {
    case 0:
        break;
    default:
        this->m_msgHandler->SetWarnView("tiresystemfault", "tiresystemfault", VIEW_ON);
        break;
    }
}

/** 
 * @brief 制动液位低报警信息初始化
 */
void Actuator::InitBrakeFluid()
{
    switch (this->m_pShareMemMcuData->response_status_data.brake_fluid_response)
    {
    case 0:
        break;
    default:
        this->m_msgHandler->SetWarnView("brakefluidlow", "brakefluidlow", VIEW_ON);
        break;
    }
}

/** 
 * @brief 诊断信息初始化
 */
void Actuator::InitDTC()
{
    std::string dtcWarnPart1List[] = {
        "dtc_radar", //与倒车雷达模块失去通讯
        "dtc_ems",   //与发动机控制器失去通讯
        "dtc_tcu",   //与变速箱控制器失去通讯
        "dtc_abs",   //与制动系统控制器失去通讯
        "dtc_eps",   //与电动助力转向控制器失去通讯
        "dtc_bcm",   //车身控制器失去通讯
        "dtc_abm",   //气囊控制器失去通讯
        "dtc_peps"}; //无钥匙系统控制器失去通讯

    char warnPart1Bits = this->m_pShareMemMcuData->mcu_status_data.dtc[0];
    for (size_t index = 0; index < GET_ARRAY_LEN(dtcWarnPart1List); ++index)
    {
        if (GET_BIT(warnPart1Bits, index))
        {
            this->m_msgHandler->SetWarnView(dtcWarnPart1List[index].c_str(),
                                            dtcWarnPart1List[index].c_str(), VIEW_ON);
        }
    }

    std::string dtcWarnPart2List[] = {
        "dtc_ac",   //空调控制器失去通讯
        "dtc_ihu",  //音响控制器失去通讯
        "dtc_epb",  //电子手刹控制器失去通讯
        "dtc_tpms", //胎压监测模块失去通讯
        "dtc_avm"}; //全景控制器失去通讯

    char warnPart2Bits = this->m_pShareMemMcuData->mcu_status_data.dtc[1];
    for (size_t index = 0; index < GET_ARRAY_LEN(dtcWarnPart2List); ++index)
    {
        if (GET_BIT(warnPart2Bits, index))
        {
            this->m_msgHandler->SetWarnView(dtcWarnPart2List[index].c_str(),
                                            dtcWarnPart2List[index].c_str(), VIEW_ON);
        }
    }
}

/** 
 * @brief AUTO HOLD 自动驻车报警信息初始化
 */
void Actuator::InitAVH()
{
    std::string avhWarnList[] = {
        "autohold_active", //自动驻车功能开启
        "autohold_fail",   //自动驻车失效报警
        "autohold_fault"}; //自动驻车故障提示

    switch (this->m_pShareMemMcuData->mcu_status_data.avh_fault)
    {
    case 1:
    case 3:
        this->m_msgHandler->SetWarnView(avhWarnList[0].c_str(),
                                        avhWarnList[0].c_str(), VIEW_ON);
        break;
    default:
        break;
    }

    switch (this->m_pShareMemMcuData->mcu_status_data.avhFail)
    {
    case 0:
        break;
    default:
        this->m_msgHandler->SetWarnView(avhWarnList[1].c_str(),
                                        avhWarnList[1].c_str(), VIEW_ON);
        break;
    }

    switch (this->m_pShareMemMcuData->mcu_status_data.avhFault)
    {
    case 0:
        break;
    default:
        this->m_msgHandler->SetWarnView(avhWarnList[2].c_str(),
                                        avhWarnList[2].c_str(), VIEW_ON);
        break;
    }
}

/** 
 * @brief CDP失效报警信息初始化
 *        ESP 动态制动功能失效
 */
void Actuator::InitCDP()
{
    if (this->m_pShareMemMcuData->mcu_status_data.cdp == 1)
    {
        this->m_msgHandler->SetWarnView("esp_cdp", "esp_cdp", VIEW_ON);
    }
}

/** 
 * @brief 门开报警/提示信息初始化
 * @param isIgnOFF      ACC_OFF是否成立
 *        ACC_OFF：     由于尚未开启IGN，车速为0，因此仅有“门开提示”功能；
 */
void Actuator::InitDoor(bool isIgnOFF)
{
    std::string doorWarnInfo[] = {
        "doorinfo",  //门开提示
        "doorwarn"}; //门开报警

    std::string doorNameList[] = {
        "DRIVER_DOOR",     //左前车门
        "PASSENGER_DOOR",  //右前车门
        "HOOD",            //引擎盖
        "REAR_RIGHT_DOOR", //右后车门
        "REAR_LEFT_DOOR",  //左后车门
        "BACK_DOOR"};      //后备箱

    float limitSpeedValue = 3.0; //3.0km/h
    int isWarnFlag = (this->m_pShareMemMcuData->response_status_data.speed_response < limitSpeedValue) ? 0 : 1;
    if (isIgnOFF)
    {
        isWarnFlag = 0;
    }

    if (this->m_pShareMemMcuData->response_status_data.fl_door_response != 0)
    {
        this->m_msgHandler->SetWarnView(doorWarnInfo[isWarnFlag].c_str(),
                                        doorNameList[0].c_str(), VIEW_ON);
    }

    if (this->m_pShareMemMcuData->response_status_data.fr_door_response != 0)
    {
        this->m_msgHandler->SetWarnView(doorWarnInfo[isWarnFlag].c_str(),
                                        doorNameList[1].c_str(), VIEW_ON);
    }

    if (this->m_pShareMemMcuData->response_status_data.hood_response != 0)
    {
        this->m_msgHandler->SetWarnView(doorWarnInfo[isWarnFlag].c_str(),
                                        doorNameList[2].c_str(), VIEW_ON);
    }

    if (this->m_pShareMemMcuData->response_status_data.rr_door_response != 0)
    {
        this->m_msgHandler->SetWarnView(doorWarnInfo[isWarnFlag].c_str(),
                                        doorNameList[3].c_str(), VIEW_ON);
    }

    if (this->m_pShareMemMcuData->response_status_data.rl_door_response != 0)
    {
        this->m_msgHandler->SetWarnView(doorWarnInfo[isWarnFlag].c_str(),
                                        doorNameList[4].c_str(), VIEW_ON);
    }

    if (this->m_pShareMemMcuData->response_status_data.back_door_response != 0)
    {
        this->m_msgHandler->SetWarnView(doorWarnInfo[isWarnFlag].c_str(),
                                        doorNameList[5].c_str(), VIEW_ON);
    }
}

/** 
 * @brief  主驾/副驾安全带未系报警信息初始化
 */
void Actuator::InitSeatBelt()
{
    float limitSpeedValue = 25.0; //25.0km/h
    if (this->m_pShareMemMcuData->response_status_data.speed_response < limitSpeedValue)
    {
        return;
    }

    std::string beatBeltInfo[] = {
        "mainbelt",       //主驾安全带
        "passengerbelt"}; ///副驾安全带

    if (this->m_pShareMemMcuData->response_status_data.drive_belt_response != 0)
    {
        this->m_msgHandler->SetWarnView(beatBeltInfo[0].c_str(),
                                        beatBeltInfo[0].c_str(), VIEW_ON);
    }

    if (this->m_pShareMemMcuData->response_status_data.copilot_belt_response != 0)
    {
        this->m_msgHandler->SetWarnView(beatBeltInfo[1].c_str(),
                                        beatBeltInfo[1].c_str(), VIEW_ON);
    }
}

/** 
 * @brief  安全气囊故障报警信息初始化
 */
void Actuator::InitAirBag()
{
    if (3 == this->m_pShareMemMcuData->response_status_data.airbag_fault_response)
    {
        this->m_msgHandler->SetWarnView("airbag", "airbag", VIEW_ON);
    }
}

/** 
 * @brief  ABS故障报警信息初始化
 */
void Actuator::InitABS()
{
    if (2 == this->m_pShareMemMcuData->response_status_data.abs_fault_response)
    {
        this->m_msgHandler->SetWarnView("abs_check", "abs_check", VIEW_ON);
    }
}

/** 
 * @brief  ESP电子稳定程序故障报警信息初始化
 */
void Actuator::InitESP()
{
    if (1 == this->m_pShareMemMcuData->response_status_data.esp_active_response)
    {
        this->m_msgHandler->SetWarnView("esp_check", "esp_check", VIEW_ON);
    }
}

/** 
 * @brief  EPS（电子控制动力转向系统）故障报警信息初始化
 */
void Actuator::InitEPS()
{
    std::string epsWarnList[] = {
        "eps_check", //EPS故障报警:请检查助力转向系统
        "eps_init"}; //EPS标定未完成:EPS转角初始化未完成

    if (1 == this->m_pShareMemMcuData->response_status_data.eps_response)
    {
        this->m_msgHandler->SetWarnView(epsWarnList[0].c_str(),
                                        epsWarnList[0].c_str(), VIEW_ON);
    }
    else if (2 == this->m_pShareMemMcuData->response_status_data.eps_response)
    {
        this->m_msgHandler->SetWarnView(epsWarnList[1].c_str(),
                                        epsWarnList[1].c_str(), VIEW_ON);
    }
}

/** 
 * @brief  变速箱故障/CVT油温高/DCT离合器温度高报警信息初始化
 */
void Actuator::InitGearBox()
{
    std::string gearBoxWarnList[] = {
        "gearbox_temp", //EPS故障报警:请检查助力转向系统
        "gearbox_overheat",
        "gearbox_check"}; //EPS标定未完成:EPS转角初始化未完成

    switch (this->m_pShareMemMcuData->response_status_data.gearbox_fault_response)
    {
    case 1:
        this->m_msgHandler->SetWarnView(gearBoxWarnList[0].c_str(),
                                        gearBoxWarnList[0].c_str(), VIEW_ON);
        break;
    case 2:
        this->m_msgHandler->SetWarnView(gearBoxWarnList[1].c_str(),
                                        gearBoxWarnList[1].c_str(), VIEW_ON);
        break;
    case 3:
        this->m_msgHandler->SetWarnView(gearBoxWarnList[2].c_str(),
                                        gearBoxWarnList[2].c_str(), VIEW_ON);
        break;
    default:
        break;
    }
}

/** 
 * @brief 电子刹车报警信息初始化
 * @param isIgnOFF      ACC_OFF是否成立
 *     特殊情况的判断    见view.xml中对于power的支持，如果与当前IGN状态不匹配的话就不会触发
 *                      IGN_OFF: wm2
 *                      非IGN_OFF: wm1
 */
void Actuator::InitEPB(bool isIgnOFF)
{
    std::string epbWarnList[] = {
        "epb_stepbreak", //释放驻车时请踩下制动踏板
        "epb_door",      //请关闭车门且系好安全带
        "epb_slope"};    //驻车坡度过大请注意安全

    if (isIgnOFF)
    {
        char epbBits = this->m_pShareMemMcuData->mcu_status_data.epb[1];
        epbBits = (epbBits >> 1) & 0x3;
        if (3 == epbBits)
        {
            this->m_msgHandler->SetWarnView(epbWarnList[2].c_str(),
                                            epbWarnList[2].c_str(), VIEW_ON);
        }
        return;
    }

    switch (this->m_pShareMemMcuData->mcu_status_data.epbWarning)
    {
    case 1:
        this->m_msgHandler->SetWarnView(epbWarnList[0].c_str(),
                                        epbWarnList[0].c_str(), VIEW_ON);
        break;
    case 2:
        this->m_msgHandler->SetWarnView(epbWarnList[1].c_str(),
                                        epbWarnList[1].c_str(), VIEW_ON);
        break;
    case 3:
        this->m_msgHandler->SetWarnView(epbWarnList[2].c_str(),
                                        epbWarnList[2].c_str(), VIEW_ON);
        break;
    default:
        break;
    }
}

/** 
 * @brief 下坡辅助系统报警信息初始化
 */
void Actuator::InitHDC()
{
    std::string hdcWarnList[] = {
        "hdc_active", //下坡辅助开启:陡坡缓降功能开启
        "hdc_fault"}; //下坡辅助系统故障:陡坡缓降功能失效

    if (this->m_pShareMemMcuData->mcu_status_data.hcd_status == 2)
    {
        this->m_msgHandler->SetWarnView(hdcWarnList[0].c_str(),
                                        hdcWarnList[0].c_str(), VIEW_ON);
    }

    if (this->m_pShareMemMcuData->mcu_status_data.hdc_fault != 0)
    {
        this->m_msgHandler->SetWarnView(hdcWarnList[1].c_str(),
                                        hdcWarnList[1].c_str(), VIEW_ON);
    }
}

/** 
 * @brief 全景故障报警信息初始化
 */
void Actuator::InitAVM()
{
    std::string avmWarnList[] = {
        "avm_check",  //全景系统故障
        "avm_fault"}; //全景探头故障

    if (1 == this->m_pShareMemMcuData->response_status_data.avmSysFault_response)
    {
        this->m_msgHandler->SetWarnView(avmWarnList[0].c_str(),
                                        avmWarnList[0].c_str(), VIEW_ON);
    }

    if (0 < this->m_pShareMemMcuData->response_status_data.avmFault_response)
    {
        char avmFaultBits = this->m_pShareMemMcuData->response_status_data.avmFault_response;
        if (GET_BIT(avmFaultBits, 0))
        {
            /* 请检查后摄像头 */
            this->m_msgHandler->SetWarnView(avmWarnList[1].c_str(), "rear", VIEW_ON);
        }
        else if (GET_BIT(avmFaultBits, 1))
        {
            /* 请检查前摄像头 */
            this->m_msgHandler->SetWarnView(avmWarnList[1].c_str(), "front", VIEW_ON);
        }
        else if (GET_BIT(avmFaultBits, 2))
        {
            /* 请检查右摄像头 */
            this->m_msgHandler->SetWarnView(avmWarnList[1].c_str(), "right", VIEW_ON);
        }
        else if (GET_BIT(avmFaultBits, 3))
        {
            /* 请检查左摄像头 */
            this->m_msgHandler->SetWarnView(avmWarnList[1].c_str(), "left", VIEW_ON);
        }
    }
}

/** 
 * @brief SPORT模式切换信息初始化
 */
void Actuator::InitSportMode()
{
    if (2 == this->m_pShareMemMcuData->response_status_data.drive_mode_response)
    {
        this->m_msgHandler->SetWarnView("sport", "sport", VIEW_ON);
    }
}

/** 
 * @brief 疲劳驾驶提示信息初始化
 */
void Actuator::InitFatigueDriving()
{
    switch (this->m_pShareMemMcuData->mcu_status_data.vehicle_mainteance_notice)
    {
    case 0x01:
    case 0x02:
        this->m_msgHandler->SetWarnView("restplease", "restplease", VIEW_ON);
        break;
    }
}

/** 
 * @brief 低燃油报警信息初始化
 */
void Actuator::InitFuelLow()
{
    switch (this->m_pShareMemMcuData->response_status_data.fuel_level_response)
    {
    case 0:
        break;
    default:
        this->m_msgHandler->SetWarnView("oillow", "oillow", VIEW_ON);
        break;
    }
}

/** 
 * @brief 暖机提示信息初始化
 */
void Actuator::InitWarmUp()
{
    if (1 == this->m_pShareMemMcuData->response_status_data.coolantWarmup_response)
    {
        this->m_msgHandler->SetWarnView("warmup", "warming", VIEW_ON);
    }
}

/** 
 * @brief 手机遗忘提示信息初始化
 */
void Actuator::InitPhoneForget()
{
    if (1 == this->m_pShareMemMcuData->response_status_data.phone_response)
    {
        this->m_msgHandler->SetWarnView("phone", "phone", VIEW_ON);
    }
}

/** 
 * @brief 车道偏离报警信息初始化
 */
void Actuator::InitLDW()
{
    std::string ldwWarnList[] = {
        "ldw_L",  //向左偏离
        "ldw_R"}; //向右偏离

    switch (this->m_pShareMemMcuData->response_status_data.lane_depature_status_response)
    {
    // case 0:
    //     this->m_msgHandler->SetWarnView(ldwWarnList[0].c_str(),
    //                                     ldwWarnList[0].c_str(), VIEW_OFF);
    //     this->m_msgHandler->SetWarnView(ldwWarnList[1].c_str(),
    //                                     ldwWarnList[1].c_str(), VIEW_OFF);
    //     break;
    case 1:
        this->m_msgHandler->SetWarnView(ldwWarnList[0].c_str(),
                                        ldwWarnList[0].c_str(), VIEW_ON);
        break;
    case 2:
        this->m_msgHandler->SetWarnView(ldwWarnList[1].c_str(),
                                        ldwWarnList[1].c_str(), VIEW_ON);
        break;
    default:
        break;
    }
}

/** 
 * @brief 档位误操作报警信息初始化
 */
void Actuator::InitWrongGear()
{
    std::string wrongGearWarnList[] = {
        "WrongGear",         //状态恢复
        "WrongGear_Right",   //请右移换挡
        "WrongGear_pedal",   //请踩刹车再换挡
        "WrongGear_P",       //请停车后再按P键
        "WrongGear_high",    //车速过高无法换挡
        "WrongGear_DtoM",    //请在D挡时再挂M挡
        "WrongGear_Unlock"}; //请按解锁键再换挡

    int index = this->m_pShareMemMcuData->response_status_data.WrongGearO_view;
    switch (index)
    {
    // case 0:
    //     this->m_msgHandler->SetWarnView(wrongGearWarnList[0].c_str(),
    //                                     wrongGearWarnList[0].c_str(), VIEW_OFF);
    //     break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        this->m_msgHandler->SetWarnView(wrongGearWarnList[index].c_str(),
                                        wrongGearWarnList[index].c_str(), VIEW_ON);
        break;
    default:
        break;
    }
}

/** 
 * @brief 低荷电报警信息初始化
 */
void Actuator::InitSocLow()
{
    if (1 == this->m_pShareMemMcuData->mcu_status_data.socLow)
    {
        this->m_msgHandler->SetWarnView("soclow", "soclow", VIEW_ON);
    }
}

/** 
 * @brief 小灯未关报警信息初始化
 */
void Actuator::InitParkingLight()
{
    if (1 == this->m_pShareMemMcuData->mcu_status_data.parking_light)
    {
        this->m_msgHandler->SetWarnView("parking", "parking", VIEW_ON);
    }
}

static void WarnShowTimerThreadFun(union sigval arg)
{
    sem_post(&g_warnShowSem);
}

int Actuator::WarnShowTimerInit()
{
    struct sigevent event;
    int retCode;

    event.sigev_notify = SIGEV_THREAD;
    event.sigev_value.sival_ptr = NULL;
    event.sigev_notify_function = WarnShowTimerThreadFun;
    event.sigev_notify_attributes = NULL;

    retCode = timer_create(CLOCK_MONOTONIC, &event, &this->m_timerId);
    if (retCode != EOK)
    {
        ErrPrint("Failed to Create WarnShow Timer. ErrCode:[%s].\n", strerror(retCode));
        exit(1);
    }

    return retCode;
}

int Actuator::WarnShowTimerStart(int iTimeWaitBeforeStart)
{
    struct itimerspec iTimer;
    int retCode;

    /* The time wait for first start. */
    iTimer.it_value.tv_sec = iTimeWaitBeforeStart; // 3/5
    iTimer.it_value.tv_nsec = 0;

    /* the interval after start. */
    iTimer.it_interval.tv_sec = 0;
    iTimer.it_interval.tv_nsec = 0;

    retCode = timer_settime(this->m_timerId, 0, &iTimer, NULL);
    if (retCode != EOK)
    {
        ErrPrint("Failed to Set WarnShow Time. ErrCode:[%s].\n", strerror(retCode));
    }

    return retCode;
}

int Actuator::WarnShowTimerStop()
{
    struct itimerspec iTimer;
    int retCode;

    /* The time wait for first start.When value is 0, mean stop it. */
    iTimer.it_value.tv_sec = 0;
    iTimer.it_value.tv_nsec = 0;

    /* the interval after start. */
    iTimer.it_interval.tv_sec = 0;
    iTimer.it_interval.tv_nsec = 0;

    retCode = timer_settime(this->m_timerId, 0, &iTimer, NULL);
    if (retCode != EOK)
    {
        ErrPrint("Failed to Set WarnShow Timer. ErrCode:[%s].\n", strerror(retCode));
    }

    return retCode;
}

int Actuator::WarnShowTimerDelete()
{
    int retCode = timer_delete(this->m_timerId);
    if (retCode != EOK)
    {
        ErrPrint("Failed to Delete WarnShow Timer. ErrCode:[%s].\n", strerror(retCode));
    }

    return retCode;
}

bool Actuator::IsIgnOFFCheck()
{
    if (ACC_OFF == this->m_pShareMemMcuData->response_status_data.ignkey_status_response)
    {
        return true;
    }

    return false;
}
