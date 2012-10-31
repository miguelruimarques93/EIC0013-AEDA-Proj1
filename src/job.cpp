#include "job.h"
#include "bytebuffer.h"

bool Job::Save(ByteBuffer& bb) const
{
    bb.WriteString(_name);
    bb.WriteUInt8(_priority);
    bb.WriteDouble(_requiredRAM);
    bb.WriteDouble(_requiredDiskSpace);
    bb.WriteUInt32(_totalExecutionTime);
    bb.WriteUInt32(_elapsedTime);

    bb.WriteUInt32(_requiredSoftware.size());
    for (auto sw : _requiredSoftware)
        sw.Save(bb);

    return true;
}

Job* Job::Load(ByteBuffer& bb)
{
    std::string name = bb.ReadString();
    uint8 priority = bb.ReadUInt8();
    double requiredRAM = bb.ReadDouble();
    double requiredDiskSpace = bb.ReadDouble();
    uint32 totalExecutionTime = bb.ReadUInt32();
    uint32 elapsedTime = bb.ReadUInt32();

    Job* j = new Job(name, priority, requiredRAM, requiredDiskSpace, totalExecutionTime);
    j->_elapsedTime = elapsedTime;

    uint32 softwareCount = bb.ReadUInt32();
    for (uint32 i = 0; i < softwareCount; ++i)
        j->_requiredSoftware.insert(Software::Load(bb));

    return j;
}

void Job::Update(uint32 diff)
{
    _ms += diff;

    if (_ms >= 1000) // diff is in seconds, job times are in seconds
    {
        if (_elapsedTime <= _totalExecutionTime)
            _elapsedTime += 1;
        _ms = 0;
    }
}

void Job::PrintHeader(std::ostream& os /*= std::cout*/)
{
    os << "---------------------------------------------------------\n"
       << "| Name | RAM (MB) | Disk (MB) | Priority (%) | Time (s) |\n";
}

void Job::Print(std::ostream& os /*=std::cout*/) const
{
    os << "| " << _name << " | " << _requiredRAM << " | " << _requiredDiskSpace
       << " | " << static_cast<uint16>(_priority)
       << " | " << _elapsedTime << " / " << _totalExecutionTime << " |\n";
}
