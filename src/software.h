#ifndef SOFTWARE_H_
#define SOFTWARE_H_

#include "interfaces.h"
#include "bytebuffer.h"

#include <string>
#include <sstream>
#include <tuple>

class Software : public ISave, public IPrint
{
public:
    Software(const std::string& name, int major, int minor, int revision);

    bool Save(ByteBuffer& bb) const override;
    static Software Load(ByteBuffer& bb);

    bool operator== (const Software& other) const { return _name == other._name && _version == other._version; }
    bool operator!= (const Software& other) const { return !(*this == other); }

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

    struct Hash
    {
        size_t operator()(const Software& sw);
    };

    const std::string& GetName() const { return _name; }
    const VersionData& GetVersion() const { return _version; }

    static std::tuple<bool, Software> ReadFromString(const std::string& name);

    void Print(std::ostream& os = std::cout) const override;
    static void PrintHeader(std::ostream& os = std::cout);

private:
    Software() : _name(""), _version(0, 0, 0) {}

    std::string _name;
    VersionData _version;

    static uint _maxNameLength;
};

#endif // SOFTWARE_H_
