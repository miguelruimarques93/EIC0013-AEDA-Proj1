#ifndef USER_H_
#define USER_H_

#include <string>
#include <vector>

#include "utils.h"
#include "interfaces.h"

class Job;
class ByteBuffer;

enum SaveUserType
{
    SAVE_USER_TYPE_ACADEMIC,
    SAVE_USER_TYPE_ENTERPRISE,
};

class User : public ISave
{
public:
    const std::string& GetName() const { return _name; }

    virtual bool Save(ByteBuffer& bb) const override = 0;

    virtual bool CanCreateJob(const Job* job) = 0;
    virtual void CreatedJob(const Job* job) = 0;

    virtual void Print(std::ostream& os) const = 0;

    virtual ~User() { };

    static User* Load(ByteBuffer& bb);

protected:
    User(const std::string& name) : _name(name) {}
    std::vector<Job*> _jobs;

private:
    std::string _name;
};

class AcademicUser : public User
{
public:
    AcademicUser(const std::string& name) : User(name), _jobCount(0) {}
    AcademicUser(const std::string& name, uint jobCount) : User(name), _jobCount(jobCount) {}

    uint GetJobCount() const { return _jobCount; }

    bool Save(ByteBuffer& bb) const override;

    void CreatedJob(const Job* job) override { if (job) _jobCount++; }

    bool CanCreateJob(const Job* job) override { return !!job; }

    void Print(std::ostream& os) const override;

private:
    uint _jobCount;
};

class EnterpriseUser : public User
{
public:
    EnterpriseUser(const std::string& name, double budget) : User(name), _budget(budget) {}

    double GetBudget() const { return _budget; }

    bool Save(ByteBuffer& bb) const override;

    void CreatedJob(const Job* job) override;

    bool CanCreateJob(const Job* job) override;

    void Print(std::ostream& os) const override;

private:
    double _budget;
};

#endif // USER_H_
