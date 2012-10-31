#include "file.h"
#include <cstdio>
#include <memory>

namespace File
{

    bool Load(const char* fileName, char*& buffer, size_t& size)
    {
        if (!fileName)
            return false;

        FILE* file = fopen(fileName, "rb");
        if (!file)
            return false;

        // Get file size
        fseek(file, 0, SEEK_END);
        size = (size_t)ftell(file);
        rewind(file);

        if (!size)
            return false;

        buffer = new char[size];
        if (!buffer)
        {
            delete[] buffer;
            return false;
        }

        size_t result = fread(buffer, sizeof(char), size, file);
        if (result != size)
        {
            delete[] buffer;
            return false;
        }

        fclose(file);
        return true;
    }

    bool Save(const char* fileName, const char* buffer, size_t size)
    {
        if (!fileName || !buffer || !size)
            return false;

        FILE* file = fopen(fileName, "wb");
        if (!file)
            return false;

        size_t result = fwrite(buffer, sizeof(char), size, file);
        if (result != size)
            return false;

        fclose(file);
        return true;
    }

    bool Remove(const char* fileName)
    {
        return !remove(fileName);
    }
}
