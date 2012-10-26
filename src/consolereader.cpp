#include "consolereader.h"

std::string ReadValueStr(std::string prompt /*= std::string()*/, std::istream& in /*= std::cin*/, std::ostream* out /*= &std::cout*/)
{
    if (out)
        (*out) << prompt;

    std::string input;
    std::getline(in, input);

    return input;
}
