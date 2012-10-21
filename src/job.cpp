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
