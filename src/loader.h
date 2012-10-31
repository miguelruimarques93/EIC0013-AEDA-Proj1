#ifndef LOADER_H_
#define LOADER_H_

#include "utils.h"
#include "file.h"
#include "bytebuffer.h"
#include <string>

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

template <class Savable>
class Saver
{
public:
    Saver(const std::string& fileName) : _fileName(fileName) {}

    bool Save(Savable* s);

private:
    std::string _fileName;
};

template <class Savable>
bool Saver<Savable>::Save(Savable* s)
{
    ByteBuffer bb(100);
    if (!s->Save(bb))
        return false;

    return File::Save(_fileName.c_str(), bb, bb.Size());
}

#endif // LOADER_H_
