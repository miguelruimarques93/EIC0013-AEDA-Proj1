#ifndef JOB_H_
#define JOB_H_

#include <string>
#include <vector>
#include <sstream>
#include <chrono>

#include "utils.h"
#include "software.h"

class Machine;

class Job
{
public:
    Job(const std::string& name, uint priority, double requiredRAM, double requiredDiskSpace, uint executionTime)
        : _name(name), _priority(priority), _requiredRAM(requiredRAM), _requiredDiskSpace(requiredDiskSpace), _totalExecutionTime(executionTime),
          _elapsedTime(0) {}

    const std::string& GetName() const { return _name; }
    uint GetPriority() const { return _priority; }
    double GetRequiredRAM() const { return _requiredRAM; }
    double GetRequiredDiskSpace() const { return _requiredDiskSpace; }
    const std::vector<Software>& GetRequiredSoftware() const { return _requiredSoftware; }

    bool Finished() const { return (_totalExecutionTime - _elapsedTime) <= 0; }
    void Finish() { _elapsedTime = _totalExecutionTime; }

private:
    const std::string _name;
    const uint _priority;
    const double _requiredRAM;
    const double _requiredDiskSpace;
    const std::vector<Software> _requiredSoftware;

    const uint _totalExecutionTime;
    uint _elapsedTime;
};

#endif // JOB_H_
