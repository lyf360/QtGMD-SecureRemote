#include "MqttCommunication.h"

MqttCommunication::MqttCommunication(const string publishtopic, const string receivetopic, int qos):publishtopic(publishtopic),receivetopic(receivetopic),qos(qos)
{
    int ret = mosquitto_lib_init();
    if(ret!=MOSQ_ERR_SUCCESS)
    {
        cout << " mosquitto_lib_init failure "<<endl;
        return; 
    }

    mosq = mosquitto_new(NULL, true, this);
    if(mosq==NULL)
    {
        cout << "mosquitto_new failure"<<endl;
        return;
    }

    #if 1
    ret = mosquitto_username_pw_set(mosq, MQTT_USER, MQTT_PASS);
    if (ret != MOSQ_ERR_SUCCESS) {
        printf("设置用户名密码失败 mosquitto_username_pw_set：%s\n", mosquitto_strerror(ret));
        mosquitto_destroy(mosq);
        cout << "mosquitto_username_pw_set failure"<<endl;
        return;
    }
    #endif

    cout << "创建成功"<<endl;
    mosquitto_connect_callback_set(mosq,MqttCommunication::connnect_callback);
    mosquitto_disconnect_callback_set(mosq,MqttCommunication::dis_connect_callback);
    mosquitto_message_callback_set(mosq,MqttCommunication::receive_message_callback);
}

int MqttCommunication::connect_server(const string serverip,int port)
{
    int  ret = mosquitto_connect(mosq,serverip.c_str(),port,5);
    if(ret!=MOSQ_ERR_SUCCESS)
    {
        cout << " mosquitto_connect failure "<<endl;
        return -1; 
    }
    cout << " mosquitto_connect success "<<endl;

    ret = subscribe_message();
    if(ret<0)
    {
        cout << " subscribe_message failure "<<endl;
        return -1; 
    }

    ret = mosquitto_loop_start(mosq);
    if(ret!=MOSQ_ERR_SUCCESS)
    {
        cout << " mosquitto_loop_start failure "<<endl;
        return -1; 
    }
    return 0;
}

int MqttCommunication::disconnect_server()
{
    int ret = mosquitto_disconnect(mosq);
    if(ret!=MOSQ_ERR_SUCCESS)
    {
        cout << " mosquitto_disconnect failure "<<endl;
        return -1; 
    }
    cout << " mosquitto_disconnect success "<<endl;
    return 0;
}

int MqttCommunication::publish_message(string message)
{
    int ret = mosquitto_publish(mosq, NULL, publishtopic.c_str(), message.size(), message.c_str(), qos, false);
    if(ret!=MOSQ_ERR_SUCCESS)
    {
        cout << " mosquitto_publish failure "<<endl;
        return -1; 
    }
    return 0;
}

int MqttCommunication::subscribe_message()
{
    int ret = mosquitto_subscribe(mosq, NULL, receivetopic.c_str(), qos);
    if(ret!=MOSQ_ERR_SUCCESS)
    {
        cout << " mosquitto_subscribe failure "<<endl;
        return -1; 
    }
    return 0;
}

void MqttCommunication::connnect_callback(mosquitto *mosq, void *obj, int rc)
{
    if(rc!=0)
    {
        cout << "connnect_callback failure" << endl;
    }else{
        cout << "connnect_callback success" << endl;
    }

}

void MqttCommunication::dis_connect_callback(mosquitto *mosq, void *obj, int rc)
{
    if(rc!=0)
    {
        cout << "dis_connect_callback failure" << endl;
    }else{
        cout << "dis_connect_callback success" << endl;
    }
}

void MqttCommunication::receive_message_callback(mosquitto *mosq, void *self_message, const mosquitto_message *mosq_message)
{
    if(mosq_message==nullptr)
    {
        cout << "mosq_message==nullptr" << endl;
        return;
    }
    MqttCommunication * mqtt = (MqttCommunication*)self_message;
    
    mqtt->pause();
    mqtt->QueueMessage.push(MosqMessage(mosq_message));
    mqtt->resume();

    // cout << "mosq_message:" << string((char*)mosq_message->payload) << endl;
}



MqttCommunication::~MqttCommunication()
{
    int ret=0;
    ret = disconnect_server();
    if(ret<0)
    {
        cout << " disconnect_server failure "<<endl;
        return; 
    }

    ret = mosquitto_loop_stop(mosq,false);
    if(ret!=MOSQ_ERR_SUCCESS)
    {
        cout << " mosquitto_lib_cleanup failure "<<endl;
        return; 
    }

    if(mosq)
    {
        mosquitto_destroy(mosq);
    }

    ret = mosquitto_lib_cleanup();
    if(ret!=MOSQ_ERR_SUCCESS)
    {
        cout << " mosquitto_lib_cleanup failure "<<endl;
        return; 
    }

}

int MqttCommunication::receive_message_data(char* buf)
{
    
    {
        unique_lock<mutex> lock(mut);
        cv.wait(lock,[this](){
            return !ispause && !QueueMessage.empty();
        });
    }

    pause();
    MosqMessage m =  QueueMessage.front();
    memcpy(buf,m.getpayload(),m.getpayloadlen());
    QueueMessage.pop();
    resume();
    return m.getpayloadlen();
}


// int MqttCommunication::receive_message_data(char** buf)
// {
//     cout << "@1 in receive " << endl;
//     // 移除内部作用域，让锁覆盖整个队列操作
//     unique_lock<mutex> lock(mut);  // 加锁，直到函数结束才释放
    
//     cv.wait(lock, [this](){
//         return !ispause && !QueueMessage.empty();
//     });

//     cout << "@2 in receive " << endl;

//     // 此时仍持有锁，队列状态不会被其他线程修改
//     if(!QueueMessage.empty())
//     {
//         cout << " QueueMessage is not empty " << endl;
//     }
    
//     MosqMessage m = QueueMessage.front();
//     if(!m.getpayload())
//     {
//         cout << "getpayload is NULL " << endl;
//         return -1; // 避免后续操作
//     }
    
//     // 检查 buf 合法性（若 *buf 为空或内存不足，需处理）
//     if (*buf == nullptr) {
//         cout << "buf is NULL " << endl;
//         return -1;
//     }
//     cout << "m.getpayloadlen()" << m.getpayloadlen() << endl;
//     memcpy(*buf, m.getpayload(), m.getpayloadlen());
//     QueueMessage.pop();  // 安全执行 pop
    
//     return m.getpayloadlen();
// }

void MqttCommunication::pause()
{
    {
        std::lock_guard<mutex> lock(mut);
        ispause=true;
    }
}

void MqttCommunication::resume()
{
    {
        std::lock_guard<mutex> lock(mut);
        ispause=false;
    }
    cv.notify_all();
}
