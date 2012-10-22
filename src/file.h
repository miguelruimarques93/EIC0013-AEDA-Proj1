#ifndef FILE_H_
#define FILE_H_

namespace File
{
    bool Load(const char* fileName, char*& buffer, size_t& size);
    bool Save(const char* fileName, const char* buffer, size_t size);
}

#endif // FILE_H_
