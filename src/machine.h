#ifndef MACHINE_H_
#define MACHINE_H_

#include <unordered_set>
#include <string>
#include <queue>
#include <map>
#include <set>
#include <stack>
#include <vector>
#include <mutex>
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <numeric>

#include "utils.h"
#include "interfaces.h"
#include "software.h"
#include "menu.h"
#include "job.h"
#include "log.h"

typedef std::unordered_set<Software, Software::Hash> SoftwareSet;
//! _Machine Class
/*!
Interface for the IMachine Generic class. This ensures us that no matter the Container used by a Machine all Jobs will have sequential ids.
*/
class _Machine
{
public:
    static uint GetLastJobId() { return _lastJobId; }
    static void SetLastJobId(uint val) { _lastJobId = val; }

protected:
    _Machine() { }
    static uint _lastJobId; ///< The Id of the last job used
};

//!Vector of const Job*
typedef std::vector<const Job*> JobVector;

//! Generic IMachine class
/*!
This class is templated on the Container so we can have different containers for the Job* of our Machines. It is derivated of the _Machine class so the ids of the jobs are sequetential and unique in all aplication.
*/
template <class Container>
class IMachine : public ISave, public IUpdate, public IPrint, public _Machine
{
public:
    /// "Search Constructor"
    IMachine(uint id) : _id(id) { }

    typedef std::set<Job*, IdLess<Job>> JobSet;

    /// Constructor
    IMachine(const std::string& machineName, uint maxJobs, double totalRAM, double totalDiskSpace) :
        _id(0), _name(machineName), _maxJobs(maxJobs), _totalRAM(totalRAM), _totalDiskSpace(totalDiskSpace), _inUseRAM(0), _inUseDiskSpace(0)
    {
        if (_name.length() > _maxNameLength)
            _maxNameLength = _name.length();
    }

    ~IMachine() { } ///< Destructor

    // Name function members
    const std::string& GetName() const { return _name; } ///< Returns the name of the Machine
    void SetName(const std::string& name); ///< Updates the name of the Machine

    // Jobs function members
    uint GetMaxJobs() const { return _maxJobs; } ///< Returns the number limit of Jobs
    void SetMaxJobs(uint val); ///< Updates the number limit of Jobs

    // RAM function members
    double GetAvailableRAM() const { return _totalRAM - _inUseRAM; } ///< Calculates the RAM that is not being used (MB)
    double GetInUseRAM() const { return _inUseRAM; } ///< Calculates the RAM that is being used (MB)
    double GetTotalRAM() const { return _totalRAM; } ///< Returns the total amount of RAM (MB)
    void SetTotalRAM(double val); ///< Updates the total amount of RAM (MB)

    // Disk Space function members
    double GetAvailableDiskSpace() const { return _totalDiskSpace - _inUseDiskSpace; } ///< Calculated the disk space that is not being used (MB)
    double GetInUseDiskSpace() const { return _inUseDiskSpace; } ///< Calculated the disk space that is being used (MB)
    double GetTotalDiskSpace() const { return _totalDiskSpace; } ///< Returns the total amount of disk space (MB)
    void SetTotalDiskSpace(double val); ///< Updates the total amount of disk space  (MB)

    // Id function members
    uint GetId() const { return _id; } ///< Returns the identifier of this Machine
    void SetId(uint id) { _id = id; } ///< Updates the identifier of this Machine

    // Software function members
    void AddAvailableSoftware(const Software& sw) { _availableSoftware.insert(sw); } ///< Adds a Software to the list of available Software
    void RemoveAvailableSoftware(const Software& sw) { _availableSoftware.erase(sw); } ///< Deletes a Software from the list of available Software
    const SoftwareSet& GetAvailableSoftware() const { return _availableSoftware; } ///< Returns the SoftwareSet of this Machine

    JobVector GetJobs() const; ///< Returns a Vector of constant Jobs
    bool AddJob(Job* job); ///< Adds a Job to this Machine. Fails if Job requirements are not supported by this Machine
    const Job* GetJob(uint id) const;
    bool RemoveJob(uint id);
    void RemoveAllJobs(); ///< Removes all Jobs from this Machine
    bool ChangeJobPriority(uint id, uint8 newPriority); ///< Changes job with the given id priority
    uint GetNumberOfCurrentJobs() const { return _currentJobs.size(); } ///< Returns the number of Jobs being executed

    bool Save(ByteBuffer& bb) const override; ///< Saves the Machine data to a ByteBuffer
    void Update(uint diff) override; ///< Updates executing Jobs timers and removes expired ones
    void Print(std::ostream& os = std::cout) const override; ///< Prints current machine information

    // Static function members
    static void PrintHeader(std::ostream& os = std::cout); ///< Prints table header for machines
    static Menu* GetMenu() { return _menu; } ///< Returns the menu associated with Machine
    static IMachine<Container>* Load(ByteBuffer& bb); ///< Loads a Machine from the given ByteBuffer

protected:
    bool SoftwareMeetsRequirements(const Software& sw) const { return _availableSoftware.find(sw) != _availableSoftware.end(); } ///< Returns true if the given Software is available in the Machine

    double _totalRAM; ///< Total RAM (MB)
    double _inUseRAM;

    double _totalDiskSpace; ///< Total disk space (MB)
    double _inUseDiskSpace;

    uint _maxJobs; ///< Max number of Jobs that can be executed

    SoftwareSet _availableSoftware; ///< The available set of Software

    std::string _name; ///< Machine name
    uint _id; ///< Machine identifier

    mutable std::mutex _mutex; ///< Mutex (multithread safe way to change Jobs)

    static Menu* _menu; ///< Menu associated with the Machine class
    static uint _maxNameLength; ///< The length of the biggest Machine name

    Container _currentJobs;
private: // no copying
    IMachine(const IMachine&);
    IMachine& operator =(IMachine const&);
};

template <class Container>
bool IMachine<Container>::ChangeJobPriority(uint id, uint8 newPriority)
{
    _mutex.lock();
    Container temp;

    bool found = false;

    for( ; !_currentJobs.empty() && !found; _currentJobs.pop())
    {
        if (_currentJobs.top() && _currentJobs.top()->GetId() == id)
        {
            found = true;

            Job* j = _currentJobs.top();
            _currentJobs.pop();
            j->SetPriority(newPriority);
            _currentJobs.push(j);
        }
        else
            temp.push(_currentJobs.top());
    }

    while (!temp.empty())
    {
        _currentJobs.push(temp.top());
        temp.pop();
    }

    _mutex.unlock();
    return found;
}


/// Exception thrown when some action tried to change a Machine while it is being used
class MachineInExecution : public std::exception
{
public:
    /// Constructor
    MachineInExecution(const uint mId) : exception("Machine in execution"), _machineId(mId) { }
    uint GetMachineId() const { return _machineId; } ///< The related Machine
private:
    uint _machineId; ///< Machine
};

template <class Container> uint  IMachine<Container>::_maxNameLength = 0;
template <class Container> Menu* IMachine<Container>::_menu = Loader<Menu>("machineMenu.txt").Load();

template <class T>
class Queue : public std::queue<T>
{
public:
    Queue() : std::queue<T>() { }
    const value_type& top() const { return front(); }
};

typedef IMachine<std::priority_queue<Job*,std::vector<Job*>,Job::PriorityLess>> PriorityMachine;
typedef IMachine<Queue<Job*>> Machine;

template <class Container>
void IMachine<Container>::SetName(const std::string& name)
{
    if (name.size() != 0)
    {
        _name = name;
        if (_name.length() > _maxNameLength)
            _maxNameLength = _name.length();
    }
}

template <class Container>
void IMachine<Container>::SetMaxJobs(uint val)
{
    if (val < _currentJobs.size())   // If the value is smaller than the current number of jobs we can't modify this parameter.
        throw MachineInExecution(_id);

    _maxJobs = val;
}

template <class Container>
void IMachine<Container>::SetTotalRAM(double val) ///< Updates the total amount of RAM (MB)
{
    if (val < GetInUseRAM()) // If the value is smaller than the current needed RAM we can't modify this parameter.
        throw MachineInExecution(_id);

    _totalRAM = val;
}

template <class Container>
void IMachine<Container>::SetTotalDiskSpace(double val) ///< Updates the total amount of disk space  (MB)
{
    if (val < GetInUseDiskSpace()) // If the value is smaller than the current needed Disk Space we can't modify this parameter.
        throw MachineInExecution(_id);

    _totalDiskSpace = val;
}

template <class Container>
JobVector IMachine<Container>::GetJobs() const
{
    JobVector res;

    _mutex.lock();
    Container temp(_currentJobs);
    _mutex.unlock();

    while (!temp.empty())
    {
        res.push_back(temp.top());
        temp.pop();
    }
    return res;
}

template <class Container>
bool IMachine<Container>::AddJob(Job* job)
{
    _mutex.lock();

    if (_currentJobs.size() >= _maxJobs)
    {
        _mutex.unlock();
        return false;
    }

    if (job->GetRequiredRAM() > GetAvailableRAM())
    {
        _mutex.unlock();
        return false;
    }

    if (job->GetRequiredDiskSpace() > GetAvailableDiskSpace())
    {
        _mutex.unlock();
        return false;
    }

    const SoftwareSet& requiredSoftware = job->GetRequiredSoftware();
    for (const auto& it : requiredSoftware)
        if (!SoftwareMeetsRequirements(it))
        {
            _mutex.unlock();
            return false;
        }

    job->SetId(_lastJobId);
    _currentJobs.push(job);
    _lastJobId++;
    _inUseRAM += job->GetRequiredRAM();
    _inUseDiskSpace += job->GetRequiredDiskSpace();

    _mutex.unlock();

    return true;
}

template <class Container>
const Job* IMachine<Container>::GetJob(uint id) const
{
    JobVector vec = GetJobs();

    auto it = std::find_if(vec.begin(), vec.end(), [id](const Job* j) { return j->GetId() == id; });

    if (it == vec.end())
        return nullptr;

    return *it;
}

template <class Container>
bool IMachine<Container>::RemoveJob(uint id)
{
    _mutex.lock();
    Container temp(_currentJobs);
    _mutex.unlock();

    bool found = false;

    for( ; !temp.empty(); temp.pop())
    {
        if (temp.top() && temp.top()->GetId() == id)
        {
            found = true;

            _mutex.lock();
            temp.top()->Finish();
            _mutex.unlock();
        }
    }

    return found;
}

template <class Container>
void IMachine<Container>::RemoveAllJobs()
{
    _mutex.lock();

    for(Container temp(_currentJobs); !temp.empty(); temp.pop())
        if (temp.top())
            temp.top()->Finish();

    _mutex.unlock();
}

template <class Container>
inline bool IMachine<Container>::Save(ByteBuffer& bb) const
{
    bb.WriteUInt32(_id);
    bb.WriteString(_name);
    bb.WriteUInt32(_maxJobs);
    bb.WriteDouble(_totalRAM);
    bb.WriteDouble(_totalDiskSpace);

    bb.WriteUInt32(_currentJobs.size());
    for (Container temp(_currentJobs); !temp.empty(); temp.pop())
        temp.top()->Save(bb);

    bb.WriteUInt32(_availableSoftware.size());
    for (auto sw : _availableSoftware)
        sw.Save(bb);

    return true;
}

template <class Container>
inline void IMachine<Container>::Update(uint diff)
{
    _mutex.lock();
    Container temp;
    for (; !_currentJobs.empty(); _currentJobs.pop())
    {
        _currentJobs.top()->Update(diff);

        if (_currentJobs.top()->Finished())
        {
            sLog(Console)->Log("Job %s removed from machine %s.", _currentJobs.top()->GetName().c_str(), _name.c_str());
            Container::value_type tempJob = _currentJobs.top();
            _inUseRAM -= tempJob->GetRequiredRAM();
            _inUseDiskSpace -= tempJob->GetRequiredDiskSpace();
            delete tempJob;
        }
        else
            temp.push(_currentJobs.top());
    }

    while (!temp.empty())
    {
        _currentJobs.push(temp.top());
        temp.pop();
    }

    _mutex.unlock();
}

template <class Container>
inline void IMachine<Container>::Print(std::ostream& os = std::cout) const
{
    os << "| " << std::setfill('0') << std::setw(4) << std::right << _id << " | "
        << std::setfill(' ') << std::setw(_maxNameLength) << std::left << _name

        << " | " << std::setw(5) << std::left << GetAvailableRAM() << " / " << std::right << std::setw(5) << _totalRAM
        << " | " << std::setw(5) << std::left << GetAvailableDiskSpace() << " / " << std::right << std::setw(5) << _totalDiskSpace
        << " | " << std::setw(4) << std::left << _currentJobs.size() << " / " << std::right << std::setw(4) << _maxJobs << " |\n"
        << "---------" << std::string(_maxNameLength, '-') << "------------------------------------------------\n";
}

template <class Container>
void IMachine<Container>::PrintHeader(std::ostream& os = std::cout)
{
    os << "---------" << std::string(_maxNameLength, '-')    << "------------------------------------------------\n"
       << "|  Id  | " << std::setw(_maxNameLength) << "Name" << " |   RAM (MB)    |   Disk (MB)   |    Jobs     |\n"
       << "---------" << std::string(_maxNameLength, '-')    << "------------------------------------------------\n";
}

template <class Container>
IMachine<Container>* IMachine<Container>::Load(ByteBuffer& bb)
{
    uint32 id = bb.ReadUInt32();
    std::string name = bb.ReadString();
    uint32 maxJobs = bb.ReadUInt32();
    double totalRAM = bb.ReadDouble();
    double totalDiskSpace = bb.ReadDouble();

    IMachine* m = new IMachine<Container>(name, maxJobs, totalRAM, totalDiskSpace);
    m->_id = id;

    uint32 jobCount = bb.ReadUInt32();
    for (uint32 i = 0; i < jobCount; ++i)
    {
        Job* job = Job::Load(bb);
        m->_currentJobs.push(job);
        m->_inUseRAM += job->GetRequiredRAM();
        m->_inUseDiskSpace += job->GetRequiredDiskSpace();
    }

    uint32 softwareCount = bb.ReadUInt32();
    for (uint32 i = 0; i < softwareCount; ++i)
        m->_availableSoftware.insert(Software::Load(bb));

    return m;
}

#endif // MACHINE_H_

