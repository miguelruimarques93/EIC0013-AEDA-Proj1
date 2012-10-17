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
