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
    void AddUser(User* user) { _users[_lastUserId++] = user; }
    void AddMachine(Machine* machine) { _machines[_lastMachineId++] = machine; }

    bool Save(ByteBuffer& bb) const override;

private:
    static uint _lastUserId;
    static uint _lastMachineId;

    std::map<uint, User*> _users;
    std::map<uint, Machine*> _machines;
};

#endif // GRIDMANAGER_H_
