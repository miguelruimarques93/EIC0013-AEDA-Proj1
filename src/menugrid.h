#ifndef MENUGRID_H_
#define MENUGRID_H_

#include <stdexcept>

class Grid;
class GridManager;
class GridNetwork;

/// Exception thrown when a menu action was canceled
class ActionCanceled : public std::exception
{
public:
    ActionCanceled(const char* msg) : exception(msg) { }
};

void NewGrid(GridNetwork* gn);
void ChangeGridName(GridNetwork* gn);
void ChangeGridTopic(GridNetwork* gn);
void RemoveGrid(GridNetwork* gn);
void SearchGrids(GridNetwork* gn);

void ChangeUserInfo(GridManager* gm); ///< Menu handling of update of Users

void ChangeMachineInfo(GridManager* gm); ///< Menu handling of update of Machines

void ChangeGridManagerInfo(GridNetwork* gn); ///< Menu handling of update of GridManager

#endif // MENUGRID_H_
