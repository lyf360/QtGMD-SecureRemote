#ifndef __SMART_DEVICE_MQTT_HEAD__
#define __SMART_DEVICE_MQTT_HEAD__

#include <iostream>
#include <vector>
// #include <sys/types.h>          /* See NOTES */
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <cstring>
// #include <unistd.h>
#include "MqttCommunication.h"

using namespace std;

class SmartDeviceMqtt:public MqttCommunication{
    public:
        SmartDeviceMqtt(const string publishtopic,const string receivetopic);
        bool sendsocket(const vector<uint8_t> senddata);
        bool receivesocket(vector<uint8_t> &recdata,uint32_t timeout=-1);
        // string peerport{"9999"};
        // string peerip{"127.0.0.1"};

    private:
    
};


#endif
