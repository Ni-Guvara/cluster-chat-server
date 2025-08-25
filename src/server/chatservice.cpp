#include "chatservice.hpp"

#include "public.hpp"
#include <muduo/base/Logging.h>

using namespace placeholders;
// 注册消息以及对应回调操作
ChatService::ChatService()
{
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
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

                // 查询用户好友信息
                vector<User> friends = _friendModel.query(id);
                if (!friends.empty())
                {
                    vector<string> vec;
                    for (User &u : friends)
                    {
                        json js;
                        js["id"] = u.getId();
                        js["name"] = u.getName();
                        js["status"] = u.getStatus();
                        vec.push_back(js.dump());
                    }

                    response["friends"] = vec;
                }

                // 群组信息
                vector<Group> groups = _groupModel.queryGroup(id);
                if (!groups.empty())
                {
                    vector<string> vec;
                    for (auto group : groups)
                    {
                        json js;
                        js["id"] = group.getId();
                        js["groupname"] = group.getName();
                        js["groupdesc"] = group.getDesc();

                        vector<GroupUser> groupUsers = group.getGroupUsers();
                        vector<string> vec2;

                        for (auto user : groupUsers)
                        {
                            json groupUsersJs;
                            groupUsersJs["id"] = user.getId();
                            groupUsersJs["name"] = user.getName();
                            groupUsersJs["status"] = user.getStatus();
                            groupUsersJs["role"] = user.getRole();
                            vec2.push_back(groupUsersJs.dump());
                        }
                        js["groupusers"] = vec2;

                        vec.push_back(js.dump());
                    }

                    response["groups"] = vec;
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
    添加好友业务
    msgid id friendid
*/
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int id = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    // 存储好友信息
    _friendModel.insert(id, friendid);
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

void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["userid"].get<int>();
    Group g(-1, js["groupname"], js["groupdesc"]);

    if (_groupModel.create(g))
    {
        // 添加创建人信息
        _groupModel.addGroup(userid, g.getId(), "creator");
    }
}

// 加入群聊
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    _groupModel.addGroup(js["userid"].get<int>(), js["groupid"].get<int>(), "normal");
}

// 群聊
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["userid"];
    int groupid = js["groupid"];
    vector<int> userVec = _groupModel.queryGroupUsers(userid, groupid);

    for (int id : userVec)
    {
        lock_guard<mutex> mtx(_connMtx);
        auto it = _userConnMap.find(id);

        if (it != _userConnMap.end()) // 如果在线,进行转发
        {
            it->second->send(js.dump());
        }
        else // 如果离线，进行离线消息存储
        {
            string msg = js.dump();
            _offlineMsgModel.insert(id, msg);
        }
    }
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

void ChatService::reset()
{
    // 登录的客户端登陆状态online--->offline
    _userModel.resetStatus();
}