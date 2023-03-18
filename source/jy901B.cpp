
#include "head.h"
#include "jy901B.h"

using namespace std;

//��������
char JY_SAVE[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x00,
    0x00,
    0X00};
//����-->��������
char JY_UnLOCK[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x69,
    0x88,
    0XB5};
//�����㷨
char JY_SIX[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x24,
    0x01,
    0X00};
/*���ٶȼ�У׼
ˮƽ��ֹ1��2 ���ģ����ٶ����������ֵ���� 0 0 1 ���ң� X �� Y ��Ƕ��� 0�� ���ҡ�
*/
char JY_A_Corrent[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x01,
    0x01,
    0X00};
//�˳�У׼
char JY_ExitCorrent[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x01,
    0x00,
    0X00};
//�������Զ�У׼(���ٶ�)
char JY_AutoCorrent[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x63,
    0x00,
    0X00};
//���ûش�����
char JY_SetData[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x02,
    0x0E, //���ٶȡ����ٶȡ��ٶ�
    0X00};
//���ûش�����
char JY_SetRate[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x03,
    0x0B,
    0X00};
//�л�����/����״̬
char JY_SwitchSleep[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x22,
    0x01,
    0X00};

//���òο��Ƕ�X Y
char JY_ZeroXY[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x01,
    0x08,
    0X00};

// Z������
char JY_ZeroZ[JYCMDLEN] = {
    0xFF,
    0xAA,
    0x01,
    0x04,
    0X00};

JY901BCollector::JY901BCollector(const char *dev, int boundrate) : Serialport(dev, boundrate)
{
    sleep(1); //�ȴ��������Զ�У׼
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
//�����㷨
bool JY901BCollector::setJY()
{
    if (!Serialport::sendBuffer(JY_UnLOCK, JYCMDLEN))
        return false;
    usleep(JY_CMD_DELAY);
    if (!Serialport::sendBuffer(JY_SetData, JYCMDLEN)) //���ٶȡ����ٶȡ��Ƕ�
        return false;
    usleep(JY_CMD_DELAY);
    if (!Serialport::sendBuffer(JY_SetRate, JYCMDLEN)) // 200Hz
        return false;
    usleep(JY_CMD_DELAY);
    if (!Serialport::sendBuffer(JY_SIX, JYCMDLEN)) // �����㷨
        return false;
    usleep(JY_CMD_DELAY);
    if (!Serialport::sendBuffer(JY_A_Corrent, JYCMDLEN)) // ���ٶȼ�У׼
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
    //�ϵ�ʱ�̻����֡ͷ����
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
