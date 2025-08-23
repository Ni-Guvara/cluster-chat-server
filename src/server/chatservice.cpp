#include "chatservice.hpp"

#include "public.hpp"
#include <muduo/base/Logging.h>

using namespace placeholders;
// 注册消息以及对应回调操作
ChatService::ChatService()
{
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
}
ChatService::~ChatService() {}

ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

MsgHandler ChatService::getHandler(int msgId)
{
    auto it = _msgHandlerMap.find(msgId);
    if (it == _msgHandlerMap.end())
    {
        LOG_ERROR << "msgid:" << msgId << " can not find handler!";
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time) {}; // 返回空操作, lambda
    }
    else
    {
        return _msgHandlerMap[msgId];
    }
}

// 处理登陆业务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    LOG_INFO << "do login service!";
}

// 处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string name = js["name"];
    string password = js["password"];

    User user;
    user.setName(name);
    user.setPassword(password);

    bool state = _userModel.insert(user);

    if (state)
    {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    }
    else
    {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}