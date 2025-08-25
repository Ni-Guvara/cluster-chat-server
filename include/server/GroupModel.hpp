#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include <vector>
#include "Group.hpp"

using namespace std;

class GroupModel
{
public:
    // 创建组
    bool create(Group &group);

    // 添加组员
    bool addGroup(int userid, int groupid, string role);

    // 获查询用户所在群组信息
    vector<Group> queryGroup(int userid);

    // 查询指定group的用户信息除了，用户本身
    vector<int> queryGroupUsers(int userid, int groupid);

private:
};
#endif