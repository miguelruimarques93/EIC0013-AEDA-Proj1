#ifndef MACHINE_H_
#define MACHINE_H_

#include <vector>
#include <string>

#include "job.h"
#include "utils.h"

class Machine
{
public:
    Machine(const std::string& machineName, uint maxJobs, double totalRAM, double totalDiskSpace)
        : _name(machineName), _maxJobs(maxJobs), _totalRAM(totalRAM), _totalDiskSpace(totalDiskSpace) {}

    const std::string& GetMachineName() const { return _name; }
    uint GetMaxJobs() const { return _maxJobs; }
    double GetAvailableRAM() const { return _availableRAM; }
    double GetAvailableDiskSpace() const { return _availableDiskSpace; }
    double GetTotalRAM() const { return _totalRAM; }
    double GetTotalDiskSpace() const { return _totalDiskSpace; }

private:
    std::string _name;
    double _availableRAM;
    double _availableDiskSpace;
    std::vector<Software> _availableSoftware;

    const uint _maxJobs;
    const double _totalRAM;
    const double _totalDiskSpace;

    std::vector<Job> _currentJobs;
};

#endif // MACHINE_H_
