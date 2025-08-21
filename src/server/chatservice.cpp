#include "chatservice.hpp"

#include "public.hpp"

ChatService::ChatService() {}
ChatService::~ChatService() {}

ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

// 处理登陆业务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
}

// 处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
}