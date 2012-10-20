#ifndef LOADER_H_
#define LOADER_H_

#include "utils.h"

#include <map>

namespace std
{
    class string;
}

class User;
class Machine;

class Loader
{
public:
    Loader(const std::string& fileName) : _fileName(fileName) {}

    bool Load();

private:

    std::string _fileName;

    std::map<uint, User*> _users;
    std::map<uint, Machine*> _machines;
};

#endif // LOADER_H_
