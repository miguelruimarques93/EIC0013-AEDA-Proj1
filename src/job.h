#ifndef JOB_H_
#define JOB_H_

#include <string>
#include <unordered_set>
#include <sstream>
#include <chrono>
#include <iostream>

#include "utils.h"
#include "interfaces.h"
#include "software.h"

class Machine;
class User;

typedef std::unordered_set<Software, Software::Hash> SoftwareSet;

class Job : public ISave, public IUpdate, public IPrint
{
public:
    Job(const std::string& name, uint8 priority, double requiredRAM, double requiredDiskSpace, uint executionTime);

    const std::string& GetName() const { return _name; }
    uint8 GetPriority() const { return _priority; }
    double GetRequiredRAM() const { return _requiredRAM; }
    double GetRequiredDiskSpace() const { return _requiredDiskSpace; }
    uint GetTotalExecutionTime() const { return _totalExecutionTime; }
    uint GetElapsedTime() const { return _elapsedTime; }

    double GetPrice() const { return 0.05 * _requiredRAM; } // TODO: Find a suitable price

    bool Finished() const { return _totalExecutionTime <= _elapsedTime; }
    void Finish() { _elapsedTime = _totalExecutionTime; }

    bool Save(ByteBuffer& bb) const override;
    static Job* Load(ByteBuffer& bb);

    void Update(uint32 diff);

    void Print(std::ostream& os = std::cout) const override;
    void PrintWithId(std::ostream& os = std::cout) const;
    static void PrintHeader(std::ostream& os = std::cout, bool Id = false);

    void AddRequiredSoftware(const Software& sw) { _requiredSoftware.insert(sw); }
    bool IsRequiredSoftware(const Software& sw) const { return _requiredSoftware.find(sw) != _requiredSoftware.end(); }
    const SoftwareSet& GetRequiredSoftware() const { return _requiredSoftware; }

private:
    const double _requiredRAM;
    const double _requiredDiskSpace;
    const uint _totalExecutionTime;
    uint _elapsedTime;
    uint _ms;
    SoftwareSet _requiredSoftware;
    const uint8 _priority;

    const std::string _name;

    static uint _maxNameLength;

private: // no copying
    Job(const Job&);
    Job& operator =(Job const&);
};

#endif // JOB_H_
