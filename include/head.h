#ifndef HEAD_H_INCLUDED
#define HEAD_H_INCLUDED

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <memory.h>
#include <string>
#include <math.h>
#include <sys/time.h> // ÒýÈë struct timeval

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "emg.h"
#include "socket.h"
#include "rt.h"
#include "jy901B.h"
#include "redis.h"

#define MS_DELAY 1000                  // 1ms
#define EMG_SAMPLE_DELAY MS_DELAY * 15 // 15ms
#define RT_SAMPLE_DELAY MS_DELAY * 500 // 500ms
#define JY_SAMPLE_DELAY MS_DELAY * 2.5 // 2.5ms
#define JY_CMD_DELAY MS_DELAY * 100    // 100ms
#define CLIENT_RECV_DELAY MS_DELAY * 2 // 2ms

#define JSONUM 3

class JSON
{
public:
    JSON();
    rapidjson::Document emgDoc;
    rapidjson::Document rtDoc;
    rapidjson::Document jyDoc;

    bool online = false;

    std::string setEmgDoc(emgCollector &emgcollector);
    std::string setRTDoc(rtCollector &rtcollector);
    std::string setJYDoc(JY901BCollector &jycollector);

    void setCardId(std::string str);
    std::string getCardId();
    bool strIsNum(std::string str);
    bool Isonline();

    ~JSON(){};

private:
    std::string CardId = "";
    std::string eof = "_$";
};

long int getTickUs(void);
void delayUs(long int us);

#endif
