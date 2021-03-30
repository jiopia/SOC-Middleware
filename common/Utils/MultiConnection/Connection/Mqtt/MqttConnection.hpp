#ifndef _MQTT_CONNECTION_
#define _MQTT_CONNECTION_

#include "mosquitto.h"
#include "Util.h"
#include "BaseConnection.hpp"
#include "Forwarder.hpp"

#define MQTT_HOST "localhost"
#define MQTT_PORT 1883

#define PUB_QOS 1
#define SUB_QOS 1

#define MQTT_TOPIC_WARN_VIEW "BDSTAR/view"
#define MQTT_TOPIC_WARN_AUDIO "BDSTAR/audio"

class MqttConnection : public BaseConnection
{
public:
    MqttConnection(string host, int port, ConnectType type);
    ~MqttConnection();

    void Init();
    void Run();
    void Connect();
    void DisConnect();
    void MsgRecieve();
    void MsgSend(std::string strTopic, std::string strMsg);

    void Subscribe(std::vector<string> &strTopicList);
    void UnSubscribe(std::vector<string> &strTopicList);

private:
    struct mosquitto *m_ptrMosq = NULL;
    int m_iKeepAlive = 60;
    int m_mid = 0;

    static void on_connect_callback(struct mosquitto *mosq, void *obj, int rc);
    static void on_disconnect_callback(struct mosquitto *mosq, void *obj, int result);
    static void on_publish_callback(struct mosquitto *mosq, void *obj, int mid);
    static void on_subscribe_callback(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos);
    static void on_message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);
};

#endif //!_MQTT_CONNECTION_