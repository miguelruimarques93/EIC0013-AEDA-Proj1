#ifndef MACHINE_H_
#define MACHINE_H_

#include <unordered_set>
#include <string>
#include <map>
#include <mutex>
#include <iostream>

#include "utils.h"
#include "interfaces.h"
#include "software.h"

class Job;
class Menu;

typedef std::unordered_set<Software, Software::Hash> SoftwareSet;

class Machine;

/// Exception thrown when some action tried to change a Machine while it is being used
class MachineInExecution : public std::exception
{
public:
    /// Constructor
    MachineInExecution(const Machine* m) : exception("Machine in execution"), _machine(m) { }
    const Machine* GetMachine() const { return _machine; } ///< The related Machine
private:
    const Machine* _machine; ///< Machine
};

//! Machine Class
/*!
    Represents a computer where the Jobs are executed
*/
class Machine : public ISave, public IUpdate, public IPrint
{
public:
    /// Constructor
    Machine(const std::string& machineName, uint maxJobs, double totalRAM, double totalDiskSpace);

    ~Machine(); /// Destructor

    const std::string& GetName() const { return _name; } ///< Returns the name of the Machine
    void SetName(const std::string& name); ///! Updates the name of the Machine

    uint GetMaxJobs() const { return _maxJobs; } ///< Returns the number limit of Jobs
    void SetMaxJobs(uint val); ///< Updates the number limit of Jobs

    double GetAvailableRAM() const { return _totalRAM - GetInUseRAM(); } ///< Calculates the RAM that is not being used (MB)
    double GetAvailableDiskSpace() const { return _totalDiskSpace - GetInUseDiskSpace(); } ///< Calculated the disk space that is not being used (MB)

    double GetInUseRAM() const; ///< Calculates the RAM that is being used (MB)
    double GetInUseDiskSpace() const; ///< Calculated the disk space that is being used (MB)

    double GetTotalRAM() const { return _totalRAM; } ///< Returns the total amount of RAM (MB)
    void SetTotalRAM(double val); ///< Updates the total amount of RAM (MB)

    double GetTotalDiskSpace() const { return _totalDiskSpace; } ///< Returns the total amount of disk space (MB)
    void SetTotalDiskSpace(double val); ///< Updates the total amount of disk space  (MB)

    uint GetId() const { return _id; } ///< Returns the identifier of this Machine
    void SetId(uint id) { _id = id; } ///< Updates the identifier of this Machine

    void AddAvailableSoftware(const Software& sw) { _availableSoftware.insert(sw); } ///< Adds a Software to the list of available Software
    void RemoveAvailableSoftware(const Software& sw) { _availableSoftware.erase(sw); } ///< Deletes a Software from the list of available Software
    const SoftwareSet& GetAvailableSoftware() const { return _availableSoftware; } ///< Returns the SoftwareSet of this Machine

    bool AddJob(Job* job); ///< Adds a Job to this Machine. Fails if Job requirements are not supported by this Machine
    Job* GetJob(uint id); ///< Returns the Job with the given id
    bool RemoveJob(uint id); ///< Removes the Job with the given id
    bool RemoveAllJobs(); ///< Removes all Jobs from this Machine
    uint GetNumberOfJobs() const; ///< Returns the number of Jobs being executed

    bool Save(ByteBuffer& bb) const override; ///< Saves the Machine data to a ByteBuffer
    static Machine* Load(ByteBuffer& bb); ///< Loads the Machine data from a ByteBuffer

    static Menu* GetMenu() { return _menu; } ///< Returns the menu associated with Machine

    const std::map<uint, Job*>& GetJobs() const { return _currentJobs; } ///< Returns the map of Jobs

    void Update(uint diff); ///< Updates executing Jobs timers and removes expired ones

    void Print(std::ostream& os = std::cout) const override;
    static void PrintHeader(std::ostream& os = std::cout);

private:
    ///< Returns true if the given Software is available in the Machine
    bool SoftwareMeetsRequirements(const Software& sw) const { return _availableSoftware.find(sw) != _availableSoftware.end(); }

    double _totalRAM; ///< Total RAM (MB)
    double _totalDiskSpace; ///< Total disk space (MB)
    uint _maxJobs; ///< Max number of Jobs that can be executed

    static uint _lastJobId; ///< The Id of the last job used

    SoftwareSet _availableSoftware; ///< The available set of Software
    std::map<uint, Job*> _currentJobs; ///< The map of Jobs being executed

    std::string _name; ///< Machine name
    uint _id; ///< Machine identifier

    mutable std::mutex _mutex; ///< Mutex (multithread safe way to change Jobs)

    static Menu* _menu; ///< Associated menu
    static uint _maxNameLength; ///< The length of the biggest Machine name

private: // no copying
    Machine(const Machine&);
    Machine& operator =(Machine const&);
};

#endif // MACHINE_H_
