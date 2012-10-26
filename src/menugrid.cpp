#include "menugrid.h"
#include "utils.h"
#include "consolereader.h"
#include "user.h"
#include "machine.h"
#include "job.h"
#include "gridmanager.h"
#include <iostream>

void NewAcademicUser(GridManager* gm)
{
    std::string name = ReadValueStr("Name: ");

    User* user = new AcademicUser(name);
    uint id = gm->AddUser(user);

    std::cout << "Academic user created, assigned id " << id << "." << std::endl;
}

void NewEnterpriseUser(GridManager* gm)
{
    std::string name = ReadValueStr("Name: ");
    double budget = ReadValue<double>("Budget: ");

    User* user = new EnterpriseUser(name, budget);
    uint id = gm->AddUser(user);

    std::cout << "Enterprise user created, assigned id " << id << "." << std::endl;
}

void RemoveUser(GridManager* gm)
{
    uint id = ReadValue<uint>("Id: ");

    if (gm->RemoveUser(id))
        std::cout << "User with id " << id << " was removed." << std::endl;
    else
        std::cout << "Could not remove user with id " << id << "." << std::endl;
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
}

void RemoveMachine(GridManager* gm)
{
    uint id = ReadValue<uint>("Id: ");

    if (gm->RemoveMachine(id))
        std::cout << "Machine with id " << id << " was removed." << std::endl;
    else
        std::cout << "Could not remove machine with id " << id << "." << std::endl;
}

void NewJob(GridManager* gm)
{
    uint userId = ReadValue<uint>("User Id: ");

    std::string name = ReadValueStr("Name: ");
    uint8 priority = ReadValue<uint8>("Priority: ");
    double requiredRAM = ReadValue<double>("Required RAM usage: ");
    double requiredDiskSpace = ReadValue<double>("Required disk space usage: ");
    uint executionTime = ReadValue<uint>("Estimated execution time: ");

    Job* job = new Job(name, priority, requiredRAM, requiredDiskSpace, executionTime);
    if (gm->AddJobByUser(userId, job))
        std::cout << "Job added successfully." << std::endl;
    else
        std::cout << "Could not create job." << std::endl; // why not?
}
