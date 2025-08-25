#ifndef PUBLIC_H
#define PUBLIC_H

/**
 * service与client公共文件
 *
 * */

enum EmMsgType
{
    LOGIN_MSG = 1,
    LOGIN_MSG_ACK,
    REG_MSG,
    REG_MSG_ACK,
    ONE_CHAT_MSG,     // 聊天消息
    ADD_FRIEND_MSG,   // 添加好友消息
    CREATE_GROUP_MSG, // 创建群聊
    ADD_GROUP_MSG,    // 加入群聊
    GROUP_CHAT_MSG
};
#endif