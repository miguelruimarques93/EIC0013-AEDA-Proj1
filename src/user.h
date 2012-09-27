#ifndef USER_H_
#define USER_H_

#include <string>
#include <vector>

#include "job.h"

class User
{
public:

protected:
    std::string _name;
    std::vector<Job> _jobs;
private:

};

class AcademicUser : public User
{
public:

private:
    int _jobCount;
	
};

class EnterpriseUser : public User
{
public:

private:
    float _budget;
};

#endif // USER_H_
