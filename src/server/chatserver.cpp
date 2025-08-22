#include "chatserver.hpp"
#include "chatservice.hpp"
#include "json/json.hpp"
#include <functional>
#include <iostream>

using namespace std;
using namespace nlohmann;
using namespace placeholders;

ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArgp) : _server(loop, listenAddr, nameArgp), _loop(loop)
{
    // 注册回调函数
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    _server.setThreadNum(4);
}

void ChatServer::start()
{
    _server.start();
}

// 上报连接相关信息的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        conn->shutdown();
    }
}

// 上报读写事件相关信息的回调函数
void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp timestamp)
{
    string buf = buffer->retrieveAllAsString();
    cout << buf << endl;
    // 数据反序列化
    json js = json::parse(buf);

    // 完全解耦网络模块以及业务模块代码
    // 通过单例模式（懒汉）得到调用函数得到对应的handler
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    // 回调消息绑定好的事件处理器，执行相应业务
    msgHandler(conn, js, timestamp);
}