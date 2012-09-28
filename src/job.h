#ifndef JOB_H_
#define JOB_H_

#include <string>
#include <vector>

#include "utils.h"

class Machine;

struct Software
{
    Software(const std::string& name, int major, int minor, int revision)
        : Name(name), Version(major, minor, revision) {}

    std::string Name;
    struct Version
    {
        Version(int major, int minor, int revision) : Major(major), Minor(minor), Revision(revision) {}
        int Major;
        int Minor;
        int Revision;
    } Version;

    std::vector<Software> Dependencies;
};

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
