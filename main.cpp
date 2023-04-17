#include "locker.h"
#include "threadpool.h"
#include "http_conn.h"
#include "webserver.h"

int main( int argc, char* argv[] ) {
    
    if( argc <= 1 ) {
        printf( "usage: %s port_number\n", basename(argv[0]));
        return 1;
    }

    int port = atoi( argv[1] );
    WebServer shuka;

    //服务器开始监听
    shuka.Listen(port);
    //服务器开始运行
    shuka.EventLoop();


    return 0;
}