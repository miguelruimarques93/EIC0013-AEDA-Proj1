#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#include <vector>
#include <ostream>
#include <cassert>
#include "utils.h"

typedef uint8 Byte;

class ByteBuffer
{
public:
    ByteBuffer(uint32 capacity);
    ByteBuffer(const ByteBuffer& other);

    const Byte* Data() const;

    void Clear();

    Byte operator[](uint32 pos) const;
    template <typename T> T Read(uint32 position) const;

    uint32 GetReadPos() const;
    void SetReadPos(uint32 readPos);

    uint32 GetWritePos() const;
    void SetWritePos(uint32 writePos);

    void FinishRead();

    template <typename T> void ReadSkip();
    void ReadSkip(uint32 size);

    uint32 Size() const;
    bool IsEmpty() const;

    void Resize(uint32 newSize);
    void Reserve(uint32 size);

    void Print(std::ostream& stream) const;

    void WriteBool(bool value);
    void WriteUInt8(uint8 value);
    void WriteUInt16(uint16 value);
    void WriteUInt32(uint32 value);
    void WriteUInt64(uint64 value);
    void WriteInt8(int8 value);
    void WriteInt16(int16 value);
    void WriteInt32(int32 value);
    void WriteInt64(int64 value);
    void WriteFloat(float value);
    void WriteDouble(double value);
    void WriteString(const std::string& value);
    void WriteString(const char* value) { WriteString(std::string(value)); }
    void WriteCString(const char* value);
    void WriteCString(const std::string& value) { WriteCString(value.c_str()); }
    void WriteBuffer(const ByteBuffer& other);
    void WriteBuffer(const char* src, uint32 count) { Append(src, count); }

    bool   ReadBool();
    uint8  ReadUInt8();
    uint16 ReadUInt16();
    uint32 ReadUInt32();
    uint64 ReadUInt64();
    int8   ReadInt8();
    int16  ReadInt16();
    int32  ReadInt32();
    int64  ReadInt64();
    float  ReadFloat();
    double ReadDouble();
    std::string ReadString();
    std::string ReadCString();

    ByteBuffer& operator <<(bool value) { WriteBool(value); return *this; }
    ByteBuffer& operator <<(uint8 value) { WriteUInt8(value); return *this; }
    ByteBuffer& operator <<(uint16 value) { WriteUInt16(value); return *this; }
    ByteBuffer& operator <<(uint32 value) { WriteUInt32(value); return *this; }
    ByteBuffer& operator <<(uint64 value) { WriteUInt64(value); return *this; }
    ByteBuffer& operator <<(int8 value) { WriteInt8(value); return *this; }
    ByteBuffer& operator <<(int16 value) { WriteInt16(value); return *this; }
    ByteBuffer& operator <<(int32 value) { WriteInt32(value); return *this; }
    ByteBuffer& operator <<(int64 value) { WriteInt64(value); return *this; }
    ByteBuffer& operator <<(float value) { WriteFloat(value); return *this; }
    ByteBuffer& operator <<(double value) { WriteDouble(value); return *this; }
    ByteBuffer& operator <<(const std::string& value) { WriteString(value); return *this; }
    ByteBuffer& operator <<(const char* value) { WriteCString(value); return *this; }
    ByteBuffer& operator <<(const ByteBuffer& value) { WriteBuffer (value); return *this; }

    ByteBuffer& operator >>(bool& value) { value = ReadBool(); return *this; }
    ByteBuffer& operator >>(uint8& value) { value = ReadUInt8(); return *this; }
    ByteBuffer& operator >>(uint16& value) { value = ReadUInt16(); return *this; }
    ByteBuffer& operator >>(uint32& value) { value = ReadUInt32(); return *this; }
    ByteBuffer& operator >>(uint64& value) { value = ReadUInt64(); return *this; }
    ByteBuffer& operator >>(int8& value) { value = ReadInt8(); return *this; }
    ByteBuffer& operator >>(int16& value) { value = ReadInt16(); return *this; }
    ByteBuffer& operator >>(int32& value) { value = ReadInt32(); return *this; }
    ByteBuffer& operator >>(int64& value) { value = ReadInt64(); return *this; }
    ByteBuffer& operator >>(float& value) { value = ReadFloat(); return *this; }
    ByteBuffer& operator >>(double& value) { value = ReadDouble(); return *this; }
    ByteBuffer& operator >>(std::string& value) { value = ReadString(); return *this; }

protected:
    std::vector<Byte> _buffer;
    uint32 _readPos;
    uint32 _writePos;

    // Use stream operators to read and write
    template <typename T> T Read();
    void Read(Byte* dest, uint32 count);
    template <typename T> void Append(T val);
    template <typename T> void Append(const T* src, uint32 count);
    void Append(const ByteBuffer& other);
    void Append(const Byte* src, uint32 count);
    template <typename T> void Put(uint32 pos, T val);
    void Put(uint32 pos, const Byte* src, uint32 count);

private:
    void Append7BitEncodedInt(uint32 value);
    uint32 Read7BitEncodedInt();
};

template <typename T>
void ByteBuffer::ReadSkip()
{
    ReadSkip(sizeof(T));
}

template <typename T>
T ByteBuffer::Read(uint32 position) const
{
    assert(position + sizeof(T) <= Size());
    T value = *((T const*)&_buffer[position]);
    return value;
}

template <typename T>
T ByteBuffer::Read()
{
    T val = Read<T>(_readPos);
    _readPos += sizeof(T);
    return val;
}

template <typename T>
void ByteBuffer::Append(T val)
{
    Append((Byte*)&val, sizeof(T));
}

template <typename T>
void ByteBuffer::Put(uint32 pos, T val)
{
    Put(pos, (Byte*)&val, sizeof(T));
}

template <typename T>
void ByteBuffer::Append(const T* src, uint32 count)
{
    return Append((const Byte*)src, count * sizeof(T));
}

#endif // BYTEBUFFER_H
