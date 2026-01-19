#ifndef __MQTT_COMMUNICATION_HEAD__
#define __MQTT_COMMUNICATION_HEAD__

#include <iostream>
#include <mosquitto.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#include "MosqMessage.h"

using namespace std;

#define MQTT_USER "xjrdqaaw:xjrdqaaw"

#define MQTT_PASS "tqGudw_cSIJj5mSMrAdL7XwwdIiRv1D2"



class MqttCommunication
{
public:
    MqttCommunication(const string publishtopic,const string receivetopic,int qos=0);

    // 连接
    int connect_server(const string serverip,int port);
    // 断开连接
    int disconnect_server();
    // 发布
    int publish_message(string message);
    // 订阅
    int subscribe_message();

    // 连接回调
    static void connnect_callback(struct mosquitto *mosq, void *obj, int rc);
    // 断开连接回调
    static void dis_connect_callback( struct mosquitto *mosq,void *obj, int rc);
    // 接收消息回调
    static void receive_message_callback(struct mosquitto *mosq, void *self_message, const struct mosquitto_message *mosq_message);
    

    int receive_message_data(char* buf);
    void pause();
    void resume();
    ~MqttCommunication();
private:

    queue<MosqMessage> QueueMessage;
    struct mosquitto *mosq{nullptr};
    string publishtopic;
    string receivetopic;

    mutex mut;
    condition_variable cv;
    bool ispause{true};
    int qos;
};

#endif