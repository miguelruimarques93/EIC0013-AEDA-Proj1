#ifndef MENUGRID_H_
#define MENUGRID_H_

#include <stdexcept>

class GridManager;
class GridNetwork;

/// Exception thrown when a menu action was canceled
class ActionCanceled : public std::exception
{
public:
    ActionCanceled(const char* msg) : exception(msg) { }
};

// GridNetwork
void NewGrid(GridNetwork* gn);
void ChangeGridName(GridNetwork* gn);
void ChangeGridTopic(GridNetwork* gn);
void RemoveGrid(GridNetwork* gn);
void SearchGrids(GridNetwork* gn);

// GridManager
void NewAcademicUser(GridManager* gm);
void NewEnterpriseUser(GridManager* gm);
void NewMachine(GridManager* gm);
void NewPriorityMachine(GridManager* gm);
void NewJob(GridManager* gm);
void RemoveUser(GridManager* gm);
void RemoveMachine(GridManager* gm);
void RemovePriorityMachine(GridManager* gm);
void SearchUsers(GridManager* gm);
void SearchMachines(GridManager* gm);
void SearchPriorityMachines(GridManager* gm);
void SearchJobs(GridManager* gm);
void SearchRemovedUsers(GridManager* gm);

void ChangeUserInfo(GridManager* gm); ///< Menu handling of update of Users
void ChangeMachineInfo(GridManager* gm); ///< Menu handling of update of Machines
void ChangePriorityMachineInfo(GridManager* gm); ///< Menu handling of update of Priority Machines
void ChangeGridManagerInfo(GridNetwork* gn); ///< Menu handling of update of GridManager

#endif // MENUGRID_H_
