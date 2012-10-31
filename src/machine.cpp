#include "machine.h"
#include "job.h"
#include "log.h"
#include "menu.h"
#include "loader.h"

#include <algorithm>
#include <iostream>
#include <numeric>

uint Machine::_lastJobId = 0;
Menu* Machine::_menu = Loader<Menu>("machineMenu.txt").Load();

Machine::~Machine()
{
    for (auto job : _currentJobs)
        delete job.second;
}

bool Machine::AddJob(Job* job)
{
    _mutex.lock();

    if (_currentJobs.size() >= _maxJobs)
    {
        _mutex.unlock();
        return false;
    }

    if (job->GetRequiredRAM() > GetAvailableRAM())
    {
        _mutex.unlock();
        return false;
    }

    if (job->GetRequiredDiskSpace() > GetAvailableDiskSpace())
    {
        _mutex.unlock();
        return false;
    }

    const SoftwareSet& requiredSoftware = job->GetRequiredSoftware();
    for (auto it = requiredSoftware.begin(); it != requiredSoftware.end(); ++it)
    {
        if (!SoftwareMeetsRequirements(*it))
        {
            _mutex.unlock();
            return false;
        }
    }

    _currentJobs[_lastJobId] = job;
    _lastJobId++;

    _mutex.unlock();
    return true;
}

bool Machine::RemoveJob(uint id)
{
    _mutex.lock();
    auto it = _currentJobs.find(id);

    if (it == _currentJobs.end())
    {
        _mutex.unlock();
        return false;
    }

    Job* job = it->second;

    delete job;

    _currentJobs.erase(it);
    _mutex.unlock();

    return true;
}

bool Machine::SoftwareMeetsRequirements(const Software& sw) const
{
    if (sw.Dependencies.empty())
        return true;

    for (auto it = sw.Dependencies.begin(); it != sw.Dependencies.end(); ++it)
        if (_availableSoftware.find(*it) == _availableSoftware.end())
            return false;

    return true;
}

bool Machine::Save(ByteBuffer& bb) const
{
    bb.WriteUInt32(_id);
    bb.WriteString(_name);
    bb.WriteUInt32(_maxJobs);
    bb.WriteDouble(_totalRAM);
    bb.WriteDouble(_totalDiskSpace);

    bb.WriteUInt32(_currentJobs.size());
    for (auto job : _currentJobs)
        job.second->Save(bb);

    bb.WriteUInt32(_availableSoftware.size());
    for (auto sw : _availableSoftware)
        sw.Save(bb);

    return true;
}

Machine* Machine::Load(ByteBuffer& bb)
{
    uint32 id = bb.ReadUInt32();
    std::string name = bb.ReadString();
    uint32 maxJobs = bb.ReadUInt32();
    double totalRAM = bb.ReadDouble();
    double totalDiskSpace = bb.ReadDouble();

    Machine* m = new Machine(name, maxJobs, totalRAM, totalDiskSpace);
    m->_id = id;

    uint32 jobCount = bb.ReadUInt32();
    for (uint32 i = 0; i < jobCount; ++i)
        m->AddJob(Job::Load(bb));

    uint32 softwareCount = bb.ReadUInt32();
    for (uint32 i = 0; i < softwareCount; ++i)
        m->_availableSoftware.insert(Software::Load(bb));

    return m;
}

void Machine::Update(uint diff)
{
    _mutex.lock();
    for (std::map<uint, Job*>::iterator it = _currentJobs.begin(); it != _currentJobs.end();)
    {
        it->second->Update(diff);

        if (it->second->Finished())
        {
            sLog(Console)->Log("Job %s removed from machine %s.", it->second->GetName().c_str(), _name.c_str());

            delete it->second;

            _currentJobs.erase(it++);

        }
        else
            ++it;
    }
    _mutex.unlock();
}

uint Machine::GetCurrentJobs() const
{
    _mutex.lock();
    uint size = _currentJobs.size();
    _mutex.unlock();
    return size;
}

void Machine::PrintHeader(std::ostream& os /*= std::cout*/)
{
    os << "-------------------------------------------\n";
    os << "| Id | Name | RAM (MB) | Disk (MB) | Jobs |\n";
}

void Machine::Print(std::ostream& os /* = std::cout */) const
{
    os << "| " << _id << " | " << _name
       << " | " << GetAvailableRAM() << "/" << _totalRAM
       << " | " << GetAvailableDiskSpace() << "/" << _totalDiskSpace
       << " | " << _currentJobs.size() << "/" << _maxJobs << " |\n";
}

void Machine::SetMaxJobs( uint val )
{
    if (val < _currentJobs.size())   // If the value is smaller than the current number of jobs we can't modify this parameter.
        throw MachineInExecution(this); 
    
    _maxJobs = val;
}

void Machine::SetTotalRAM( double val )
{
    if (val < GetInUseRAM()) // If the value is smaller than the current needed RAM we can't modify this parameter.
        throw MachineInExecution(this);

    _totalRAM = val;
}

void Machine::SetTotalDiskSpace( double val )
{
    if (val < GetInUseDiskSpace()) // If the value is smaller than the current needed Disk Space we can't modify this parameter.
        throw MachineInExecution(this);

    _totalDiskSpace = val;
}

double Machine::GetInUseRAM() const
{
    return std::accumulate(_currentJobs.begin(), _currentJobs.end(), 0.0, [](double sum, std::pair<uint, Job*> j) { return sum + j.second->GetRequiredRAM(); } );
}

double Machine::GetInUseDiskSpace() const
{
    return std::accumulate(_currentJobs.begin(), _currentJobs.end(), 0.0, [](double sum, std::pair<uint, Job*> j) { return sum + j.second->GetRequiredDiskSpace(); } );
}
