#include "loader.h"
#include "gridmanager.h"
#include "bytebuffer.h"
#include "file.h"

GridManager* Loader::Load()
{
    size_t size;
    char* buffer;
    if (!File::Load(_fileName.c_str(), buffer, size))
    {
        delete[] buffer;
        return NULL;
    }

    ByteBuffer bb(size);
    bb.WriteBuffer(buffer, size);

    delete[] buffer;

    return GridManager::Load(bb);
}
