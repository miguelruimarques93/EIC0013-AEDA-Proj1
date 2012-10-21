#ifndef FILE_H_
#define FILE_H_

namespace File
{
    bool Load(const char* fileName, char*& buffer, int& size);
    bool Save(const char* fileName, const char* buffer, int size);
}

#endif // FILE_H_
