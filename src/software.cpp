#include "software.h"

#include <regex>
#include <iomanip>

uint Software::_maxNameLength = 0;

Software::Software(const std::string& name, int major, int minor, int revision) : _name(name), _version(major, minor, revision)
{
    if (_name.length() > _maxNameLength)
        _maxNameLength = _name.length();
}

std::tuple<bool, Software> Software::ReadFromString(const std::string& name)
{
    const std::regex pattern("([A-Za-z0-9\\s]+) ([0-9]+)\\.([0-9]+)\\.([0-9]+)");

    std::match_results<std::string::const_iterator> result;

    if (!std::regex_match(name, result, pattern))
        return std::make_tuple(false, Software());

    return std::make_tuple(true, Software(result[1], std::atoi(result[2].str().c_str()), std::atoi(result[3].str().c_str()), std::atoi(result[4].str().c_str())));
}

bool Software::Save(ByteBuffer& bb) const
{
    bb.WriteString(_name);
    bb.WriteUInt32(_version.Major);
    bb.WriteUInt32(_version.Minor);
    bb.WriteUInt32(_version.Revision);

    return true;
}

Software Software::Load(ByteBuffer& bb)
{
    std::string name = bb.ReadString();
    uint32 major = bb.ReadUInt32();
    uint32 minor = bb.ReadUInt32();
    uint32 revision = bb.ReadUInt32();

    return Software(name, major, minor, revision);
}

void Software::PrintHeader(std::ostream& os /*= std::cout*/)
{
    os << "--" << std::string(_maxNameLength, '-')    << "-----------------------------------\n"
       << "| " << std::setw(_maxNameLength) << "Name" << " |  Major   |  Minor   | Revision |\n"
       << "--" << std::string(_maxNameLength, '-')    << "-----------------------------------\n";
}

void Software::Print(std::ostream& os /*= std::cout*/) const
{
    os << "| " << std::setw(_maxNameLength) << std::left << _name << " | "
       << std::right << std::setw(8) << _version.Major << " | "
       << std::right << std::setw(8) << _version.Minor << " | "
       << std::right << std::setw(8) << _version.Revision << " |\n"
       << "--" << std::string(_maxNameLength, '-')    << "-----------------------------------\n";
}

size_t Software::Hash::operator()(const Software& sw)
{
    return std::hash<std::string>()(sw._name) ^ Software::VersionData::Hash()(sw._version);
}
