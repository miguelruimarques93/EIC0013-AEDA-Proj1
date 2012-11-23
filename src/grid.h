#ifndef GRID_H_
#define GRID_H_

#include <string>
#include "utils.h"
#include "gridmanager.h"
#include "interfaces.h"

class Grid : public ISave
{
private:
    GridManager* _gm; ///> Underlying GridManager.
    std::string _name; ///> Name.
    std::string _topic; ///> Subject.

    /// Constructor of Grid, takes a name, subject and existing GridManager as arguments.
    Grid(const std::string& name, const std::string& topic, GridManager* gm) : _name(name), _topic(topic), _gm(gm) { }

public:
    /// Constructor of Grid, takes a name and subject as arguments.
    Grid(const std::string& name, const std::string& topic) : _name(name), _topic(topic) { _gm = new GridManager(); }

    /// Destructor, free allocated GridManager
    ~Grid() { delete _gm; }

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
    uint GetAvailableMachineCount() const { return _gm->GetNumberOfMachines(); }

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

private: // no copying
    //! Copy constructor. Private to avoid copies of a Grid instance.
    Grid(const Grid&);
    //! Assignment operator. Private to avoid copies of a Grid instance.
    Grid& operator =(Grid const&);
};

#endif // GRID_H_
