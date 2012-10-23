#include <iostream>
#include <cstdlib>
#include <fstream>
#include <functional>

#include "gridmanager.h"
#include "loader.h"
#include "user.h"
#include "machine.h"
#include "bytebuffer.h"
#include "job.h"
#include "file.h"
#include "log.h"
#include "utils.h"

int main(int argc, char* argv[])
{
    std::auto_ptr<GridManager> gm(new GridManager());

    Machine* m1 = new Machine("Machine1", 2, 200, 200);
    Machine* m2 = new Machine("Machine2", 1, 200, 200);
    Machine* m3 = new Machine("Machine3", 5, 500, 500);

    Job* j1 = new Job("Job1", 100, 100, 100, 1);
    Job* j2 = new Job("Job2", 100, 100, 100, 2);
    Job* j3 = new Job("Job3", 100, 100, 100, 3);
    Job* j4 = new Job("Job4", 100, 100, 100, 4);

    gm->AddMachine(m1);
    gm->AddMachine(m2);
    gm->AddMachine(m3);

    /*
    if (gm->AddJob(j1))
        sLog(Console)->Log("j1 added");
    if (gm->AddJob(j2))
        sLog(Console)->Log("j2 added");

    std::this_thread::sleep_for(std::chrono::seconds(2));

    if (gm->AddJob(j3))
        sLog(Console)->Log("j3 added");
    if (gm->AddJob(j4))
        sLog(Console)->Log("j4 added");
        */


    /*
    Loader loader("test");
    std::auto_ptr<GridManager> gm(loader.Load());

    if (!gm.get())
    {
        gm = std::auto_ptr<GridManager>(new GridManager());
        Machine* m = new Machine("Machine1", 2, 200, 200);
        User* u = new EnterpriseUser("User1", 200);
        Job* j = new Job("Job1", 100, 200, 200, 5);

        m->AddJob(j);

        gm->AddMachine(m);
        gm->AddUser(u);
    }
    */

    PauseConsole();

    /*
    ByteBuffer bb(100);
    gm->Save(bb);

    File::Save("test", (char*)bb.Data(), bb.Size());
    */

    return EXIT_SUCCESS;
}
