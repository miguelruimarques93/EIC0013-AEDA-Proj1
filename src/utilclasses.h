#ifndef UTILCLASSES_H_
#define UTILCLASSES_H_

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

#endif // UTILCLASSES_H_
