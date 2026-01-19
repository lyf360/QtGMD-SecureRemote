#include "FingerPrintDevice.h"

FingerPrintDevice::FingerPrintDevice(const string fingerprintdevice, const uint32_t BaudRate)
{
    this->linuxuart = new LinuxUart(fingerprintdevice, BaudRate); // 为成员变量赋值
}

FingerPrintDevice::~FingerPrintDevice()
{
    if (linuxuart)
    {
        delete linuxuart;
        linuxuart=nullptr;
    }
}

// bool FingerPrintDevice::sendProtocolPacker(const vector<uint8_t> writepacket)
// {
//     uint32_t ret = linuxuart->writeuartdata(reinterpret_cast<char *>(const_cast<uint8_t *>(writepacket.data())), writepacket.size());
//     if (ret == -1)
//     {
//         cout << "send protocol failure" << endl;
//         return false;
//     }
//     return true;
// }

bool FingerPrintDevice::sendProtocolPacker(const vector<uint8_t>& writepacket)
{
    ssize_t ret = linuxuart->writeuartdata(reinterpret_cast<const char *>(writepacket.data()), writepacket.size());
    if (ret == -1)
    {
        cout << "send protocol failure" << endl;
        return false;
    }
    return true;
}

bool FingerPrintDevice::receiveDeviceData(vector<uint8_t> &readpacket)
{
    readpacket.resize(FIXED_NINE_LENGTH);
    bool ret = linuxuart->ReceiveFixedPacket(reinterpret_cast<char *>(readpacket.data()), FIXED_NINE_LENGTH);
    if (!ret)
    {
        cout << "failure read prefix" << endl;
        return false;
    }
    uint16_t reclength = static_cast<uint16_t>(readpacket[7] << 8) | readpacket[8];

    vector<uint8_t> houfix;
    houfix.resize(reclength);
    ret = linuxuart->ReceiveFixedPacket(reinterpret_cast<char *>(houfix.data()), reclength);
    if (!ret)
    {
        cout << "failure read hou_prefix" << endl;
        return false;
    }
    readpacket.insert(readpacket.end(), houfix.begin(), houfix.end());
    return true;
}

bool FingerPrintDevice::sendHandshackProtocol()
{
    FingerPrintProtocol sendprotocol = FingerPrintProtocol::generatehandshack();
    cout << "----------------发送握手协议-----------"<< endl;
    sendprotocol.FingerPrintProtocolshow();
    bool ok = sendProtocolPacker(sendprotocol.getFingerPrintProtocol());
    if (!ok)
    {
        cout << "发送指纹协议失败" << endl;
        return false;
    }
    vector<uint8_t> recbuf;
    cout << "recbuf=" << recbuf.size() << endl;
    ok = receiveDeviceData(recbuf);
    if (!ok)
    {
        cout << "接收指纹协议失败" << endl;
        return false;
    }
    cout << "recbuf=" << recbuf.size() << endl;
    // 转化协议包格式
    FingerPrintProtocol recprotocol = FingerPrintProtocol::gengerateSimulatedAcksingal(recbuf);
    // cout << "recbuf**1**--------------------------------------------------" << endl;
    recprotocol.FingerPrintProtocolshow();
    //cout << "recbuf**1**" << endl;
    ok = recprotocol.datapacketisvaild();
    if(!ok){
        cout << "协议包校验失败" <<endl;
        return false;
    }
    //cout << "recbuf**2**" << endl;
    ok = recprotocol.VerifyFingerPrint(recbuf);
    if(!ok)
    {
        cout << "应答包错误" << endl;
        return false;
    }
    //cout << "recbuf**3**" << endl;
    return true;
}

/*
// 获取指纹图像
    bool getFingerPrintImage();
    // 生成特征码
    bool makeFeatureCode();
    // 合成模板
    bool synthesizeTemplate();
    // 存储模板
    bool storeTemplate();
*/

bool FingerPrintDevice::getFingerPrintImage()
{
    FingerPrintProtocol sendprotocol = FingerPrintProtocol::generateGetImage();
    cout << "--------------获取指纹图像协议-----------"<< endl;
    //sendprotocol.FingerPrintProtocolshow();
    bool ok = sendProtocolPacker(sendprotocol.getFingerPrintProtocol());
    if (!ok)
    {
        cout << "发送指纹协议失败" << endl;
        return false;
    }
    vector<uint8_t> recbuf;
    ok = receiveDeviceData(recbuf);
    if (!ok)
    {
        cout << "接收指纹协议失败" << endl;
        return false;
    }
    // 转化协议包格式
    FingerPrintProtocol recprotocol = FingerPrintProtocol::gengerateSimulatedAcksingal(recbuf);
    //recprotocol.FingerPrintProtocolshow();
    ok = recprotocol.datapacketisvaild();
    if(!ok){
        cout << "协议包校验失败" <<endl;
        return false;
    }
    ok = recprotocol.VerifyFingerPrint(recbuf);
    if(!ok)
    {
        cout << "应答包错误" << endl;
        return false;
    }
    return true;
    
}

bool FingerPrintDevice::makeFeatureCode(uint8_t bufferid)
{
    FingerPrintProtocol sendprotocol = FingerPrintProtocol::generateGetCharCode(bufferid);
    cout << "--------------生成特征码协议-----------"<< endl;
    sendprotocol.FingerPrintProtocolshow();
    bool ok = sendProtocolPacker(sendprotocol.getFingerPrintProtocol());
    if (!ok)
    {
        cout << "发送指纹协议失败" << endl;
        return false;
    }
    vector<uint8_t> recbuf;
    ok = receiveDeviceData(recbuf);
    if (!ok)
    {
        cout << "接收指纹协议失败" << endl;
        return false;
    }
    // 转化协议包格式
    FingerPrintProtocol recprotocol = FingerPrintProtocol::gengerateSimulatedAcksingal(recbuf);
    recprotocol.FingerPrintProtocolshow();
    ok = recprotocol.datapacketisvaild();
    if(!ok){
        cout << "协议包校验失败" <<endl;
        return false;
    }
    ok = recprotocol.VerifyFingerPrint(recbuf);
    if(!ok)
    {
        cout << "应答包错误" << endl;
        return false;
    }
    return true;
}

bool FingerPrintDevice::synthesizeTemplate()
{
    FingerPrintProtocol sendprotocol = FingerPrintProtocol::generateSyntheticTemplate();
    cout << "--------------合成模板协议-----------"<< endl;
    sendprotocol.FingerPrintProtocolshow();
    bool ok = sendProtocolPacker(sendprotocol.getFingerPrintProtocol());
    if (!ok)
    {
        cout << "发送指纹协议失败" << endl;
        return false;
    }
    vector<uint8_t> recbuf;
    ok = receiveDeviceData(recbuf);
    if (!ok)
    {
        cout << "接收指纹协议失败" << endl;
        return false;
    }
    // 转化协议包格式
    FingerPrintProtocol recprotocol = FingerPrintProtocol::gengerateSimulatedAcksingal(recbuf);
    recprotocol.FingerPrintProtocolshow();
    ok = recprotocol.datapacketisvaild();
    if(!ok){
        cout << "协议包校验失败" <<endl;
        return false;
    }
    ok = recprotocol.VerifyFingerPrint(recbuf);
    if(!ok)
    {
        cout << "应答包错误" << endl;
        return false;
    }
    return true;
}

bool FingerPrintDevice::storeTemplate(uint16_t pageid, uint8_t bufferid)
{
    FingerPrintProtocol sendprotocol = FingerPrintProtocol::generateStorageTemplate(pageid,bufferid);
    cout << "--------------存储模板协议-----------"<< endl;
    sendprotocol.FingerPrintProtocolshow();
    bool ok = sendProtocolPacker(sendprotocol.getFingerPrintProtocol());
    if (!ok)
    {
        cout << "发送指纹协议失败" << endl;
        return false;
    }
    vector<uint8_t> recbuf;
    ok = receiveDeviceData(recbuf);
    if (!ok)
    {
        cout << "接收指纹协议失败" << endl;
        return false;
    }
    // 转化协议包格式
    FingerPrintProtocol recprotocol = FingerPrintProtocol::gengerateSimulatedAcksingal(recbuf);
    recprotocol.FingerPrintProtocolshow();
    ok = recprotocol.datapacketisvaild();
    if(!ok){
        cout << "协议包校验失败" <<endl;
        return false;
    }
    ok = recprotocol.VerifyFingerPrint(recbuf);
    if(!ok)
    {
        cout << "应答包错误" << endl;
        return false;
    }
    return true;
}
// generateValidTemplate

bool FingerPrintDevice::getvaildTemplatenum(uint16_t &num)
{
    FingerPrintProtocol sendprotocol = FingerPrintProtocol::generateValidTemplate();
    cout << "--------------有效模板个数协议-----------"<< endl;
    sendprotocol.FingerPrintProtocolshow();
    bool ok = sendProtocolPacker(sendprotocol.getFingerPrintProtocol());
    if (!ok)
    {
        cout << "发送指纹协议失败" << endl;
        return false;
    }
    vector<uint8_t> recbuf;
    ok = receiveDeviceData(recbuf);
    if (!ok)
    {
        cout << "接收指纹协议失败" << endl;
        return false;
    }
    // 转化协议包格式
    FingerPrintProtocol recprotocol = FingerPrintProtocol::gengerateSimulatedAcksingal(recbuf);
    recprotocol.FingerPrintProtocolshow();
    ok = recprotocol.datapacketisvaild();
    if(!ok){
        cout << "协议包校验失败" <<endl;
        return false;
    }
    ok = recprotocol.VerifyFingerPrint(recbuf);
    if(!ok)
    {
        cout << "应答包错误" << endl;
        return false;
    }
    num=recbuf[10] << 8 | recbuf[11];
    return true;
}

bool FingerPrintDevice::deleteidTemplate(uint16_t packetid, uint16_t delnum)
{
    FingerPrintProtocol sendprotocol = FingerPrintProtocol::generateDeleteTemplate(packetid,delnum);
    cout << "--------------删除模板协议-----------"<< endl;
    sendprotocol.FingerPrintProtocolshow();
    bool ok = sendProtocolPacker(sendprotocol.getFingerPrintProtocol());
    if (!ok)
    {
        cout << "发送指纹协议失败" << endl;
        return false;
    }
    vector<uint8_t> recbuf;
    ok = receiveDeviceData(recbuf);
    if (!ok)
    {
        cout << "接收指纹协议失败" << endl;
        return false;
    }
    // 转化协议包格式
    FingerPrintProtocol recprotocol = FingerPrintProtocol::gengerateSimulatedAcksingal(recbuf);
    recprotocol.FingerPrintProtocolshow();
    ok = recprotocol.datapacketisvaild();
    if(!ok){
        cout << "协议包校验失败" <<endl;
        return false;
    }
    ok = recprotocol.VerifyFingerPrint(recbuf);
    if(!ok)
    {
        cout << "应答包错误" << endl;
        return false;
    }
    return true;
}
// 覆盖缓冲区
bool FingerPrintDevice::clearCharBuffer(uint8_t bufferid) {
    // 1. 先获取有效图像（必须成功，否则ImageBuffer为空，生成特征会失败）
    // bool imgOk = getFingerPrintImage(); // 调用PS_GetImage指令
    // if (!imgOk) {
    //     cout << "获取图像失败，无法覆盖缓冲区" << endl;
    //     return false;
    // }
    // // 2. 生成特征到指定bufferid，覆盖旧数据
    // bool genOk = makeFeatureCode(bufferid); // 调用PS_GenChar指令
    // if (genOk) {
    //     cout << "bufferid " << (int)bufferid << " 旧数据已被新特征覆盖" << endl;
    //     return true;
    // } else {
    //     cout << "生成特征失败，缓冲区旧数据未清除" << endl;
    //     return false;
    // }
    int errorcnt=0;
    for(int i=1;i<=5;i++)
    {
        if(errorcnt>5)
        {
            cout << " -------------获取指纹图像失败---------------"<<endl;
            return false;
        }
        
        bool ok = getFingerPrintImage();
        if(ok)
        {
            bool ret = makeFeatureCode(bufferid);
            if(ret)
            {
                cout << "第"<< i << "次获取指纹成功" << endl;
                return true;
            }else{
                cout << "生成特征失败，缓冲区旧数据未清除" << endl;
                return false;
            }
        }
        cout << "第 "<<i<<" 次获取指纹失败" <<  endl;
        errorcnt++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return false;
}
bool FingerPrintDevice::serchfingerprint(bool &flagbuffid,uint16_t &id,uint16_t &score,uint16_t startpage, uint16_t pagenum, uint8_t bufferid)
{
    flagbuffid=false;
    // 清空bufferid的旧数据
    bool bufCleared = clearCharBuffer(bufferid);
    if (!bufCleared) {
        // 若无法覆盖（如无手指按），直接返回失败，避免使用旧数据
        flagbuffid=true;
        return true;
    }
    
    //int errorcnt=0;
    // for(int i=1;i<=5;i++)
    // {
    //     if(errorcnt>5)
    //     {
    //         cout << " -------------获取指纹图像失败---------------"<<endl;
    //         return false;
    //     }
        
    //     bool ok = getFingerPrintImage();
    //     if(ok)
    //     {
    //         bool ret = makeFeatureCode(bufferid);
    //         if(ret)
    //         {
    //             cout << "第"<< i << "次获取指纹成功" << endl;
    //         }
    //         break;
    //     }
    //     cout << "第 "<<i<<" 次获取指纹失败" <<  endl;
    //     errorcnt++;
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    // }
    //std::this_thread::sleep_for(std::chrono::seconds(1));

    FingerPrintProtocol sendprotocol = FingerPrintProtocol::generateSearchTemplate(startpage,pagenum,bufferid);
    cout << "--------------搜索模板协议-----------"<< endl;
    sendprotocol.FingerPrintProtocolshow();
    bool ok = sendProtocolPacker(sendprotocol.getFingerPrintProtocol());
    if (!ok)
    {
        cout << "发送指纹协议失败" << endl;
        return false;
    }
    vector<uint8_t> recbuf;
    ok = receiveDeviceData(recbuf);
    if (!ok)
    {
        cout << "接收指纹协议失败" << endl;
        return false;
    }

    // cout << "调试：recbuf原始数据（十六进制）：";
    // for (size_t i = 0; i < recbuf.size(); i++) {
    //     // 按2位十六进制打印，不足补0（便于对齐协议格式）
    //     cout << hex << setw(2) << setfill('0') << (int)recbuf[i] << " ";
    // }
    // cout << dec << endl; // 恢复为十进制输出

    // 转化协议包格式
    FingerPrintProtocol recprotocol = FingerPrintProtocol::gengerateSimulatedAcksingal(recbuf);
    recprotocol.FingerPrintProtocolshow();
    ok = recprotocol.datapacketisvaild();
    if(!ok){
        cout << "协议包校验失败" <<endl;
        return false;
    }
    ok = recprotocol.VerifyFingerPrint(recbuf);
    if(!ok)
    {
        cout << "应答包错误" << endl;
        return false;
    }
    // if(recbuf[9]!=0x00)
    // {
    //     cout << "匹配指纹失败"<<endl;
    //     return false;
    // }
    id=recbuf[10] << 8 | recbuf[11];
    score=recbuf[12] << 8 | recbuf[13];
    return true;
}

bool FingerPrintDevice::deleteAllfingerprint()
{
    FingerPrintProtocol sendprotocol = FingerPrintProtocol::generateEraseAllTemplate();
    cout << "--------------搜索模板协议-----------"<< endl;
    sendprotocol.FingerPrintProtocolshow();
    bool ok = sendProtocolPacker(sendprotocol.getFingerPrintProtocol());
    if (!ok)
    {
        cout << "发送指纹协议失败" << endl;
        return false;
    }
    vector<uint8_t> recbuf;
    ok = receiveDeviceData(recbuf);
    if (!ok)
    {
        cout << "接收指纹协议失败" << endl;
        return false;
    }

    // 转化协议包格式
    FingerPrintProtocol recprotocol = FingerPrintProtocol::gengerateSimulatedAcksingal(recbuf);
    recprotocol.FingerPrintProtocolshow();
    ok = recprotocol.datapacketisvaild();
    if(!ok){
        cout << "协议包校验失败" <<endl;
        return false;
    }
    ok = recprotocol.VerifyFingerPrint(recbuf);
    if(!ok)
    {
        cout << "应答包错误" << endl;
        return false;
    }
    return true;
}

/*
// 获取指纹图像
    bool getFingerPrintImage();
    // 生成特征码
    bool makeFeatureCode(uint8_t bufferid = 1);
    // 合成模板
    bool synthesizeTemplate();
    // 存储模板
    bool storeTemplate(uint16_t pageid, uint8_t bufferid = 1);
*/



bool FingerPrintDevice::inputAndStoreFingerPrint2(uint16_t pageid, uint8_t bufferid)
{
    int calnum=0;
    for(int i=1;i<=5;i++)
    {
        if(calnum==2) break;
        cout << "第 %d 次获取指纹" << i << endl;
        bool ok = getFingerPrintImage();
        if(ok)
        {
            bool ret = makeFeatureCode(bufferid);
            if(ret)
            {
                cout << "第 %d 次获取指纹成功" << i << endl;
                calnum++;
            }
            break;
        }
    }
    if(calnum==2)
    {
        bool ret = synthesizeTemplate();
        if(ret)
        {
            cout << "合成指纹成功" << endl;
        }
        ret = storeTemplate(pageid,bufferid);
        if(ret)
        {
            cout << "存储指纹成功" << endl;
        }
    }else{
        cout << "识别指纹失败" << endl;
        return false;
    }

    return true;
}

bool FingerPrintDevice::inputAndStoreFingerPrint(uint16_t pageid, uint8_t bufferid)
{
    int errornum=0;
    bool ok = false;
    for(int i=1;i<=2;i++)
    {
        for(int j=1;j<=5;j++)
        {
            ok=getFingerPrintImage();
            if(ok)
            {
                cout << "第 %d 次获取图像成功" <<i<< endl;
                break;
            }else{
                // sleep(1);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        if(!ok)
        {
            return false;
        }

        ok=makeFeatureCode(i);
        if(!ok)
        {
            return false;
        }else{
            cout << "第 %d 次生成指纹特征成功" <<i<< endl;
        }
    }
    ok=synthesizeTemplate();
    if(!ok)
    {
        return false;
    }else{
        cout << "生成指纹模板成功"<< endl;
    }

    ok=storeTemplate(pageid);
    if(!ok)
    {
        return false;
    }
    return true;
}
