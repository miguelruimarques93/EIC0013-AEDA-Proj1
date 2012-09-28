#ifndef USER_H_
#define USER_H_

#include <string>
#include <vector>

#include "utils.h"

class Job;


class User
{
public:
    const std::string& GetName() const { return _name; }

protected:
    User(const std::string& name) : _name(name) {} // private cctor, remove if this class turns pure
    std::vector<Job> _jobs;
    
private:
    std::string _name;
};

class AcademicUser : public User
{
public:
    AcademicUser(const std::string& name) : User(name) {}

private:
    uint _jobCount;
};

class EnterpriseUser : public User
{
public:
    EnterpriseUser(const std::string& name, double budget) : User(name), _budget(budget) {}

private:
    double _budget;
};

#endif // USER_H_
