#include "idleuser.h"
#include "user.h"
#include "log.h"

#include <algorithm>
#include <iomanip>

uint IdleUser::_maxNameLength = 0;

IdleUser::IdleUser(const std::string& name, uint id, UserType type) : _name(name), _id(id), _type(type), _elapsedTime(0)
{
    if (_name.length() > _maxNameLength)
        _maxNameLength = _name.length();
}

bool IdleUser::Save(ByteBuffer& bb) const
{
    bb.WriteString(_name);
    bb.WriteUInt32(_id);
    bb.WriteUInt8((uint8)_type);
    bb.WriteUInt32(_elapsedTime);

    return true;
}

IdleUser* IdleUser::Load(ByteBuffer& bb)
{
    std::string name = bb.ReadString();
    uint id = bb.ReadUInt32();
    uint8 type = bb.ReadUInt8();
    uint32 elapsedTime = bb.ReadUInt32();

    IdleUser* iu = new IdleUser(name, id, (UserType)type);
    iu->_elapsedTime = elapsedTime;

    return iu;
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

void IdleUser::Print(std::ostream& os /*= std::cout*/) const
{
    os << "| " << std::setfill('0') << std::setw(4) << std::right << _id << " | " << std::setfill(' ') <<
        std::setw(_maxNameLength) << std::left << _name << " | " << std::setw(10) << UserTypeStr[_type] << " | " << std::setw(13) << _elapsedTime << " |\n"
       << "---------" << std::string(_maxNameLength, '-')    << "-------------------------------\n";
}

void IdleUser::PrintHeader(std::ostream& os /*= std::cout*/)
{
    os << "---------" << std::string(_maxNameLength, '-')    << "-------------------------------\n"
       << "|  Id  | " << std::setw(_maxNameLength) << "Name" << " |    Type    | Idle time (s) |\n"
       << "---------" << std::string(_maxNameLength, '-')    << "-------------------------------\n";
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
