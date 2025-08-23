#include "usermodel.hpp"
#include "db.hpp"

bool UserModel::insert(User &user)
{
    // 组装sql
    char sql[1024] = {0};
    sprintf(sql, "insert into User(name, password, status) values ('%s', '%s', '%s')", user.getName().c_str(), user.getPassWord().c_str(), user.getStatus().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 插入成功获取用户主键id
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

bool UserModel::updateStatus(User &user)
{
    // 组装更新sql
    char sql[1024] = {0};
    sprintf(sql, "update User set status = '%s' where id = %d", user.getStatus().c_str(), user.getId());

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

User UserModel::query(int id)
{
    // 组装查询sql
    char sql[1024] = {0};
    sprintf(sql, "select * from User where id = %d", id);

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                user.setStatus(row[3]);

                // 释放MYSQL_RES
                mysql_free_result(res);
                return user;
            }
        }
    }

    return User();
}

void UserModel::resetStatus()
{

    // 组装更新sql
    char sql[1024] = {0};
    sprintf(sql, "update User set status = 'offline' where status = 'online'");

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return;
        }
    }
}