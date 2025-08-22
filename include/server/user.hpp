#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

class User
{

public:
    User(int id = -1, string name = "", string pwd = "", string status = "offline")
    {
        this->id = id;
        this->name = name;
        this->password = pwd;
        this->status = status;
    }

    void setId(int id)
    {
        this->id = id;
    }

    void setName(string name)
    {
        this->name = name;
    }

    void setPassword(string passWord)
    {
        this->password = passWord;
    }

    void setStatus(string status)
    {
        this->status = status;
    }

    int getId()
    {
        return id;
    }

    string getName()
    {
        return name;
    }
    string getPassWord()
    {
        return password;
    }
    string getStatus()
    {
        return status;
    }

private:
    int id;
    string name;
    string password;
    string status;
};
#endif