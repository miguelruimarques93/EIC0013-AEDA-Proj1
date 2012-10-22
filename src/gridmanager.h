#ifndef GRIDMANAGER_H_
#define GRIDMANAGER_H_

#include "utils.h"
#include "interfaces.h"
#include "runnable.h"

#include <map>

class User;
class Machine;
class Job;

class GridManager : public ISave, public IUpdate, public Runnable
{
public:
    GridManager() : _realPrevTime(0), _realCurrTime(0) { Start(); }
    ~GridManager();

    uint AddUser(User* user) { _users[_lastUserId++] = user; return _lastUserId-1; }
    uint AddMachine(Machine* machine) { _machines[_lastMachineId++] = machine; return _lastMachineId-1; }

    bool RemoveUser(const User* user);
    bool RemoveUser(uint id);

    bool RemoveMachine(const Machine* machine);
    bool RemoveMachine(uint id);

    User* GetUser(uint id) const;
    Machine* GetMachine(uint id) const;

    bool Save(ByteBuffer& bb) const override;

    void Update(uint32 diff);

    static GridManager* Load(ByteBuffer& bb);

    bool AddJob(Job* job);

protected:

    void Run();

private:
    static uint _lastUserId;
    static uint _lastMachineId;

    uint64 _realPrevTime;
    uint64 _realCurrTime;

    std::map<uint, User*> _users;
    std::map<uint, Machine*> _machines;

private: // no copying
    GridManager(const GridManager &);
    GridManager& operator =(GridManager const&);
};

#endif // GRIDMANAGER_H_
