#ifndef CONSOLEREADER_H_
#define CONSOLEREADER_H_

#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <functional>

/// Exception thrown when an invalid value is read from the console
class InvalidValue : public std::runtime_error
{
public:
    InvalidValue(const char* val) : std::runtime_error(val) { }
};

/// Exception thrown when Ctrl+Z (Windows) or Ctrl+D (Linux) is pressed in the console
class EOFCharacterValue : public InvalidValue
{
public:
    EOFCharacterValue() : InvalidValue("") { }
};

/**
*   @brief Reads a value of the given type T from the console
*   @param prompt Message to display before requiring a value to be entered in the console
*   @param validator A function that accepts that read value as argument and
*                     it must return true for the value to be considered valid
*   @example @code
*   int value = ReadValue<int>("Age: ", [](int val) { return val > 0 && val < 150; } @endcode
*/
template <typename T>
T ReadValue(const std::string& prompt, std::function<bool(T)> validator)
{
    bool success = false;
    T val = T();

    while (!success)
    {
        std::cout << prompt;

        std::string input;
        std::getline(std::cin, input);

        if (std::cin.fail())
        {
            if (std::cin.eof())
            {
                std::cin.clear();
                throw EOFCharacterValue();
            }
            else
            {
                std::cin.clear();
                std::cout << "Invalid value. Please try again." << std::endl;
                continue;
            }
        }

        // convert string to T
        std::stringstream ss(input);

        if (!(ss >> val) || ss.rdbuf()->in_avail() != 0)
            std::cout << "Invalid value. Please try again." << std::endl;
        else
        {
            if (validator(val))
                success = true;
        }
    }

    return val;
}

/// See ReadValue<T>(const std::string&, std::function<bool(T)>)
template <typename T>
T ReadValue(const std::string& prompt)
{
    return ReadValue<T>(prompt, [](T) { return true; });
}

template<>
inline std::string ReadValue<std::string>(const std::string& prompt, std::function<bool(std::string)> validator)
{
    std::cout << prompt;

    std::string input;
    bool success = false;

    while (!success)
    {
        std::getline(std::cin, input);

        if (std::cin.fail())
        {
            if (std::cin.eof())
            {
                std::cin.clear();
                throw EOFCharacterValue();
            }
            else
            {
                std::cin.clear();
                std::cout << "Invalid value. Please try again." << std::endl;
                continue;
            }
        }

        if (validator(input))
            success = true;
    }

    return input;
}

#endif // CONSOLEREADER_H_
