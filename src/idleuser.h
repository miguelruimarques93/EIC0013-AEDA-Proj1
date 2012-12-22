#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <unordered_map>
#include <string>

#include "utils.h"
#include "interfaces.h"

#define REMOVAL_TIME_SECS 3600

enum UserType;
class User;

class IdleUser : public IUpdate, public ISave
{
private:
    std::string _name;
    uint _id;
    UserType _type;
    uint32 _elapsedTime;

public:
    IdleUser(const std::string& name, uint id, UserType type) : _name(name), _id(id), _type(type) {}

    static IdleUser* FromUser(const User* user);

    const std::string& GetName() const { return _name; }
    uint GetId() const { return _id; }
    UserType GetUserType() const { return _type; }
    uint32 GetElapsedTime() const { return _elapsedTime; }

    virtual bool Save(ByteBuffer& bb) const;

    static IdleUser* Load(ByteBuffer& bb);

    virtual void Update(uint32 diff);
};

class IdleUserContainer : public IUpdate, public ISave
{
private:
    std::unordered_map<uint, IdleUser*> _hashTable;

public:
    /*
    bool Empty();
    void InsertUser(User* user);
    bool RemoveUser(const std::string& name);
    bool RemoveUser(const uint& id);
    */

    virtual bool Save(ByteBuffer& bb) const;

    static IdleUserContainer* Load(ByteBuffer& bb);

    virtual void Update(uint32 diff);
};

#endif