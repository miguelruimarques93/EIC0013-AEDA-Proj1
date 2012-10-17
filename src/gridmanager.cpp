#include "gridmanager.h"

uint GridManager::_lastUserId = 0;
uint GridManager::_lastMachineId = 0;

bool GridManager::Save(ByteBuffer& bb) const 
{
    bb.WriteUInt32(_users.size());
    for (auto user : _users)
        user.second->Save(bb);

    bb.WriteUInt32(_machines.size());
    for (auto machine : _machines)
        machine.second->Save(bb);

    // ...
}
