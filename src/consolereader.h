#ifndef CONSOLEREADER_H_
#define CONSOLEREADER_H_

#include <istream>
#include <ostream>
#include <iostream>
#include <string>
#include <stdexcept>

class InvalidValue : public std::runtime_error
{
public:
    InvalidValue(const char* val) : std::runtime_error(val) { }
};


template <typename T>
T ReadValue(std::string prompt = std::string(), std::istream& in = std::cin, std::ostream* out = &std::cout)
{
    bool success = false;
    T val;

    while (!success)
    {
        if (out) (*out) << prompt;

        std::string input;
        std::getline(in, input);

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
 