#include "gridmanager.h"
#include "loader.h"
#include "bytebuffer.h"
#include "file.h"
#include "log.h"
#include "utils.h"
#include "menu.h"
#include "menugrid.h"

#define GRID_SAVE_FILE "gridComputing.grid"
#define MENU_SAVE_FILE "mainMenu.txt"

int main(int argc, char* argv[])
{
    std::auto_ptr<Menu> menu(Loader<Menu>(MENU_SAVE_FILE).Load());

    if (!menu.get())
    {
        std::cerr << "mainMenu.text could not be loaded." << std::endl;
        return EXIT_FAILURE;
    }

    std::auto_ptr<GridManager> gm(Loader<GridManager>(GRID_SAVE_FILE).Load());
    if (!gm.get()) // no previous saves
        gm = std::auto_ptr<GridManager>(new GridManager());

    bool executing = true;

    std::function<void(GridManager*)> functions[] =
    {
        [&executing](GridManager*) { executing = false; },  // 1
        NewAcademicUser,                                    // 2
        NewEnterpriseUser,                                  // 3
        RemoveUser,                                         // 4
        NewMachine,                                         // 5
        RemoveMachine,                                      // 6
        NewJob                                              // 7
    };

    while (executing)
        functions[menu->Print()-1](gm.get());

    ByteBuffer bb(100);
    gm->Save(bb);

    File::Save(GRID_SAVE_FILE, bb, bb.Size());

    PauseConsole();

    return EXIT_SUCCESS;
}
