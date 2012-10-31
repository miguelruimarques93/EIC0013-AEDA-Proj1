#ifndef INTERFACES_H_
#define INTERFACES_H_

#include "utils.h"
#include <iostream>

class ByteBuffer;

class ISave
{
public:
    virtual bool Save(ByteBuffer& bb) const = 0;
};

class IUpdate
{
public:
    virtual void Update(uint32 diff) = 0;
};

class IPrint
{
public:
    virtual void Print(std::ostream& os = std::cout) const = 0;
};

#endif // INTERFACES_H_
