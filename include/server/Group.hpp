#ifndef GROUP_H
#define GROUP_H
#include <iostream>
#include <vector>
#include "GroupUser.hpp"
using namespace std;

class Group
{
public:
    Group(int id = -1, string name = "", string desc = "")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }

    ~Group()
    {
    }

    void setId(int id)
    {
        this->id = id;
    }
    int getId()
    {
        return this->id;
    }

    void setName(string name)
    {
        this->name = name;
    }
    string getName()
    {
        return this->name;
    }

    void setDesc(string desc)
    {
        this->desc = desc;
    }

    string getDesc()
    {
        return this->desc;
    }

    void setGroupUsers(vector<GroupUser> users)
    {
        this->users = users;
    }

    vector<GroupUser> getGroupUsers()
    {
        return users;
    }

private:
    int id;
    string name;
    string desc;
    vector<GroupUser> users;
};
#endif
