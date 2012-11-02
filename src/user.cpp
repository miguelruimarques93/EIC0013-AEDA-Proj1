#include "user.h"
#include "bytebuffer.h"
#include "job.h"
#include "menu.h"
#include "loader.h"

#include <iomanip>

Menu* User::_menu = Loader<Menu>("userMenu.txt").Load();
uint User::_maxNameLength = 0;

User::User(const std::string& name) : _id(0), _name(name)
{
    if (_name.length() > _maxNameLength)
        _maxNameLength = _name.length();
}

void User::SetName(const std::string& name)
{
    if (name.size() != 0)
    {
        _name = name;
        if (_name.length() > _maxNameLength)
            _maxNameLength = _name.length();
    }
}

void User::PrintHeader(std::ostream& os /*= std::cout*/)
{
    os << "---------" << std::string(_maxNameLength, '-')    << "-----------------------\n"
       << "|  Id  | " << std::setw(_maxNameLength) << "Name" << " | Job count | Budget |\n"
       << "---------" << std::string(_maxNameLength, '-')    << "-----------------------\n";
}

void User::Print(std::ostream& os /*= std::cout */) const
{
    os << "| " << std::setfill('0') << std::setw(4) << std::right << _id << " | "
       << std::setfill(' ') << std::setw(_maxNameLength) << std::left << _name;
}

void AcademicUser::Print(std::ostream& os /*= std::cout */) const
{
    User::Print(os);
    os << " | " << std::setw(9) << std::right << _jobCount << " |      - |\n"
       << "---------" << std::string(_maxNameLength, '-') << "-----------------------\n";
}

void EnterpriseUser::Print(std::ostream& os /*= std::cout */) const
{
    User::Print(os);
    os << " |         - | " << std::setw(6) << std::right << _budget << " |\n"
       << "---------" << std::string(_maxNameLength, '-') << "-----------------------\n";
}

void EnterpriseUser::CreatedJob(const Job* job)
{
    if (job)
        _budget -= job->GetPrice();
}

bool EnterpriseUser::CanCreateJob(const Job* job)
{
    return job && job->GetPrice() > _budget;
}

User* User::Load(ByteBuffer& bb)
{
    User* user = NULL;

    uint id = bb.ReadUInt32();
    std::string name = bb.ReadString();
    uint8 type = bb.ReadUInt8();

    switch (type)
    {
        case SAVE_USER_TYPE_ACADEMIC:
        {
            uint32 jobCount = bb.ReadUInt32();

            user = new AcademicUser(name, jobCount);
            break;
        }
        case SAVE_USER_TYPE_ENTERPRISE:
        {
            double budget = bb.ReadDouble();

            user = new EnterpriseUser(name, budget);
            break;
        }
        default:
        {
            // Log("Invalid type(%u) in file.\n", type);
            break;
        }
    }

    if (user)
        user->SetId(id);
    return user;
}

bool User::Save(ByteBuffer& bb) const
{
    if (_id == 0)
        return false;

    bb.WriteUInt32(_id);
    bb.WriteString(_name);

    return true;
}

bool AcademicUser::Save(ByteBuffer& bb) const
{
    if (!User::Save(bb))
        return false;

    bb.WriteUInt8(SAVE_USER_TYPE_ACADEMIC);
    bb.WriteUInt32(_jobCount);

    return true;
}

bool EnterpriseUser::Save(ByteBuffer& bb) const
{
    if (!User::Save(bb))
        return false;

    bb.WriteUInt8(SAVE_USER_TYPE_ENTERPRISE);
    bb.WriteDouble(_budget);

    return true;
}
