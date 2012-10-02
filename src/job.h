#ifndef JOB_H_
#define JOB_H_

#include <string>
#include <vector>
#include <sstream>

#include "utils.h"

class Machine;

struct Software
{
    Software(const std::string& name, int major, int minor, int revision)
        : Name(name), Version(major, minor, revision) {}

    bool operator== (const Software& other) const { return Name == other.Name && Version == other.Version; }
    bool operator!= (const Software& other) const { return !(*this == other); }

    std::string Name;
    struct VersionData
    {
        VersionData(int major, int minor, int revision) : Major(major), Minor(minor), Revision(revision) {}

        bool operator== (const VersionData& other) const { return Major == other.Major && Minor == other.Minor && Revision == other.Revision; }

        int Major;
        int Minor;
        int Revision;

        std::string ToString() const
        {
            std::ostringstream ss("V");

            ss << Major << '.' << Minor << '.' << Revision;

            return ss.str();
        }

        struct Hash
        {
            size_t operator()(const VersionData& ver) { return std::hash<std::string>()(ver.ToString()); }
        };
    };

    VersionData Version;

    std::vector<Software> Dependencies;

    struct Hash
    {
        size_t operator()(const Software& sw)
        {
            return std::hash<std::string>()(sw.Name) ^ Software::VersionData::Hash()(sw.Version);
        }
    };
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
