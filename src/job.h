#ifndef JOB_H_
#define JOB_H_

#include <string>
#include <vector>
#include <sstream>

#include "utils.h"
#include "software.h"

class Machine;

class Job
{
public:
    Job(const std::string& name, uint priority, double requiredRAM, double requiredDiskSpace)
        : _name(name), _priority(priority), _requiredRAM(requiredRAM), _requiredDiskSpace(requiredDiskSpace) {}

    const std::string& GetName() const { return _name; }
    uint GetPriority() const { return _priority; }
    double GetRequiredRAM() const { return _requiredRAM; }
    double GetRequiredDiskSpace() const { return _requiredDiskSpace; }
    const std::vector<Software>& GetRequiredSoftware() const { return _requiredSoftware; }

private:
    std::string _name;
    uint _priority;
    double _requiredRAM;
    double _requiredDiskSpace;
    std::vector<Software> _requiredSoftware;

    Machine* _machine;
};

#endif // JOB_H_
