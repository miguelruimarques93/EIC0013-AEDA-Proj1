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
        : _name(name), _priority(priority), _requiredRam(requiredRAM), _requiredDiskSpace(requiredDiskSpace) {}

private:
    std::string _name;
    uint _priority;
    double _requiredRam;
    double _requiredDiskSpace;
    std::vector<Software> _requiredSoftware;

    Machine* _machine;
};

#endif // JOB_H_
