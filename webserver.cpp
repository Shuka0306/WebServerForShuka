#include "webserver.h"
WebServer::WebServer()
{
    m_listenfd=0;
    users=new http_conn[MAX_FD];
    events=new epoll_event[MAX_EVENT_NUMBER];
    try {
        //保护段代码
        pool = new threadpool<http_conn>;
    } catch( ... ) {
        return ;
    }
    epollfd=0;
};
WebServer::~WebServer()
{
    delete[] users;
    delete[] events;
    delete pool;
    close(epollfd);
};
void WebServer::init()
{

};
void WebServer::Listen(int port)
{
    addsig( SIGPIPE, SIG_IGN );
    threadpool< http_conn >* pool = NULL;
    try {
        //保护段代码
        pool = new threadpool<http_conn>;
    } catch( ... ) {
        return ;
    }
    //创建监听描述符
    m_listenfd = socket( PF_INET, SOCK_STREAM, 0 );
    int ret = 0;
    //结构体赋值 
    struct sockaddr_in address;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_family = AF_INET;
    address.sin_port = htons( port );

    // 端口复用
    int reuse = 1;
    setsockopt( m_listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof( reuse ) );
    //绑定 然后返回一个返回值
    ret = bind( m_listenfd, ( struct sockaddr* )&address, sizeof( address ) );
    //开始监听 
    ret = listen( m_listenfd,5);
}
void WebServer::EventLoop()
{
    //帮向内核注册事件表 返回的是 事件表的文件描述符
    epollfd = epoll_create( 5 );
    //将监听描述符写入epollfd
    addfd( epollfd, m_listenfd, false );
    http_conn::m_epollfd = epollfd;
    //使用模拟proctor方式
    while(true) {
        //阻塞函数 返回有变化的fd的个数 并且把发生变化的放到 event数组里
        int number = epoll_wait( epollfd, events, MAX_EVENT_NUMBER, -1 );
        //判断epoll_wait是否出错
        if ( ( number < 0 ) && ( errno != EINTR ) ) {
            printf( "epoll failure\n" );
            break;
        }
        //把event数组遍历一遍 event
        for ( int i = 0; i < number; i++ ) {
            //取出events中的文件描述符
            int sockfd = events[i].data.fd;
            //如果遍历到监听描述符 检测
            if( sockfd == m_listenfd ) {
                
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof( client_address );
                int connfd = accept(m_listenfd, ( struct sockaddr* )&client_address, &client_addrlength );
                //connfd<0 说明发生错误
                if ( connfd < 0 ) {
                    printf( "errno is: %d\n", errno );
                    continue;
                } 
                //超过最大监听数 不接受这个描述符
                if( http_conn::m_user_count >= MAX_FD ) {
                    close(connfd);
                    continue;
                }
                //把这个描述符放到 数组当中 这个数组用来存放接进来数组
                users[connfd].init( connfd, client_address);

            } else if( events[i].events & ( EPOLLRDHUP | EPOLLHUP | EPOLLERR ) ) {

                users[sockfd].close_conn();

            } else if(events[i].events & EPOLLIN) {
                //读实践
                if(users[sockfd].read()) {
                    pool->append(users + sockfd);
                } else {
                    users[sockfd].close_conn();
                }

            }  else if( events[i].events & EPOLLOUT ) {
                //写事件
                if( !users[sockfd].write() ) {
                    users[sockfd].close_conn();
                }

            }
        }
    }
    
}