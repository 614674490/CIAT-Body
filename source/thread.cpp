
#include "thread.h"

using namespace std;

readWriteLock rwLock;
JSON json;

//�ɼ����ͼ����ź�
void emgThread(emgCollector &emgcollector, Socket &socketclient)
{
    while (true)
    {
        if (json.online)
        {
            if (emgcollector.getEmgDate())
            {
                // cout << json.setEmgDoc(emgcollector) << endl;
                socketclient.send_data(json.setEmgDoc(emgcollector));
            }
            else
            {
                while (!(emgcollector.Init_EmgHardWork()))
                    usleep(MS_DELAY * 2);
            }
        }
        usleep(MS_DELAY * 200); // 2ms
    }
}

//�ɼ����������ź�
void rtThread(rtCollector &rtcollector, Socket &socketclient)
{
    while (true)
    {
        if (json.online)
        {
            if (rtcollector.getRTData())
            {
                if (rtcollector.rt_pack.validity)
                {
                    // cout << json.setRTDoc(rtcollector) << endl;
                    socketclient.send_data(json.setRTDoc(rtcollector));
                }
            }
            else
            {
                cout << "Not Found RT, Try to startUp" << endl;
                rtcollector.startWork();
            }
        }

        usleep(RT_SAMPLE_DELAY); // 500ms
    }
}

//�ɼ�������̬�ź�
void jyThread(JY901BCollector &jycollector, Socket &socketclient)
{
    while (true)
    {
        if (json.online && jycollector.getJYData())
        {
            // cout << json.setJYDoc(jycollector) << endl;
            //socketclient.send_data(json.setJYDoc(jycollector));
        }
        // usleep(MS_DELAY * 200);
        usleep(JY_SAMPLE_DELAY); // 2.5ms
    }
}

//��ȡѧ��CardId
void redisThread(Redis &redis)
{
    redis.listen_channel_message();
}

// ����server�����ݲ��������״̬
void socketThread(Socket &socketclient)
{
    while (true)
    {
        if (socketclient.checkConnectState())
        {
            if (socketclient.recv_data(socketclient.recvBuff) > 0)
            {

            }
            else
            {
                socketclient.con_state = false;
                cout << "Reconnect Server" << endl;
                socketclient.connectServer();
            }
        }
        else
        {
            cout << "Reconnect Server" << endl;
            socketclient.connectServer();
        }

        usleep(CLIENT_RECV_DELAY); //ÿ��2ms���һ������״̬
    }
}
