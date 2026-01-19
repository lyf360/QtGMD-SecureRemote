
# -------------------- Mosquitto MQTT库配置（最终版） --------------------
# 1. 头文件路径（指向根文件系统中的mosquitto.h）
INCLUDEPATH += /home/linux/imx6ull_2405/nfsworkdir/rootfs/usr/local/include/

# 2. 库文件路径（指向根文件系统中的libmosquitto.so）
LIBS += -L/home/linux/imx6ull_2405/nfsworkdir/rootfs/usr/local/lib/

# 3. 链接mosquitto库（-l后面是库名，对应libmosquitto.so）
LIBS += -lmosquitto


QT       += core gui serialport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    $$PWD/../../mqtt_connect/MosqMessage.cpp \
    $$PWD/../../mqtt_connect/MqttCommunication.cpp \
    $$PWD/../../mqtt_connect/SmartDeviceMqtt.cpp \
    $$PWD/../../FingerPrintConnect/FingerPrintDevice.cpp \
    $$PWD/../../FingerPrintConnect/FingerPrintProtocol.cpp \
    $$PWD/../../FingerPrintConnect/LinuxUart.cpp \
    mqttworker.cpp \
    sqlitedata.cpp


HEADERS += \
    mainwindow.h \
    mqttworker.h \
    $$PWD/../../mqtt_connect/MosqMessage.h \
    $$PWD/../../mqtt_connect/MqttCommunication.h \
    $$PWD/../../mqtt_connect/SmartDeviceMqtt.h \
    $$PWD/../../FingerPrintConnect/FingerPrintDataFormat.h \
    $$PWD/../../FingerPrintConnect/FingerPrintDevice.h \
    $$PWD/../../FingerPrintConnect/FingerPrintProtocol.h \
    $$PWD/../../FingerPrintConnect/LinuxUart.h \
    mqttworker.h \
    sqlitedata.h


FORMS += \
    mainwindow.ui

INCLUDEPATH += $$PWD/../../mqtt_connect
INCLUDEPATH += $$PWD/../../FingerPrintConnect
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
