#ifndef GRID_H_
#define GRID_H_

#include <string>

#include "utils.h"
#include "interfaces.h"

class GridManager;

class Grid : public ISave, public IUpdate, public IPrint
{
private:
    GridManager* _gm; ///< Underlying GridManager.
    std::string _name; ///< Name.
    std::string _topic; ///< Subject.

    /// Constructor of Grid, takes a name, subject and existing GridManager as arguments.
    Grid(const std::string& name, const std::string& topic, GridManager* gm) : _name(name), _topic(topic), _gm(gm) { }

    static uint _maxNameLength; ///< The length of the biggest name, used in Print methods
    static uint _maxTopicLength; ///< The length of the biggest topic, used in Print methods

public:
    /// Constructor of Grid, takes a name and subject as arguments.
    Grid(const std::string& name, const std::string& topic);

    /// Destructor, free allocated GridManager
    ~Grid();

    /// Returns the underlying GridManager (allows modification)
    GridManager* GetGridManager() { return _gm; } // not const

    /// Returns the name of the Grid
    const std::string& GetName() const { return _name; }
    /// Modifies the name of the Grid with a new name
    void ChangeName(const std::string& name) { _name = name; }

    /// Returns the subject of the Grid
    const std::string& GetTopic() const { return _topic; }
    /// Modifies the subject of the Grid with a new subject
    void ChangeTopic(const std::string& topic) { _topic = topic; }

    /// Returns the total amount of machines in the underlying GridManager.
    uint GetAvailableMachineCount() const;

    /// Returns the RAM (in GB) of the machine with the highest amount of RAM. 0 if there are no machines.
    double HighestMachineRAM() const;

    /// Returns the total amount of disk space (in GB) available in the machines
    double TotalDiskSpace() const;

    /**
    *   @brief Returns a pointer to a Grid loaded from the given ByteBuffer.
    *   @param bb source of the data.
    *   @return A pointer to the loaded Grid.
    */
    static Grid* Load(ByteBuffer& bb);

    /**
    *   @brief Saves the information of the instance of the class to the given ByteBuffer.
    *   @param bb destination of the data.
    *   @return A boolean value that indicates whether the save was successful or not.
    */
    bool Save(ByteBuffer& bb) const override;

    /// Updates the underlying GridManager
    void Update(uint32 diff) override;

    void Print(std::ostream& os = std::cout) const override; ///< Prints current Grid information
    static void PrintHeader(std::ostream& os = std::cout); ///< Prints table header for Grids

private: // no copying
    //! Copy constructor. Private to avoid copies of a Grid instance.
    Grid(const Grid&);
    //! Assignment operator. Private to avoid copies of a Grid instance.
    Grid& operator =(Grid const&);
};

#endif // GRID_H_
