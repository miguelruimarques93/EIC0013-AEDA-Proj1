#ifndef GRIDMANAGER_H_
#define GRIDMANAGER_H_

#include "utils.h"
#include "interfaces.h"
#include "machine.h"
#include "idleuser.h"

#include <map>
#include <vector>
#include <functional>
#include <set>

class Menu;
class User;
class Job;

//! GridManager Class
/*!
    The GridManager class has information about users and machines.
    It enables the user of this class to Add, Remove and Get and Search Users and Machines, and has an interface to add Jobs to a User.
*/
class GridManager : public ISave, public IUpdate
{
public:
    typedef std::set<User*, IdLess<User>> UserSet;
    typedef std::set<Machine*, IdLess<Machine>> MachineSet;
    typedef std::set<PriorityMachine*, IdLess<PriorityMachine>> PriorityMachineSet;
    //! Constructor
    GridManager() {}

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
    *   @brief Add a Priority Machine.
    *   @param  machine a pointer to the priority machine to be added.
    *   @return The ID of the added machine.
    */
    uint AddPriorityMachine(PriorityMachine* pMachine);

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
    *   @brief Removes the machine with the given ID.
    *   @param  id of the machine to be removed.
    *   @return A boolean value that indicates whether the removal was successful or not.
    */
    bool RemoveMachine(uint id);

    /**
    *   @brief Removes the given priority machine.
    *   @param  machine a pointer to the priority machine to be removed.
    *   @return A boolean value that indicates whether the removal was successful or not.
    */
    bool RemovePriorityMachine(const PriorityMachine* machine);

    /**
    *   @brief Removes the priority machine with the given ID.
    *   @param  id of the priority machine to be removed.
    *   @return A boolean value that indicates whether the removal was successful or not.
    */
    bool RemovePriorityMachine(uint id);

    /**
    *   @brief Returns the user with the given ID.
    *   @param  id of the user.
    *   @return A pointer to the User, NULL if id doesn't corresponds to any user.
    */
    User* GetUser(uint id) const;

    /**
    *   @brief Returns the number of existing users.
    *   @return Number of existing users.
    */
    uint GetNumberOfUsers() const { return _users.size(); }

    /**
    *   @brief Returns the machine with the given ID.
    *   @param  id of the machine.
    *   @return A pointer to the Machine, NULL if id doesn't corresponds to any Machine.
    */
    Machine* GetMachine(uint id) const;

    /**
    *   @brief Returns the priority machine with the given ID.
    *   @param  id of the priority machine.
    *   @return A pointer to the Priority Machine, NULL if id doesn't corresponds to any Machine.
    */
    PriorityMachine* GetPriorityMachine(uint id) const;

    /**
    *   @brief Returns the number of existing machines.
    *   @return Number of existing machines.
    */
    uint GetNumberOfMachines() const { return _machines.size(); }

    /**
    *   @brief Returns the number of existing priority machines.
    *   @return Number of existing priority machines.
    */
    uint GetNumberOfPriorityMachines() const { return _priorityMachines.size(); }

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
    *   @brief Adds the given job by the user with the given Id.
    *   @param userId Id of the user that wants to create the job.
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
    *   @brief Removes the given job from the given machine.
    *   @param machine pointer to the machine that contains the job.
    *   @param jobId id of the job to be removed.
    *   @return A boolean value that indicates whether the removal was successful or not.
    */
    bool RemoveMachineJob(Machine* machine, uint jobId);

    /**
    *   @brief Returns the number of existing jobs.
    *   @return Number of existing jobs.
    */
    uint GetNumberOfJobs() const;

    /**
    *   @brief Applies the given predicate to the containers of the instance of the class based in the type and returns a vector with the values that meet the requirements.
    *   @param  predicate to evaluate the data: unary function that returns a bool and accepts a T* as argument.
    *   @return A vector with the result of the search.
    *   @remark Only defined for T = Job, User, IdleUser or Machine
    */
    template<class T>
    std::vector<const T*> ApplyPredicate(std::function<bool(const T*)> predicate) const;

    /**
    *   @brief Uses ApplyPredicate with the given predicate and then selects a certain element of each object in the vector returned by ApplyPredicate.
    *   @param selector to retrieve an element from the data: unary function that returns R and accepts a T* as argument.
    *   @param predicate to evaluate the data: unary function that returns a bool and accepts a T* as argument. Default value applies no restrictions.
    *   @return A vector with the result of the search.
    *   @remark Only defined for T = Job, User, IdleUser or Machine
    */
    template<class T, class R>
    std::vector<R> ApplySelector(std::function<R(const T*)> selector, std::function<bool(const T*)> predicate) const;

    /**
    *   @brief Selects a certain element of each object in the vector of T*.
    *   @param selector to retrieve an element from the data: unary function that returns R and accepts a T* as argument.
    *   @return A vector with the result of the search.
    *   @remark Only defined for T = Job, User, IdleUser or Machine
    */
    template<class T, class R>
    std::vector<R> ApplySelector(std::function<R(const T*)> selector) const;

    static Menu* GetMenu() { return _menu; } ///< Returns the menu for the GridManager class

    const IdleUserContainer& GetIdleUsers() const { return _idleUsers; }

private:
    /**
    *   @brief Searches for a available machine and adds the given job to it.
    *   @param job to be added.
    *   @return A boolean value that indicates whether the insertion was successful or not.
    */
    bool AddJob(Job* job);

    //! Last User Id added
    static uint _lastUserId;
    //! Last Machine Id added
    static uint _lastMachineId;

    //! Container that saves users
    UserSet _users;
    //! Container that saves machines
    MachineSet _machines;
    //! Container that saves priority machines
    PriorityMachineSet _priorityMachines;
    //! Container that stores waiting jobs
    std::queue<Job*> _waitingJobs;


    static Menu* _menu; ///< Menu associated with the GridManager class

    IdleUserContainer _idleUsers; ///< Container used to store users that are removed

private: // no copying
    //! Copy constructor. Private to avoid copies of a GridManager instance.
    GridManager(const GridManager &);
    //! Assignment operator. Private to avoid copies of a GridManager instance.
    GridManager& operator =(GridManager const&);
};

template<class T, class R>
std::vector<R> GridManager::ApplySelector(std::function<R(const T*)> selector) const
{
    return ApplySelector<T, R>(selector, [](const T*) { return true; });
}

template<class T, class R>
std::vector<R> GridManager::ApplySelector(std::function<R(const T*)> selector, std::function<bool(const T*)> predicate) const
{
    // get all machines, jobs, etc. that match the predicate
    std::vector<const T*> ts = ApplyPredicate<T>(predicate);

    // apply our selector to the initial vector to create a new one
    std::vector<R> result;
    result.reserve(ts.size()); // max size of the resulting vector

    std::transform(ts.begin(), ts.end(), std::back_inserter(result), selector);

    return result;
}

#endif // GRIDMANAGER_H_
