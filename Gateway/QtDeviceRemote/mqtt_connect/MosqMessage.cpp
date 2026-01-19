#include "MosqMessage.h"

MosqMessage::MosqMessage(const mosquitto_message *mosq_message) : ptr(new Message(mosq_message))
{
    if (!ptr) {
        cout << "MosqMessage: new Message failed" << endl;
        return;
    }
    
}

MosqMessage::~MosqMessage()
{
}

char *MosqMessage::getpayload()
{
    return ptr->getpayload();
}

int MosqMessage::getqos()
{
    return ptr->getqos();
}

char *MosqMessage::gettopic()
{
    return ptr->gettopic();
}

int MosqMessage::getpayloadlen()
{
    return ptr->getpayloadlen();
}

MosqMessage::Message::Message(const mosquitto_message *mosq_message)
{
    message.payloadlen = mosq_message->payloadlen;
    message.qos = mosq_message->qos;
    message.payload = malloc(strlen((char *)(mosq_message->payload)) + 1);
    strcpy((char *)message.payload, (char *)mosq_message->payload);
    message.topic = (char *)malloc(strlen((char *)(mosq_message->topic)) + 1);
    strcpy((char *)message.topic, (char *)mosq_message->topic);
}

MosqMessage::Message::~Message()
{
    if (message.payload != nullptr) {
        free(message.payload);
        message.payload = nullptr;  // 置空，避免重复释放
    }
    if (message.topic != nullptr) {
        free(message.topic);
        message.topic = nullptr;
    }
}

char *MosqMessage::Message::getpayload()
{
    return (char*)message.payload;
}

int MosqMessage::Message::getqos()
{
    return message.qos;
}

char *MosqMessage::Message::gettopic()
{
    return message.topic;
}

int MosqMessage::Message::getpayloadlen()
{
    return message.payloadlen;
}
