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
        name = ReadValue<std::string>("Name: ",[](std::string val) 
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
        });
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
        name = ReadValue<std::string>("Name: ",[](std::string val) 
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
        });

        budget = ReadValue<double>("Budget: ",[](double val) 
        { 
            if (val < 0) 
            {
                std::cout << "Budget cannot be negative." << std::endl << "Please try again." << std::endl;
                return false;
            }
            else if (val > 99999)
            {
                std::cout << "Budget cannot be greater than 99999 €." << std::endl << "Please try again." << std::endl;
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
        id = ReadValue<uint>("Id: ",[gm](uint val) 
        { 
            if (!gm->GetUser(val)) 
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
        name = ReadValue<std::string>("Name: ",[](std::string val) 
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
        });
        maxJobs = ReadValue<uint>("Max number of jobs: ",[](uint val)
        {
            if (val > 9999)
            {
                std::cout << "Max number of Jobs cannot be greater than 9999." << std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        });
        totalRAM = ReadValue<double>("Amount of RAM: ",[](double val) 
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
        totalDiskSpace = ReadValue<double>("Amount of disk space: ",[](double val) 
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

    if (gm->GetNumberOfMachines() == 0)
        throw std::runtime_error("There are no machines in the GridManager.");

    try
    {
        id = ReadValue<uint>("Id: ",[gm](uint val) 
        { 
            if (!gm->GetMachine(val)) 
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
        userId = ReadValue<uint>("User Id: ",[gm](uint val) 
        { 
            if (!gm->GetUser(val)) 
            {
                std::cout << "Id """ << val << """ is not currently in use."<< std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        });
        name = ReadValue<std::string>("Name: ",[](std::string val) 
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
        });
        priority = static_cast<uint8>(ReadValue<uint16>("Priority (0-100%): ",[](uint16 val)
        {
            if (val < 0 || val > 100)
            {
                std::cout << "Priority must be between 0 % and 100 %."<< std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        }));
        requiredRAM = ReadValue<double>("Required RAM usage (MB): ",[](double val) 
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
        requiredDiskSpace = ReadValue<double>("Required disk space usage (MB): ",[](double val) 
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
        executionTime = ReadValue<uint>("Estimated execution time (s): ",[](uint val)
        {
            if (val > 99999)
            {
                std::cout << "Estimated execution time cannot be greater than 99999 s." << std::endl << "Please try again." << std::endl;
                return false;
            }
            return true;
        });
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

    if (gm->GetNumberOfUsers() == 0)
        throw std::runtime_error("There are no users in the GridManager.");

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
            std::string name = ReadValue<std::string>("Name: ",[](std::string val) 
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
            });
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
            vec = gm->ApplyPredicate<User>([](User*) { return true; });
            break;
        }
    }

    if (vec.size() == 0)
    {
        std::cout << "No results." << std::endl;
    }
    else
    {
        User::PrintHeader();
        for (User* user : vec)
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
                std::string name = ReadValue<std::string>("Name: ",[](std::string val) 
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
                });
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
    }
    else
    {
        Machine::PrintHeader(std::cout);
        for (Machine* machine : vec)
            machine->Print(std::cout);
    }

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

    if (gm->GetNumberOfJobs() == 0)
        throw std::runtime_error("There are no jobs executing.");

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
                std::string name = ReadValue<std::string>("Name: ",[](std::string val) 
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
                });
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
    }
    else
    {
        Job::PrintHeader(std::cout);
        for (Job* job : vec)
        job->Print(std::cout);
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
        user = gm->GetUser(ReadValue<uint>("Id: ",[gm](uint val) 
        { 
            if (!gm->GetUser(val)) 
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
                    std::string val = ReadValue<std::string>("New name: ",[](std::string val) 
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
                    });
                    user->SetName(val);
                    std::cout << "User name changed with success." << std::endl;
                    success = true;
                    break;
                }

            case 2:
                {
                    if (typeid(user) == typeid(EnterpriseUser*))
                    {
                        double val = ReadValue<double>("New budget: ",[](double val) 
                        { 
                            if (val < 0) 
                            {
                                std::cout << "Budget cannot be negative." << std::endl << "Please try again." << std::endl;
                                return false;
                            }
                            else if (val > 99999)
                            {
                                std::cout << "Budget cannot be greater than 99999 €." << std::endl << "Please try again." << std::endl;
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
        machine = gm->GetMachine(ReadValue<uint>("Id: ",[gm](uint val) 
        { 
            if (!gm->GetMachine(val)) 
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
                    std::string val = ReadValue<std::string>("New name: ",[](std::string val) 
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
                    });
                    machine->SetName(val);
                    std::cout << "Machine name changed with success." << std::endl;
                    success = true;
                    break;
                }

            case 2: // Change Machine Ram
                {
                    double ram = ReadValue<double>("New RAM: ",[](double val) 
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
                        std::cout << "Unable to modify RAM in machine " << err.GetMachine()->GetName() << " (id:" << err.GetMachine()->GetId() << ") because it is executing." << std::endl;
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
                    double diskSpace = ReadValue<double>("New Disk Space: ",[](double val) 
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
                        std::cout << "Unable to modify disk space in machine " << err.GetMachine()->GetName() << " (id:" << err.GetMachine()->GetId() << ") because it is executing." << std::endl;
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
                    uint maxJobs = ReadValue<uint>("New Max Jobs: ",[](uint val)
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
                        std::cout << "Unable to modify max jobs in machine " << err.GetMachine()->GetName() << " (id:" << err.GetMachine()->GetId() << ") because it is executing." << std::endl;
                        PauseConsole();
                        ClearConsole();
                        return;
                    }
                    std::cout << "Machine max number of jobs changed with success." << std::endl;
                    success = true;
                    break;
                }

            case 5: // Change Machine Available Software
                {
                    PauseConsole("Not implemented yet.\n Press enter to continue...");
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

    PauseConsole();
    ClearConsole();
}
