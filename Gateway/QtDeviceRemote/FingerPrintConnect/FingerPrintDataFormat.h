#ifndef __FINGER_PRINT_DATA_FROMAT__
#define __FINGER_PRINT_DATA_FROMAT__

#include <iostream>
#include <vector>
using namespace std;

typedef enum
{
    REQUEST_PACKET_ID = 0x01,
    ACEKAGE_PACKET_ID = 0x07,
    PACKET_DATA1 = 0x02,
    PACKET_DATA2 = 0x08,
} packet_id;

class FingerPrintDataFormat
{
public:
    FingerPrintDataFormat()
    {
        
    }
    FingerPrintDataFormat(const packet_id id, vector<uint8_t> payload)
    {
        this->packetid = id;
        this->packetlength = 2 + payload.size();
        this->payload.assign(payload.begin(), payload.end());
        // this->payload = move(payload);
        // this->payload = move(payload.begin(),payload.end(),back_inserter(this->payload));
        this->checksum = this->packetid;
        this->checksum += (this->packetlength >> 8 & 0xff);
        this->checksum += (this->packetlength & 0xff);
        for (auto it : this->payload)
        {
            this->checksum += it;
        }
    }
    friend class FingerPrintProtocol;

private:
    uint16_t packethead = {0xEF01};
    uint32_t deviceaddr = {0xFFFFFFFF};
    packet_id packetid;
    uint16_t packetlength;
    vector<uint8_t> payload;
    uint16_t checksum;
};
#endif //