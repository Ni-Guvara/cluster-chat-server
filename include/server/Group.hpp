#ifndef GROUP_H
#define GROUP_H
#include <iostream>
#include <vector>
#include "GroupUser.hpp"
using namespace std;

class Group
{
public:
    Group(int id = -1, string name = "", string desc = "");
    ~Group();

    void setId(int id);
    int getId();
    void setName(string name);
    string getName();
    void setDesc(string desc);
    string getDesc();
    void setGroupUsers(vector<GroupUser> users);
    vector<GroupUser> getGroupUsers();

private:
    int id;
    string name;
    string desc;
    vector<GroupUser> users;
};
#endif
