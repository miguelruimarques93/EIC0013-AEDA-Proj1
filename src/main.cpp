#include <iostream>

#include "gridmanager.h"
#include "loader.h"
#include "user.h"
#include "machine.h"
#include "bytebuffer.h"
#include "job.h"
#include "file.h"

int main(int argc, char* argv[])
{
    Loader loader("test");

    GridManager* gm = loader.Load();

    if (!gm)
    {
        gm = new GridManager();
        Machine* m = new Machine("Machine1", 2, 200, 200);
        User* u = new EnterpriseUser("User1", 200);
        Job* j = new Job("Job1", 100, 200, 200, 5);

        m->AddJob(j);

        gm->AddMachine(m);
        gm->AddUser(u);
    }

    ByteBuffer bb(100);
    gm->Save(bb);

    File::Save("test", (char*)bb.Data(), bb.Size());

    system("PAUSE");

    // delete gm;
   
    return EXIT_SUCCESS;
}
