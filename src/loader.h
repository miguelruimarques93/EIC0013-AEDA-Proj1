#ifndef LOADER_H_
#define LOADER_H_

#include "utils.h"
#include <string>

class GridManager;

class User;
class Machine;

class Loader
{
public:
    Loader(const std::string& fileName) : _fileName(fileName) {}

    GridManager* Load();

private:
    std::string _fileName;
};

#endif // LOADER_H_
