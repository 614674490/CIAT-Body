#include "head.h"
#include "redis.h"
#include "thread.h"

using namespace std;

Redis::Redis(const char *host, int port, const char *password)
{
    this->host = host;
    this->port = port;
    this->password = password;
    connect();
    auth();
    json.setCardId(get(cmd.CardId)); // read student cardId before subscribe
    subscribe(cmd.channel);
}

void Redis::init()
{
    connect();
    auth();
    subscribe(cmd.channel);
}

bool Redis::connect()
{
    try
    {
        this->_context = redisConnect(this->host, this->port);
        if (this->_context != NULL && this->_context->err)
        {
            cerr << "connect error: " << this->_context->errstr << endl;
            this->con_state = false;
            redisFree(this->_context);
            return false;
        }
    }
    catch (exception e)
    {
        cerr << "Redis Connect Failed: " << e.what() << endl;
        this->con_state = false;
        redisFree(this->_context);
        return false;
    }
    cout << "connected to redis database" << endl;
    this->con_state = true;
    return true;
}

bool Redis::disconnect()
{
    if (this->_context)
    {
        redisFree(this->_context);
        con_state = false;
    }
    else
    {
        cerr << "Disconnect Failed: "
             << "redis context not exits" << endl;
        return false;
    }
    return true;
}

bool Redis::auth()
{
    try
    {
        if (con_state)
        {
            this->_reply = (redisReply *)redisCommand(this->_context, "AUTH %s", this->password);
            if (this->_reply->type == REDIS_REPLY_ERROR)
            {
                cout << "Redis authentucatuin failed" << endl;
                this->con_state = false;
                freeReplyObject(this->_reply);
                return false;
            }
            else
            {
                cout << "Redis authentucatuin successed" << endl;
                freeReplyObject(this->_reply);
            }
        }
        else
        {
            cerr << "AUTH Failed: "
                 << "not redis context" << endl;
            return false;
        }
    }
    catch (const exception &e)
    {
        cerr << "Auth Failed: " << e.what() << endl;
        return false;
    }
    return true;
}

string Redis::get(string key)
{
    string str = "";
    try
    {
        if (this->con_state)
        {
            this->_reply = (redisReply *)redisCommand(this->_context, "GET %s", key.c_str());
            if (this->_reply->type == REDIS_REPLY_ERROR)
            {
                cerr << this->_reply->str << endl;
            }
            else
                str = this->_reply->str;

            freeReplyObject(this->_reply);
        }
        else
        {
            cerr << "GET Failed: "
                 << "not redis context" << endl;
        }
    }
    catch (const exception &e)
    {
        cerr << "Get Failed: " << e.what() << endl;
    }

    return str;
}

bool Redis::set(string key, string value)
{
    try
    {
        if (this->con_state)
        {
            this->_reply = (redisReply *)redisCommand(this->_context, "SET %s %s", key.c_str(), value.c_str());
            if (this->_reply->str != "OK")
            {
                cerr << this->_reply->str << endl;
                freeReplyObject(this->_reply);
                return false;
            }
            freeReplyObject(this->_reply);
        }
        else
        {
            cerr << "SET Failed: "
                 << "not redis context" << endl;
            return false;
        }
    }
    catch (const exception &e)
    {
        cerr << "Set Failed: " << e.what() << endl;
        return false;
    }
    return true;
}

bool Redis::subscribe(string channel)
{
    try
    {
        if (con_state)
        {
            this->_reply = (redisReply *)redisCommand(this->_context, "SUBSCRIBE %s", channel.c_str());
            if (this->_reply->type == REDIS_REPLY_ERROR)
            {
                cerr << this->_reply->str << endl;
                freeReplyObject(this->_reply);
                return false;
            }
            freeReplyObject(this->_reply);
        }
        else
        {
            cerr << "Subscribe Failed: "
                 << "not redis context" << endl;
            return false;
        }
    }
    catch (const exception &e)
    {
        cerr << "Subscribe Failed" << e.what() << endl;
        return false;
    }
    cout << "Subscribe Successed" << endl;
    return true;
}

bool Redis::unsubscribe(string channel)
{
    try
    {
        if (con_state)
        {
            this->_reply = (redisReply *)redisCommand(this->_context, "UNSUBSCRIBE %s", channel.c_str());
            if (this->_reply->type == REDIS_REPLY_ERROR)
            {
                cerr << this->_reply->str << endl;
                freeReplyObject(this->_reply);
                return false;
            }
            freeReplyObject(this->_reply);
        }
        else
        {
            cerr << "Unsubscribe Failed: "
                 << "not redis context" << endl;
            return false;
        }
    }
    catch (const exception &e)
    {
        cerr << "UnSubscribe Failed" << e.what() << endl;
        return false;
    }
    return true;
}

void Redis::listen_channel_message()
{
    while(!this->con_state)
    {
        cout << "Reconnect Redis" << endl;
        init();
        sleep(1);
    }
    cout << "start listen to redis" << endl;
    while (REDIS_OK == redisGetReply(this->_context, (void **)&(this->_reply))) // block recv
    {
        if (this->_reply)
        {
            cout << "recv message: ";
            switch (this->_reply->type)
            {
            case REDIS_REPLY_ARRAY: // recv message is array
                /* array process */
                cout << "Array - ";
                for (int i = 0; i < this->_reply->elements; i++)
                    cout << this->_reply->element[i]->str << " ";
                cout << endl;
                json.setCardId(this->_reply->element[2]->str);
                break;

            case REDIS_REPLY_STATUS:
                /* status message */
                cout << "Status - " << this->_reply->str << endl;
                break;

            case REDIS_REPLY_ERROR: // command play error
                /* error process */
                cerr << "Error - " << this->_reply->str << endl;
                con_state = false;
                init();
                break;

            case REDIS_REPLY_STRING: // recv message is str
                /* string process */
                cout << "String - " << this->_reply->str << endl;
                break;

            case REDIS_REPLY_INTEGER: // recv message is int
                /* int process */
                cout << "Integer - " << this->_reply->integer << endl;
                break;

            case REDIS_REPLY_NIL: // recv message is NULL
                /* NULL process */
                cout << "NULL" << endl;
                break;

            default:
                break;
            }
            freeReplyObject(this->_reply);
        }
        else
        {
            cerr << "Listen Failed: "
                 << "not redis reply" << endl;
            con_state = false;
            init();
        }
    }
    cout << "listen exit" << endl;
}

Redis::~Redis()
{
    disconnect();
    cout << "close Redis" << endl;
}
