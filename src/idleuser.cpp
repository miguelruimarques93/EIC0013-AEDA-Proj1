#include "idleuser.h"

bool IdleUser::Save(ByteBuffer& bb) const
{
    bb.WriteString(_name);
    bb.WriteUInt32(_id);
    bb.WriteUInt8((uint8)_type);

    return true;
}

IdleUser* IdleUser::Load(ByteBuffer& bb)
{
    std::string name = bb.ReadString();
    uint id = bb.ReadUInt32();
    uint8 type = bb.ReadUInt8();

    return new IdleUser(name, id, (UserType)type);
}

IdleUser* IdleUser::FromUser(const User* user)
{
    return new IdleUser(user->GetName(), user->GetId(), typeid(*user) == typeid(AcademicUser) ? USER_TYPE_ACADEMIC : USER_TYPE_ENTERPRISE);
}

void IdleUser::Update(uint32 diff)
{
    static uint32 elapsedMS = 0;

    elapsedMS += diff;

    // increase one second if 1000 milliseconds have passed
    if (elapsedMS > 1000)
    {
        _elapsedTime += 1;
        elapsedMS = 0;
    }
}

bool IdleUserContainer::Save(ByteBuffer& bb) const
{
    throw std::exception("The method or operation is not implemented.");
}

void IdleUserContainer::Update(uint32 diff)
{
    throw std::exception("The method or operation is not implemented.");
}
