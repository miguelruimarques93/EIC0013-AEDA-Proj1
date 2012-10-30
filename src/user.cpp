#include "user.h"
#include "bytebuffer.h"
#include "job.h"

bool AcademicUser::Save(ByteBuffer& bb) const
{
    bb.WriteUInt8(SAVE_USER_TYPE_ACADEMIC);
    bb.WriteString(GetName());
    bb.WriteUInt32(_jobCount);

    return true;
}

void AcademicUser::Print(std::ostream& os /*= std::cout */) const
{
    os << "| Id: " << GetID() << " | Name: " << GetName() << " | Job count: " << _jobCount << " |\n";
}

void EnterpriseUser::Print(std::ostream& os /*= std::cout */) const
{
    os << "| Id: " << GetID() << " | Name: " << GetName() << " | Budget: " << _budget << " |\n";
}

bool EnterpriseUser::Save(ByteBuffer& bb) const
{
    bb.WriteUInt8(SAVE_USER_TYPE_ENTERPRISE);
    bb.WriteString(GetName());
    bb.WriteDouble(_budget);

    return true;
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
    uint8 type = bb.ReadUInt8();
    std::string name = bb.ReadString();

    switch (type)
    {
        case SAVE_USER_TYPE_ACADEMIC:
        {
            uint32 jobCount = bb.ReadUInt32();
            return new AcademicUser(-1, name, jobCount);
        }
        case SAVE_USER_TYPE_ENTERPRISE:
        {
            double budget = bb.ReadDouble();
            return new EnterpriseUser(-1, name, budget);
        }
        default:
        {
            // Log("Invalid type(%u) in file.\n", type);
        }
    }

    return NULL;
}
