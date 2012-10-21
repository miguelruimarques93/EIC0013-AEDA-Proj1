#include "gridmanager.h"
#include "user.h"
#include "machine.h"

#include <chrono>
#include <iostream>
#include <map>
#include <algorithm>

uint GridManager::_lastUserId = 0;
uint GridManager::_lastMachineId = 0;


GridManager::~GridManager()
{
    for (auto user : _users)
        delete user.second;

    for (auto machine : _machines)
        delete machine.second;
}

bool GridManager::Save(ByteBuffer& bb) const
{
    bb.WriteUInt32(_users.size());
    for (auto user : _users)
        user.second->Save(bb);

    bb.WriteUInt32(_machines.size());
    for (auto machine : _machines)
        machine.second->Save(bb);

    // ...

    return true;
}

GridManager* GridManager::Load(ByteBuffer& bb)
{
    GridManager* gm = new GridManager();

    uint32 usersCount = bb.ReadUInt32();
    for (uint32 i = 0; i < usersCount; ++i)
        gm->AddUser(User::Load(bb));

    uint32 machinesCount = bb.ReadUInt32();
    for (uint32 i = 0; i < machinesCount; ++i)
        gm->AddMachine(Machine::Load(bb));

    return gm;
}

bool GridManager::RemoveUser(const User* user)
{
    auto it = std::find_if(_users.begin(), _users.end(), [user] (std::pair<uint,User*> usr) { return usr.second == user; });
    if (it == _users.end())
        return false;

    delete it->second;
    _users.erase(it);

    return true;
}

bool GridManager::RemoveUser(uint id)
{
    auto it = _users.find(id);
    if (it == _users.end())
        return false;

    delete it->second;
    _users.erase(it);

    return true;
}

bool GridManager::RemoveMachine(const Machine* machine)
{
    auto it = std::find_if(_machines.begin(), _machines.end(), [machine] (std::pair<uint,Machine*> mach) { return mach.second == machine; });
    if (it == _machines.end())
        return false;

    delete it->second;
    _machines.erase(it);

    return true;
}

bool GridManager::RemoveMachine(uint id)
{
    auto it = _machines.find(id);
    if (it == _machines.end())
        return false;

    delete it->second;
    _machines.erase(it);

    return true;
}

User* GridManager::GetUser(uint id) const
{
    auto it = _users.find(id);
    if (it == _users.end())
        return NULL;

    return it->second;

}

Machine* GridManager::GetMachine(uint id) const
{
    auto it = _machines.find(id);
    if (it == _machines.end())
        return NULL;

    return it->second;

}

void GridManager::Update(uint32 diff)
{
    for (auto machine : _machines)
        machine.second->Update(diff);
}

void GridManager::Run()
{
    _realCurrTime = GetCurrentTime();

    while (!_stop)
    {
        _realPrevTime = _realCurrTime;
        _realCurrTime = GetCurrentTime();

        uint32 diff = GetTimeDiff(_realCurrTime, _realPrevTime);

        std::cout << diff << std::endl;

        Update(diff);

        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // sleep for half a second
    }
}
