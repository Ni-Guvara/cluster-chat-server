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
            // 插入成功获取用户逐渐id
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}