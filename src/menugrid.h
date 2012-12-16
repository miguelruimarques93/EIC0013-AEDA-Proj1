#ifndef MENUGRID_H_
#define MENUGRID_H_

#include <stdexcept>

#include "gridmanager.h"

/// Exception thrown when a menu action was canceled
class ActionCanceled : public std::exception
{
public:
    ActionCanceled(const char* msg) : exception(msg) { }
};

void NewAcademicUser(GridManager* gm); ///< Menu handling of creation of an AcademicUser
void NewEnterpriseUser(GridManager* gm); ///< Menu handling of creation of an EnterpriseUser
void RemoveUser(GridManager* gm); ///< Menu handling of deletion of an User
void NewMachine(GridManager* gm); ///< Menu handling of creation of a Machine
void RemoveMachine(GridManager* gm); ///< Menu handling of deletion of a Machine
void NewJob(GridManager* gm); ///< Menu handling of creation of a Job
void SearchUsers(GridManager* gm); ///< Menu handling of listings of Users
void SearchMachines(GridManager* gm); ///< Menu handling of listings of Machines
void SearchJobs(GridManager* gm); ///< Menu handling of listings of Jobs
void ChangeUserInfo(GridManager* gm); ///< Menu handling of update of Users
void ChangeMachineInfo(GridManager* gm); ///< Menu handling of update of Machines

#endif // MENUGRID_H_
