#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <unordered_map>
#include <string>

#include "utils.h"
#include "interfaces.h"

#define REMOVAL_TIME_SECS 3600

enum UserType;
class User;

class IdleUser : public IUpdate, public ISave, public IPrint
{
private:
    std::string _name;
    uint _id;
    UserType _type;
    uint32 _elapsedTime;
    static uint _maxNameLength;

public:
    IdleUser(const std::string& name, uint id, UserType type);

    static IdleUser* FromUser(const User* user);

    const std::string& GetName() const { return _name; }
    uint GetId() const { return _id; }
    UserType GetUserType() const { return _type; }
    uint32 GetElapsedTime() const { return _elapsedTime; }

    virtual bool Save(ByteBuffer& bb) const;

    static IdleUser* Load(ByteBuffer& bb);

    virtual void Update(uint32 diff);

    virtual void Print(std::ostream& os = std::cout) const;
    static void PrintHeader(std::ostream& os = std::cout); ///< Prints table header for IdleUsers
};

class IdleUserContainer : public IUpdate, public ISave
{
private:
    std::unordered_map<uint, IdleUser*> _hashTable;

public:
    const std::unordered_map<uint, IdleUser*>& GetContainer() const { return _hashTable; }

    void InsertUser(IdleUser* user) { _hashTable[user->GetId()] = user; }

    virtual bool Save(ByteBuffer& bb) const;

    static IdleUserContainer* Load(ByteBuffer& bb);

    virtual void Update(uint32 diff);
};

#endif
