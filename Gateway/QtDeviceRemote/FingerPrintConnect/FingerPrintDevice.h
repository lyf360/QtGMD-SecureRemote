#ifndef __FINGER_PRINT_DEVICE__
#define __FINGER_PRINT_DEVICE__

#include <iostream>
#include "LinuxUart.h"
#include "FingerPrintProtocol.h"
#include <vector>
#include <thread>    // 包含sleep_for
#include <chrono>    // 包含时间单位
using namespace std;



#define FIXED_NINE_LENGTH 9
class FingerPrintDevice{

public:
    FingerPrintDevice(const string fingerprintdevice,const uint32_t BaudRate=115200);
    ~FingerPrintDevice();
    bool sendProtocolPacker(const vector<uint8_t>& writepacket);
    bool receiveDeviceData(vector<uint8_t> &readpacket);
    
    // 发送握手协议
    bool sendHandshackProtocol();
    // 获取指纹图像
    bool getFingerPrintImage();
    // 生成特征码
    bool makeFeatureCode(uint8_t bufferid = 1);
    // 合成模板
    bool synthesizeTemplate();
    // 存储模板
    bool storeTemplate(uint16_t pageid, uint8_t bufferid = 1);

    // 录入并存储指纹
    bool inputAndStoreFingerPrint(uint16_t pageid, uint8_t bufferid = 1);
    bool inputAndStoreFingerPrint2(uint16_t pageid, uint8_t bufferid = 1);

    // 模板个数
    bool getvaildTemplatenum(uint16_t &num);
    //删除指定id指纹
    bool deleteidTemplate(uint16_t packetid, uint16_t delnum = 1);
    // 搜索指纹 比对
    bool serchfingerprint(bool &flagbuffid,uint16_t &id,uint16_t &score,uint16_t startpage=0, uint16_t pagenum=200, uint8_t bufferid = 1);
    // 清空指纹库
    bool deleteAllfingerprint(); 

    // clear缓冲区
    bool clearCharBuffer(uint8_t bufferid);
    private:
    LinuxUart* linuxuart{nullptr};
    // vector<uint8_t> readpacket;
};

#endif
