#include "machine.h"
#include "job.h"
#include "log.h"

#include <algorithm>
#include <iostream>

uint Machine::_lastJobId = 0;

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

    if (job->GetRequiredRAM() > _availableRAM)
    {
        _mutex.unlock();
        return false;
    }

    if (job->GetRequiredDiskSpace() > _availableDiskSpace)
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

    _availableRAM -= job->GetRequiredRAM();
    _availableDiskSpace -= job->GetRequiredDiskSpace();

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

    _availableRAM += job->GetRequiredRAM();
    _availableDiskSpace += job->GetRequiredDiskSpace();

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
    bb.WriteDouble(_availableRAM);
    bb.WriteDouble(_availableDiskSpace);
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
    double availableRAM = bb.ReadDouble();
    double availableDiskSpace = bb.ReadDouble();
    uint32 maxJobs = bb.ReadUInt32();
    double totalRAM = bb.ReadDouble();
    double totalDiskSpace = bb.ReadDouble();

    Machine* m = new Machine(name, maxJobs, totalRAM, totalDiskSpace);
    m->_id = id;
    m->_availableRAM = availableRAM;
    m->_availableDiskSpace = availableDiskSpace;

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

            // Duplicated logic in RemoveJob()
            _availableRAM += it->second->GetRequiredRAM();
            _availableDiskSpace += it->second->GetRequiredDiskSpace();

            delete it->second;
            //

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

void Machine::Print(std::ostream& os /* = std::cout */) const
{
    os << "| ID: " << GetID();
    os << " | Name: " << _name << " | RAM: " << _availableRAM << "/" << _totalRAM;
    os << " MB | Disk: " << _availableDiskSpace << "/" << _totalDiskSpace;
    os << " MB | Jobs: " << _currentJobs.size() << "/" << _maxJobs << " |\n";
}
