#ifndef FRIEDNFMODEL_H
#define FRIEDNMODEL_H

#include <vector>
#include "user.hpp"
using namespace std;

class FriendModel
{
public:
    // 添加好友
    void insert(int id, int friednid);
    // 删除好友
    void remove(int id, int friendid);
    // 返回好友列表
    vector<User> query(int id);

private:
};
#endif