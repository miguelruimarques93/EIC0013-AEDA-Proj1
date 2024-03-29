#include "ByteBuffer.h"
#include <vector>
#include <ostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <iomanip>
#include <cctype>
#include <malloc.h>

#ifdef _WIN32
#define alloca _alloca
#define snprintf sprintf_s
#endif

ByteBuffer::ByteBuffer(uint32 capacity) : _readPos(0), _writePos(0)
{
    _buffer.reserve(capacity);
}

ByteBuffer::ByteBuffer(const ByteBuffer& other)
    : _readPos(other._readPos), _writePos(other._writePos), _buffer(other._buffer) {}

const Byte* ByteBuffer::Data() const
{
    return &_buffer[0];
}

void ByteBuffer::Clear()
{
    _buffer.clear();
    _readPos = 0;
    _writePos = 0;
}

Byte ByteBuffer::operator[](uint32 pos) const
{
    return Read<Byte>(pos);
}

uint32 ByteBuffer::GetReadPos() const
{
    return _readPos;
}

void ByteBuffer::SetReadPos(uint32 readPos)
{
    _readPos = readPos;
}

uint32 ByteBuffer::GetWritePos() const
{
    return _writePos;
}

void ByteBuffer::SetWritePos(uint32 writePos)
{
    _writePos = writePos;
}

void ByteBuffer::FinishRead()
{
    _readPos = _writePos;
}

void ByteBuffer::ReadSkip(uint32 size)
{
    // assert(_readPos + size <= Size());

    if (_readPos + size > Size())
        throw ByteBufferException();

    _readPos += size;
}

void ByteBuffer::Read(Byte* dest, uint32 count)
{
    // assert(_readPos + count <= Size());

    if (_readPos + count > Size())
        throw ByteBufferException();

    memcpy(dest, &_buffer[_readPos], count);
    _readPos += count;
}

void ByteBuffer::Append(const ByteBuffer& other)
{
    if (_writePos)
        Append(other.Data(), other._writePos);
}

void ByteBuffer::Append(const Byte* src, uint32 count)
{
    // assert(count);
    // assert(src);
    // assert(Size() < 100000);

    if (!count || !src || Size() >= 100000)
        throw ByteBufferException();

    if (Size() < _writePos + count)
        _buffer.resize(_writePos + count);
    memcpy(&_buffer[_writePos], src, count);
    _writePos += count;
}

void ByteBuffer::Put(uint32 pos, const Byte* src, uint32 count)
{
    // assert(pos + count <= Size());
    // assert(src);

    if (pos + count > Size() || !src)
        throw ByteBufferException();

    memcpy(&_buffer[pos], src, count);
}

uint32 ByteBuffer::Size() const
{
    return _buffer.size();
}

bool ByteBuffer::IsEmpty() const
{
    return _buffer.empty();
}

void ByteBuffer::Resize(uint32 newSize)
{
    _buffer.resize(newSize, 0);
    _readPos = 0;
    _writePos = Size();
}

void ByteBuffer::Reserve(uint32 size)
{
    if (size > Size())
        _buffer.reserve(size);
}

void ByteBuffer::Print(std::ostream& stream) const
{
    const char* header = "|-------------------------------------------------|---------------------------------|\n"
        "| 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F | 0 1 2 3 4 5 6 7 8 9 A B C D E F |\n"
        "|-------------------------------------------------|---------------------------------|\n";

    stream << header;

    std::stringstream text;
    std::stringstream hex;

    uint32 length = Size();
    for (uint32 i = 0; i < length; i += 16) // 16 bytes per line
    {
        hex << "| ";

        for (int j = 0; j < 16; ++j)
        {
            if (j + i < length)
            {
                Byte val = _buffer[j + i];
                char h[3]; // 2 chars + 1 null
                snprintf(h, sizeof(h), "%X", val); // convert byte to hex upper-case
                hex << std::setfill('0') << std::setw(2) << h << " ";

                // print all printable characters in the text side
                if (std::isgraph(val))
                    text << (char)val;
                else
                    text << ".";

                text << " ";
            }
            else
            {
                hex << "   ";
                text << "  ";
            }
        }

        hex << "| " << text.str() << "|\n";
        stream << hex.str();

        // reset streams to be re-used
        hex.str("");
        hex.clear();
        text.str("");
        text.clear();
    }

    stream << "|-------------------------------------------------|---------------------------------|\n";
}

void ByteBuffer::WriteBool(bool value) { Append<bool>(value); }
void ByteBuffer::WriteUInt8(uint8 value) { Append<uint8>(value); }
void ByteBuffer::WriteUInt16(uint16 value) { Append<uint16>(value); }
void ByteBuffer::WriteUInt32(uint32 value) { Append<uint32>(value); }
void ByteBuffer::WriteUInt64(uint64 value) { Append<uint64>(value); }
void ByteBuffer::WriteInt8(int8 value) { Append<int8>(value); }
void ByteBuffer::WriteInt16(int16 value) { Append<int16>(value); }
void ByteBuffer::WriteInt32(int32 value) { Append<int32>(value); }
void ByteBuffer::WriteInt64(int64 value) { Append<int64>(value); }
void ByteBuffer::WriteFloat(float value) { Append<float>(value); }
void ByteBuffer::WriteDouble(double value) { Append<double>(value); }
void ByteBuffer::WriteBuffer(const ByteBuffer& value) { Append(value); }
void ByteBuffer::WriteString(const std::string& value)
{
    if (uint32 length = value.length())
    {
        Append7BitEncodedInt(length);
        Append((Byte const*)value.c_str(), length);
    }
}

void ByteBuffer::WriteCString(const char* str)
{
    if (str)
        if (size_t length = strlen(str))
            Append((Byte const*)str, length);

    Append<char>(0); // null terminator
}

bool ByteBuffer::ReadBool() { return Read<uint8>() > 0 ? true : false; }
uint8 ByteBuffer::ReadUInt8() { return Read<uint8>(); }
uint16 ByteBuffer::ReadUInt16() { return Read<uint16>(); }
uint32 ByteBuffer::ReadUInt32() { return Read<uint32>(); }
uint64 ByteBuffer::ReadUInt64() { return Read<uint64>(); }
int8 ByteBuffer::ReadInt8() { return Read<int8>(); }
int16 ByteBuffer::ReadInt16() { return Read<int16>(); }
int32 ByteBuffer::ReadInt32() { return Read<int32>(); }
int64 ByteBuffer::ReadInt64() { return Read<int64>(); }
float ByteBuffer::ReadFloat() { return Read<float>(); }
double ByteBuffer::ReadDouble() { return Read<double>(); }
std::string ByteBuffer::ReadString()
{
    uint32 length = Read7BitEncodedInt();
    // assert(length < 250000); // alloca is dangerous when allocating many bytes

    if (length >= 250000)
        throw ByteBufferException();

    Byte* res = (Byte*)alloca(length+1);
    Read(res, length);
    res[length] = 0; // null terminator
    std::string value = (const char*)res;

    return value;
}

std::string ByteBuffer::ReadCString()
{
    std::string value;
    value.reserve(50);

    while (GetWritePos() < Size())
    {
        char c = Read<char>();
        if (c == 0)
            break;

        value += c;
    }

    return value;
}

void ByteBuffer::Append7BitEncodedInt(uint32 value)
{
    // assert(value <= 0xFFFFFFF); // 8*4 - 4*1 bits

    if (value > 0xFFFFFFF)
        throw ByteBufferException();

    while (value >= 0x80)
    {
        Append((Byte)(value | 0x80));
        value >>= 7;
    }
    Append((Byte)value);
}

uint32 ByteBuffer::Read7BitEncodedInt()
{
    uint32 result = 0;
    for (uint8 i = 0, shift = 0; i < 4; ++i, shift += 7) // read max 4 bytes (max value 0xFFFFFFF)
    {
        Byte val = Read<Byte>();
        Byte realVal = Byte(val & 0x7F);
        result |= realVal << shift;
        if (!(val & 0x80)) // if highbit is 1, continue reading otherwise stop
            break;
    }
    return result;
}
