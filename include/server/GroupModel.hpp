#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include <vector>
#include "Group.hpp"

using namespace std;

class GroupModel
{
public:
    // 创建组
    void create(int groupid);

    // 添加组员
    void addGroupUser(int groupid, int userid, string role);

    // 获查询用户所在群组信息
    vector<Group> queryGroup(int userid);

    // 查询指定group的用户信息除了，用户本身
    vector<int> queryGroupUsers(int userid, int groupid);

private:
};
#endif