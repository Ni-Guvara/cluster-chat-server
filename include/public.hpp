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
    ONE_CHAT_MSG // 聊天消息
};
#endif