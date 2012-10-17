#ifndef UTILCLASSES_H_
#define UTILCLASSES_H_

#include <ostream>

class ByteBuffer;

class Saveable
{
public:
    virtual bool Save(ByteBuffer& bb) const = 0;
};

#endif // UTILCLASSES_H_
