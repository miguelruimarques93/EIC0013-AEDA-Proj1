#include "loader.h"
#include "gridmanager.h"
#include "bytebuffer.h"
#include "file.h"

GridManager* Loader::Load()
{
    int size;
    char* buffer;
    File::Load(_fileName.c_str(), buffer, size);

    ByteBuffer bb(size);
    bb.WriteBuffer(buffer, size);

    delete[] buffer;

    return GridManager::Load(bb);
}
