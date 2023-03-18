#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include "head.h"

using namespace std;

extern queue<char> data_queue; // 用于线程间通信的队列

extern JSON json;

void emgThread(emgCollector &emgcollector, Socket &socketclient);
void rtThread(rtCollector &rtcollector, Socket &socketclient);
void jyThread(JY901BCollector &jycollector, Socket &socketclient);
void redisThread(Redis &redis);
void socketThread(Socket &socketclient);

//读者优先
class readWriteLock
{
private:
    std::mutex readMtx;
    std::mutex writeMtx;
    int readCnt;  // 已加读锁个数
    int writeCnt; //已加写锁个数

public:
    readWriteLock() : readCnt(0), writeCnt(0) {}

    void readLock()
    {
        readMtx.lock();
        if (++readCnt == 1)
        {
            writeMtx.lock(); // 存在线程读操作时，写加锁（只加一次）
        }
        readMtx.unlock();
    }
    void readUnlock()
    {
        readMtx.lock();
        if (--readCnt == 0)
        { // 没有线程读操作时，释放写锁
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

//写者优先
class writeReadLock
{
private:
    std::mutex readMtx;
    std::mutex writeMtx;
    std::mutex rcMtx;
    std::mutex wcMtx;
    int readCnt;  // 已加读锁个数
    int writeCnt; //已加写锁个数

public:
    writeReadLock() : readCnt(0), writeCnt(0) {}

    void readLock()
    {
        readMtx.lock();
        rcMtx.lock();
        if (++readCnt == 1)
        {
            writeMtx.lock(); // 存在线程读操作时，写加锁（只加一次）
        }
        rcMtx.unlock();
        readMtx.unlock();
    }
    void readUnlock()
    {
        rcMtx.lock();
        if (--readCnt == 0)
        {
            writeMtx.unlock(); // 没有线程读操作时，释放写锁
        }
        rcMtx.unlock();
    }

    void writeLock()
    {
        wcMtx.lock();
        if (++writeCnt == 1)
        {
            readMtx.lock(); // 存在线程写操作时，读加锁
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
            readMtx.unlock(); // 没有线程写操作时，释放读锁
        }
        wcMtx.unlock();
    }
};

extern readWriteLock rwLock; // 定义全局的读写锁变量(读优先)
extern writeReadLock wrLock; // 定义全局的读写锁变量(写优先)

#endif
