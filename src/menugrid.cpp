#include "menugrid.h"
#include "utils.h"
#include "consolereader.h"
#include "user.h"
#include "machine.h"
#include "job.h"
#include "menu.h"
#include "gridmanager.h"
#include "gridnetwork.h"
#include "grid.h"
#include "loader.h"
#include "software.h"

#include <iostream>
#include <typeinfo>
#include <functional>
#include <iomanip>

static std::function<bool(std::string)> _namePredicate = [](const std::string& val)
{
    if (val.size() <= 0)
    {
        std::cout << "Name cannot be empty." << std::endl << "Please try again." << std::endl;
        return false;
    }
    else if (val.size() > 25)
    {
        std::cout << "Name cannot have more than 25 characters." << std::endl << "Please try again." << std::endl;
        return false;
    }

    return true;
};

void NewGrid(GridNetwork* gn)
{
    std::string name, topic;
    try
    {
        name = ReadValue<std::string>("Name (max 25 characters): ", [gn](const std::string& name)
        {
            if (!_namePredicate(name))
                return false;

            try
            {
                gn->GetGrid(name);
            }
            catch (UnknownGrid&) // grid doesn't already exist, name is valid
            {
            	return true;
            }

            std::cout << "A Grid with the same name already exists." << std::endl << "Please try again." << std::endl;
            return false;
        });
        topic = ReadValue<std::string>("Topic (max 25 characters): ", _namePredicate);
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("New Grid");
    }

    gn->NewGrid(name, topic);

    PauseConsole();
    ClearConsole();
}

void ChangeGridName(GridNetwork* gn)
{
    std::string name, newName;

    try
    {
        name = ReadValue<std::string>("Old name (max 25 characters): ", [gn](const std::string& name)
        {
            if (!_namePredicate(name))
                return false;

            try
            {
                gn->GetGrid(name);
            }
            catch (UnknownGrid&) // grid doesn't exist, name is invalid
            {
                std::cout << "A Grid with that name could not be found." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });

        newName = ReadValue<std::string>("New name (max 25 characters): ", [gn](const std::string& name)
        {
            if (!_namePredicate(name))
                return false;

            try
            {
                gn->GetGrid(name);
            }
            catch (UnknownGrid&) // grid doesn't already exist, name is valid
            {
                return true;
            }

            std::cout << "A Grid with the same name already exists." << std::endl << "Please try again." << std::endl;
            return false;
        });
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("New Grid");
    }

    gn->ChangeGridName(name, newName);

    PauseConsole();
    ClearConsole();
}

void ChangeGridTopic(GridNetwork* gn)
{
    std::string name, newTopic;

    try
    {
        name = ReadValue<std::string>("Name (max 25 characters): ", [gn](const std::string& name)
        {
            if (!_namePredicate(name))
                return false;

            try
            {
                gn->GetGrid(name);
            }
            catch (UnknownGrid&) // grid doesn't exist, name is invalid
            {
                std::cout << "A Grid with that name could not be found." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });

        newTopic = ReadValue<std::string>("New topic (max 25 characters): ", _namePredicate);
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("New Grid");
    }

    gn->ChangeGridTopic(name, newTopic);

    PauseConsole();
    ClearConsole();
}

void RemoveGrid(GridNetwork* gn)
{
    std::string name;

    try
    {
        name = ReadValue<std::string>("Name (max 25 characters): ", [gn](const std::string& name)
        {
            if (!_namePredicate(name))
                return false;

            try
            {
                gn->GetGrid(name);
            }
            catch (UnknownGrid&) // grid doesn't exist, name is invalid
            {
                std::cout << "A Grid with that name could not be found." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });

        char sure = ReadValue<char>("Are you sure you want to remove " + name + " and all related resources? [Y/N] ");

        if (sure != 'Y' && sure != 'y')
        {
            std::cout << "Action canceled." << std::endl;
            return;
        }
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("New Grid");
    }

    gn->RemoveGrid(name);

    PauseConsole();
    ClearConsole();
}

void SearchGrids(GridNetwork* gn)
{
    enum GridSearchOption
    {
        None = 0,
        ByName = 1,
        ByTopic = 2,
        All = 3
    };

    if (gn->GetContainer().empty())
        throw std::runtime_error("There are no Grids in the GridNetwork.");

    static Menu* searchMenu = Loader<Menu>("gridSearchMenu.txt").Load();

    uint option = searchMenu->Print();

    std::vector<const Grid*> vec;

    switch (option)
    {
        case None:
        {
            throw ActionCanceled("Search Grids");
        }
        case ByName:
        {
            std::string name = ReadValue<std::string>("Exact Name (max 25 characters): ", _namePredicate);
            vec = gn->ApplyPredicate([name](const Grid* grid) { return grid->GetName() == name; });
            break;
        }
        case ByTopic:
        {
            std::string topic = ReadValue<std::string>("Exact Topic (max 25 characters): ", _namePredicate);
            vec = gn->ApplyPredicate([topic](const Grid* grid) { return grid->GetTopic() == topic; });
            break;
        }
        case All:
        {
            vec = gn->ApplyPredicate([](const Grid*) { return true; });
            break;
        }
    }

    if (vec.empty())
        std::cout << "No results." << std::endl;
    else
    {
        Grid::PrintHeader();
        for (auto g : gn->GetContainer())
            g.second->Print();
    }

    PauseConsole();
    ClearConsole();
}


void NewAcademicUser(GridManager* gm)
{
    std::string name;
    try
    {
        name = ReadValue<std::string>("Name (max 25 characters): ", _namePredicate);
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("New Academic User");
    }

    User* user = new AcademicUser(name);
    uint id = gm->AddUser(user);

    std::cout << "Academic user created, assigned id " << id << "." << std::endl;

    PauseConsole();
    ClearConsole();
}

void NewEnterpriseUser(GridManager* gm)
{
    std::string name;
    double budget;
    try
    {
        name = ReadValue<std::string>("Name (max 25 characters): ", _namePredicate);

        budget = ReadValue<double>("Budget [0-99999](€): ", [](double val)
        {
            if (val < 0)
            {
                std::cout << "Budget cannot be negative." << std::endl << "Please try again." << std::endl;
                return false;
            }
            else if (val > 99999)
            {
                std::cout << "Budget cannot be greater than 99999€." << std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        });
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("New Enterprise User");
    }

    User* user = new EnterpriseUser(name, budget);
    uint id = gm->AddUser(user);

    std::cout << "Enterprise user created, assigned id " << id << "." << std::endl;

    PauseConsole();
    ClearConsole();
}

void RemoveUser(GridManager* gm)
{
    uint id;

    if (gm->GetNumberOfUsers() == 0)
        throw std::runtime_error("There are no users in the GridManager.");

    try
    {
        id = ReadValue<uint>("Id (0 - Show list): ", [gm](uint val)
        {
            if (val == 0)
            {
                User::PrintHeader(std::cout);
                for (auto m: gm->ApplyPredicate<User>([](const User*) { return true; }))
                    m->Print(std::cout);
                return false;
            }
            else if (!gm->GetUser(val))
            {
                std::cout << "Id """ << val << """ is not currently in use."<< std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        });
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Remove User");
    }

    if (gm->RemoveUser(id))
        std::cout << "User with id " << id << " was removed." << std::endl;
    else
        std::cout << "Could not remove user with id " << id << "." << std::endl;

    PauseConsole();
    ClearConsole();
}

void NewMachine(GridManager* gm)
{
    std::string name;
    uint maxJobs;
    double totalRAM;
    double totalDiskSpace;
    std::vector<Software> software;

    try
    {
        name = ReadValue<std::string>("Name (max 25 characters): ", _namePredicate);
        maxJobs = ReadValue<uint>("Max number of jobs [0-9999]: ", [](uint val)
        {
            if (val > 9999)
            {
                std::cout << "Max number of Jobs cannot be greater than 9999." << std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        });

        totalRAM = ReadValue<double>("Amount of RAM [0-99999](MB): ", [](double val)
        {
            if (val < 0)
            {
                std::cout << "Amount of RAM cannot be negative." << std::endl << "Please try again." << std::endl;
                return false;
            }
            else if (val > 99999)
            {
                std::cout << "Amount of RAM cannot be greater than 99999 MB." << std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        });

        totalDiskSpace = ReadValue<double>("Amount of disk space [0-99999](MB): ", [](double val)
        {
            if (val < 0)
            {
                std::cout << "Amount of disk space cannot be negative." << std::endl << "Please try again." << std::endl;
                return false;
            }
            else if (val > 99999)
            {
                std::cout << "Amount of disk space cannot be greater than 99999 MB." << std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        });

        std::cout << "Available software (-1 to end list):  " << std::endl;
        std::cout << "Format: \"name major.minor.revision\""  << std::endl;
        for (int i = 1; i <= 100; ++i)
        {
            char temp[3];
            sprintf_s(temp, "%i", i);

            std::string prompt = "\t" + std::string(temp) + " - ";

            const std::string input = ReadValue<std::string>(prompt);

            if (input == "-1")
                break;

            std::tuple<bool, Software> sw = Software::ReadFromString(input);

            if (std::get<0>(sw))
                software.push_back(std::get<1>(sw));
            else
            {
                std::cout << "Software needs to be in the format \"name major.minor.revision\". Try again." << std::endl;
                --i;
            }
        }
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("New Machine");
    }

    Machine* machine = new Machine(name, maxJobs, totalRAM, totalDiskSpace);

    std::for_each(software.begin(), software.end(), [&machine](const Software& sw) { machine->AddAvailableSoftware(sw); });

    uint id = gm->AddMachine(machine);

    std::cout << "Machine created, assigned id " << id << "." << std::endl;

    PauseConsole();
    ClearConsole();
}

void RemoveMachine(GridManager* gm)
{
    uint id;

    if (gm->GetNumberOfMachines() == 0)
        throw std::runtime_error("There are no machines in the GridManager.");

    try
    {
        id = ReadValue<uint>("Id (0 - Show list): ", [gm](uint val)
        {
            if (val == 0)
            {
                Machine::PrintHeader(std::cout);
                for (auto m: gm->ApplyPredicate<Machine>([](const Machine*) { return true; }))
                    m->Print(std::cout);
                return false;
            }
            else if (!gm->GetMachine(val))
            {
                std::cout << "Id """ << val << """ is not currently in use."<< std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        });
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Remove Machine");
    }

    if (gm->RemoveMachine(id))
        std::cout << "Machine with id " << id << " was removed." << std::endl;
    else
        std::cout << "Could not remove machine with id " << id << "." << std::endl;

    PauseConsole();
    ClearConsole();
}

void NewJob(GridManager* gm)
{
    uint userId;
    std::string name;
    uint8 priority;
    double requiredRAM;
    double requiredDiskSpace;
    uint executionTime;
    std::vector<Software> software;

    if (gm->GetNumberOfMachines() == 0)
        throw std::runtime_error("There are no machines in the GridManager.");

    if (gm->GetNumberOfUsers() == 0)
        throw std::runtime_error("There are no users in the GridManager.");

    try
    {
        userId = ReadValue<uint>("User Id (0 - Show list): ", [gm](uint val)
        {
            if (val == 0)
            {
                User::PrintHeader(std::cout);
                for (auto m: gm->ApplyPredicate<User>([](const User*) { return true; }))
                    m->Print(std::cout);
                return false;
            }
            else if (!gm->GetUser(val))
            {
                std::cout << "Id """ << val << """ is not currently in use."<< std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        });

        name = ReadValue<std::string>("Name (max 25 characters): ", _namePredicate);

        priority = static_cast<uint8>(ReadValue<uint16>("Priority [0-100](%): ", [](uint16 val)
        {
            if (val < 0 || val > 100)
            {
                std::cout << "Priority must be between 0 % and 100 %."<< std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        }));

        requiredRAM = ReadValue<double>("Required RAM usage [0-99999](MB): ", [](double val)
        {
            if (val < 0)
            {
                std::cout << "Required RAM cannot be negative." << std::endl << "Please try again." << std::endl;
                return false;
            }
            else if (val > 99999)
            {
                std::cout << "Required RAM cannot be greater than 99999 MB." << std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        });

        requiredDiskSpace = ReadValue<double>("Required disk space usage [0-99999](MB): ", [](double val)
        {
            if (val < 0)
            {
                std::cout << "Required disk space cannot be negative." << std::endl << "Please try again." << std::endl;
                return false;
            }
            else if (val > 99999)
            {
                std::cout << "Required disk space cannot be greater than 99999 MB." << std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        });

        executionTime = ReadValue<uint>("Estimated execution time [0-99999](s): ", [](uint val)
        {
            if (val > 99999)
            {
                std::cout << "Estimated execution time cannot be greater than 99999 s." << std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        });
        std::cout << "Required software (-1 to end list):  " << std::endl;

        // duplicated code with NewMachine()
        for (int i = 1; i <= 100; ++i)
        {
            char temp[3];
            sprintf_s(temp, "%i", i);

            std::string prompt = "\t" + std::string(temp) + " - ";

            const std::string input = ReadValue<std::string>(prompt);

            if (input == "-1")
                break;

            std::tuple<bool, Software> sw = Software::ReadFromString(input);

            if (std::get<0>(sw))
                software.push_back(std::get<1>(sw));
            else
            {
                std::cout << "Software needs to be in the format \"name major.minor.revision\". Try again." << std::endl;
                --i;
            }
        }

    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("New Job");
    }

    Job* job = new Job(name, priority, requiredRAM, requiredDiskSpace, executionTime);

    std::for_each(software.begin(), software.end(), [&job](const Software& sw) { job->AddRequiredSoftware(sw); });

    if (gm->AddJobByUser(userId, job))
        std::cout << "Job added successfully." << std::endl;
    else
        std::cout << "Could not create job because there are no available machines that meet the needed requirements for this job right now." << std::endl;

    PauseConsole();
    ClearConsole();
}

void SearchUsers(GridManager* gm)
{
    enum UserSearchOption
    {
        ByName = 1,
        AllAcademic = 2,
        AllEnterprise = 3,
        All = 4
    };

    if (gm->GetNumberOfUsers() == 0)
        throw std::runtime_error("There are no users in the GridManager.");

    static Menu* searchMenu = Loader<Menu>("userSearchMenu.txt").Load();

    uint option = searchMenu->Print();

    std::vector<const User*> vec;

    switch (option)
    {
        case 0:
        {
            throw ActionCanceled("Search Users");
        }
        case ByName:
        {
            std::string name = ReadValue<std::string>("Exact Name (max 25 characters): ", _namePredicate);

            vec = gm->ApplyPredicate<User>([name](const User* user) { return user->GetName() == name; });
            break;
        }
        case AllAcademic:
        {
            vec = gm->ApplyPredicate<User>([](const User* user) { return typeid(*user) == typeid(AcademicUser); });
            break;
        }
        case AllEnterprise:
        {
            vec = gm->ApplyPredicate<User>([](const User* user) { return typeid(*user) == typeid(EnterpriseUser); });
            break;
        }
        case All:
        {
            vec = gm->ApplyPredicate<User>([](const User*) { return true; });
            break;
        }
    }

    if (vec.size() == 0)
        std::cout << "No results." << std::endl;
    else
    {
        User::PrintHeader();
        for (const User* user : vec)
            user->Print();
    }

    PauseConsole();
    ClearConsole();
}

void SearchMachines(GridManager* gm)
{
    enum MachineSearchOption
    {
        ByName = 1,
        ByRAM = 2,
        ByDisk = 3,
        ByNumberJobs = 4,
        All = 5
    };

    if (gm->GetNumberOfMachines() == 0)
        throw std::runtime_error("There are no machines in the GridManager.");

    static Menu* searchMenu = Loader<Menu>("machineSearchMenu.txt").Load();

    uint option = searchMenu->Print();

    std::vector<const Machine*> vec;

    try
    {
        switch (option)
        {
            case 0:
            {
                throw ActionCanceled("Search Machines");
            }
            case ByName:
            {
                std::string name = ReadValue<std::string>("Exact Name (max 25 characters): ", _namePredicate);

                vec = gm->ApplyPredicate<Machine>([name](const Machine* machine) { return machine->GetName() == name; });
                break;
            }
            case ByRAM:
            {
                char comp = ReadValue<char>("Comparison ( > or < or = ): ");
                double value = ReadValue<double>("Value [0-99999](MB): ");

                std::function<bool(const Machine*)> func;

                switch (comp)
                {
                    case '>':
                    {
                        func = [value](const Machine* machine) { return machine->GetAvailableRAM() > value; };
                        break;
                    }
                    case '<':
                    {
                        func = [value](const Machine* machine) { return machine->GetAvailableRAM() < value; };
                        break;
                    }
                    default:
                    {
                        std::cout << "Invalid comparison, using equality." << std::endl;
                        // no break intended
                    }
                    case '=':
                    {
                        func = [value](const Machine* machine) { return machine->GetAvailableRAM() == value; };
                        break;
                    }
                }

                vec = gm->ApplyPredicate(func);
                break;
            }
            case ByDisk:
            {
                char comp = ReadValue<char>("Comparison ( > or < or = ): ");
                double value = ReadValue<double>("Value [0-99999](MB): ");

                std::function<bool(const Machine*)> func;

                switch (comp)
                {
                    case '>':
                    {
                        func = [value](const Machine* machine) { return machine->GetAvailableDiskSpace() > value; };
                        break;
                    }
                    case '<':
                    {
                        func = [value](const Machine* machine) { return machine->GetAvailableDiskSpace() < value; };
                        break;
                    }
                    default:
                    {
                        std::cout << "Invalid comparison, using equality." << std::endl;
                        // no break intended
                    }
                    case '=':
                    {
                        func = [value](const Machine* machine) { return machine->GetAvailableDiskSpace() == value; };
                        break;
                    }
                }

                vec = gm->ApplyPredicate(func);
                break;
            }
            case ByNumberJobs:
            {
                char comp = ReadValue<char>("Comparison ( > or < or = ): ");
                uint value = ReadValue<uint>("Value [0-9999]: ");

                std::function<bool(const Machine*)> func;

                switch (comp)
                {
                    case '>':
                    {
                        func = [value](const Machine* machine) { return machine->GetNumberOfCurrentJobs() > value; };
                        break;
                    }
                    case '<':
                    {
                        func = [value](const Machine* machine) { return machine->GetNumberOfCurrentJobs() < value; };
                        break;
                    }
                    default:
                    {
                        std::cout << "Invalid comparison, using equality." << std::endl;
                        // no break intended
                    }
                    case '=':
                    {
                        func = [value](const Machine* machine) { return machine->GetNumberOfCurrentJobs() == value; };
                        break;
                    }
                }

                vec = gm->ApplyPredicate(func);
                break;
            }
            default:
            {
                std::cout << "You need to provide an option between 1 and 5, showing all machines." << std::endl;
                // no break intended
            }
            case All:
            {
                vec = gm->ApplyPredicate<Machine>([](const Machine*) { return true; });
                break;
            }
        }
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Search Machines");
    }

    if (vec.size() == 0)
        std::cout << "No results." << std::endl;
    else
    {
        Machine::PrintHeader(std::cout);
        for (const Machine* machine : vec)
            machine->Print(std::cout);
    }

    PauseConsole();
    ClearConsole();
}

void SearchJobs(GridManager* gm)
{
    enum JobSearchOption
    {
        None = 0,
        ByName = 1,
        ByRAM = 2,
        ByDisk = 3,
        ByPriority = 4,
        ByElapsedTime = 5,
        All = 6
    };

    if (gm->GetNumberOfJobs() == 0)
        throw std::runtime_error("There are no jobs executing.");

    std::vector<const Job*> vec;

    static Menu* searchMenu = Loader<Menu>("jobSearchMenu.txt").Load();

    uint option = searchMenu->Print();

    try
    {
        switch (option)
        {
            case None:
            {
                throw ActionCanceled("Search Machines");
            }
            case ByName:
            {
                std::string name = ReadValue<std::string>("Exact Name (max 25 characters): ", _namePredicate);

                vec = gm->ApplyPredicate<Job>([name](const Job* job) { return job->GetName() == name; });
                break;
            }
            case ByRAM:
            {
                char comp = ReadValue<char>("Comparison ( > or < or = ): ");
                double value = ReadValue<double>("Value [0-99999](MB): ");

                std::function<bool(const Job*)> func;

                switch (comp)
                {
                    case '>':
                    {
                        func = [value](const Job* job) { return job->GetRequiredRAM() > value; };
                        break;
                    }
                    case '<':
                    {
                        func = [value](const Job* job) { return job->GetRequiredRAM() < value; };
                        break;
                    }
                    default:
                    {
                        std::cout << "Invalid comparison, using equality." << std::endl;
                        // no break intended
                    }
                    case '=':
                    {
                        func = [value](const Job* job) { return job->GetRequiredRAM() == value; };
                        break;
                    }
                }

                vec = gm->ApplyPredicate(func);
                break;
            }
            case ByDisk:
            {
                char comp = ReadValue<char>("Comparison ( > or < or = ): ");
                double value = ReadValue<double>("Value [0-99999](MB): ");

                std::function<bool(const Job*)> func;

                switch (comp)
                {
                    case '>':
                    {
                        func = [value](const Job* job) { return job->GetRequiredDiskSpace() > value; };
                        break;
                    }
                    case '<':
                    {
                        func = [value](const Job* job) { return job->GetRequiredDiskSpace() < value; };
                        break;
                    }
                    default:
                    {
                        std::cout << "Invalid comparison, using equality." << std::endl;
                        // no break intended
                    }
                    case '=':
                    {
                        func = [value](const Job* job) { return job->GetRequiredDiskSpace() == value; };
                        break;
                    }
                }

                vec = gm->ApplyPredicate(func);
                break;
            }
            case ByPriority:
            {
                char comp = ReadValue<char>("Comparison ( > or < or = ): ");
                uint8 value = ReadValue<uint8>("Value [0-100](%): ");

                std::function<bool(const Job*)> func;

                switch (comp)
                {
                    case '>':
                    {
                        func = [value](const Job* job) { return job->GetPriority() > value; };
                        break;
                    }
                    case '<':
                    {
                        func = [value](const Job* job) { return job->GetPriority() < value; };
                        break;
                    }
                    default:
                    {
                        std::cout << "Invalid comparison, using equality." << std::endl;
                        // no break intended
                    }
                    case '=':
                    {
                        func = [value](const Job* job) { return job->GetPriority() == value; };
                        break;
                    }
                }

                vec = gm->ApplyPredicate(func);
                break;
            }
            case ByElapsedTime:
            {
                char comp = ReadValue<char>("Comparison ( > or < or = ): ");
                uint value = ReadValue<uint>("Value [0-99999](s): ");

                std::function<bool(const Job*)> func;

                switch (comp)
                {
                    case '>':
                    {
                        func = [value](const Job* job) { return job->GetElapsedTime() > value; };
                        break;
                    }
                    case '<':
                    {
                        func = [value](const Job* job) { return job->GetElapsedTime() < value; };
                        break;
                    }
                    default:
                    {
                        std::cout << "Invalid comparison, using equality." << std::endl;
                        // no break intended
                    }
                    case '=':
                    {
                        func = [value](const Job* job) { return job->GetElapsedTime() == value; };
                        break;
                    }
                }

                vec = gm->ApplyPredicate(func);
                break;
            }
            case All:
            {
                vec = gm->ApplyPredicate<Job>([](const Job*) { return true; });
                break;
            }
        }
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Search Jobs");
    }
    catch (ParentNULL)
    {
        throw ActionCanceled("Search Jobs");
    }

    if (vec.size() == 0)
        std::cout << "No results." << std::endl;
    else
    {
        Job::PrintHeader();
        for (const Job* job : vec)
            job->Print();
    }

    PauseConsole();
    ClearConsole();
}

void ChangeUserInfo(GridManager* gm)
{
    std::cout << "Change User Information" << std::endl;

    User* user;

    try
    {
        user = gm->GetUser(ReadValue<uint>("Id (0 - Show list): ", [gm](uint val)
        {
            if (val == 0)
            {
                User::PrintHeader(std::cout);
                for (auto m: gm->ApplyPredicate<User>([](const User*) { return true; }))
                    m->Print(std::cout);
                return false;
            }
            else if (!gm->GetUser(val))
            {
                std::cout << "Id """ << val << """ is not currently in use."<< std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        }));
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Change User Information");
    }

    ClearConsole();

    bool success = false;

    try
    {
        do
        {
            std::cout << "User - Id: " << user->GetId() << " Name: " << user->GetName() << std::endl;
            uint32 option = User::GetMenu()->Print();

            switch (option)
            {
                case 0:
                {
                    throw EOFCharacterValue();
                }
                case 1:
                {
                    std::string val = ReadValue<std::string>("New name (max 25 characters): ", _namePredicate);

                    user->SetName(val);
                    std::cout << "User name changed with success." << std::endl;
                    success = true;
                    break;
                }
                case 2:
                {
                    if (typeid(*user) == typeid(EnterpriseUser))
                    {
                        double val = ReadValue<double>("New budget [0-99999](€): ", [](double val)
                        {
                            if (val < 0)
                            {
                                std::cout << "Budget cannot be negative." << std::endl << "Please try again." << std::endl;
                                return false;
                            }
                            else if (val > 99999)
                            {
                                std::cout << "Budget cannot be greater than 99999€." << std::endl << "Please try again." << std::endl;
                                return false;
                            }
                            return true;
                        });
                        ((EnterpriseUser*)user)->SetBudget(val);
                        std::cout << "Enterprise user budget changed with success." << std::endl;
                        success = true;
                    }
                    else
                    {
                        std::cout << "This action is not available to academic users." << std::endl;
                    }
                    break;
                }
            }
        } while (!success);
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Change User Information");
    }
}

void ChangeMachineInfo(GridManager* gm)
{
    std::cout << "Change Machine Information" << std::endl;

    Machine* machine;

    try
    {
        machine = gm->GetMachine(ReadValue<uint>("Id (0 - Show list): ", [gm](uint val)
        {
            if (val == 0)
            {
                Machine::PrintHeader(std::cout);
                for (auto m: gm->ApplyPredicate<Machine>([](const Machine*) { return true; }))
                    m->Print(std::cout);
                return false;
            }
            else if (!gm->GetMachine(val))
            {
                std::cout << "Id """ << val << """ is not currently in use."<< std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        }));
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Change Machine Information");
    }

    ClearConsole();

    bool success = false;

    try
    {
        do
        {
            std::cout << "Machine - Id: " << machine->GetId() << " Name: " << machine->GetName() << std::endl;
            uint32 option = Machine::GetMenu()->Print();

            switch (option)
            {
                case 0:
                {
                    throw EOFCharacterValue();
                }
                case 1: // Change Machine Name
                {
                    std::string val = ReadValue<std::string>("New name (max 25 characters): ", _namePredicate);
                    machine->SetName(val);
                    std::cout << "Machine name changed with success." << std::endl;
                    success = true;
                    break;
                }
                case 2: // Change Machine Ram
                {
                    double ram = ReadValue<double>("New RAM [0-99999](MB): ", [](double val)
                    {
                        if (val < 0)
                        {
                            std::cout << "Amount of RAM cannot be negative." << std::endl << "Please try again." << std::endl;
                            return false;
                        }
                        else if (val > 99999)
                        {
                            std::cout << "Amount of RAM cannot be greater than 99999 MB." << std::endl << "Please try again." << std::endl;
                            return false;
                        }
                        return true;
                    });

                    try
                    {
                        machine->SetTotalRAM(ram);
                    }
                    catch (MachineInExecution& err)
                    {
                        Machine* m = gm->GetMachine(err.GetMachineId());
                        std::cout << "Unable to modify RAM in machine " << m->GetName() << " (id:" << m->GetId() << ") because it is executing." << std::endl;
                        PauseConsole();
                        ClearConsole();
                        return;
                    }

                    std::cout << "Machine RAM changed with success." << std::endl;
                    success = true;
                    break;
                }
                case 3: // Change Machine Disk Space
                {
                    double diskSpace = ReadValue<double>("New Disk Space [0-99999](MB): ", [](double val)
                    {
                        if (val < 0)
                        {
                            std::cout << "Amount of disk space cannot be negative." << std::endl << "Please try again." << std::endl;
                            return false;
                        }
                        else if (val > 99999)
                        {
                            std::cout << "Amount of disk space cannot be greater than 99999 MB." << std::endl << "Please try again." << std::endl;
                            return false;
                        }
                        return true;
                    });
                    try
                    {
                        machine->SetTotalDiskSpace(diskSpace);
                    }
                    catch (MachineInExecution& err)
                    {
                        Machine* m = gm->GetMachine(err.GetMachineId());
                        std::cout << "Unable to modify disk space in machine " << m->GetName() << " (id:" << m->GetId() << ") because it is executing." << std::endl;
                        PauseConsole();
                        ClearConsole();
                        return;
                    }
                    std::cout << "Machine disk space changed with success." << std::endl;
                    success = true;
                    break;
                }
                case 4: // Change Machine Max Jobs
                {
                    uint maxJobs = ReadValue<uint>("New Max Jobs [0-9999]: ", [](uint val)
                    {
                        if (val > 9999)
                        {
                            std::cout << "Max number of Jobs cannot be greater than 9999." << std::endl << "Please try again." << std::endl;
                            return false;
                        }
                        return true;
                    });

                    try
                    {
                        machine->SetMaxJobs(maxJobs);
                    }
                    catch (MachineInExecution& err)
                    {
                        Machine* m = gm->GetMachine(err.GetMachineId());
                        std::cout << "Unable to modify max jobs in machine " << m->GetName() << " (id:" << m->GetId() << ") because it is executing." << std::endl;
                        PauseConsole();
                        ClearConsole();
                        return;
                    }

                    std::cout << "Machine max number of jobs changed with success." << std::endl;
                    success = true;
                    break;
                }
                case 5: // Add Software
                {
                    bool successSW = false;

                    while (!successSW)
                    {
                        auto sw = Software::ReadFromString(ReadValue<std::string>("Software [name major.minor.revision]: "));

                        if (std::get<0>(sw))
                        {
                            machine->AddAvailableSoftware(std::get<1>(sw));
                            successSW = true;
                        }
                        else
                            std::cout << "You need to provide a valid software in the format \"name major.minor.revision\". Try again." << std::endl;

                    }

                    std::cout << "Machine software added with success." << std::endl;
                    success = true;
                    break;
                }
                case 6: // Remove Software
                {
                    bool successSW = false;

                    while (!successSW)
                    {
                        auto sw = Software::ReadFromString(ReadValue<std::string>("Software [name major.minor.revision]: "));

                        if (std::get<0>(sw))
                        {
                            machine->RemoveAvailableSoftware(std::get<1>(sw));
                            successSW = true;
                        }
                        else
                            std::cout << "You need to provide a valid software in the format \"name major.minor.revision\". Try again." << std::endl;
                    }

                    std::cout << "Machine software removed with success." << std::endl;
                    success = true;
                    break;
                }
                case 7: // List Available Software
                {
                    if (machine->GetAvailableSoftware().size() == 0)
                        std::cout << "This machine doesn't have any software installed." << std::endl;
                    else
                    {
                        Software::PrintHeader(std::cout);
                        for (auto sw : machine->GetAvailableSoftware())
                            sw.Print(std::cout);
                    }

                    success = true;
                    break;
                }
                case 8: // List Machine Jobs
                {
                    if (machine->GetNumberOfCurrentJobs() == 0)
                        std::cout << "No jobs on this machine." << std::endl;
                    else
                    {
                        Job::PrintHeader(std::cout, true);
                        for (auto elem : machine->GetJobs())
                        {
                            std::cout << "| " << std::setfill('0') << std::setw(4) << std::right << elem << " ";
                            elem->PrintWithId(std::cout);
                        }
                    }

                    success = true;
                    break;
                }
                case 9: // Remove Job From Machine
                {
                    uint JobId = ReadValue<uint>("Id (0 - Show list): ", [machine](uint val)
                    {
                        if (val == 0)
                        {
                            Job::PrintHeader(std::cout, true);
                            for (auto elem : machine->GetJobs())
                            {
                                std::cout << "| " << std::setfill('0') << std::setw(4) << std::right << elem << " ";
                                elem->PrintWithId(std::cout);
                            }
                            return false;
                        }
                        else if (!machine->GetJob(val))
                        {
                            std::cout << "Id """ << val << """ is not currently in use."<< std::endl << "Please try again." << std::endl;
                            return false;
                        }
                        return true;
                    });

                    if (machine->RemoveJob(JobId))
                        std::cout << "Job removed with success." << std::endl;
                    else
                        std::cout << "Error removing job." << std::endl;

                    success = true;
                    break;
                }
                case 10: // Cancel All Jobs
                {
                    machine->RemoveAllJobs();
                    std::cout << "Jobs removed with success." << std::endl;

                    success = true;
                    break;
                }
                case 11: // List Job Required Software
                {
                    const Job* job = machine->GetJob(ReadValue<uint>("Id (0 - Show list): ", [machine](uint val)
                    {
                        if (val == 0)
                        {
                            Job::PrintHeader(std::cout, true);
                            for (auto elem : machine->GetJobs())
                            {
                                std::cout << "| " << std::setfill('0') << std::setw(4) << std::right << elem << " ";
                                elem->PrintWithId(std::cout);
                            }
                            return false;
                        }
                        else if (!machine->GetJob(val))
                        {
                            std::cout << "Id """ << val << """ is not currently in use."<< std::endl << "Please try again." << std::endl;
                            return false;
                        }
                        return true;
                    }));

                    if (job->GetRequiredSoftware().size() == 0)
                        std::cout << "This job doesn't require any software to execute." << std::endl;
                    else
                    {
                        Software::PrintHeader(std::cout);
                        for (auto sw : job->GetRequiredSoftware())
                            sw.Print(std::cout);
                    }

                    success = true;
                    break;
                }
            }
        } while (!success);
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Change Machine Information");
    }

    PauseConsole();
    ClearConsole();
}

void ChangeGridManagerInfo(GridNetwork* gn)
{
    std::cout << "Change GridManager Information" << std::endl;

    GridManager* gm;

    try
    {
        gm = gn->GetGrid(ReadValue<std::string>("Name (max 25 characters, 0 - Show list): ", [gn](const std::string& name)
        {
            if (name == "0")
            {
                Grid::PrintHeader();

                for (auto g : gn->GetContainer())
                    g.second->Print();
                return false;
            }

            try
            {
                gn->GetGrid(name);
            }
            catch (const UnknownGrid&)
            {
                std::cout << "A Grid with that name could not be found." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        }))->GetGridManager();
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Change GridManager Information");
    }

    ClearConsole();

    bool success = false;

    try
    {
        do
        {
            static std::function<void(GridManager*)> functions[] =
            {
                [&success](GridManager*) { success = true; },  // 1 - exit
                NewAcademicUser,                               // 2
                NewEnterpriseUser,                             // 3
                RemoveUser,                                    // 4
                NewMachine,                                    // 5
                RemoveMachine,                                 // 6
                NewJob,                                        // 7
                SearchUsers,                                   // 8
                SearchMachines,                                // 9
                SearchJobs,                                    //10
                ChangeUserInfo,                                //11
                ChangeMachineInfo                              //12
            };

            uint32 option = GridManager::GetMenu()->Print();
            functions[option == 0 ? 0 : option - 1](gm);

        } while (!success);
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Change GridManager Information");
    }
}
