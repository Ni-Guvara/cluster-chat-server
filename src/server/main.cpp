#include "chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h>

void handlerReset(int)
{
    ChatService::instance()->reset();
    exit(0);
}
int main()
{

    signal(SIGINT, handlerReset);

    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}