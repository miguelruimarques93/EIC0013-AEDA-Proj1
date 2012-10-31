#include "user.h"
#include "bytebuffer.h"
#include "job.h"
#include "menu.h"
#include "loader.h"

Menu* User::_menu = Loader<Menu>("userMenu.txt").Load();

void User::PrintHeader(std::ostream& os /*= std::cout*/)
{
    os << "----------------------------------\n";
    os << "| Id | Name | Job count | Budget |\n";
}

void User::Print(std::ostream& os /*= std::cout */) const
{
    os << "| " << _id << " | " << _name;
}

void AcademicUser::Print(std::ostream& os /*= std::cout */) const
{
    User::Print(os);
    os << " | " << _jobCount << " | - |\n";
}

void EnterpriseUser::Print(std::ostream& os /*= std::cout */) const
{
    User::Print(os);
    os << " | - | " << _budget << "|\n";
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
