#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <unordered_set>
#include "user.h"

class HashTable : public IUpdate
{
private:
	std::unordered_set<User*> _hashTable;

public:
	bool Empty();
	void InsertUser(User* user);
	bool RemoveUser(const std::string& name);
	bool RemoveUser(const uint& id);

	virtual void Update( uint32 diff );

};

#endif