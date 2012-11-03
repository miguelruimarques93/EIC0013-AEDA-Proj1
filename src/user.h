#ifndef USER_H_
#define USER_H_

#include <string>
#include <vector>

#include "utils.h"
#include "interfaces.h"

class Job;
class ByteBuffer;
class Menu;

/// Enum used in load and save actions to distinguish academic and enterprise Users
enum SaveUserType
{
    SAVE_USER_TYPE_ACADEMIC,
    SAVE_USER_TYPE_ENTERPRISE,
};

//! User abstract Class
/*!
    User represents a person or a collective of persons that can interact with GridManager by creating Jobs
*/
class User : public ISave, public IPrint
{
public:
    virtual ~User() { }; ///> Virtual destructor

    uint GetId() const { return _id; } ///> Returns the User's identifier
    void SetId(uint val) { _id = val; } ///> Changes the User's identifier

    void SetName(const std::string& name); ///> Updates the User's name
    const std::string& GetName() const { return _name; } ///> Returns the User's name

    virtual bool Save(ByteBuffer& bb) const override; ///> Savers User data to a ByteBuffer
    static User* Load(ByteBuffer& bb); ///> Loads (1) User data from a ByteBuffer

    virtual bool CanCreateJob(const Job* job) = 0; ///> Returns true if User can create the given Job
    virtual void CreatedJob(const Job* job) = 0; ///> Updates User info when a Job creation was successfull

    virtual void Print(std::ostream& os = std::cout) const override;
    static void PrintHeader(std::ostream& os = std::cout);

    static Menu* GetMenu() { return _menu; } ///> Returns the menu for the User class

protected:
    User(const std::string& name); ///> Constructor used by subclasses

    static Menu* _menu; ///> Menu associated with the User class
    static uint _maxNameLength; ///> The length of the biggest name, used in Print methods

private:
    std::string _name; ///> User's name
    uint _id; ///> User's identifier
};

//! AcademicUser Class
/*!
    AcademicUser represents a student with no limit on the number of created jobs
*/
class AcademicUser : public User
{
public:
    AcademicUser(const std::string& name) : User( name), _jobCount(0) {}
    AcademicUser(const std::string& name, uint jobCount) : User(name), _jobCount(jobCount) {}

    uint GetJobCount() const { return _jobCount; } ///> Returns the number of executed jobs

    bool Save(ByteBuffer& bb) const override;

    void CreatedJob(const Job* job) override { if (job) _jobCount++; }

    bool CanCreateJob(const Job* job) override { return !!job; }

    void Print(std::ostream& os = std::cout) const override;

private:
    uint _jobCount; ///> Number of executed jobs
};

//! EnterpriseUser Class
/*!
    EnterpriseUser represents a company that pays for each job created (with a budget)
*/
class EnterpriseUser : public User
{
public:
    EnterpriseUser(const std::string& name, double budget) : User(name), _budget(budget) {}

    double GetBudget() const { return _budget; } ///> Returns the current budget, in euros

    void SetBudget(double val) { _budget = val; } ///> Changes the budget

    bool Save(ByteBuffer& bb) const override;

    void CreatedJob(const Job* job) override;

    bool CanCreateJob(const Job* job) override;

    void Print(std::ostream& os = std::cout) const override;

private:
    double _budget; ///> Current budget
};

#endif // USER_H_
