#ifndef CONSOLEREADER_H_
#define CONSOLEREADER_H_

#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <functional>

class InvalidValue : public std::runtime_error
{
public:
    InvalidValue(const char* val) : std::runtime_error(val) { }
};

class EOFCharacterValue : public InvalidValue
{
public:
    EOFCharacterValue() : InvalidValue("") { }
};

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
            if (!validator(val))
                std::cout << "Value does not match the requirements. Please try again." << std::endl;
            else
                success = true;
        }
    }

    return val;
}

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

        if (!validator(input))
            std::cout << "Value does not match the requirements. Please try again." << std::endl;
        else
            success = true;
    }

    return input;
}

#endif // CONSOLEREADER_H_
