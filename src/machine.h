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

class MachineInExecution : public std::exception
{
public:
    MachineInExecution(const Machine* m) : exception("Machine in execution"), _machine(m) { }
    const Machine* GetMachine() const { return _machine; }
private:
    const Machine* _machine;
};

class Machine : public ISave, public IUpdate, public IPrint
{
public:
    Machine(const std::string& machineName, uint maxJobs, double totalRAM, double totalDiskSpace);

    virtual ~Machine();

    const std::string& GetName() const { return _name; }
    void SetName(const std::string& name);

    uint GetMaxJobs() const { return _maxJobs; }
    void SetMaxJobs(uint val);

    uint GetCurrentJobs() const;
    double GetAvailableRAM() const { return _totalRAM - GetInUseRAM(); }
    double GetAvailableDiskSpace() const { return _totalDiskSpace - GetInUseDiskSpace(); }

    double GetInUseRAM() const;
    double GetInUseDiskSpace() const;

    double GetTotalRAM() const { return _totalRAM; }
    void SetTotalRAM(double val);

    double GetTotalDiskSpace() const { return _totalDiskSpace; }
    void SetTotalDiskSpace(double val);

    uint GetId() const { return _id; }
    void SetId(uint id) { _id = id; }

    void AddAvailableSoftware(const Software& sw) { _availableSoftware.insert(sw); }
    void RemoveAvailableSoftware(const Software& sw) { _availableSoftware.erase(sw); }
    bool IsAvailableSoftware(const Software& sw) const { return _availableSoftware.find(sw) != _availableSoftware.end(); }

    bool AddJob(Job* job);
    Job* GetJob(uint ID);
    bool RemoveJob(uint id);
    bool RemoveAllJobs();
    uint GetNumberOfJobs() const { return _currentJobs.size(); }

    bool Save(ByteBuffer& bb) const override;

    static Machine* Load(ByteBuffer& bb);
    static Menu* GetMenu() { return _menu; }

    const std::map<uint, Job*>& GetJobs() const { return _currentJobs; }

    void Update(uint diff);

    void Print(std::ostream& os = std::cout) const override;
    static void PrintHeader(std::ostream& os = std::cout);

private:
    bool SoftwareMeetsRequirements(const Software& sw) const { return _availableSoftware.find(sw) != _availableSoftware.end(); }

    double _totalRAM;
    double _totalDiskSpace;
    uint _maxJobs;

    static uint _lastJobId;

    SoftwareSet _availableSoftware;
    std::map<uint, Job*> _currentJobs;

    std::string _name;
    uint _id;

    mutable std::mutex _mutex;

    static Menu* _menu;
    static uint _maxNameLength;

private: // no copying
    Machine(const Machine&);
    Machine& operator =(Machine const&);
};

#endif // MACHINE_H_
