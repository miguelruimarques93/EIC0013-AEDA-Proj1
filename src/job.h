#ifndef JOB_H_
#define JOB_H_

#include <string>
#include <unordered_set>
#include <sstream>
#include <chrono>

#include "utils.h"
#include "interfaces.h"
#include "software.h"

class Machine;

typedef std::unordered_set<Software, Software::Hash> SoftwareSet;

class Job : public ISave, public IUpdate
{
public:
    Job(const std::string& name, uint priority, double requiredRAM, double requiredDiskSpace, uint executionTime)
        : _name(name), _priority(priority), _requiredRAM(requiredRAM), _requiredDiskSpace(requiredDiskSpace), _totalExecutionTime(executionTime),
          _elapsedTime(0), _ms(0) {}

    const std::string& GetName() const { return _name; }
    uint GetPriority() const { return _priority; }
    double GetRequiredRAM() const { return _requiredRAM; }
    double GetRequiredDiskSpace() const { return _requiredDiskSpace; }
    const SoftwareSet& GetRequiredSoftware() const { return _requiredSoftware; }

    bool Finished() const { return (_totalExecutionTime - _elapsedTime) <= 0; }
    void Finish() { _elapsedTime = _totalExecutionTime; }

    bool Save(ByteBuffer& bb) const override;
    static Job* Load(ByteBuffer& bb);

    void Update(uint32 diff)
    {
        _ms += diff;

        if (_ms >= 1000) // diff is in seconds, job times are in seconds
        {
            if (_elapsedTime <= _totalExecutionTime)
                _elapsedTime += 1;
            _ms = 0;
        }      
    }

private:
    const std::string _name;
    const uint8 _priority;
    const double _requiredRAM;
    const double _requiredDiskSpace;
    const uint _totalExecutionTime;

    SoftwareSet _requiredSoftware;

    uint _elapsedTime;
    uint32 _ms;
};

#endif // JOB_H_
