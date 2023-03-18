#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include "head.h"

using namespace std;

extern queue<char> data_queue; // �����̼߳�ͨ�ŵĶ���

extern JSON json;

void emgThread(emgCollector &emgcollector, Socket &socketclient);
void rtThread(rtCollector &rtcollector, Socket &socketclient);
void jyThread(JY901BCollector &jycollector, Socket &socketclient);
void redisThread(Redis &redis);
void socketThread(Socket &socketclient);

//��������
class readWriteLock
{
private:
    std::mutex readMtx;
    std::mutex writeMtx;
    int readCnt;  // �ѼӶ�������
    int writeCnt; //�Ѽ�д������

public:
    readWriteLock() : readCnt(0), writeCnt(0) {}

    void readLock()
    {
        readMtx.lock();
        if (++readCnt == 1)
        {
            writeMtx.lock(); // �����̶߳�����ʱ��д������ֻ��һ�Σ�
        }
        readMtx.unlock();
    }
    void readUnlock()
    {
        readMtx.lock();
        if (--readCnt == 0)
        { // û���̶߳�����ʱ���ͷ�д��
            writeMtx.unlock();
        }
        readMtx.unlock();
    }

    void writeLock()
    {
        writeMtx.lock();
    }
    void writeUnlock()
    {
        writeMtx.unlock();
    }
};

//д������
class writeReadLock
{
private:
    std::mutex readMtx;
    std::mutex writeMtx;
    std::mutex rcMtx;
    std::mutex wcMtx;
    int readCnt;  // �ѼӶ�������
    int writeCnt; //�Ѽ�д������

public:
    writeReadLock() : readCnt(0), writeCnt(0) {}

    void readLock()
    {
        readMtx.lock();
        rcMtx.lock();
        if (++readCnt == 1)
        {
            writeMtx.lock(); // �����̶߳�����ʱ��д������ֻ��һ�Σ�
        }
        rcMtx.unlock();
        readMtx.unlock();
    }
    void readUnlock()
    {
        rcMtx.lock();
        if (--readCnt == 0)
        {
            writeMtx.unlock(); // û���̶߳�����ʱ���ͷ�д��
        }
        rcMtx.unlock();
    }

    void writeLock()
    {
        wcMtx.lock();
        if (++writeCnt == 1)
        {
            readMtx.lock(); // �����߳�д����ʱ��������
        }
        wcMtx.unlock();
        writeMtx.lock();
    }
    void writeUnlock()
    {
        writeMtx.unlock();
        wcMtx.lock();
        if (--writeCnt == 0)
        {
            readMtx.unlock(); // û���߳�д����ʱ���ͷŶ���
        }
        wcMtx.unlock();
    }
};

extern readWriteLock rwLock; // ����ȫ�ֵĶ�д������(������)
extern writeReadLock wrLock; // ����ȫ�ֵĶ�д������(д����)

#endif
