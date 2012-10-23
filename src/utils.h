#ifndef UTILS_H_
#define UTILS_H_

#include <cstdint>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef uint32 uint;

uint64 GetCurrentTime();
uint32 GetTimeDiff(uint64 curTime, uint64 prevTime);

void PauseConsole();

// ...

#endif // UTILS_H_
