
#include "head.h"
#include "jy901B.h"

using namespace std;

//保存配置
char JY_SAVE[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x00,
    0x00,
    0X00};
//解锁-->进行配置
char JY_UnLOCK[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x69,
    0x88,
    0XB5};
//六轴算法
char JY_SIX[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x24,
    0x01,
    0X00};
/*加速度计校准
水平静止1～2 秒后模块加速度三个轴向的值会在 0 0 1 左右， X 和 Y 轴角度在 0° 左右。
*/
char JY_A_Corrent[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x01,
    0x01,
    0X00};
//退出校准
char JY_ExitCorrent[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x01,
    0x00,
    0X00};
//陀螺仪自动校准(角速度)
char JY_AutoCorrent[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x63,
    0x00,
    0X00};
//设置回传内容
char JY_SetData[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x02,
    0x0E, //加速度、角速度、速度
    0X00};
//设置回传速率
char JY_SetRate[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x03,
    0x0B,
    0X00};
//切换待机/工作状态
char JY_SwitchSleep[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x22,
    0x01,
    0X00};

//设置参考角度X Y
char JY_ZeroXY[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x01,
    0x08,
    0X00};

// Z轴置零
char JY_ZeroZ[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x01,
    0x04,
    0X00};

JY901BCollector::JY901BCollector(const char *dev, int boundrate) : Serialport(dev, boundrate)
{
    sleep(1); //等待陀螺仪自动校准
    ZeroAngle();
    cout << "JY Device Config Successed" << endl;
}

void JY901BCollector::ZeroAngle()
{
    Serialport::sendBuffer(JY_UnLOCK, JYCMDLEN);
    usleep(JY_CMD_DELAY);
    Serialport::sendBuffer(JY_ZeroXY, JYCMDLEN);
    usleep(JY_CMD_DELAY);
    Serialport::sendBuffer(JY_ZeroZ, JYCMDLEN);
    usleep(JY_CMD_DELAY);
    Serialport::sendBuffer(JY_SAVE, JYCMDLEN);
    usleep(JY_CMD_DELAY);
}
//六轴算法
bool JY901BCollector::setJY()
{
    if (!Serialport::sendBuffer(JY_UnLOCK, JYCMDLEN))
        return false;
    usleep(JY_CMD_DELAY);
    if (!Serialport::sendBuffer(JY_SetData, JYCMDLEN)) //加速度、角速度、角度
        return false;
    usleep(JY_CMD_DELAY);
    if (!Serialport::sendBuffer(JY_SetRate, JYCMDLEN)) // 200Hz
        return false;
    usleep(JY_CMD_DELAY);
    if (!Serialport::sendBuffer(JY_SIX, JYCMDLEN)) // 六轴算法
        return false;
    usleep(JY_CMD_DELAY);
    if (!Serialport::sendBuffer(JY_A_Corrent, JYCMDLEN)) // 加速度计校准
        return false;
    sleep(5);
    if (!Serialport::sendBuffer(JY_ExitCorrent, JYCMDLEN)) // 200Hz
        return false;
    usleep(JY_CMD_DELAY);
    if (!Serialport::sendBuffer(JY_SAVE, JYCMDLEN))
        return false;
    return true;
}
void JY901BCollector::ParseData(char chr)
{
    static char chrBuf[100];
    static unsigned char chrCnt = 0;
    signed short sData[4];
    unsigned char i;

    chrBuf[chrCnt++] = chr;
    if (chrCnt < 11)
        return;
    //上电时刻会出现帧头错误
    if ((chrBuf[0] != 0x55) || ((chrBuf[1] & 0x50) != 0x50))
    {
        printf("JY Data Header Error:%x %x\r\n", chrBuf[0], chrBuf[1]);
        memcpy(&chrBuf[0], &chrBuf[1], 10);
        chrCnt--;
        return;
    }

    memcpy(&sData[0], &chrBuf[2], 8);
    switch (chrBuf[1])
    {
    case 0x51:
        for (i = 0; i < 3; i++)
            jy_pack.a[i] = (float)sData[i] / 32768.0 * 16.0;
        // printf("\r\na:%6.3f %6.3f %6.3f ", jy_pack.a[X], jy_pack.a[Y], jy_pack.a[Z]);
        break;
    case 0x52:
        for (i = 0; i < 3; i++)
            jy_pack.w[i] = (float)sData[i] / 32768.0 * 2000.0;
        // printf("w:%7.3f %7.3f %7.3f ", jy_pack.w[X], jy_pack.w[Y], jy_pack.w[Z]);
        break;
    case 0x53:
        for (i = 0; i < 3; i++)
            jy_pack.angle[i] = (float)sData[i] / 32768.0 * 180.0;
        // printf("A:%7.3f %7.3f %7.3f ", jy_pack.angle[Pitch], jy_pack.angle[Roll], jy_pack.angle[Yaw]);
        break;
    default:
        printf("JY901B Data Error!\n");
        break;
    }
    chrCnt = 0;
}

bool JY901BCollector::getJYData()
{
    int ret = Serialport::readBuffer(buff, JYBUFFLEN);
    if (ret)
    {
        for (int i = 0; i < ret; i++)
        {
            ParseData(buff[i]);
        }
        return true;
    }
    return false;
}

JY901BCollector::~JY901BCollector()
{
    cout << "close JY" << endl;
}
