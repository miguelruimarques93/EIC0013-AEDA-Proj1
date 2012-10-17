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

    for (auto sw : _requiredSoftware)
        sw.Save(bb);

    return true;
}
