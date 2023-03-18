/*
 * @Author: Ken Kaneki
 * @Date: 2021-11-16 22:02:56
 * @LastEditTime: 2022-06-01 10:35:33
 * @Description: README
 */
#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/net.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <string.h>

#define IP (const char *)("192.168.0.102")
#define PORT 50001

#define RECVLEN 100

class Socket
{
public:
    struct sockaddr_in remote_addr; //服务器端网络地址结构体
    bool con_state = false;
    char *recvBuff;
    Socket(const char *addr, int port);

    bool connectServer();
    bool disconnect();
    bool checkConnectState();
    int send_data(std::string data);
    bool SendAll(const char *buffer, int size);
    int recv_data(char *data);

    ~Socket();

private:
    int client_sockfd=0;
};

#endif
