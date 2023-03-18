#include "head.h"
#include "socket.h"

using namespace std;

Socket::Socket(const char *addr, int port)
{
    memset(&remote_addr, 0, sizeof(remote_addr));      //数据初始化--清零
    remote_addr.sin_family = AF_INET;                  //设置为IP通信
    remote_addr.sin_addr.s_addr = inet_addr(addr);     //服务器IP地址
    remote_addr.sin_port = htons(port);                //服务器端口号
    recvBuff = (char *)malloc(sizeof(char) * RECVLEN); //接收缓存
    connectServer();
}

bool Socket::connectServer()
{
    try
    {
        /*创建客户端套接字--IPv4协议，面向连接通信，TCP协议*/
        if ((this->client_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("socket");
            con_state = false;
            return false;
        }
        /*将套接字绑定到服务器的网络地址上*/
        if (connect(this->client_sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) < 0)
        {
            perror("connect");
            con_state = false;
            return false;
        }
    }
    catch (exception e)
    {
        cout << "Server connect failed" << e.what() << endl;
        con_state = false;
        return false;
    }

    cout<<"connected to server"<<endl;
    con_state = true;
    return true;
}

bool Socket::disconnect()
{
    if (this->client_sockfd)
    {
        close(this->client_sockfd);
        return true;
    }
    else
    {
        cerr << "socket connect not exits" << endl;
        return false;
    }
}

bool Socket::checkConnectState()
{
    struct tcp_info info;
    int len = sizeof(info);
    getsockopt(this->client_sockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
    if ((info.tcpi_state != TCP_ESTABLISHED))
    {
        con_state = false;
        cout << "Reconnect Server" << endl;
        connectServer();
    }
    else
        con_state = true;
    return con_state;
}

int Socket::send_data(string data)
{
    int len = 0;
    if (!con_state)
    {
        cout << "Not found connect" << endl;
        return -1;
    }

    try
    {
        len = send(this->client_sockfd, data.c_str(), data.length(), 0);
    }
    catch (exception &e)
    {
        con_state = false;
        cout << "Client send failed" << e.what() << endl;
        return -1;
    }
    if (len <= 0)
        con_state = false;

    return len;
}

bool Socket::SendAll(const char *buffer, int size)
{
    while (size > 0)
    {
        int SendSize = send(this->client_sockfd, buffer, size, 0);

        if (SendSize < 0)
            return false;
        size = size - SendSize; //用于循环发送且退出功能
        buffer += SendSize;     //用于计算已发buffer的偏移量
    }
    return true;
}

//阻塞接收server端数据
int Socket::recv_data(char *data)
{
    int len = 0;
    memset(data, '\0', strlen(data));
    len = recv(this->client_sockfd, data, strlen(data), 0);
    return len;
}

Socket::~Socket()
{
    disconnect();
    cout << "close Socket" << endl;
}
