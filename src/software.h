#ifndef SOFTWARE_H_
#define SOFTWARE_H_

#include "utilclasses.h"
#include "bytebuffer.h"
#include <string>
#include <functional>
#include <sstream>

struct Software : ISave
{
    Software(const std::string& name, int major, int minor, int revision)
        : Name(name), Version(major, minor, revision) {}

    bool Save(ByteBuffer& bb) const override
    {
        bb.WriteString(Name);
        bb.WriteUInt32(Version.Major);
        bb.WriteUInt32(Version.Minor);
        bb.WriteUInt32(Version.Revision);

        return true;
    }

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

#endif // SOFTWARE_H_
