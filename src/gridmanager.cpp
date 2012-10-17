#include "gridmanager.h"

#include <map>
#include <algorithm>

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
	return true;
}

bool GridManager::RemoveUser( const User* user )
{
	auto it = std::find_if(_users.begin(), _users.end(), [user] (std::pair<uint,User*> usr) { return usr.second == user; });
	if (it == _users.end())
		return false;

	delete it->second;
	_users.erase(it);

	return true;
}

bool GridManager::RemoveUser( uint id )
{
	auto it = _users.find(id);
	if (it == _users.end())
		return false;

	delete it->second;
	_users.erase(it);

	return true;
}

bool GridManager::RemoveMachine( const Machine* machine )
{
	auto it = std::find_if(_machines.begin(), _machines.end(), [machine] (std::pair<uint,Machine*> mach) { return mach.second == machine; });
	if (it == _machines.end())
		return false;

	delete it->second;
	_machines.erase(it);

	return true;
}

bool GridManager::RemoveMachine( uint id )
{
	auto it = _machines.find(id);
	if (it == _machines.end())
		return false;

	delete it->second;
	_machines.erase(it);

	return true;
}

User* GridManager::GetUser( uint id ) const
{
	auto it = _users.find(id);
	if (it == _users.end())
		return NULL;

	return it->second;

}

Machine* GridManager::GetMachine( uint id ) const
{
	auto it = _machines.find(id);
	if (it == _machines.end())
		return NULL;

	return it->second;

}
