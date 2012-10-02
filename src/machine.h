#ifndef MACHINE_H_
#define MACHINE_H_

#include <unordered_set>
#include <string>

#include "job.h"
#include "utils.h"

class Machine
{
public:
    Machine(const std::string& machineName, uint maxJobs, double totalRAM, double totalDiskSpace)
        : _name(machineName), _maxJobs(maxJobs), _totalRAM(totalRAM), _totalDiskSpace(totalDiskSpace),
          _availableDiskSpace(totalDiskSpace), _availableRAM(totalRAM) {}

    const std::string& GetMachineName() const { return _name; }
    uint GetMaxJobs() const { return _maxJobs; }
    double GetAvailableRAM() const { return _availableRAM; }
    double GetAvailableDiskSpace() const { return _availableDiskSpace; }
    double GetTotalRAM() const { return _totalRAM; }
    double GetTotalDiskSpace() const { return _totalDiskSpace; }

    void AddRequiredSoftware(const Software& sw) { _availableSoftware.insert(sw); }
    bool IsRequiredSoftware(const Software& sw) const { return _availableSoftware.find(sw) != _availableSoftware.end(); }

private:
    std::string _name;
    double _availableRAM;
    double _availableDiskSpace;
    std::unordered_set<Software, Software::Hash> _availableSoftware;

    const uint _maxJobs;
    const double _totalRAM;
    const double _totalDiskSpace;

    std::vector<Job> _currentJobs;
};

#endif // MACHINE_H_
