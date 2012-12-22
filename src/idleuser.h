#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <unordered_map>
#include <string>

#include "utils.h"
#include "interfaces.h"

/// After this time has passed, the user is removed from the list of removed users
#define REMOVAL_TIME_SECS 3600 

enum UserType;
class User;

//! IdleUser Class
/*!
    IdleUser represents an user that was removed from a Grid
*/
class IdleUser : public IUpdate, public ISave, public IPrint
{
private:
    std::string _name; ///< User name
    uint _id; ///< Original User id
    UserType _type; ///< User type (academic or enterprise)
    uint32 _elapsedTime; ///< Time in seconds since the user was removed
    static uint _maxNameLength; ///< The length of the biggest name, used in Print methods

public:
    IdleUser(const std::string& name, uint id, UserType type); ///< Constructor

    static IdleUser* FromUser(const User* user); ///< Converts User into IdleUser

    const std::string& GetName() const { return _name; } ///< Returns the User name
    uint GetId() const { return _id; } ///< Returns the original user id
    UserType GetUserType() const { return _type; } ///< Returns the user type (academic or enterprise)
    uint32 GetElapsedTime() const { return _elapsedTime; } ///> Returns the time in seconds since the user was removed

    virtual bool Save(ByteBuffer& bb) const; ///< Saves IdleUser data to a ByteBuffer

    static IdleUser* Load(ByteBuffer& bb); ///< Loads (1) IdleUser data from a ByteBuffer

    virtual void Update(uint32 diff); ///< Updates elapsed time

    virtual void Print(std::ostream& os = std::cout) const; ///< Prints current IdleUser information
    static void PrintHeader(std::ostream& os = std::cout); ///< Prints table header for IdleUsers
};

//! IdleUserContainer Class
/*!
    IdleUserContainer is used to store and manage IdleUsers
*/
class IdleUserContainer : public IUpdate, public ISave
{
private:
    std::unordered_map<uint, IdleUser*> _hashTable; ///< Underlying container

public:
    const std::unordered_map<uint, IdleUser*>& GetContainer() const { return _hashTable; } ///< Returns the underlying container

    void InsertUser(IdleUser* user) { _hashTable[user->GetId()] = user; } ///< Inserts an IdleUser in the table

    virtual bool Save(ByteBuffer& bb) const;///< Saves IdleUserContainer data to a ByteBuffer

    static IdleUserContainer* Load(ByteBuffer& bb);///< Loads IdleUserContainer data from a ByteBuffer

    virtual void Update(uint32 diff); ///< Updates all the stored IdleUser and removes users who were here for too long (see REMOVAL_TIME_SECS)
};

#endif
