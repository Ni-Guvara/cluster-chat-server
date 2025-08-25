#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>

#include "json/json.hpp"
#include "usermodel.hpp"
#include "OfflineMessageModel.hpp"
#include "FriendModel.hpp"
#include "GroupModel.hpp"

using namespace std;
using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;

using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp time)>; // 聊天服务器业务类

class ChatService
{
public:
    static ChatService *instance();

    // 获取消息对应的处理器
    MsgHandler getHandler(int msgId);

    // 处理登陆业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 处理注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // one to one message
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 添加好友业务
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 创建群聊
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 加入群聊
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 群聊
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 处理客户端异常退出
    void closeClientException(const TcpConnectionPtr &conn);

    // 服务器异常，重置方法
    void reset();

private:
    ChatService();
    ~ChatService();

    unordered_map<int, TcpConnectionPtr> _userConnMap;
    unordered_map<int, MsgHandler> _msgHandlerMap;

    mutex _connMtx;

    // 数据操作类
    UserModel _userModel;
    OfflineMessageModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;
};

#endif