#ifndef CONSOLEREADER_H_
#define CONSOLEREADER_H_

#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>

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
T ReadValue(std::string prompt = std::string(), std::istream& in = std::cin, std::ostream* out = &std::cout)
{
    bool success = false;
    T val = T();

    while (!success)
    {
        if (out)
            (*out) << prompt;

        std::string input;
        std::getline(in, input);

        if (in.fail())
        {
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
        }

        // convert string to T
        std::stringstream ss(input);

        if (!(ss >> val))
        {
            if (out)
                (*out) << "Invalid value. Please try again." << std::endl;
            else
                throw InvalidValue("ReadValue: Invalid value found with no out stream.");
        }
        else
            success = true;
    }

    return val;
}

template<>
inline std::string ReadValue<std::string>(std::string prompt /* = std::string()*/, std::istream& in /*= std::cin*/ , std::ostream* out/* = &std::cout */)
{
    if (out)
        (*out) << prompt;

    std::string input;
    bool success = false;

    while (!success) 
    {        
        std::getline(in, input);

        if (in.fail())
        {
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
        }
        success = true;
    }

    return input;
}


#endif // CONSOLEREADER_H_
