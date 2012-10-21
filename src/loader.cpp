#include "loader.h"
#include "gridmanager.h"
#include "bytebuffer.h"

#include <cstdio>
#include <memory>

GridManager* Loader::Load()
{
    FILE* file = fopen(_fileName.c_str(), "rb");
    if (!file)
        return NULL;

    // Get file size
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);

    if (!size)
        return NULL;

    std::unique_ptr<char> buffer(new char[size]);
    if (!buffer.get())
        return NULL;

    size_t result = fread(buffer.get(), sizeof(char), size, file);
    if (result != size)
        return NULL;

    fclose(file);

    ByteBuffer bb(size);

    bb.WriteBuffer(buffer.get(), size);

    return GridManager::Load(bb);
}
