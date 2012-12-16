#include "hashtable.h"

bool HashTable::Empty()
{
	return _hashTable.empty();
}


void HashTable::InsertUser(User* user)
{
	_hashTable.insert(user);
}

bool HashTable::RemoveUser( const std::string& name )
{
	for (auto it = _hashTable.begin(); it != _hashTable.end(); ++it)
	{
		if ((*it)->GetName() == name)
		{
			_hashTable.erase(it);
			return true;
		}
	}
	return false;
}

bool HashTable::RemoveUser( const uint& id )
{
	for (auto it = _hashTable.begin(); it != _hashTable.end(); ++it)
	{
		if ((*it)->GetId() == id)
		{
			_hashTable.erase(it);
			return true;
		}
	}
	return false;
}

void HashTable::Update( uint32 diff )
{
	throw std::exception("The method or operation is not implemented.");
}

