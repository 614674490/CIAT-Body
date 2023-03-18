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
    uint8_t header;    //����ͷ
    int8_t acdata[64]; //���ɲ�������
    uint8_t heartrate; //����
    uint8_t spo2;      //Ѫ��
    uint8_t bk;        //΢ѭ��
    uint8_t rsv[8];    //��������
    uint8_t systolic;  //����ѹ
    uint8_t diastolic; //����ѹ
    bool validity;     //������Ч��
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
