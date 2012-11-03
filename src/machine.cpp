#include "machine.h"
#include "job.h"
#include "log.h"
#include "menu.h"
#include "loader.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <iomanip>

uint Machine::_lastJobId = 0;
uint Machine::_maxNameLength = 0;
Menu* Machine::_menu = Loader<Menu>("machineMenu.txt").Load();

Machine::Machine(const std::string& machineName, uint maxJobs, double totalRAM, double totalDiskSpace) :
    _id(0), _name(machineName), _maxJobs(maxJobs), _totalRAM(totalRAM), _totalDiskSpace(totalDiskSpace)
{
    if (_name.length() > _maxNameLength)
        _maxNameLength = _name.length();
}

void Machine::SetName( const std::string& name )
{
    if (name.size() != 0)
    {
        _name = name;
        if (_name.length() > _maxNameLength)
            _maxNameLength = _name.length();
    }
}

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
    os << "---------" << std::string(_maxNameLength, '-')    << "------------------------------------------------\n"
       << "|  Id  | " << std::setw(_maxNameLength) << "Name" << " |   RAM (MB)    |   Disk (MB)   |    Jobs     |\n"
       << "---------" << std::string(_maxNameLength, '-')    << "------------------------------------------------\n";
}

void Machine::Print(std::ostream& os /* = std::cout */) const
{
    os << "| " << std::setfill('0') << std::setw(4) << std::right << _id << " | "
       << std::setfill(' ') << std::setw(_maxNameLength) << std::left << _name

       << " | " << std::setw(5) << std::left << GetAvailableRAM() << " / " << std::right << std::setw(5) << _totalRAM
       << " | " << std::setw(5) << std::left << GetAvailableDiskSpace() << " / " << std::right << std::setw(5) << _totalDiskSpace
       << " | " << std::setw(4) << std::left << _currentJobs.size() << " / " << std::right << std::setw(4) << _maxJobs << " |\n"
       << "---------" << std::string(_maxNameLength, '-') << "------------------------------------------------\n";
}

void Machine::SetMaxJobs(uint val)
{
    if (val < _currentJobs.size())   // If the value is smaller than the current number of jobs we can't modify this parameter.
        throw MachineInExecution(this);

    _maxJobs = val;
}

void Machine::SetTotalRAM(double val)
{
    if (val < GetInUseRAM()) // If the value is smaller than the current needed RAM we can't modify this parameter.
        throw MachineInExecution(this);

    _totalRAM = val;
}

void Machine::SetTotalDiskSpace(double val)
{
    if (val < GetInUseDiskSpace()) // If the value is smaller than the current needed Disk Space we can't modify this parameter.
        throw MachineInExecution(this);

    _totalDiskSpace = val;
}

double Machine::GetInUseRAM() const
{
    return std::accumulate(_currentJobs.begin(), _currentJobs.end(), 0.0,
        [](double sum, std::pair<uint, Job*> j) { return sum + j.second->GetRequiredRAM(); });
}

double Machine::GetInUseDiskSpace() const
{
    return std::accumulate(_currentJobs.begin(), _currentJobs.end(), 0.0,
        [](double sum, std::pair<uint, Job*> j) { return sum + j.second->GetRequiredDiskSpace(); });
}
