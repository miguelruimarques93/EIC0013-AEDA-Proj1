#ifndef LOADER_H_
#define LOADER_H_

#include "utils.h"
#include "file.h"
#include "bytebuffer.h"
#include <string>

//! Loader Class
/*!
    The Loader class calls the Load method of a Loadable class.
*/
template <class Loadable>
class Loader
{
public:
    /**
    *   @brief Constructor.
    *   @param fileName std::string that contains the source fileName.
    */
    Loader(const std::string& fileName) : _fileName(fileName) {}

    /**
    *   @brief Load method.
    *   @return Pointer to the loaded object.
    */
    Loadable* Load();
private:
    //! Source file name.
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

//! Saver Class
/*!
    The Saver class calls the Save method of a Savable class.
*/
template <class Savable>
class Saver
{
public:
    /**
    *   @brief Constructor.
    *   @param fileName std::string that contains the destination fileName.
    */
    Saver(const std::string& fileName) : _fileName(fileName) {}

    /**
    *   @brief Save method.
    *   @return A boolean value that indicates whether the save was successful or not.
    */
    bool Save(Savable* s);

private:
    //! Destination file name.
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
