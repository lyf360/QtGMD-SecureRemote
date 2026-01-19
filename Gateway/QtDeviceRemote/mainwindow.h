#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit> // 包含QPlainTextEdit的头文件（必须）
#include <QTimer>
#include "FingerPrintDevice.h"
#include "SmartDeviceMqtt.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include <QDebug>
#include "mqttworker.h"

#include "sqlitedata.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    bool Fingerprint_verification();

    // 定时器更新槽函数
    void updateLoginTimer();

    vector<uint8_t> getsenddata();
    void receivestr();


    void on_pushButton_10_clicked();

    // 处理子线程发来的消息（主线程执行）
    void onMessageReceived(const QString& jsonStr);
    // 处理错误信息（主线程执行）
    void onErrorOccurred(const QString& error);

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_18_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

    void initPlainTextEdit();
private:

    int asd=0;
    int qos=0;
    FingerPrintDevice *fingerPrint;
    QString currentpermissions="S";
    sqliteData * sqlitedata;
    SmartDeviceMqtt *mqttdevice;
    Ui::MainWindow *ui;
    QPlainTextEdit * lasttext;
    QPlainTextEdit * usertext;
    QPlainTextEdit * page1test;
    QPlainTextEdit *deviceAckShow;
    // 定时器相关成员
    QTimer *loginTimer;
    int remainingTime; // 剩余倒计时时间

    QThread *m_mqttThread;      // 子线程
    MqttWorker *m_mqttWorker;   // 子线程中的工作对象

    int deviceTypeCount=0;
    int deviceNumCount=0;
    int operationCount=0;
    int fingerprintCount=0;
    // 启用/禁用登录相关按钮
    void setLoginButtonsEnabled(bool enabled);
};
#endif // MAINWINDOW_H
