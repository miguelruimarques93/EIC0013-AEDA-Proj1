#include "utils.h"
#include <chrono>
#include <cassert>
#include <iostream>
#include <string>
#include <limits>

uint64 GetCurrentTime()
{
    return (uint64)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

uint32 GetTimeDiff(uint64 curTime, uint64 prevTime)
{
    assert(curTime >= prevTime);
    return uint32(curTime - prevTime);
}

void ClearConsole()
{
#ifdef _WIN32 // Windows: Console command (alternative: Windows API)
    system("cls");
#else // UNIX: ANSI escape codes
    std::cout << "\x1B[2J\x1B[H";
#endif
}

void PauseConsole(const std::string& message/* = "Press enter to continue..."*/)
{
    std::cout << message << std::endl;
    std::cin.get();

    std::cin.clear();

    if (std::cin.rdbuf()->in_avail() != 0)
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
