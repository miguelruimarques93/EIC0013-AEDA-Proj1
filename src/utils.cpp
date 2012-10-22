#include "utils.h"
#include <chrono>
#include <cassert>

uint64 GetCurrentTime()
{
    return (uint64)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

uint32 GetTimeDiff(uint64 curTime, uint64 prevTime)
{
    assert(curTime >= prevTime);
    return uint32(curTime - prevTime);
}