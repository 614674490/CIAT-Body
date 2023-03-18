#include "head.h"
#include <sstream>
#include "thread.h"

using namespace std;
using namespace rapidjson;

const char *EMGJSON = "{\"type\":11,\"cardId\":\"\",\"devicesMsg\":{\"emg_above\":0,\"emg_below\":0}}";
const char *RTJSON = "{\"type\":12,\"cardId\":\"\",\"devicesMsg\":{\"heartrate\":0,\"spo2\":0,\"bk\":0,\"systolic\":0,\"diastolic\":0}}";
const char *JYJSON = "{\"type\":13,\"cardId\":\"\",\"devicesMsg\":{\"ax\":0.0,\"ay\":0.0,\"az\":0.0,\"wx\":0.0,\"wy\":0.0,\"wz\":0.0,\"pitch\":0.0,\"roll\":0.0,\"yaw\":0.0}}";

// ��ȡϵͳtick������Ϊusʱ���ο�
long int getTickUs(void)
{
    struct timeval tv = {0};
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000u + tv.tv_usec;
}

void delayUs(long int us)
{
    struct timeval tv;
    tv.tv_sec = us / 1000000;
    tv.tv_usec = us % 1000000;
    select(0, NULL, NULL, NULL, &tv);
}

JSON::JSON()
{
    emgDoc.Parse(EMGJSON);
    rtDoc.Parse(RTJSON);
    jyDoc.Parse(JYJSON);
    this->eof = "_$";
    this->CardId = "21031211576";
    online = true;
    cout << "Json Convert Config Successed" << endl;
}

string JSON::setEmgDoc(emgCollector &emgcollector)
{
    Value &cardId = emgDoc["cardId"];
    string resCardId = getCardId();
    cardId = StringRef(resCardId.c_str(), resCardId.length());

    Value &devicesMsg = emgDoc["devicesMsg"];

    Value &emg_above = devicesMsg["emg_above"];
    emg_above.SetUint(emgcollector.emg_val[EmgAbove]);

    Value &emg_below = devicesMsg["emg_below"];
    emg_below.SetUint(emgcollector.emg_val[EmgBelow]);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    emgDoc.Accept(writer);
    return (buffer.GetString() + eof);
}

string JSON::setRTDoc(rtCollector &rtcollector)
{
    Value &cardId = rtDoc["cardId"];
    string resCardId = getCardId();
    cardId = StringRef(resCardId.c_str(), resCardId.length());

    Value &devicesMsg = rtDoc["devicesMsg"];

    Value &heartrate = devicesMsg["heartrate"];
    heartrate.SetUint(rtcollector.rt_pack.heartrate);

    Value &spo2 = devicesMsg["spo2"];
    spo2.SetUint(rtcollector.rt_pack.spo2);

    Value &bk = devicesMsg["bk"];
    bk.SetUint(rtcollector.rt_pack.bk);

    Value &systolic = devicesMsg["systolic"];
    systolic.SetUint(rtcollector.rt_pack.systolic);

    Value &diastolic = devicesMsg["diastolic"];
    diastolic.SetUint(rtcollector.rt_pack.diastolic);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    rtDoc.Accept(writer);

    return (buffer.GetString() + eof);
}

string JSON::setJYDoc(JY901BCollector &jycollector)
{
    Value &cardId = jyDoc["cardId"];
    string resCardId = getCardId();
    cardId = StringRef(resCardId.c_str(), resCardId.length());

    Value &devicesMsg = jyDoc["devicesMsg"];

    Value &ax = devicesMsg["ax"];
    ax.SetFloat(jycollector.jy_pack.a[X]);
    Value &ay = devicesMsg["ay"];
    ay.SetFloat(jycollector.jy_pack.a[Y]);
    Value &az = devicesMsg["az"];
    az.SetFloat(jycollector.jy_pack.a[Z]);

    Value &wx = devicesMsg["wx"];
    wx.SetFloat(jycollector.jy_pack.w[X]);
    Value &wy = devicesMsg["wy"];
    wy.SetFloat(jycollector.jy_pack.w[Y]);
    Value &wz = devicesMsg["wz"];
    wz.SetFloat(jycollector.jy_pack.w[Z]);

    Value &pitch = devicesMsg["pitch"];
    pitch.SetFloat(jycollector.jy_pack.angle[Pitch]);
    Value &roll = devicesMsg["roll"];
    roll.SetFloat(jycollector.jy_pack.angle[Roll]);
    Value &yaw = devicesMsg["yaw"];
    yaw.SetFloat(jycollector.jy_pack.angle[Yaw]);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.SetMaxDecimalPlaces(3);
    jyDoc.Accept(writer);

    return (buffer.GetString() + eof);
}

void JSON::setCardId(string str)
{
    rwLock.writeLock();
    cout << "write lock" << endl;
    this->CardId = str;
    Isonline();
    cout << "write unlock" << endl;
    rwLock.writeUnlock();
}

string JSON::getCardId()
{
    string res = "";
    rwLock.readLock();
    //cout << "read lock" << endl;
    res = this->CardId;
    rwLock.readUnlock();
    return res;
}

bool JSON::strIsNum(string str)
{
    stringstream sin(str);
    double d;
    char c;
    if (str.empty())
        return false;
    if (!(sin >> d))
    {
        /*���ͣ�
            sin>>t��ʾ��sinת����double�ı�������ʵ����int��float�͵Ķ�����գ���
            ���ת���ɹ�����ֵΪ��0�����ת�����ɹ��ͷ���Ϊ0
        */
        return false;
    }
    if (sin >> c)
    {
        /*���ͣ�
        �˲������ڼ����������У����ּ��ַ�����������ʽ�����磺34.f����������ĵĲ��֣�sin>>t��
        �Ѿ����ղ�ת������������ֲ��֣���stringstream����ӦҲ�����һ���ָ������
        ��ʱ���յ���.f�ⲿ�֣�������������������false
          */
        return false;
    }
    return true;
}

bool JSON::Isonline()
{
    static string proCardId = this->CardId;
    if (strIsNum(this->CardId))
    {
        cout << "student <" << this->CardId << "> online" << endl;
        online = true;
        proCardId = this->CardId;
        return true;
    }
    else if (this->CardId.empty())
    {
        cout << "student <" << proCardId << "> offline" << endl;
        online = false;
        proCardId = this->CardId;
        return false;
    }
    else
    {
        cerr << "error student cardId" << endl;
        online = false;
        return false;
    }
}
