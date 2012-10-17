#ifndef USER_H_
#define USER_H_

#include <string>
#include <vector>

#include "utils.h"
#include "utilclasses.h"

class Job;

enum SaveUserType
{
    SAVE_USER_TYPE_ACADEMIC,
    SAVE_USER_TYPE_ENTERPRISE,
};

class User : public Saveable
{
public:
    const std::string& GetName() const { return _name; }

    virtual bool Save(ByteBuffer& bb) const override = 0;

    virtual ~User() { };

protected:
    User(const std::string& name) : _name(name) {}
    std::vector<Job*> _jobs;

private:
    const std::string _name;
};

class AcademicUser : public User
{
public:
    AcademicUser(const std::string& name, uint jobCount) : User(name), _jobCount(jobCount) {}

    uint GetJobCount() const { return _jobCount; }

    bool Save(ByteBuffer& bb) const override;

private:
    uint _jobCount;
};

class EnterpriseUser : public User
{
public:
    EnterpriseUser(const std::string& name, double budget) : User(name), _budget(budget) {}

    double GetBudget() const { return _budget; }

    bool Save(ByteBuffer& bb) const override;

private:
    double _budget;
};

#endif // USER_H_
