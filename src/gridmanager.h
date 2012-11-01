#ifndef GRIDMANAGER_H_
#define GRIDMANAGER_H_

#include "utils.h"
#include "interfaces.h"
#include "runnable.h"

#include <map>
#include <vector>

class Job;
class Machine;
class User;

//! Hard coded save file. 
#define GRID_SAVE_FILE "gridComputing.grid"

//! GridManager Class
/*!
    The GridManager class has information about users and machines.
    It enables the user of this class to Add, Remove and Get and Search Users and Machines, and has an interface to add Jobs to a User.
*/
class GridManager : public ISave, public IUpdate, public Runnable
{
public:
    //! Constructor
    GridManager() : _realPrevTime(0), _realCurrTime(0) { Start(); }

    //! Destructor
    ~GridManager();

    /**
    *   @brief Add a User.
    *   @param user a pointer to the user to be added.
    *   @return The ID of the added user.
    */
    uint AddUser(User* user);

    /** 
    *   @brief Add a Machine.
    *   @param  machine a pointer to the machine to be added.
    *   @return The ID of the added machine.
    */
    uint AddMachine(Machine* machine);

    /**
    *   @brief Removes the given user.
    *   @param  user a pointer to the user to be removed.
    *   @return A boolean value that indicates whether the removal was successful or not.
    */
    bool RemoveUser(const User* user);

    /** 
    *   @brief Removes the user with the given ID.
    *   @param  id of the user to be removed.
    *   @return A boolean value that indicates whether the removal was successful or not.
    */
    bool RemoveUser(uint id);

    /**
    *   @brief Removes the given machine.
    *   @param  machine a pointer to the machine to be removed.
    *   @return A boolean value that indicates whether the removal was successful or not.
    */
    bool RemoveMachine(const Machine* machine);

    /** 
    *   @brief Removes the given machine with the given ID.
    *   @param  id of the machine to be removed.
    *   @return A boolean value that indicates whether the removal was successful or not.
    */
    bool RemoveMachine(uint id);

    /**
    *   @brief Returns the user with the given ID.
    *   @param  id of the user.
    *   @return A pointer to the User, NULL if id doesn't corresponds to any user.
    */
    User* GetUser(uint id) const;

    /**
    *   @brief Returns the machine with the given ID.
    *   @param  id of the machine.
    *   @return A pointer to the Machine, NULL if id doesn't corresponds to any Machine.
    */
    Machine* GetMachine(uint id) const;

    /**
    *   @brief Saves the information of the instance of the class to the given ByteBuffer.
    *   @param bb destination of the data.
    *   @return A boolean value that indicates whether the save was successful or not.
    */
    bool Save(ByteBuffer& bb) const override;

    /**
    *   @brief Update function that updates all jobs and machine. It removes finished jobs and updates the respective machines.
    *   @param diff time that has passed since last update.
    */
    void Update(uint32 diff);

    /**
    *   @brief Returns a pointer to a GridManager loaded from the given ByteBuffer.
    *   @param bb source of the data.
    *   @return A pointer to the loaded GridManager.
    */
    static GridManager* Load(ByteBuffer& bb);

    /**
    *   @brief Adds the given job to the user with the given ID.
    *   @param userId Id of the user that created the job.
    *   @param job to be added.
    *   @return A boolean value that indicates whether the insertion was successful or not.
    */
    bool AddJobByUser(int userId, Job* job) { return AddJobByUser(GetUser(userId), job); }

    /**
    *   @brief Adds the given job to the given user.
    *   @param user pointer to the user that created the job.
    *   @param job to be added.
    *   @return A boolean value that indicates whether the insertion was successful or not.
    */
    bool AddJobByUser(User* user, Job* job);

    /**
    *   @brief Applies the given predicate to the containers of the instance of the class based in the type and returns a vector with the values that meet the requirements.
    *   @param  predicate to evaluate the data.
    *   @return A vector with the result of the search.
    */
    template<class T>
    std::vector<T*> ApplyPredicate(std::function<bool(T*)> predicate) const;

private:

    /**
    *   @brief Calls update functions of all the elements of the class, that can be updated, two times per second. It saves the instance every 5 seconds.
    */
    void Run();

    /**
    *   @brief Searches for a available machine and adds the given job to it.
    *   @param job to be added.
    *   @return A boolean value that indicates whether the insertion was successful or not.
    */
    bool AddJob(Job* job);

    //! Last User ID added
    static uint _lastUserId;
    //! Last Machine ID added
    static uint _lastMachineId;

    //! Time of the last update
    uint64 _realPrevTime;
    //! Time of the current update
    uint64 _realCurrTime;

    //! Container that saves users
    std::map<uint, User*> _users;
    //! Container that saves machines
    std::map<uint, Machine*> _machines;

private: // no copying
    //! Copy constructor. Private to avoid copies of a GridManager instance.
    GridManager(const GridManager &);
    //! Assignment operator. Private to avoid copies of a GridManager instance.
    GridManager& operator =(GridManager const&);
};

#endif // GRIDMANAGER_H_
