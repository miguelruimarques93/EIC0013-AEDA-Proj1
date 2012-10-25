#ifndef CONSOLEREADER_H_
#define CONSOLEREADER_H_

#include <istream>
#include <ostream>
#include <iostream>
#include <string>
#include <stdexcept>


//#ifdef _WIN32
//#include <windows.h>
//#endif

void ClearScreen() // multi-platform, by "Cat Plus Plus"
{
#ifdef _WIN32 // Windows: Windows API
    //COORD topLeft  = { 0, 0 };
    //HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    //CONSOLE_SCREEN_BUFFER_INFO screen;
    //DWORD written;
    //
    //GetConsoleScreenBufferInfo(console, &screen);
    //FillConsoleOutputCharacterA(console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
    //FillConsoleOutputAttribute(console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE, screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
    //SetConsoleCursorPosition(console, topLeft);
    system("cls");
#else // UNIX: ANSI escape codes
    std::cout << "\x1B[2J\x1B[H";
#endif
}

class InvalidValue : public std::runtime_error
{
public:
    InvalidValue(const char* val) : std::runtime_error(val) { }
};

class EOFCharacterValue : InvalidValue
{
public:
    EOFCharacterValue() : InvalidValue("") { }
};

template <typename T>
T ReadValue(std::string prompt = std::string(), std::istream& in = std::cin, std::ostream* out = &std::cout)
{
    bool success = false;
    T val = T();

    while (!success)
    {
        if (out) (*out) << prompt;

        std::string input;
        std::getline(in, input);

        if (in.fail())
            if (in.eof())
            {
                in.clear();
                throw EOFCharacterValue();
            }
            else
            {
                in.clear();
                if (out)
                    (*out) << "Invalid value. Please try again." << std::endl;
                else
                    throw InvalidValue("ReadValue: Invalid value found with no out stream.");
                continue;
            }

        // convert string to T
        std::stringstream ss(input);

        if (!(ss >> val))
            if (out)
                (*out) << "Invalid value. Please try again." << std::endl;
            else
                throw InvalidValue("ReadValue: Invalid value found with no out stream.");
        else
            success = true;
    }

    return val;
}

template <>
std::string ReadValue<std::string> (std::string prompt /* = std::string()*/, std::istream& in /*= std::cin */, std::ostream* out/* = &std::cout */)
{
    if (out) (*out) << prompt;

    std::string input;
    std::getline(in, input);

    return input;
}

#endif // CONSOLEREADER_H_
 