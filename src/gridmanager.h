#ifndef GRIDMANAGER_H_
#define GRIDMANAGER_H_

#include "user.h"
#include "machine.h"
#include "utils.h"
#include <map>

class GridManager
{
public:

protected:

private:
    static uint _lastUserId;
    static uint _lastMachienId;

    std::map<uint, User*> _users;
    std::map<uint, Machine*> _machines;

};

#endif // GRIDMANAGER_H_
