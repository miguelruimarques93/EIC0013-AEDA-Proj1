#ifndef UTILS_H_
#define UTILS_H_

#include <cstdint>
#include <string>

typedef int8_t int8; ///< Signed byte
typedef int16_t int16; ///< Signed short
typedef int32_t int32; ///< Signed int
typedef int64_t int64; ///< Signed 64 bits int
typedef uint8_t uint8; ///< Unsigned byte
typedef uint16_t uint16; ///< Unsigned short
typedef uint32_t uint32; ///< Unsigned 32 bits int
typedef uint64_t uint64; ///< Unsigned 64 bits int

typedef uint32 uint; ///< Signed 32 bits int

uint64 GetCurrentTime(); ///< Number of milliseconds representing time
uint32 GetTimeDiff(uint64 curTime, uint64 prevTime); ///< The time difference between 2 times, in milliseconds

void PauseConsole(const std::string& message = "Press enter to continue..."); ///< Prints a message and waits for a key to be pressed
void ClearConsole(); ///< Empties the console screen

#endif // UTILS_H_
