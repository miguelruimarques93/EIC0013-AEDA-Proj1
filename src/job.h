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
    Job(const std::string& name, uint8 priority, double requiredRAM, double requiredDiskSpace, uint executionTime)
        : _name(name), _priority(priority), _requiredRAM(requiredRAM), _requiredDiskSpace(requiredDiskSpace), _totalExecutionTime(executionTime),
          _elapsedTime(0), _ms(0) {}

    virtual ~Job() {}

    const std::string& GetName() const { return _name; }
    uint GetPriority() const { return _priority; }
    double GetRequiredRAM() const { return _requiredRAM; }
    double GetRequiredDiskSpace() const { return _requiredDiskSpace; }
    const SoftwareSet& GetRequiredSoftware() const { return _requiredSoftware; }

    double GetPrice() const { return 0.05 * _requiredRAM; } // TODO: Find a suitable price

    bool Finished() const { return (_totalExecutionTime - _elapsedTime) <= 0; }
    void Finish() { _elapsedTime = _totalExecutionTime; }

    bool Save(ByteBuffer& bb) const override;
    static Job* Load(ByteBuffer& bb);

    void Update(uint32 diff);

private:
    const double _requiredRAM;
    const double _requiredDiskSpace;
    const uint _totalExecutionTime;
    uint _elapsedTime;
    uint _ms;
    SoftwareSet _requiredSoftware;
    const uint8 _priority;

    const std::string _name;

private: // no copying
    Job(const Job&);
    Job& operator =(Job const&);
};

#endif // JOB_H_
