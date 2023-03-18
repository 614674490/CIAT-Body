#ifndef JY901B_H_INCLUDED
#define JY901B_H_INCLUDED

#include "serial.h"

#define JYCMDLEN 5
#define JYBUFFLEN 33
#define JYDEV "/dev/ttyAMA2"
#define JYBAUD 115200

#define X 0 // Pitch(X:0~90/90~-0/-0~-90/-90~0)˳ʱ��Ϊ��
#define Y 1 // Roll(Y: 0~180/-180~0)��Ϊ��
#define Z 2 // Yaw(Z: 0~180/-180~0)��Ϊ��

#define Pitch 0 // Pitch(X:0~90/90~-0/-0~-90/-90~0)˳ʱ��Ϊ��
#define Roll 1  // Roll(Y: 0~180/-180~0)��Ϊ��
#define Yaw 2   // Yaw(Z: 0~180/-180~0)��Ϊ��

typedef struct
{
    float a[3];     //���ٶ� x y z
    float w[3];     //���ٶ� x y z
    float angle[3]; //�Ƕ� Pitch(X) Roll(Y) Yaw(Z)
} JY_PACK;          // 33byte

class JY901BCollector : public Serialport
{
public:
    JY901BCollector(const char *dev, int boundrate);
    ~JY901BCollector();
    char buff[JYBUFFLEN] = {0};
    JY_PACK jy_pack = {
        a : {0.0},
        w : {0.0},
        angle : {0.0}
    };
    void ZeroAngle();
    bool setJY();
    void ParseData(char chr);
    bool getJYData();
};

#endif
