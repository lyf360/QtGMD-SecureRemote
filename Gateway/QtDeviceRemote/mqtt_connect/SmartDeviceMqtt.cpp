#include "SmartDeviceMqtt.h"

SmartDeviceMqtt::SmartDeviceMqtt(const string publishtopic, const string receivetopic):MqttCommunication(publishtopic,receivetopic)
{
    int ret = this->connect_server("fuji.lmq.cloudamqp.com",1883);       //    MQTT IP          192.168.124.220        3.114.210.215  fuji.lmq.cloudamqp.com
    if(ret<0)
    {
        cout << "connect_server  failure."<<endl;
    }
}

bool SmartDeviceMqtt::sendsocket(const vector<uint8_t> senddata)
{
    //cout << "a" << endl;
    string str(senddata.begin(),senddata.end());
    int ret = this->publish_message(str);
    if(ret<0)
    {
        cout << "sendsocket failure" << endl;
        return false;
    }
    //cout << "b" << endl;
    return true;
}

bool SmartDeviceMqtt::receivesocket(vector<uint8_t> &recdata, uint32_t timeout)
{
    //cout << "a" << endl;
    //cout << "b" << endl;
    //cout << "c  mqtt" << endl;

    char buf[1024] = {0};
    int ret = this->receive_message_data(buf);
    cout << " buf strlen" << strlen(buf) << endl;
    cout << " buf " << string(buf) << endl;
    recdata = vector<uint8_t>(reinterpret_cast<const uint8_t*>(buf),reinterpret_cast<const uint8_t*>(buf)+strlen(buf));
    if(ret < 0)
    {
        cout << "receive_message_data failure" << endl;
        return false;
    }

    //cout << "d  mqtt" << endl;
    return true;
}
