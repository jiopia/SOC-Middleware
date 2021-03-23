#include "CoreMsgHandler.hpp"

CoreMsgHandler::CoreMsgHandler()
{
    CoreMqttInit();
}

CoreMsgHandler::~CoreMsgHandler()
{
}

void CoreMsgHandler::CoreMqttInit(const std::string &host, const int port)
{
    _host = host;
    _port = port;

    // init subscribe struct
    _mosq = mosquitto_new(nullptr, true, nullptr);
    if (mosquitto_connect(_mosq, _host.c_str(), _port, _keep_alive) != MOSQ_ERR_SUCCESS)
    {
        std::cerr << "connect error!!" << std::endl;
        _connected = false;
        exit(-1);
    }
    else
    {
        _connected = true;
    }
    std::cout << "subscribe init finished" << std::endl;

    // init recive callback
    mosquitto_lib_init();

    auto register_callback = [=] {
        mosquitto_subscribe_callback(
            &CoreMsgHandler::CoreMsgHandlerMqttCallBack, NULL,
            "MessageHandler/#", 0,
            _host.c_str(), _port,
            NULL, 60, true,
            NULL, NULL,
            NULL, NULL);
        // this api can not return??
    };
    std::thread register_callback_thread(register_callback);
    register_callback_thread.detach();
    std::cout << "finished callback init" << std::endl;
}

int CoreMsgHandler::CoreMqttCallBack(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg)
{
    std::cout << "FROM topic: " << msg->topic << std::endl;
    std::string strWarnMsg = (const char *)msg->payload;
    std::cout << "GOT message:\n"
              << strWarnMsg << std::endl;

    std::lock_guard<std::mutex> lockGuard(m_mtxMsgListen);
    if (!strWarnMsg.empty())
    {
        InfoPrint("Recieve Warning Message From MCU:[%s]\r\n", strWarnMsg.c_str());
        std::string strViewName, strExtraInfo, strViewStatus;
        JsonHandler::GetInstance()->WarnMsgParse(strWarnMsg, strViewName, strExtraInfo, strViewStatus);
        if (!strViewName.empty() &&
            !strExtraInfo.empty() &&
            !strViewStatus.empty())
        {
           CoreMsgHandler::m_msgHandler->SetWarnView(strViewName.c_str(),
                                            strExtraInfo.c_str(), strViewStatus.compare("ON") == 0 ? VIEW_ON : VIEW_OFF);
        }
    }

    return 0;
}

void CoreMsgHandler::run()
{
    while (1)
    {
        InfoPrint("RUN......\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}