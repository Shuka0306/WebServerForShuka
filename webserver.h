#ifndef _WEBSERVER_H__
#define _WEBSERVER_H__
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "locker.h"
#include "threadpool.h"
#include "http_conn.h"
#define MAX_FD 65536   // 最大的文件描述符个数
#define MAX_EVENT_NUMBER 10000  // 监听的最大的事件数量
extern void addfd( int epollfd, int fd, bool one_shot );
extern void removefd( int epollfd, int fd );
static void addsig(int sig,void( handler )(int))
{
        struct sigaction sa;
    memset( &sa, '\0', sizeof( sa ) );
    sa.sa_handler = handler;
    sigfillset( &sa.sa_mask );
    assert( sigaction( sig, &sa, NULL ) != -1 );
}
class WebServer
{
    public:
    WebServer();
    ~WebServer();

    private:
    void init();
    private:
    //监听描述符
    int m_listenfd;
    //用户fd数组
    http_conn* users;
    //epoll Array
    epoll_event* events;
    //epoll fd;
    threadpool<http_conn>* pool;

    int epollfd;
    public:
    void Listen(int port);
    void EventLoop();

};
#endif