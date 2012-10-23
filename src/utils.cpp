#include "utils.h"
#include <chrono>
#include <cassert>
#include <iostream>

uint64 GetCurrentTime()
{
    return (uint64)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

uint32 GetTimeDiff(uint64 curTime, uint64 prevTime)
{
    assert(curTime >= prevTime);
    return uint32(curTime - prevTime);
}

void PauseConsole()
{
    std::cout << "Press any key to continue..." << std::endl;
    std::cin.get();
}
