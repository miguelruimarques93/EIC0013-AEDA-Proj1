#include "menugrid.h"
#include "utils.h"
#include "consolereader.h"
#include "user.h"
#include "machine.h"
#include "job.h"
#include "gridmanager.h"
#include <iostream>
#include <typeinfo>

void NewAcademicUser(GridManager* gm)
{
    std::string name = ReadValueStr("Name: ");

    User* user = new AcademicUser(name);
    uint id = gm->AddUser(user);

    std::cout << "Academic user created, assigned id " << id << "." << std::endl;

    PauseConsole();
    ClearConsole();
}

void NewEnterpriseUser(GridManager* gm)
{
    std::string name = ReadValueStr("Name: ");
    double budget = ReadValue<double>("Budget: ");

    User* user = new EnterpriseUser(name, budget);
    uint id = gm->AddUser(user);

    std::cout << "Enterprise user created, assigned id " << id << "." << std::endl;

    PauseConsole();
    ClearConsole();
}

void RemoveUser(GridManager* gm)
{
    uint id = ReadValue<uint>("Id: ");

    if (gm->RemoveUser(id))
        std::cout << "User with id " << id << " was removed." << std::endl;
    else
        std::cout << "Could not remove user with id " << id << "." << std::endl;

    PauseConsole();
    ClearConsole();
}

void NewMachine(GridManager* gm)
{
    std::string name = ReadValueStr("Name: ");
    uint maxJobs = ReadValue<uint>("Max number of jobs: ");
    double totalRAM = ReadValue<double>("Amount of RAM: ");
    double totalDiskSpace = ReadValue<double>("Amount of disk space: ");

    Machine* machine = new Machine(name, maxJobs, totalRAM, totalDiskSpace);
    uint id = gm->AddMachine(machine);

    std::cout << "Machine created, assigned id " << id << "." << std::endl;

    PauseConsole();
    ClearConsole();
}

void RemoveMachine(GridManager* gm)
{
    uint id = ReadValue<uint>("Id: ");

    if (gm->RemoveMachine(id))
        std::cout << "Machine with id " << id << " was removed." << std::endl;
    else
        std::cout << "Could not remove machine with id " << id << "." << std::endl;

    PauseConsole();
    ClearConsole();
}

void NewJob(GridManager* gm)
{
    uint userId = ReadValue<uint>("User Id: ");

    std::string name = ReadValueStr("Name: ");
    uint8 priority = static_cast<uint8>(ReadValue<uint16>("Priority (0-100%): "));
    double requiredRAM = ReadValue<double>("Required RAM usage (MB): ");
    double requiredDiskSpace = ReadValue<double>("Required disk space usage (MB): ");
    uint executionTime = ReadValue<uint>("Estimated execution time (s): ");

    Job* job = new Job(name, priority, requiredRAM, requiredDiskSpace, executionTime);
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

    uint option = ReadValue<uint>("Option (1 - by name, 2 - all academic, 3 - all enterprise, 4 - all users): ");

    std::vector<User*> vec;

    switch (option)
    {
        case ByName:
        {
            std::string name = ReadValueStr("Name: ");
            vec = gm->ApplyPredicate<User>([name](User* user) { return user->GetName() == name; });
            break;
        }
        case AllAcademic:
        {
            vec = gm->ApplyPredicate<User>([](User* user) { return typeid(user) == typeid(AcademicUser); });
            break;
        }
        case AllEnterprise:
        {
            vec = gm->ApplyPredicate<User>([](User* user) { return typeid(user) == typeid(EnterpriseUser); });
            break;
        }
        default:
        {
            std::cout << "You need to provide an option between 1 and 4, showing all users." << std::endl;
            // no break intended
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

    for (User* user : vec)
        user->Print(std::cout);

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

    uint option = ReadValue<uint>("Option (1 - by name, 2 - by available RAM, 3 -by available disk space, 4 - by number of jobs, 5 - all): ");

    std::vector<Machine*> vec;

    switch (option)
    {
        case ByName:
        {
            std::string name = ReadValueStr("Name: ");
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
            std::cout << "You need to provide an option between 1 and 4, showing all users." << std::endl;
            // no break intended
        }
        case All:
        {
            vec = gm->ApplyPredicate<Machine>([](Machine*) { return true; });
            break;
        }
    }

    if (vec.size() == 0)
    {
        std::cout << "No results." << std::endl;
        return;
    }

    for (Machine* machine : vec)
        machine->Print(std::cout);

    PauseConsole();
    ClearConsole();
}

void SearchJobs( GridManager* gm )
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

    uint option = ReadValue<uint>("Option (1 - by name, 2 - by available RAM, 3 -by available disk space, 4 - by priority, 5 - by execution time, 6 - all): ");


    std::vector<Job*> vec;

    switch (option)
    {
        case ByName:
        {
            std::string name = ReadValueStr("Name: ");
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
        default:
        {
            std::cout << "You need to provide an option between 1 and 4, showing all users." << std::endl;
            // no break intended
        }
        case All:
        {
            vec = gm->ApplyPredicate<Job>([](Job*) { return true; });
            break;
        }
    }

    if (vec.size() == 0)
    {
        std::cout << "No results." << std::endl;
        return;
    }

    for (Job* job : vec)
        job->Print(std::cout);  

    PauseConsole();
    ClearConsole();
}
