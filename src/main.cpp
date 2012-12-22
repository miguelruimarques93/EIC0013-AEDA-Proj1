#include "loader.h"
#include "bytebuffer.h"
#include "file.h"
#include "log.h"
#include "utils.h"
#include "menu.h"
#include "menugrid.h"
#include "consolereader.h"

#include "gridnetwork.h"

#include <memory>

#define MENU_SAVE_FILE "mainMenu.txt"

int main(int, char**)
{
    std::unique_ptr<Menu> menu(Loader<Menu>(MENU_SAVE_FILE).Load());

    if (!menu)
    {
        std::cerr << MENU_SAVE_FILE << " could not be loaded." << std::endl;
        return EXIT_FAILURE;
    }

    std::unique_ptr<GridNetwork> gn;

    try
    {
        gn = std::unique_ptr<GridNetwork>(Loader<GridNetwork>(GRID_SAVE_FILE).Load());
    }
    catch (std::exception)
    {
        std::cout << "Error while loading " << GRID_SAVE_FILE << ". Corrupted file?" << std::endl;
        std::cout << "Deleting old file and continuing with a new Grid Manager" << std::endl;

        File::Remove(GRID_SAVE_FILE);
    }

    if (!gn) // no previous saves
        gn = std::unique_ptr<GridNetwork>(new GridNetwork());

    bool executing = true;

    std::function<void(GridNetwork*)> functions[] =
    {
        [&executing](GridNetwork*) { executing = false; },  // 1 - exit
        NewGrid,                                            // 2
        ChangeGridName,                                     // 3
        ChangeGridTopic,                                    // 4
        RemoveGrid,                                         // 5
        SearchGrids,                                        // 6
        ChangeGridManagerInfo,                              // 7
    };

    while (executing)
    {
        try
        {
            uint32 option = menu->Print();
            functions[option == 0 ? 0 : option - 1](gn.get());
        }
        catch (ActionCanceled& action)
        {
            PauseConsole(std::string(action.what()) + " canceled...\nPress enter to continue...");
            ClearConsole();
            continue;
        }
        catch (std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
            PauseConsole();
            ClearConsole();
        }

    }

    ByteBuffer bb(100); // TODO: use a better guess
    gn->Save(bb);
    File::Save(GRID_SAVE_FILE, bb, bb.Size());

    return EXIT_SUCCESS;
}
