#ifndef __MOSQMESSAGE_HEAD__
#define __MOSQMESSAGE_HEAD__

#include <iostream>
#include <mosquitto.h>
#include <stdlib.h>
#include <string.h>
#include <memory>


using namespace std;

class MosqMessage
{

public:
    MosqMessage(const mosquitto_message *mosq_message);
    ~MosqMessage();
    char *getpayload();

    int getqos();

    char *gettopic();

    int getpayloadlen();

    class Message
    {
    public:
        Message(const mosquitto_message *mosq_message);

        ~Message();

        char *getpayload();

        int getqos();

        char *gettopic();

        int getpayloadlen();
    private:
        mosquitto_message message;
    };

private:
    std::shared_ptr<Message> ptr;
};

/*
struct mosquitto_message{
    int mid;
    char *topic;
    void *payload;
    int payloadlen;
    int qos;
    bool retain;
};
*/

#endif