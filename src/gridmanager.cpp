#include "gridmanager.h"
#include "user.h"
#include "machine.h"
#include "job.h"
#include "log.h"

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

bool GridManager::AddJob(Job* job)
{
    std::list<Machine*> machineList; // TODO: Every time a job is added this sorted list is being rebuilt; change that

    // put machines in the map into a list so we can sort them
    std::transform(_machines.begin(), _machines.end(), std::back_inserter(machineList),
        [](std::map<uint, Machine*>::value_type& val) { return val.second; });

    // sort of load balancing, better machines get assigned jobs first
    machineList.sort([](Machine* m1, Machine* m2) {
        double score1 = ((m1->GetMaxJobs() - m1->GetCurrentJobs()) + m1->GetAvailableDiskSpace() + m1->GetAvailableRAM());
        double score2 = ((m2->GetMaxJobs() - m2->GetCurrentJobs()) + m2->GetAvailableDiskSpace() + m2->GetAvailableRAM());

        return score1 > score2;
    });

    for (auto it = machineList.begin(); it != machineList.end(); ++it)
    {
        if ((*it)->AddJob(job))
        {
            sLog(Console)->Log("Job %s added to machine %s", job->GetName().c_str(), (*it)->GetMachineName().c_str());
            return true;
        }
    }

    return false;
}
