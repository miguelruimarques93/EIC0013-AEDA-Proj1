#ifndef INTERFACES_H_
#define INTERFACES_H_

#include "utils.h"
#include <iostream>

class ByteBuffer;

//! ISave Interface
/*!
    The ISave Interface represents a Saveable class.
*/
class ISave
{
public:
    /**
    *   @brief Saves the class.
    *   @param bb destination of the data.
    *   @return A boolean value that indicates whether the save was successful or not.
    */
    virtual bool Save(ByteBuffer& bb) const = 0;
};

//! IUpdate Interface
/*!
    The IUpdate Interface represents a class that can be updated.
*/
class IUpdate
{
public:
    /**
    *   @brief Updates the class.
    *   @param diff time that has passed since last update.
    */
    virtual void Update(uint32 diff) = 0;
};

//! IPrint Interface
/*!
    The IPrint Interface represents a Printable class.
*/
class IPrint
{
public:
    /**
    *   @brief Prints the class.
    *   @param os stream to print to.
    */
    virtual void Print(std::ostream& os = std::cout) const = 0;
};

#endif // INTERFACES_H_
