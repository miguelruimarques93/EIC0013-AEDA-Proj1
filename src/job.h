#ifndef JOB_H_
#define JOB_H_

#include <string>
#include <vector>
#include <sstream>
#include <chrono>

#include "utils.h"
#include "utilclasses.h"
#include "software.h"

class Machine;

class Job : public ISave, public IUpdate
{
public:
    Job(const std::string& name, uint priority, double requiredRAM, double requiredDiskSpace, uint executionTime)
        : _name(name), _priority(priority), _requiredRAM(requiredRAM), _requiredDiskSpace(requiredDiskSpace), _totalExecutionTime(executionTime),
          _elapsedTime(0) {}

    const std::string& GetName() const { return _name; }
    uint GetPriority() const { return _priority; }
    double GetRequiredRAM() const { return _requiredRAM; }
    double GetRequiredDiskSpace() const { return _requiredDiskSpace; }
    const std::vector<Software>& GetRequiredSoftware() const { return _requiredSoftware; }

    bool Finished() const { return (_totalExecutionTime - _elapsedTime) <= 0; }
    void Finish() { _elapsedTime = _totalExecutionTime; }

    bool Save(ByteBuffer& bb) const override;

    void Update(uint32 diff)
    {
        if (_elapsedTime <= _totalExecutionTime)
            _elapsedTime += diff;
    }

private:
    const std::string _name;
    const uint8 _priority;
    const double _requiredRAM;
    const double _requiredDiskSpace;
    const std::vector<Software> _requiredSoftware;

    const uint _totalExecutionTime;
    uint _elapsedTime;
};

#endif // JOB_H_
