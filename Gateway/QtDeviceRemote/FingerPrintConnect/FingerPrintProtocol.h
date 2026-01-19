#ifndef __FINGER_PRINT_PROTOCOL__
#define __FINGER_PRINT_PROTOCOL__

#include "FingerPrintDataFormat.h"
#include <iomanip>

class FingerPrintProtocol
{
public:
    FingerPrintProtocol(FingerPrintDataFormat fingerPrintDataFormat);

    void FingerPrintProtocolInit();
    void FingerPrintProtocolshow();
    void FingerPrintshow2(vector<uint8_t> vect);

    // 握手协议
    static FingerPrintProtocol generatehandshack(); // 握手协议 // 属于类 可以直接调用
    // 删除模板
    static FingerPrintProtocol generateDeleteTemplate(uint16_t packetid, uint16_t delnum = 1);
    // 清空指纹库
    static FingerPrintProtocol generateEraseAllTemplate();
    // 有效模板个数
    static FingerPrintProtocol generateValidTemplate();
    // 获取图像
    static FingerPrintProtocol generateGetImage();
    // 注册用获取图像
    static FingerPrintProtocol generateAcqureImage();
    // 生成特证码
    static FingerPrintProtocol generateGetCharCode(uint8_t bufferid = 1);
    // 合成编码模板      synthetic
    static FingerPrintProtocol generateSyntheticTemplate();
    // 存储模板
    static FingerPrintProtocol generateStorageTemplate(uint16_t pageid, uint8_t bufferid = 1);
    // 搜索模板
    static FingerPrintProtocol generateSearchTemplate(uint16_t startpage, uint16_t pagenum, uint8_t bufferid = 1);

    // 解析应答包
    static FingerPrintProtocol gengerateSimulatedAcksingal(vector<uint8_t> ackpacket);

    // 数据报数否有效
    bool datapacketisvaild();

    // 校验数据 是否读取成功
    bool VerifyFingerPrint(vector<uint8_t> readpacket);

    vector<uint8_t> getFingerPrintProtocol()
    {
        return fp_protocol;
    }

private:
    FingerPrintDataFormat fingerPrintDataFormat; // 协议格式
    vector<uint8_t> fp_protocol;
};

#endif