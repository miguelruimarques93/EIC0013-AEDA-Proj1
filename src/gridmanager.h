#ifndef GRIDMANAGER_H_
#define GRIDMANAGER_H_

#include "user.h"
#include "machine.h"
#include "utils.h"
#include "utilclasses.h"
#include <map>

class GridManager : public Saveable
{
public:
    uint AddUser(User* user) { _users[_lastUserId++] = user; return _lastUserId-1; }
    uint AddMachine(Machine* machine) { _machines[_lastMachineId++] = machine; return _lastMachineId-1; }

	bool RemoveUser(const User* user);
	bool RemoveUser(uint id);

	bool RemoveMachine(const Machine* machine);
	bool RemoveMachine(uint id);

	User* GetUser(uint id) const;
	Machine* GetMachine(uint id) const;

    bool Save(ByteBuffer& bb) const override;

private:
    static uint _lastUserId;
    static uint _lastMachineId;

    std::map<uint, User*> _users;
    std::map<uint, Machine*> _machines;

	time_t _lastUpdate;
};

#endif // GRIDMANAGER_H_
