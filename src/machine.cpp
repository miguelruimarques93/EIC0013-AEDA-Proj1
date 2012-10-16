#include "machine.h"
#include <algorithm>

uint Machine::_lastJobId = 0;

bool Machine::AddJob(Job* job)
{
    if (_currentJobs.size() >= _maxJobs)
        return false;

    if (job->GetRequiredRAM() > _availableRAM)
        return false;

    if (job->GetRequiredDiskSpace() > _availableDiskSpace)
        return false;

    const std::vector<Software>& requiredSoftware = job->GetRequiredSoftware();
    for (auto it = requiredSoftware.begin(); it != requiredSoftware.end(); ++it)
        if (!SoftwareMeetsRequirements(*it))
            return false;

    _availableRAM -= job->GetRequiredRAM();
    _availableDiskSpace -= job->GetRequiredDiskSpace();

    _currentJobs[_lastJobId] = job;
    _lastJobId++;

    return true;
}

bool Machine::RemoveJob(uint id)
{
    auto it = _currentJobs.find(id);

    if (it == _currentJobs.end())
        return false;

    Job* job = it->second;

    _availableRAM += job->GetRequiredRAM();
    _availableDiskSpace += job->GetRequiredDiskSpace();

    delete job;

    _currentJobs.erase(it);

    return true;
}

bool Machine::SoftwareMeetsRequirements(const Software& sw) const
{
    if (sw.Dependencies.empty())
        return true;

    for (auto it = sw.Dependencies.begin(); it != sw.Dependencies.end(); ++it)
        if (_availableSoftware.find(*it) == _availableSoftware.end())
            return false;

    return true;
}
