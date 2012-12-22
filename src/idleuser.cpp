#include "idleuser.h"
#include "user.h"
#include "log.h"

#include <algorithm>

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
    bb.WriteUInt32(_hashTable.size());

    for (const auto iu : _hashTable)
    {
        bb.WriteUInt32(iu.first);
        iu.second->Save(bb);
    }

    return true;
}

IdleUserContainer* IdleUserContainer::Load(ByteBuffer& bb)
{
    IdleUserContainer* iuc = new IdleUserContainer();

    uint32 size = bb.ReadUInt32();

    for (uint32 i = 0; i < size; ++i)
    {
        uint32 id = bb.ReadUInt32();
        iuc->_hashTable[id] = IdleUser::Load(bb);
    }

    return iuc;
}

void IdleUserContainer::Update(uint32 diff)
{
    for (std::unordered_map<uint, IdleUser*>::iterator it = _hashTable.begin(); it != _hashTable.end();)
    {
        it->second->Update(diff);

        if (it->second->GetElapsedTime() > REMOVAL_TIME_SECS)
        {
            sLog(Console)->Log("IdleUser %s (id: %u) removed due to extended inactivity.", it->second->GetName(), it->first);
            delete it->second;
            it = _hashTable.erase(it);
        }
        else
            ++it;
    }
}
