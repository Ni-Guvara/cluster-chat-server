#include "FriendModel.hpp"
#include "db.hpp"

void FriendModel::insert(int id, int friednid)
{
    // 组装更新sql
    char sql[1024] = {0};
    sprintf(sql, "insert into Friend values(%d, %d)", id, friednid);

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return;
        }
    }
    return;
}

void FriendModel::remove(int id, int friendid)
{
}

vector<User> FriendModel::query(int id)
{
    // 组装更新sql
    char sql[1024] = {0};
    sprintf(sql, "select * from User u left join Friend f on f.friendid = u.id where f.userid = %d;", id);

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            vector<User> friends;
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User u;
                u.setId(atoi(row[0]));
                u.setName(row[1]);
                u.setPassword("");
                u.setStatus(row[3]);
                friends.push_back(u);
            }
            mysql_free_result(res);
            return friends;
        }
    }
    return vector<User>();
}