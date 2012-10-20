#include "user.h"
#include "bytebuffer.h"

bool AcademicUser::Save(ByteBuffer& bb) const
{
    bb.WriteUInt8(SAVE_USER_TYPE_ACADEMIC);
    bb.WriteString(GetName());
    bb.WriteUInt32(_jobCount);

    return true;
}

bool EnterpriseUser::Save(ByteBuffer& bb) const
{
    bb.WriteUInt8(SAVE_USER_TYPE_ENTERPRISE);
    bb.WriteString(GetName());
    bb.WriteDouble(_budget);

    return true;
}

User* User::Load( ByteBuffer& bb )
{
    uint8 type = bb.ReadUInt8();
    std::string name = bb.ReadString();

    switch (type)
    {
    case SAVE_USER_TYPE_ACADEMIC:
        {
            uint32 jobCount = bb.ReadUInt32();
            return new AcademicUser(name, jobCount);
        }
    case SAVE_USER_TYPE_ENTERPRISE:
        {
            double budget = bb.ReadDouble();
            return new EnterpriseUser(name, budget);
        }
    default:
        {
            // Log("Invalid type(%u) in file.\n", type);
        }
    }

    return NULL;
}
