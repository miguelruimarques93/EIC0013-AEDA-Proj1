#include "file.h"
#include "log.h"
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
        {
            sLog(Console)->Log("File::Load: Could not open file %s (fopen)", fileName);
            return false;
        }

        // Get file size
        fseek(file, 0, SEEK_END);
        size = (size_t)ftell(file);
        rewind(file);

        if (!size)
        {
            sLog(Console)->Log("File::Load: Could not open file %s (size is 0)", fileName);
            return false;
        }

        buffer = new char[size];
        if (!buffer)
        {
            sLog(Console)->Log("File::Load: Failed to allocate buffer for file %s (size %u)", fileName, size);
            return false;
        }

        size_t result = fread(buffer, sizeof(char), size, file);
        if (result != size)
        {
            sLog(Console)->Log("File::Load: Could not read the same number of bytes (size != result) for file %s (size %u, result %u)", fileName, size, result);
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
        {
            sLog(Console)->Log("File::Save: Could not open file %s (fopen)", fileName);
            return false;
        }

        size_t result = fwrite(buffer, sizeof(char), size, file);
        if (result != size)
        {
            sLog(Console)->Log("File::Save: Could not write the same number of bytes (size != result) for file %s (size %u, result %u)", fileName, size, result);
            return false;
        }

        fclose(file);
        return true;
    }

    bool Remove(const char* fileName)
    {
        return !remove(fileName);
    }
}
