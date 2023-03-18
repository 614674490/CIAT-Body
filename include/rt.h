#ifndef RT_H_INCLUDED
#define RT_H_INCLUDED

#include "serial.h"

#define RTBUFFLEN 76
#define RTDEV "/dev/ttyAMA1"
#define RTBAUD 38400

#define Work 0X8A
#define Standby 0X88
#define OpenHealth 0X8E
#define CloseHealth 0X8C
#define InSleep 0X98
#define OutSleep 0X00

typedef struct
{
    uint8_t header;    //数据头
    int8_t acdata[64]; //心律波形数据
    uint8_t heartrate; //心率
    uint8_t spo2;      //血氧
    uint8_t bk;        //微循环
    uint8_t rsv[8];    //保留数据
    uint8_t systolic;  //收缩压
    uint8_t diastolic; //舒张压
    bool validity;     //数据有效性
} RT_PACK;             // 76byte

class rtCollector : public Serialport
{
public:
    rtCollector(const char *dev, int boundrate);
    ~rtCollector();

    char buff[RTBUFFLEN] = {0};
    char datalen = 0;
    RT_PACK rt_pack = {
        header : 0x00,
        acdata : {0},
        heartrate : 0,
        spo2 : 0,
        bk : 0,
        rsv : {0},
        systolic : 0,
        diastolic : 0,
        validity : false
    };

    void startWork();
    bool readChar(uint8_t &rt_char);
    int readBuffer(char *buf, unsigned int size);
    bool getRTData();
};

#endif
