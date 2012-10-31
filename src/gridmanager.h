#ifndef GRIDMANAGER_H_
#define GRIDMANAGER_H_

#include "utils.h"
#include "interfaces.h"
#include "runnable.h"

#include <map>
#include <vector>

class Job;
class Machine;
class User;

class GridManager : public ISave, public IUpdate, public Runnable
{
public:
    GridManager() : _realPrevTime(0), _realCurrTime(0) { Start(); }
    ~GridManager();

    uint AddUser(User* user); 	
    uint AddMachine(Machine* machine);

    bool RemoveUser(const User* user);
    bool RemoveUser(uint id);

    bool RemoveMachine(const Machine* machine);
    bool RemoveMachine(uint id);

    User* GetUser(uint id) const;
    Machine* GetMachine(uint id) const;

    bool Save(ByteBuffer& bb) const override;

    void Update(uint32 diff);

    static GridManager* Load(ByteBuffer& bb);

    bool AddJobByUser(int userId, Job* job) { return AddJobByUser(GetUser(userId), job); }
    bool AddJobByUser(User* user, Job* job);

    template<class T>
    std::vector<T*> ApplyPredicate(std::function<bool(T*)> predicate) const;

private:
    void Run();
    bool AddJob(Job* job);

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
