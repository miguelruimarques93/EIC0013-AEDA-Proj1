#include "consolereader.h"

std::string ReadValueStr(std::string prompt /*= std::string()*/, std::istream& in /*= std::cin*/, std::ostream* out /*= &std::cout*/)
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
