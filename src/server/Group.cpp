#include "Group.hpp"

Group::Group(int id = -1, string name = "", string desc = "")
{
    this->id = id;
    this->name = name;
    this->desc = desc;
}

Group ::~Group()
{
}

void Group::setId(int id)
{
    this->id = id;
}
int Group::getId()
{
    return this->id;
}

void Group::setName(string name)
{
    this->name = name;
}
string Group::getName()
{
    return this->name;
}

void Group::setDesc(string desc)
{
    this->desc = desc;
}

string Group::getDesc()
{
    return this->desc;
}

void Group::setGroupUsers(vector<GroupUser> users)
{
    this->users = users;
}

vector<GroupUser> Group::getGroupUsers()
{
    return users;
}
