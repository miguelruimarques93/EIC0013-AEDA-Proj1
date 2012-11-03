#ifndef SOFTWARE_H_
#define SOFTWARE_H_

#include "interfaces.h"
#include "bytebuffer.h"

#include <string>
#include <sstream>
#include <tuple>

//! Software Class
/*!
    Represents a Software that is required by a Job or available in a Machine.
*/
class Software : public ISave, public IPrint
{
public:
    Software(const std::string& name, int major, int minor, int revision); ///> Constructor.

    bool Save(ByteBuffer& bb) const override; ///> Save Software data to a ByteBuffer.
    static Software Load(ByteBuffer& bb); ///> Load Software data from a ByteBuffer.

    /// Equality operator.
    bool operator== (const Software& other) const { return _name == other._name && _version == other._version; }
    bool operator!= (const Software& other) const { return !(*this == other); } ///> Inequality operator.

    /// Struct to hold version information (major, minor and revision).
    struct VersionData
    {
        /// Constructor.
        VersionData(int major, int minor, int revision) : Major(major), Minor(minor), Revision(revision) {}

        /// Equality operator.
        bool operator== (const VersionData& other) const { return Major == other.Major && Minor == other.Minor && Revision == other.Revision; }

        int Major; ///> Major version.
        int Minor; ///> Minor version.
        int Revision; ///> Revision.

        /// String representation of the version.
        std::string ToString() const
        {
            std::ostringstream ss("V");

            ss << Major << '.' << Minor << '.' << Revision;

            return ss.str();
        }

        /// Hash so it is possible to compare 2 VersionData.
        struct Hash
        {
            size_t operator()(const VersionData& ver) { return std::hash<std::string>()(ver.ToString()); }
        };
    };

    /// Hash so it is possible to compare 2 VersionData.
    struct Hash
    {
        size_t operator()(const Software& sw);
    };

    const std::string& GetName() const { return _name; } ///> Returns the name of the Software.
    const VersionData& GetVersion() const { return _version; } ///> Returns the VersionData of the Software.

    /**
    *   @brief Converts a string into a Software.
    *   @param name The string to parse.
    *   @return A tuple with two values, a boolean and software.
    *           If the boolean is true, the method was successful and Software can be used.
    */
    static std::tuple<bool, Software> ReadFromString(const std::string& name);

    void Print(std::ostream& os = std::cout) const override;
    static void PrintHeader(std::ostream& os = std::cout);

private:
    Software() : _name(""), _version(0, 0, 0) {} /// Constructor for invalid Software

    std::string _name; ///> Software name
    VersionData _version; ///> Software version information

    static uint _maxNameLength; ///> The length of the biggest Software name
};

#endif // SOFTWARE_H_
