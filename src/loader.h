#ifndef LOADER_H_
#define LOADER_H_

#include "utils.h"
#include <string>

class GridManager;

class User;
class Machine;

template <class Loadable>
class Loader
{
public:
    Loader(const std::string& fileName) : _fileName(fileName) {}

    Loadable* Load();
    

private:
    std::string _fileName;
};

template <class Loadable>
Loadable* Loader<Loadable>::Load()
{
    size_t size;
    char* buffer;
    if (!File::Load(_fileName.c_str(), buffer, size))
        return NULL;

    ByteBuffer bb(size);
    bb.WriteBuffer(buffer, size);

    delete[] buffer;

    return Loadable::Load(bb);
}

#endif // LOADER_H_
