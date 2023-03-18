/*
 * @Author: Ken Kaneki
 * @Date: 2021-11-16 22:02:56
 * @LastEditTime: 2022-05-31 23:07:05
 * @Description: README
 */
#include <getopt.h>
#include <signal.h>
#include "thread.h"

using namespace std;

bool app_stopped = false;

void sigint_handler(int sig)
{
    if (sig == SIGINT)
    {
        // ctrl+c退出时执行的代码
        cout << "ctrl+c pressed!" << endl;
        app_stopped = true;
    }
}

int main(int argc, char **argv)
{
    const char *socket_ip = IP;
    uint16_t socket_port = PORT;
    const char *redis_ip = Redis_IP;
    uint16_t redis_port = Redis_PORT;
    const char *redis_pw = Redis_PASSWORD;
    int opt, lopt, loidx;
    const char *optstring = "a:b:c:d:";
    const struct option long_options[] = {
        {"socket_ip", required_argument, &lopt, 1},
        {"socket_port", required_argument, &lopt, 2},
        {"redis_ip", required_argument, &lopt, 3},
        {"redis_port", required_argument, &lopt, 4},
        {"redis_pw", required_argument, &lopt, 5},
    };
    signal(SIGINT, sigint_handler);
    while ((opt = getopt_long(argc, argv, optstring, long_options, &loidx)) != -1)
    {
        if (opt == 0)
            opt = lopt;
        switch (opt)
        {
        case 1:
            socket_ip = optarg;
            break;
        case 2:
            socket_port = atoi(optarg);
            break;
        case 3:
            redis_ip = optarg;
            break;
        case 4:
            redis_port = atoi(optarg);
            break;
        case 5:
            redis_pw = optarg;
            break;
        default:
            printf("error opt %c");
            return -1;
        }
    }
    cout << "socket_ip:socket_port<-->" << socket_ip << ":" << socket_port << endl;
    cout << "redis_ip:redis_port:redis_pw<-->" << redis_ip << ":" << redis_port << ":" << redis_pw << endl;

    //Redis redis(redis_ip, redis_port, redis_pw);

    emgCollector emgcollector;
    rtCollector rtcollector(RTDEV, RTBAUD);
    JY901BCollector jycollector(JYDEV, JYBAUD);
    Socket socketclient(socket_ip, socket_port);

    //thread student(redisThread, ref(redis));
    thread emg(emgThread, ref(emgcollector), ref(socketclient));
    thread rt(rtThread, ref(rtcollector), ref(socketclient));
    thread jy(jyThread, ref(jycollector), ref(socketclient));
    thread socket(socketThread, ref(socketclient));

    //student.detach();
    socket.detach();
    emg.detach();
    rt.detach();
    jy.detach();

    while (true)
    {
        if (app_stopped)
        {
            break;
        }
    }

    return 1;
}
