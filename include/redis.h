#ifndef REDIS_H_INCLUDED
#define REDIS_H_INCLUDED

#define REDISBUFFLEN 100
#include <hiredis/hiredis.h>

#define Redis_IP (const char *)("192.168.0.102")
#define Redis_PORT 6379
#define Redis_PASSWORD "123456"

typedef struct
{
    std::string CardId = "student";
    std::string channel = "studentTopic";
} RedisCMD;

class Redis
{
public:
    const char *host = "";
    int port = 0;
    const char *password = "";
    bool con_state = false;
    RedisCMD cmd;
    Redis(const char *host, int port, const char *password);
    ~Redis();

    void init();
    bool connect();
    bool disconnect();
    bool auth();

    std::string get(std::string key);
    bool set(std::string key, std::string value);

    bool subscribe(std::string channel);
    bool unsubscribe(std::string channel);
    void listen_channel_message();

private:
    redisContext *_context = NULL;
    redisReply *_reply = NULL;
};

#endif
