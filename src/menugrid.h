#ifndef MENUGRID_H_
#define MENUGRID_H_

class GridManager;

void NewAcademicUser(GridManager* gm);
void NewEnterpriseUser(GridManager* gm);
void RemoveUser(GridManager* gm);
void NewMachine(GridManager* gm);
void RemoveMachine(GridManager* gm);
void NewJob(GridManager* gm);
void SearchUsers(GridManager* gm);
void SearchMachines(GridManager* gm);
void SearchJobs(GridManager* gm);
void ChangeUserInfo(GridManager* gm);
void ChangeMachineInfo(GridManager* gm);

#endif // MENUGRID_H_
