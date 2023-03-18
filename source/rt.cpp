#include "head.h"
#include "rt.h"

using namespace std;

rtCollector::rtCollector(const char *dev, int boundrate) : Serialport(dev, boundrate)
{
    startWork();
    cout << "RT Device Config Successed" << endl;
}

void rtCollector::startWork()
{
    Serialport::sendChar(Work);
    usleep(MS_DELAY * 2);
}

bool rtCollector::readChar(uint8_t &rt_char)
{
    int res = serialGetchar(this->fd);
    if (res < 0)
        return false;
    else
    {
        rt_char = res;
        return true;
    }
}
/*
 * 接受RT数据
 * 一次性接受76个字节
 */
int rtCollector::readBuffer(char *buf, unsigned int size)
{
    int sum_len = 0;
    int len = 0;
    if (!readChar(rt_pack.header))
        return sum_len;

    if (rt_pack.header == 0xff)
    {
        buf[sum_len++] = rt_pack.header;
        while (sum_len < size - 1)
        {
            len = Serialport::readBuffer(buf + sum_len, size);
            sum_len += len;
        }
        if (buf[0] != 0xff || sum_len != size)
            sum_len = 0; //帧头不为0xff or 接受长度未达到期望长度 --> 数据包接受错误
    }
    return sum_len;
}

//手指、手臂; 贴近皮肤; 注意遮光
bool rtCollector::getRTData()
{
    int i = 0;
    static uint8_t sys=0;
    static uint8_t dias=0;
    static uint8_t bk=0;

    datalen = readBuffer(buff, RTBUFFLEN);
    if (datalen)
    {
        // 1~64 64byte
        /*for (i = 1; i <= 64; i++)
        {
            rt_pack.acdata[i - 1] = buff[i];
        }
        rt_pack.heartrate = buff[i++]; // 65
        rt_pack.spo2 = buff[i++];      // 66
        rt_pack.bk = buff[i++];        // 67
        */
        rt_pack.heartrate = buff[65]; // 65
        rt_pack.spo2 = buff[66];      // 66

        if(buff[65] == 0 && buff[66] == 0)
        {
            bk = 0;
            sys = 0;  // 71
            dias = 0; // 72
        }
        else
        {
            if(buff[67] != 0)
                bk=buff[67];
            if(buff[71] != 0 && buff[72] != 0)
            {
                sys = buff[71];  // 71

                dias = buff[72]; // 72
            }
        }

        rt_pack.bk = bk;        // 67
        rt_pack.systolic = sys;  // 71
        rt_pack.diastolic = dias; // 72

        if (rt_pack.heartrate || rt_pack.spo2 || rt_pack.bk || rt_pack.systolic || rt_pack.diastolic)
            rt_pack.validity = true;
        else
            rt_pack.validity = false;

        return true;
    }
    return false;
}

rtCollector::~rtCollector()
{
    Serialport::sendChar(Standby);
    cout << "close RT" << endl;
}
