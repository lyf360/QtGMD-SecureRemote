// MqttWorker.h（已有的正确实现）
#ifndef MQTTWORKER_H
#define MQTTWORKER_H

#include <QObject>
#include <vector>
#include "SmartDeviceMqtt.h"
#include <QDebug>
class MqttWorker : public QObject {
    Q_OBJECT
public:
    explicit MqttWorker(SmartDeviceMqtt* mqtt, QObject *parent = nullptr)
        : QObject(parent), m_mqtt(mqtt) {}

    ~MqttWorker() {
        qDebug() << "MqttWorker 被 deleteLater() 删除！"; // 加日志
    }
signals:
    void messageReceived(const QString& jsonStr);  // 收到消息信号
    void errorOccurred(const QString& error);      // 错误信号

public slots:
    void startReceiving() {  // 在子线程中循环接收
        while (true) {
            std::vector<uint8_t> recdata;
            // 调用SmartDeviceMqtt的接收接口（阻塞等待消息）
            bool ret = m_mqtt->receivesocket(recdata);
            if (!ret) {
                emit errorOccurred("接收消息失败");
                continue;
            }
            // 转换为QString并发送信号给主线程
            QByteArray jsonBytes(reinterpret_cast<const char*>(recdata.data()), recdata.size());
            QString jsonStr = QString::fromUtf8(jsonBytes);
            emit messageReceived(jsonStr);
        }
    }

private:
    SmartDeviceMqtt* m_mqtt;  // 不负责销毁，由主线程管理
};

#endif // MQTTWORKER_H
