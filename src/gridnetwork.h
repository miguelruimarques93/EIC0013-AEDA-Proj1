#ifndef GRIDNETWORK_H_
#define GRIDNETWORK_H_

#include <map>
#include <string>
#include <functional>
#include <stdexcept>
#include "bytebuffer.h"
#include "interfaces.h"
#include "runnable.h"

class Grid;

//! Hard coded save file.
#define GRID_SAVE_FILE "gridComputing.grid"

//! ExistingGrid Exception
/*!
    Exception thrown by GridNetwork when trying to add a Grid with an existing name
*/
class ExistingGrid : public std::runtime_error
{
private:
    std::string _name; ///< Duplicated name

public:
    ExistingGrid(const std::string& name) : std::runtime_error("Grid already exists"), _name(name) {} ///< Constructor
    const std::string& GetExistingName() const { return _name; } ///< Return duplicated name
};

//! UnknownGrid Exception
/*!
    Exception thrown by GridNetwork when a Grid could not be found
*/
class UnknownGrid : public std::runtime_error
{
private:
    std::string _name; ///< Not found name

public:
    UnknownGrid(const std::string& name) : std::runtime_error("Grid not found"), _name(name) {} ///< Constructor
    const std::string& GetUnknownName() const { return _name; } ///< Return name that was not found
};

//! GridNetwork Class
/*!
    The GridNetwork class is a collection of Grids, organized in a BST by name
*/
class GridNetwork : public ISave, public IUpdate, public Runnable
{
private:
    std::map<std::string, Grid*> _bst; ///< Underlying binary search tree. Grids sorted by name
    void AddGrid(const std::string& name, Grid* grid); ///< Add existing grid to the network

    uint64 _realPrevTime; ///< Time of the last update (ms)
    uint64 _realCurrTime; ///< Time of the current update (ms)

    /// Calls update functions of all the elements of the class, that can be updated, two times per second. It saves the GridNetwork to file every 5 seconds.
    void Run();

public:
    GridNetwork() : _realPrevTime(0), _realCurrTime(0) { Start(); }
    ~GridNetwork(); ///< Destructor, deallocate all Grids

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

    /**
    *   @brief Update function that updates all grids. It removes finished jobs and updates the respective machines.
    *   @param diff time that has passed since last update.
    */
    void Update(uint32 diff);

    /// Returns the underlying Grid container
    const std::map<std::string, Grid*>& GetContainer() const { return _bst; }

    /**
    *   @brief Applies the given predicate to the Grids stored in this GridNetwork and returns a vector with the values that meet the requirement.
    *   @param  predicate to evaluate the data: unary function that returns a bool and accepts a Grid* as argument.
    *   @return A vector with the result of the search.
    */
    std::vector<const Grid*> ApplyPredicate(std::function<bool(const Grid*)> predicate) const;

private: // no copying
    //! Copy constructor. Private to avoid copies of a GridManager instance.
    GridNetwork(const GridNetwork &);
    //! Assignment operator. Private to avoid copies of a GridManager instance.
    GridNetwork& operator =(GridNetwork const&);
};

#endif // GRIDNETWORK_H_
