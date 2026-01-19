#include "FingerPrintProtocol.h"

FingerPrintProtocol::FingerPrintProtocol(FingerPrintDataFormat fingerPrintDataFormat):fingerPrintDataFormat(fingerPrintDataFormat)
{
    FingerPrintProtocolInit();
}

void FingerPrintProtocol::FingerPrintProtocolshow()
{

    /*
    uint16_t packethead={0xEF01};
    uint32_t deviceaddr={0xFFFFFFFF};
    packet_id packetid;
    uint16_t packetlength;
    vector<uint8_t> payload;
    uint16_t checksum;
    */

    //cout << "---------------------协议-----------------" << endl;
    //cout << "packethead " << hex << setw(2) << setfill('0') << this->fingerPrintDataFormat.packethead << " ";
    cout << "packethead ";
    cout << hex << setw(2) << setfill('0') << static_cast<int>(this->fingerPrintDataFormat.packethead >> 8 & 0xff) << " ";
    cout << hex << setw(2) << setfill('0') << static_cast<int>(this->fingerPrintDataFormat.packethead & 0xff) << " ";
    cout << endl;

    cout << "deviceaddr ";
    for(int i=1;i<=4;i++)
    {
        cout << hex << setw(2) << setfill('0') << static_cast<int>(this->fingerPrintDataFormat.deviceaddr >> (4-i)*8 & 0xff) << " ";
    }
    cout << endl;
    
    cout << "packetid ";
    cout << hex << setw(2) << setfill('0') << static_cast<int>(this->fingerPrintDataFormat.packetid & 0xff) << " ";
    cout << endl;

    cout << "packetlength ";
    cout << hex << setw(2) << setfill('0') << static_cast<int>(this->fingerPrintDataFormat.packetlength >> 8 & 0xff) << " ";
    cout << hex << setw(2) << setfill('0') << static_cast<int>(this->fingerPrintDataFormat.packetlength & 0xff) << " ";
    cout << endl;

    cout << "payload ";
    for (auto s : this->fingerPrintDataFormat.payload)
    {
        cout << hex << setw(2) << setfill('0') << static_cast<int>(s) << " "; 
    }
    cout << endl;


    cout << "checksum ";
    cout << hex << setw(2) << setfill('0') << static_cast<int>(this->fingerPrintDataFormat.checksum >> 8 & 0xff) << " ";
    cout << hex << setw(2) << setfill('0') << static_cast<int>(this->fingerPrintDataFormat.checksum & 0xff) << " ";
    cout << endl;

    //cout << " --------------------------- " << endl;
    
    cout << "fp_protocol ";
    for(auto byte:this->fp_protocol)
    {
        cout << hex << setw(2) << setfill('0') << static_cast<int>(byte) << " "; 
    }
    cout << endl;
}

void FingerPrintProtocol::FingerPrintshow2(vector<uint8_t> vect)
{
    cout << "vect ";
    for(auto byte:vect)
    {
        cout << hex << setw(2) << setfill('0') << static_cast<int>(byte) << " "; 
    }
    cout << endl;
}

void FingerPrintProtocol::FingerPrintProtocolInit()
{
    this->fp_protocol.push_back((this->fingerPrintDataFormat.packethead >> 8) & 0xff);
    this->fp_protocol.push_back(this->fingerPrintDataFormat.packethead  & 0xff);

    for(int i=0;i<4;i++)
    {
        this->fp_protocol.push_back(this->fingerPrintDataFormat.deviceaddr >> ((3-i)*8) & 0xff);
    }

    this->fp_protocol.push_back(this->fingerPrintDataFormat.packetid);

    this->fp_protocol.push_back((this->fingerPrintDataFormat.packetlength >> 8) & 0xff);
    this->fp_protocol.push_back(this->fingerPrintDataFormat.packetlength & 0xff);

    this->fp_protocol.insert(fp_protocol.end(),fingerPrintDataFormat.payload.begin(),fingerPrintDataFormat.payload.end());

    this->fp_protocol.push_back((this->fingerPrintDataFormat.checksum >> 8) & 0xff);
    this->fp_protocol.push_back(this->fingerPrintDataFormat.checksum & 0xff);
}

FingerPrintProtocol FingerPrintProtocol::generatehandshack()
{
    vector<uint8_t> vect;
    vect.push_back(0x35);
    FingerPrintDataFormat DataFormat(REQUEST_PACKET_ID,vect);
    return FingerPrintDataFormat(DataFormat);
}

FingerPrintProtocol FingerPrintProtocol::generateDeleteTemplate(uint16_t packetid,uint16_t delnum)
{
    vector<uint8_t> vect;
    vect.push_back(0x0C);
    vect.push_back((packetid >> 8) & 0xff);
    vect.push_back(packetid & 0xff);
    vect.push_back((delnum >> 8) & 0xff);
    vect.push_back(delnum & 0xff);
    FingerPrintDataFormat DataFormat(REQUEST_PACKET_ID,vect);
    return FingerPrintDataFormat(DataFormat);
}

FingerPrintProtocol FingerPrintProtocol::generateEraseAllTemplate()
{
    vector<uint8_t> vect;
    vect.push_back(0x0D);
    FingerPrintDataFormat DataFormat(REQUEST_PACKET_ID,vect);
    return FingerPrintDataFormat(DataFormat);
}

FingerPrintProtocol FingerPrintProtocol::generateValidTemplate()
{
    vector<uint8_t> vect;
    vect.push_back(0x1D);
    FingerPrintDataFormat DataFormat(REQUEST_PACKET_ID,vect);
    return FingerPrintDataFormat(DataFormat);
}

FingerPrintProtocol FingerPrintProtocol::generateGetImage()
{
    vector<uint8_t> vect;
    vect.push_back(0x01);
    FingerPrintDataFormat DataFormat(REQUEST_PACKET_ID,vect);
    return FingerPrintDataFormat(DataFormat);
}

FingerPrintProtocol FingerPrintProtocol::generateAcqureImage()
{
    vector<uint8_t> vect;
    vect.push_back(0x29);
    FingerPrintDataFormat DataFormat(REQUEST_PACKET_ID,vect);
    return FingerPrintDataFormat(DataFormat);
}


FingerPrintProtocol FingerPrintProtocol::generateGetCharCode(uint8_t bufferid)
{
    vector<uint8_t> vect;
    vect.push_back(0x02);
    vect.push_back(bufferid);
    FingerPrintDataFormat DataFormat(REQUEST_PACKET_ID,vect);
    return FingerPrintDataFormat(DataFormat);
}

FingerPrintProtocol FingerPrintProtocol::generateSyntheticTemplate()
{
    vector<uint8_t> vect;
    vect.push_back(0x05);
    FingerPrintDataFormat DataFormat(REQUEST_PACKET_ID,vect);
    return FingerPrintDataFormat(DataFormat);
}

FingerPrintProtocol FingerPrintProtocol::generateStorageTemplate(uint16_t pageid,uint8_t bufferid)
{
    vector<uint8_t> vect;
    vect.push_back(0x06);
    vect.push_back(bufferid);
    vect.push_back((pageid >> 8) & 0xff);
    vect.push_back(pageid & 0xff);
    FingerPrintDataFormat DataFormat(REQUEST_PACKET_ID,vect);
    return FingerPrintDataFormat(DataFormat);
}


FingerPrintProtocol FingerPrintProtocol::generateSearchTemplate(uint16_t startpage,uint16_t pagenum,uint8_t bufferid)
{
    vector<uint8_t> vect;
    vect.push_back(0x04);
    vect.push_back(bufferid);
    vect.push_back((startpage >> 8) & 0xff);
    vect.push_back(startpage & 0xff);
    vect.push_back((pagenum >> 8) & 0xff);
    vect.push_back(pagenum & 0xff);
    FingerPrintDataFormat DataFormat(REQUEST_PACKET_ID,vect);
    return FingerPrintDataFormat(DataFormat);
}

bool FingerPrintProtocol::VerifyFingerPrint(vector<uint8_t> readpacket)
{
    return readpacket[9]==0x00;
}

FingerPrintProtocol FingerPrintProtocol::gengerateSimulatedAcksingal(vector<uint8_t> ackpacket)
{
    /*
    uint16_t packethead={0xEF01};
    uint32_t deviceaddr={0xFFFFFFFF};
    packet_id packetid;
    uint16_t packetlength;
    vector<uint8_t> payload;
    uint16_t checksum;
    */

    /*
    typedef enum
{
    REQUEST_PACKET_ID = 0x01,
    ACEKAGE_PACKET_ID = 0x07,
    PACKET_DATA1 = 0x02,
    PACKET_DATA2 = 0x08,
} packet_id;
    */
   
    FingerPrintDataFormat datafromat;
    packet_id id;
    switch(ackpacket[6])
    {
        case 0x01:
            id=REQUEST_PACKET_ID;
            break;
        case 0x07:
            id=ACEKAGE_PACKET_ID;
            break;
        case 0x02:
            id=PACKET_DATA1;
            break;
        case 0x08:
            id=PACKET_DATA2;
            break;
        default:
            cout << "id 无标识" << endl;
            exit(EXIT_FAILURE);
    }
    datafromat.packetid=id;
    datafromat.packetlength = static_cast<uint16_t>(ackpacket[7] << 8) | ackpacket[8];
    for(int i=9;i<ackpacket.size()-2;i++)
    {
        datafromat.payload.push_back(ackpacket[i]);
    }
    datafromat.checksum = static_cast<uint16_t>(ackpacket[ackpacket.size()-2] << 8) | ackpacket[ackpacket.size()-1];
    return FingerPrintDataFormat(datafromat);
}

bool FingerPrintProtocol::datapacketisvaild()
{
    FingerPrintDataFormat datafromat = this->fingerPrintDataFormat;
    uint16_t calculate_checksum=datafromat.packetid;
    calculate_checksum+=datafromat.packetlength << 8 & 0xff;
    calculate_checksum+=datafromat.packetlength & 0xff;
    for(auto it:datafromat.payload)
    {
        calculate_checksum+=it;
    }
    return calculate_checksum==datafromat.checksum;
}
