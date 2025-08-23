#include "chatservice.hpp"

#include "public.hpp"
#include <muduo/base/Logging.h>

using namespace placeholders;
// 注册消息以及对应回调操作
ChatService::ChatService()
{
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
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
    int id = js["id"];
    string password = js["password"];
    User user = _userModel.query(id);

    if (user.getId() == id && user.getPassWord() == password)
    {
        if (user.getStatus() == "online")
        {
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "该账号已登录，请勿重复登录！！！";
            conn->send(response.dump());
        }
        else
        {
            /*
             * 某用户发送信息的时候，服务器需要向其他用户推送信息，因此需要保存每个登录用户的连接
             */
            // 加锁, _userConnMap添加连接的时候保证线程安全
            {
                lock_guard<mutex> lck(_connMtx);
                // 登录成功，记录用户登录连接
                _userConnMap.insert({user.getId(), conn});
            }

            // 登录成功, 更新用户状态信息
            user.setStatus("online");
            _userModel.updateStatus(user);
            {
                json response;
                response["msgid"] = LOGIN_MSG_ACK;
                response["id"] = user.getId();
                response["name"] = user.getName();
                response["errno"] = 0;

                // 查询当前用户是否有离线消息
                vector<string> msgs = _offlineMsgModel.query(id);

                if (!msgs.empty())
                {
                    response["offlinemsg"] = msgs;
                    // 读取完所有离线消息将消息删除
                    _offlineMsgModel.remove(id);
                }

                conn->send(response.dump());
            }
        }
    }
    else
    {
        // 登陆失败
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "用户名或密码错误！！！";
        conn->send(response.dump());
    }
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

/*
    点对点聊天
*/
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toId = js["to"];
    string msg = js["msg"];

    {
        lock_guard<mutex> lck(_connMtx);
        auto it = _userConnMap.find(toId);
        if (it != _userConnMap.end()) // toId在线
        {
            // 转发消息
            it->second->send(js.dump());
            return;
        }
    }

    // 存储离线消息
    _offlineMsgModel.insert(toId, msg);
}

/*
    客户端会异常退出

    1、删除_userConnMap中的连接
    2、将客户的状态从online--->offline
*/
void ChatService::closeClientException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lck(_connMtx);

        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    // 更新用户状态信息
    if (user.getId() != -1)
    {
        user.setStatus("offline");
        _userModel.updateStatus(user);
    }
}