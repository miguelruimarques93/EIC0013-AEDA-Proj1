#include "gridmanager.h"
#include "loader.h"
#include "bytebuffer.h"
#include "file.h"
#include "log.h"
#include "utils.h"
#include "menu.h"
#include "menugrid.h"
#include "consolereader.h"

#include <memory>

#define MENU_SAVE_FILE "mainMenu.txt"

int main(int argc, char* argv[])
{
    std::unique_ptr<Menu> menu(Loader<Menu>(MENU_SAVE_FILE).Load());

    if (!menu.get())
    {
        std::cerr << MENU_SAVE_FILE << " could not be loaded." << std::endl;
        return EXIT_FAILURE;
    }

    std::unique_ptr<GridManager> gm;

    try
    {
        gm = std::unique_ptr<GridManager>(Loader<GridManager>(GRID_SAVE_FILE).Load());
    }
    catch (std::exception)
    {
        std::cout << "Error while loading " << GRID_SAVE_FILE << ". Corrupted file?" << std::endl;
        std::cout << "Deleting old file and continuing with a new Grid Manager" << std::endl;

        File::Remove(GRID_SAVE_FILE);
    }

    if (!gm.get()) // no previous saves
        gm = std::unique_ptr<GridManager>(new GridManager());

    bool executing = true;

    std::function<void(GridManager*)> functions[] =
    {
        [&executing](GridManager*) { executing = false; },  // 1
        NewAcademicUser,                                    // 2
        NewEnterpriseUser,                                  // 3
        RemoveUser,                                         // 4
        NewMachine,                                         // 5
        RemoveMachine,                                      // 6
        NewJob,                                             // 7
        SearchUsers,                                        // 8
        SearchMachines,                                     // 9
        SearchJobs,                                         //10
        ChangeUserInfo,                                     //11
        ChangeMachineInfo                                   //12
    };

    while (executing)
    {
        try
        {
            uint32 option = menu->Print();
            functions[option == 0 ? 0 : option - 1](gm.get());
        }
        catch (ActionCanceled& action)
        {
            PauseConsole(std::string(action.what()) + " canceled...\nPress enter to continue...");
            ClearConsole();
            continue;
        }

    }

    ByteBuffer bb(100);
    gm->Save(bb);

    File::Save(GRID_SAVE_FILE, bb, bb.Size());

    return EXIT_SUCCESS;
}
