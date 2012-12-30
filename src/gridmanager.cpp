#include "gridmanager.h"
#include "user.h"
#include "machine.h"
#include "job.h"
#include "log.h"
#include "loader.h"
#include "menu.h"

#include <chrono>
#include <iostream>
#include <map>
#include <algorithm>
#include <numeric>

uint GridManager::_lastUserId = 0;
uint GridManager::_lastMachineId = 0;
Menu* GridManager::_menu = Loader<Menu>("gridManagerMenu.txt").Load();

GridManager::~GridManager()
{
    for (auto user : _users)
        delete user;

    for (auto machine : _machines)
        delete machine;
}

bool GridManager::Save(ByteBuffer& bb) const
{
    bb.WriteUInt32(_lastUserId);
    bb.WriteUInt32(_lastMachineId);
    bb.WriteUInt32(Machine::GetLastJobId());

    bb.WriteUInt32(_users.size());
    for (auto user : _users)
        user->Save(bb);

    bb.WriteUInt32(_machines.size());
    for (auto machine : _machines)
        machine->Save(bb);

    bb.WriteUInt32(_priorityMachines.size());
    for (auto pMachine : _priorityMachines)
        pMachine->Save(bb);

    bb.WriteUInt32(_waitingJobs.size());
    for (std::queue<Job*> temp(_waitingJobs); !temp.empty(); temp.pop())
        temp.front()->Save(bb);

    _idleUsers.Save(bb);

    return true;
}

GridManager* GridManager::Load(ByteBuffer& bb)
{
    GridManager* gm = new GridManager();

    _lastUserId = bb.ReadUInt32();
    _lastMachineId = bb.ReadUInt32();
    Machine::SetLastJobId(bb.ReadUInt32());

    uint32 usersCount = bb.ReadUInt32();
    for (uint32 i = 0; i < usersCount; ++i)
        gm->AddUser(User::Load(bb));

    uint32 machinesCount = bb.ReadUInt32();
    for (uint32 i = 0; i < machinesCount; ++i)
        gm->AddMachine(Machine::Load(bb));

    uint32 priorityMachinesCount = bb.ReadUInt32();
    for (uint32 i = 0; i < priorityMachinesCount; ++i)
        gm->AddPriorityMachine(PriorityMachine::Load(bb));

    uint32 waitingJobsCount = bb.ReadUInt32();
    for (uint32 i = 0; i < waitingJobsCount; ++i)
        gm->AddJob(Job::Load(bb));

    gm->_idleUsers = *IdleUserContainer::Load(bb);

    return gm;
}

bool GridManager::RemoveUser(const User* user)
{
    if (!user) return NULL;
    auto it = std::find_if(_users.begin(), _users.end(), [user] (User* usr) { return usr == user; });
    if (it == _users.end())
    {
        sLog(Console)->Log("GridManager::RemoveUser(User*): Could not find user %s in GridManager", user->GetName().c_str());
        return false;
    }

    _idleUsers.InsertUser(IdleUser::FromUser(*it));

    delete *it;
    _users.erase(it);

    return true;
}

bool GridManager::RemoveUser(uint id)
{
    AcademicUser usr(id);
    auto it = _users.find(&usr);
    if (it == _users.end())
    {
        sLog(Console)->Log("GridManager::RemoveUser(uint): Could not find user with id %u in GridManager", id);
        return false;
    }

    _idleUsers.InsertUser(IdleUser::FromUser(*it));

    delete *it;
    _users.erase(it);

    return true;
}

bool GridManager::RemoveMachine(const Machine* machine)
{
    auto it = std::find_if(_machines.begin(), _machines.end(), [machine] (Machine* mach) { return mach == machine; });
    if (it == _machines.end())
    {
        sLog(Console)->Log("GridManager::RemoveMachine(Machine*): Could not find machine %s in GridManager", machine->GetName().c_str());
        return false;
    }

    delete *it;
    _machines.erase(it);

    return true;
}

bool GridManager::RemoveMachine(uint id)
{
    Machine m(id);
    auto it = _machines.find(&m);
    if (it == _machines.end())
    {
        sLog(Console)->Log("GridManager::RemoveMachine(uint): Could not find machine with id %u in GridManager", id);
        return false;
    }

    delete *it;
    _machines.erase(it);

    return true;
}

bool GridManager::RemovePriorityMachine(const PriorityMachine* machine)
{
    auto it = std::find_if(_priorityMachines.begin(), _priorityMachines.end(), [machine] (PriorityMachine* mach) { return mach == machine; });
    if (it == _priorityMachines.end())
    {
        sLog(Console)->Log("GridManager::RemovePriorityMachine(PriorityMachine*): Could not find priority machine %s in GridManager", machine->GetName().c_str());
        return false;
    }

    delete *it;
    _priorityMachines.erase(it);

    return true;
}

bool GridManager::RemovePriorityMachine(uint id)
{
    PriorityMachine m(id);
    auto it = _priorityMachines.find(&m);
    if (it == _priorityMachines.end())
    {
        sLog(Console)->Log("GridManager::RemovePriorityMachine(uint): Could not find priority machine with id %u in GridManager", id);
        return false;
    }

    delete *it;
    _priorityMachines.erase(it);

    return true;
}

User* GridManager::GetUser(uint id) const
{
    AcademicUser usr(id);
    auto it = _users.find(&usr);
    if (it == _users.end())
    {
        sLog(Console)->Log("GridManager::GetUser: Could not find user with id %u in GridManager", id);
        return NULL;
    }

    return *it;

}

Machine* GridManager::GetMachine(uint id) const
{
    Machine m(id);
    auto it = _machines.find(&m);
    if (it == _machines.end())
    {
        sLog(Console)->Log("GridManager::GetMachine: Could not find machine with id %u in GridManager", id);
        return NULL;
    }

    return *it;
}

PriorityMachine* GridManager::GetPriorityMachine( uint id ) const
{
    PriorityMachine m(id);
    auto it = _priorityMachines.find(&m);
    if (it == _priorityMachines.end())
    {
        sLog(Console)->Log("GridManager::GetPriorityMachine: Could not find priority machine with id %u in GridManager", id);
        return NULL;
    }

    return *it;
}

void GridManager::Update(uint32 diff)
{
    for (auto machine : _machines)
        machine->Update(diff);

    for (auto pMachine : _priorityMachines)
        pMachine->Update(diff);

    if (!_waitingJobs.empty())
    {
        Job* job = _waitingJobs.front();
        if (job->GetPriority() == 0)
        {
            std::vector<Machine*> machineVector; // TODO: Every time a job is added this sorted vector is being rebuilt; change that
            machineVector.reserve(_machines.size());

            // put machines in the map into a vector so we can sort them
            std::transform(_machines.begin(), _machines.end(), std::back_inserter(machineVector),
                [](MachineSet::value_type val) { return val; });

            // sort of load balancing, better machines get assigned jobs first
            std::sort(machineVector.begin(), machineVector.end(), [](Machine* m1, Machine* m2) {
                double score1 = ((m1->GetMaxJobs() - m1->GetNumberOfCurrentJobs()) + m1->GetAvailableDiskSpace() + m1->GetAvailableRAM());
                double score2 = ((m2->GetMaxJobs() - m2->GetNumberOfCurrentJobs()) + m2->GetAvailableDiskSpace() + m2->GetAvailableRAM());

                return score1 > score2;
            });

            for (auto& mach : machineVector)
            {
                if (mach->AddJob(job))
                {
                    sLog(Console)->Log("Job %s added to machine %s", job->GetName().c_str(), mach->GetName().c_str());
                    _waitingJobs.pop();
                }
            }
        }
        else
        {
            std::vector<PriorityMachine*> machineVector; // TODO: Every time a job is added this sorted vector is being rebuilt; change that
            machineVector.reserve(_priorityMachines.size());

            // put machines in the map into a vector so we can sort them
            std::transform(_priorityMachines.begin(), _priorityMachines.end(), std::back_inserter(machineVector),
                [](PriorityMachineSet::value_type val) { return val; });

            // sort of load balancing, better machines get assigned jobs first
            std::sort(machineVector.begin(), machineVector.end(), [](PriorityMachine* m1, PriorityMachine* m2) {
                double score1 = ((m1->GetMaxJobs() - m1->GetNumberOfCurrentJobs()) + m1->GetAvailableDiskSpace() + m1->GetAvailableRAM());
                double score2 = ((m2->GetMaxJobs() - m2->GetNumberOfCurrentJobs()) + m2->GetAvailableDiskSpace() + m2->GetAvailableRAM());

                return score1 > score2;
            });

            for (auto& mach : machineVector)
            {
                if (mach->AddJob(job))
                {
                    sLog(Console)->Log("Job %s added to machine %s", job->GetName().c_str(), mach->GetName().c_str());
                    _waitingJobs.pop();
                }
            }
        }
    }

    _idleUsers.Update(diff);
}

bool GridManager::AddJob(Job* job)
{
    if (!job)
    {
        sLog(Console)->Log("GridManager::AddJob: Tried to added a Job with null pointer");
        return false;
    }

    _waitingJobs.push(job);

    return true;
}

bool GridManager::AddJobByUser(User* user, Job* job)
{
    if (!user)
    {
        sLog(Console)->Log("GridManager::AddJobByUser: Tried to added a Job with user null pointer");
        return false;
    }

    if (!job)
    {
        sLog(Console)->Log("GridManager::AddJobByUser: Tried to added a Job with job null pointer");
        return false;
    }

    if (!user->CanCreateJob(job))
        throw std::runtime_error("Can't create Job because User doesn't have enough budget.");

    if (!AddJob(job))
        return false;

    user->CreatedJob(job);

    return true;
}

template<>
std::vector<const Job*> GridManager::ApplyPredicate<Job>(std::function<bool(const Job*)> predicate) const
{
    std::vector<const Job*> result;

    for (auto machine : _machines)
        for (auto job : machine->GetJobs())
            if (predicate(job))
                result.push_back(job);

    for (auto pmachine : _priorityMachines)
        for (auto job : pmachine->GetJobs())
            if (predicate(job))
                result.push_back(job);

    return result;
}

template<>
std::vector<const User*> GridManager::ApplyPredicate<User>(std::function<bool(const User*)> predicate) const
{
    std::vector<const User*> result;

    for (auto user : _users)
        if (predicate(user))
            result.push_back(user);

    return result;
}

template<>
std::vector<const IdleUser*> GridManager::ApplyPredicate<IdleUser>(std::function<bool(const IdleUser*)> predicate) const
{
    std::vector<const IdleUser*> result;

    for (auto iu : _idleUsers.GetContainer())
        if (predicate(iu.second))
            result.push_back(iu.second);

    return result;
}

template<>
std::vector<const Machine*> GridManager::ApplyPredicate<Machine>(std::function<bool(const Machine*)> predicate) const
{
    std::vector<const Machine*> result;

    for (auto machine : _machines)
        if (predicate(machine))
            result.push_back(machine);

    return result;
}

template<>
std::vector<const PriorityMachine*> GridManager::ApplyPredicate<PriorityMachine>(std::function<bool(const PriorityMachine*)> predicate) const
{
    std::vector<const PriorityMachine*> result;

    for (auto machine : _priorityMachines)
        if (predicate(machine))
            result.push_back(machine);

    return result;
}

uint GridManager::AddUser(User* user)
{
    if (!user)
        return 0;

    if (user->GetId() == 0) // inserting new user
    {
        _lastUserId++;
        user->SetId(_lastUserId);
    }

    _users.insert(user);

    return user->GetId();
}

uint GridManager::AddMachine(Machine* machine)
{
    if (!machine)
        return 0;

    if (machine->GetId() == 0) // inserting new machine
    {
        _lastMachineId++;
        machine->SetId(_lastMachineId);
    }

    _machines.insert(machine);

    return machine->GetId();
}

uint GridManager::GetNumberOfJobs() const
{
    return std::accumulate(_machines.begin(), _machines.end(), 0,
        [](uint sum, Machine* mach) { return sum + mach->GetNumberOfCurrentJobs(); }) +
           std::accumulate(_priorityMachines.begin(), _priorityMachines.end(), 0,
        [](uint sum, PriorityMachine* pMach) { return sum + pMach->GetNumberOfCurrentJobs(); });
}

bool GridManager::RemoveMachineJob(Machine* machine, uint jobId)
{
    if (!machine)
    {
        sLog(Console)->Log("GridManager::RemoveMachineJob: Tried to remove machine with machine null pointer");
        return false;
    }

    return machine->RemoveJob(jobId);
}

uint32 GridManager::AddPriorityMachine(PriorityMachine* machine)
{
    if (!machine)
        return 0;

    if (machine->GetId() == 0) // inserting new machine
    {
        _lastMachineId++;
        machine->SetId(_lastMachineId);
    }

    _priorityMachines.insert(machine);

    return machine->GetId();
}
