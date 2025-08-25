#include "GroupModel.hpp"
#include "db.hpp"
#include <muduo/base/Logging.h>

// 创建组
bool GroupModel::create(Group &group)
{
    // 组装更新sql
    char sql[1024] = {0};
    sprintf(sql, "insert into AllGroup(groupname, groupdesc) values('%s', '%s')", group.getName().c_str(), group.getDesc().c_str());
    LOG_INFO << sql;

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            group.setId(mysql_insert_id(mysql.getConnection())); // 创建群组id设置到对象中
            return true;
        }
    }
    return false;
}

// 加入群组
bool GroupModel::addGroup(int userid, int groupid, string role)
{
    // 组装更新sql
    char sql[1024] = {0};
    sprintf(sql, "insert into GroupUser values(%d, %d, '%s')", groupid, userid, role.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

// 获查询用户所在群组信息
vector<Group> GroupModel::queryGroup(int userid)
{
    // 查询用户所在群组
    // 组装更新sql
    char sql[1024] = {0};
    sprintf(sql, "select a.id, a.groupname, a.groupdesc from AllGroup a inner join GroupUser b on a.id = b.groupid where b.userid = %d", userid);

    MySQL mysql;
    vector<Group> groups;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                Group g;
                g.setId(atoi(row[0]));
                g.setName(row[1]);
                g.setDesc(row[2]);
                groups.push_back(g);
            }
        }
        mysql_free_result(res);
    }

    // 查群组里面除了该用户的其他用户
    for (Group &g : groups)
    {
        char sql[1024] = {0};
        sprintf(sql, "select u.id, u.name, u.password, u.status, gu.grouprole from User u inner join GroupUser gu on u.id = gu.userid where gu.groupid = %d and u.id != %d", g.getId(), userid);
        MySQL mysql;
        vector<GroupUser> vec;
        if (mysql.connect())
        {
            MYSQL_RES *res = mysql.query(sql);
            if (res != nullptr)
            {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(res)) != nullptr)
                {
                    GroupUser g;
                    g.setId(atoi(row[0]));
                    g.setName(row[1]);
                    g.setPassword(row[2]);
                    g.setStatus(row[3]);
                    g.setRole(row[4]);
                    vec.push_back(g);
                }
            }
            mysql_free_result(res);
        }
        g.setGroupUsers(vec);
    }
    return groups;
}

// 查询指定group的用户信息除了，用户本身
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    char sql[1024] = {0};
    sprintf(sql, "select a.id from User a inner join GroupUser b on b.userid = a.id where a.id != %d and b.groupid = %d", userid, groupid);

    MySQL mysql;
    vector<int> vec;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                vec.push_back(atoi(row[0]));
            }
        }
        mysql_free_result(res);

        return vec;
    }
    return vector<int>();
}
