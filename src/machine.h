#ifndef MACHINE_H_
#define MACHINE_H_

#include <unordered_set>
#include <string>
#include <algorithm>

#include "job.h"
#include "utils.h"
#include "software.h"

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
    bool SoftwareMeetsRequirements(const Software& sw) const
    {
        if (sw.Dependencies.empty())
            return true;

        for (auto it = sw.Dependencies.begin(); it != sw.Dependencies.end(); ++it)
            if (_availableSoftware.find(*it) == _availableSoftware.end())
                return false;

        return true;
    }

    bool AddJob(const Job& job)
    {
        if (_currentJobs.size() >= _maxJobs)
            return false;

        if (job.GetRequiredRAM() > _availableRAM)
            return false;

        if (job.GetRequiredDiskSpace() > _availableDiskSpace)
            return false;
        
        const std::vector<Software>& requiredSoftware = job.GetRequiredSoftware();
        for (auto it = requiredSoftware.begin(); it != requiredSoftware.end(); ++it)
            if (!SoftwareMeetsRequirements(*it))
                return false;

        _availableRAM -= job.GetRequiredRAM();
        _availableDiskSpace -= job.GetRequiredDiskSpace();

        _currentJobs.push_back(job);
    }

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
