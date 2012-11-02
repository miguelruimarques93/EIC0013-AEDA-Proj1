#include "menugrid.h"
#include "utils.h"
#include "consolereader.h"
#include "user.h"
#include "machine.h"
#include "job.h"
#include "menu.h"
#include "gridmanager.h"
#include "loader.h"
#include "software.h"

#include <iostream>
#include <typeinfo>

void NewAcademicUser(GridManager* gm)
{
    std::string name;
    try
    {
        name = ReadValue<std::string>("Name: ");
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
        name = ReadValue<std::string>("Name: ");
        budget = ReadValue<double>("Budget: ");
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
    try
    {
        id = ReadValue<uint>("Id: ");
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
        name = ReadValue<std::string>("Name: ");
        maxJobs = ReadValue<uint>("Max number of jobs: ");
        totalRAM = ReadValue<double>("Amount of RAM: ");
        totalDiskSpace = ReadValue<double>("Amount of disk space: ");

        std::cout << "Available software (-1 to end list): " << std::endl;

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

    try
    {
        id = ReadValue<uint>("Id: ");
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

    try
    {
        userId = ReadValue<uint>("User Id: ");
        name = ReadValue<std::string>("Name: ");
        priority = static_cast<uint8>(ReadValue<uint16>("Priority (0-100%): "));
        requiredRAM = ReadValue<double>("Required RAM usage (MB): ");
        requiredDiskSpace = ReadValue<double>("Required disk space usage (MB): ");
        executionTime = ReadValue<uint>("Estimated execution time (s): ");

        std::cout << "Required software (-1 to end list): " << std::endl;
        
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

    std::for_each(software.begin(), software.end(), [&job](const Software& sw) { job->AddRequiredSoftwareSoftware(sw); });

    if (gm->AddJobByUser(userId, job))
        std::cout << "Job added successfully." << std::endl;
    else
        std::cout << "Could not create job." << std::endl; // why not?

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

    static Menu* searchMenu = Loader<Menu>("userSearchMenu.txt").Load();

    uint option = searchMenu->Print();


    std::vector<User*> vec;

    switch (option)
    {
    case 0:
        {
            throw ActionCanceled("Search Users");
        }
    case ByName:
        {
            std::string name = ReadValue<std::string>("Name: ");
            vec = gm->ApplyPredicate<User>([name](User* user) { return user->GetName() == name; });
            break;
        }
    case AllAcademic:
        {
            vec = gm->ApplyPredicate<User>([](User* user) { return typeid(*user) == typeid(AcademicUser); });
            break;
        }
    case AllEnterprise:
        {
            vec = gm->ApplyPredicate<User>([](User* user) { return typeid(*user) == typeid(EnterpriseUser); });
            break;
        }
    case All:
        {
            vec = gm->ApplyPredicate<User>([](User* user) { return true; });
            break;
        }
    }

    if (vec.size() == 0)
    {
        std::cout << "No results." << std::endl;
        return;
    }

    User::PrintHeader();
    for (User* user : vec)
        user->Print();

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

    static Menu* searchMenu = Loader<Menu>("machineSearchMenu.txt").Load();

    uint option = searchMenu->Print();

    std::vector<Machine*> vec;

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
                std::string name = ReadValue<std::string>("Name: ");
                vec = gm->ApplyPredicate<Machine>([name](Machine* machine) { return machine->GetName() == name; });
                break;
            }
        case ByRAM:
            {
                char comp = ReadValue<char>("Comparison ( > or < or = ): ");
                double value = ReadValue<double>("Value: ");

                std::function<bool(Machine*)> func;

                switch (comp)
                {
                case '>':
                    {
                        func = [value](Machine* machine) { return machine->GetAvailableRAM() > value; };
                        break;
                    }
                case '<':
                    {
                        func = [value](Machine* machine) { return machine->GetAvailableRAM() < value; };
                        break;
                    }
                default:
                    {
                        std::cout << "Invalid comparison, using equality." << std::endl;
                        // no break intended
                    }
                case '=':
                    {
                        func = [value](Machine* machine) { return machine->GetAvailableRAM() == value; };
                        break;
                    }
                }

                vec = gm->ApplyPredicate(func);
                break;
            }
        case ByDisk:
            {
                char comp = ReadValue<char>("Comparison ( > or < or = ): ");
                double value = ReadValue<double>("Value: ");

                std::function<bool(Machine*)> func;

                switch (comp)
                {
                case '>':
                    {
                        func = [value](Machine* machine) { return machine->GetAvailableDiskSpace() > value; };
                        break;
                    }
                case '<':
                    {
                        func = [value](Machine* machine) { return machine->GetAvailableDiskSpace() < value; };
                        break;
                    }
                default:
                    {
                        std::cout << "Invalid comparison, using equality." << std::endl;
                        // no break intended
                    }
                case '=':
                    {
                        func = [value](Machine* machine) { return machine->GetAvailableDiskSpace() == value; };
                        break;
                    }
                }

                vec = gm->ApplyPredicate(func);
                break;
            }
        case ByNumberJobs:
            {
                char comp = ReadValue<char>("Comparison ( > or < or = ): ");
                uint value = ReadValue<uint>("Value: ");

                std::function<bool(Machine*)> func;

                switch (comp)
                {
                case '>':
                    {
                        func = [value](Machine* machine) { return machine->GetCurrentJobs() > value; };
                        break;
                    }
                case '<':
                    {
                        func = [value](Machine* machine) { return machine->GetCurrentJobs() < value; };
                        break;
                    }
                default:
                    {
                        std::cout << "Invalid comparison, using equality." << std::endl;
                        // no break intended
                    }
                case '=':
                    {
                        func = [value](Machine* machine) { return machine->GetCurrentJobs() == value; };
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
                vec = gm->ApplyPredicate<Machine>([](Machine*) { return true; });
                break;
            }
        }
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Search Machines");
    }
    if (vec.size() == 0)
    {
        std::cout << "No results." << std::endl;
        return;
    }

    Machine::PrintHeader(std::cout);
    for (Machine* machine : vec)
        machine->Print(std::cout);

    PauseConsole();
    ClearConsole();
}

void SearchJobs(GridManager* gm)
{
    enum JobSearchOption
    {
        ByName = 1,
        ByRAM = 2,
        ByDisk = 3,
        ByPriority = 4,
        ByElapseTime =5,
        All = 6
    };

    std::vector<Job*> vec;


    static Menu* searchMenu = Loader<Menu>("jobSearchMenu.txt").Load();

    uint option = searchMenu->Print();



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
                std::string name = ReadValue<std::string>("Name: ");
                vec = gm->ApplyPredicate<Job>([name](Job* job) { return job->GetName() == name; });
                break;
            }

        case ByRAM:
            {
                char comp = ReadValue<char>("Comparison ( > or < or = ): ");
                double value = ReadValue<double>("Value: ");

                std::function<bool(Job*)> func;

                switch (comp)
                {
                case '>':
                    {
                        func = [value](Job* job) { return job->GetRequiredRAM() > value; };
                        break;
                    }
                case '<':
                    {
                        func = [value](Job* job) { return job->GetRequiredRAM() < value; };
                        break;
                    }
                default:
                    {
                        std::cout << "Invalid comparison, using equality." << std::endl;
                        // no break intended
                    }
                case '=':
                    {
                        func = [value](Job* job) { return job->GetRequiredRAM() == value; };
                        break;
                    }
                }

                vec = gm->ApplyPredicate(func);
                break;
            }
        case ByDisk:
            {
                char comp = ReadValue<char>("Comparison ( > or < or = ): ");
                double value = ReadValue<double>("Value: ");

                std::function<bool(Job*)> func;

                switch (comp)
                {
                case '>':
                    {
                        func = [value](Job* job) { return job->GetRequiredDiskSpace() > value; };
                        break;
                    }
                case '<':
                    {
                        func = [value](Job* job) { return job->GetRequiredDiskSpace() < value; };
                        break;
                    }
                default:
                    {
                        std::cout << "Invalid comparison, using equality." << std::endl;
                        // no break intended
                    }
                case '=':
                    {
                        func = [value](Job* job) { return job->GetRequiredDiskSpace() == value; };
                        break;
                    }
                }

                vec = gm->ApplyPredicate(func);
                break;
            }
        case ByPriority:
            {
                char comp = ReadValue<char>("Comparison ( > or < or = ): ");
                uint8 value = ReadValue<uint8>("Value: ");

                std::function<bool(Job*)> func;

                switch (comp)
                {
                case '>':
                    {
                        func = [value](Job* job) { return job->GetPriority() > value; };
                        break;
                    }
                case '<':
                    {
                        func = [value](Job* job) { return job->GetPriority() < value; };
                        break;
                    }
                default:
                    {
                        std::cout << "Invalid comparison, using equality." << std::endl;
                        // no break intended
                    }
                case '=':
                    {
                        func = [value](Job* job) { return job->GetPriority() == value; };
                        break;
                    }
                }

                vec = gm->ApplyPredicate(func);
                break;
            }
        case ByElapseTime:
            {
                char comp = ReadValue<char>("Comparison ( > or < or = ): ");
                uint value = ReadValue<uint>("Value: ");

                std::function<bool(Job*)> func;

                switch (comp)
                {
                case '>':
                    {
                        func = [value](Job* job) { return job->GetElapsedTime() > value; };
                        break;
                    }
                case '<':
                    {
                        func = [value](Job* job) { return job->GetElapsedTime() < value; };
                        break;
                    }
                default:
                    {
                        std::cout << "Invalid comparison, using equality." << std::endl;
                        // no break intended
                    }
                case '=':
                    {
                        func = [value](Job* job) { return job->GetElapsedTime() == value; };
                        break;
                    }
                }

                vec = gm->ApplyPredicate(func);
                break;
            }
        case All:
            {
                vec = gm->ApplyPredicate<Job>([](Job*) { return true; });
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
    {
        std::cout << "No results." << std::endl;
        return;
    }

    Job::PrintHeader(std::cout);
    for (Job* job : vec)
        job->Print(std::cout);

    PauseConsole();
    ClearConsole();
}

void ChangeUserInfo(GridManager* gm)
{
    std::cout << "Change User Information" << std::endl;

    User* user;


    try
    {
        do
        {
            user = gm->GetUser(ReadValue<uint>("Id: "));
            if (!user)
                std::cout << "User ID doesn't exists." << std::endl << "Please try again..." << std::endl << std::endl;
        } while (!user);
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Change User Information");
    }


    bool success = false;

    try
    {
        do
        {
            uint32 option = User::GetMenu()->Print();

            switch (option)
            {
            case 0:
                {
                    throw EOFCharacterValue();
                }

            case 1:
                {
                    std::string val = ReadValue<std::string>("New name: ");
                    user->SetName(val);
                    success = true;
                    break;
                }

            case 2:
                {
                    if (typeid(user) == typeid(EnterpriseUser*))
                    {
                        double val = ReadValue<double>("New budget: ");
                        ((EnterpriseUser*)user)->SetBudget(val);
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
        do
        {
            machine = gm->GetMachine(ReadValue<uint>("Id: "));
            if (!machine)
                std::cout << "Machine ID doesn't exists." << std::endl << "Please try again..." << std::endl << std::endl;
        } while (!machine);
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Change Machine Information");
    }

    bool success = false;

    try
    {
        do
        {
            uint32 option = Machine::GetMenu()->Print();

            switch (option)
            {
            case 0:
                {
                    throw EOFCharacterValue();
                }

            case 1: // Change Machine Name
                {
                    std::string val = ReadValue<std::string>("New name: ");
                    machine->SetName(val);
                    success = true;
                    break;
                }

            case 2: // Change Machine Ram
                {
                    double ram = ReadValue<double>("New RAM: ");
                    try
                    {
                        machine->SetTotalRAM(ram);
                    }
                    catch (MachineInExecution& err)
                    {
                        std::cout << "Unable to modify RAM in machine " << err.GetMachine()->GetName() << " (id:" << err.GetMachine()->GetId() << ") because it is executing." << std::endl;
                        PauseConsole();
                        ClearConsole();
                        return;
                    }
                    success = true;
                    break;
                }

            case 3: // Change Machine Disk Space
                {
                    double diskSpace = ReadValue<double>("New Disk Space: ");
                    try
                    {
                        machine->SetTotalDiskSpace(diskSpace);
                    }
                    catch (MachineInExecution& err)
                    {
                        std::cout << "Unable to modify disk space in machine " << err.GetMachine()->GetName() << " (id:" << err.GetMachine()->GetId() << ") because it is executing." << std::endl;
                        PauseConsole();
                        ClearConsole();
                        return;
                    }
                    success = true;
                    break;
                }

            case 4: // Change Machine Max Jobs
                {
                    uint maxJobs = ReadValue<uint>("New Max Jobs: ");
                    try
                    {
                        machine->SetMaxJobs(maxJobs);
                    }
                    catch (MachineInExecution& err)
                    {
                        std::cout << "Unable to modify max jobs in machine " << err.GetMachine()->GetName() << " (id:" << err.GetMachine()->GetId() << ") because it is executing." << std::endl;
                        PauseConsole();
                        ClearConsole();
                        return;
                    }
                    success = true;
                    break;
                }

            case 5: // Change Machine Available Software
                {
                    PauseConsole("Not implemented yet.\n Press any key to continue...");
                    ClearConsole();
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

}
