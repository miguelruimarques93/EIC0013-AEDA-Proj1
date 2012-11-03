#ifndef FILE_H_
#define FILE_H_

namespace File
{
    bool Load(const char* fileName, char*& buffer, size_t& size); ///> Loads a file to a buffer of bytes (char)
    bool Save(const char* fileName, const char* buffer, size_t size); ///> Saves a file from a buffer of bytes (char)
    bool Remove(const char* fileName); ///> Deletes file with the given file name
}

#endif // FILE_H_
