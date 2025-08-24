#ifndef GROUPUSER_H
#define GRIUPUSER_H
#include "user.hpp"

class GroupUser : public User
{
public:
    void setRole(string role);

    string getRole();

private:
    string role;
};
#endif