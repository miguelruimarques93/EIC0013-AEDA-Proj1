#ifndef GRIDNETWORK_H_
#define GRIDNETWORK_H_

#include <map>
#include <string>
#include <functional>
#include <stdexcept>
#include "bytebuffer.h"
#include "interfaces.h"

class Grid;

//! ExistingGrid Exception
/*!
    Exception thrown by GridNetwork when trying to add a Grid with an existing name
*/
class ExistingGrid : public std::runtime_error
{
private:
    std::string _name; //> Duplicated name

public:
    ExistingGrid(const std::string& name) : std::runtime_error("Grid already exists"), _name(name) {} //> Constructor
    const std::string& GetExistingName() const { return _name; } //> Return duplicated name
};

//! UnknownGrid Exception
/*!
    Exception thrown by GridNetwork when a Grid could not be found
*/
class UnknownGrid : public std::runtime_error
{
private:
    std::string _name; //> Not found name

public:
    UnknownGrid(const std::string& name) : std::runtime_error("Grid not found"), _name(name) {} //> Constructor
    const std::string& GetUnknownName() const { return _name; } //> Return name that was not found
};

//! GridNetwork Class
/*!
    The GridNetwork class is a collection of Grids
*/
class GridNetwork : public ISave
{
private:
    std::map<std::string, Grid*> _bst; //> Underlying binary search tree. Grids sorted by name
    void AddGrid(const std::string& name, Grid* grid); //> Add existing grid to the network

public:
    ~GridNetwork(); //> Destructor, deallocate all Grids

    /// Add a new Grid to the network with the given name and topic
    void NewGrid(const std::string& name, const std::string& topic);
    /// Change the name of an existing Grid
    void ChangeGridName(const std::string& oldName, const std::string& newName);
    /// Change the topic of an existing Grid
    void ChangeGridTopic(const std::string& name, const std::string& newTopic);
    /// Remove a Grid from the Network
    void RemoveGrid(const std::string& name);

    /// Returns a pointer to an existing Grid
    Grid* GetGrid(const std::string& name);

    /**
    *   @brief Returns a pointer to a Grid loaded from the given ByteBuffer.
    *   @param bb source of the data.
    *   @return A pointer to the loaded Grid.
    */
    static GridNetwork* Load(ByteBuffer& bb);

    /**
    *   @brief Saves the information of the instance of the class to the given ByteBuffer.
    *   @param bb destination of the data.
    *   @return A boolean value that indicates whether the save was successful or not.
    */
    bool Save(ByteBuffer& bb) const override;
};

#endif // GRIDNETWORK_H_
