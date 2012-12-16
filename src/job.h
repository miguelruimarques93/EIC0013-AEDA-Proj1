#ifndef JOB_H_
#define JOB_H_

#include <string>
#include <unordered_set>
#include <sstream>
#include <chrono>
#include <iostream>

#include "utils.h"
#include "interfaces.h"
#include "software.h"

class User;

template <class Idable>
struct IdLess
{
    bool operator()(Idable* const & lhs, Idable* const & rhs) const
    {
        return lhs->GetId() < rhs->GetId();
    }
};

typedef std::unordered_set<Software, Software::Hash> SoftwareSet;

//! Job Class
/*!
Represents a Job that is executed in a Machine
*/
class Job : public ISave, public IUpdate, public IPrint
{
public:
    struct PriorityLess 
    {
        bool operator()(Job* const & lhs, Job* const & rhs) const
        {
            return lhs->_priority * 37 + lhs->_id * 97 > rhs->_priority * 37 + rhs->_id * 97;
        }
    };
    /// "Search" Constructor
    Job(uint id) : _id(id), _name(""), _priority(0), _requiredRAM(0), _requiredDiskSpace(0), _totalExecutionTime(0) { }
    /// Constructor
    Job(const std::string& name, uint8 priority, double requiredRAM, double requiredDiskSpace, uint executionTime);

    uint GetId() const { return _id;}
    void SetId(uint val) { _id = val; }
    const std::string& GetName() const { return _name; } ///< Returns the name of the Job
    uint8 GetPriority() const { return _priority; } ///< Returns the priority of the Job
    double GetRequiredRAM() const { return _requiredRAM; } ///< Returns the required amount of RAM (MB)
    double GetRequiredDiskSpace() const { return _requiredDiskSpace; } ///< Returns the required amount of disk space (MB)
    uint GetTotalExecutionTime() const { return _totalExecutionTime; } ///< Returns the total number of seconds to execute the Job
    uint GetElapsedTime() const { return _elapsedTime; } ///< Returns the number of seconds since the Job was created

    double GetPrice() const { return 0.05 * _requiredRAM; } ///< Returns the price of the Job (5 cents per MB of RAM)

    bool Finished() const { return _totalExecutionTime <= _elapsedTime; } ///< Returns true if the Job fnished
    void Finish() { _elapsedTime = _totalExecutionTime; } ///< Forces the Job to abruptly end

    bool Save(ByteBuffer& bb) const override; ///< Saves Job data to a ByteBuffer
    static Job* Load(ByteBuffer& bb); ///< Loads a Job from a ByteBuffer

    void Update(uint32 diff); ///< Updates Jobs timers

    void Print(std::ostream& os = std::cout) const override;
    void PrintWithId(std::ostream& os = std::cout) const;
    static void PrintHeader(std::ostream& os = std::cout, bool withId = false);

    void AddRequiredSoftware(const Software& sw) { _requiredSoftware.insert(sw); } ///< Adds Software to the set of required Software
    /// Deletes Software from the set of required Software
    bool IsRequiredSoftware(const Software& sw) const { return _requiredSoftware.find(sw) != _requiredSoftware.end(); }
    const SoftwareSet& GetRequiredSoftware() const { return _requiredSoftware; } ///< Returns the set of required Software

private:
    const double _requiredRAM; ///< Amount of required RAM to execute the Job (MB)
    const double _requiredDiskSpace; ///< Amount of required disk space to execute the Job (MB)
    const uint _totalExecutionTime; ///< The total number of seconds to execute the Job
    uint _elapsedTime; ///< The number of seconds since the Job was created
    uint _ms; ///< Helper member to count the number of milliseconds since _elapsedTime was changed
    SoftwareSet _requiredSoftware; ///< Set of required software to execute the Job
    const uint8 _priority; ///< Priority

    const std::string _name; ///< Name of the job
    uint _id;

    static uint _maxNameLength; ///< Length of the biggest name

private: // no copying
    Job(const Job&);
    Job& operator=(Job const&);
};



#endif // JOB_H_
