#include "menu.h"
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <sstream>
#include "consolereader.h"

IMenu::IMenu( const std::string& label, Menu* Parent /*= NULL*/ ) :  _parent(Parent)
{
    _label = [] (const std::string& label) 
    { 
        std::string result(label);

        std::string::const_iterator it = std::find(result.begin(), result.end(), '\r');
        while (it != result.end())
        {
            result.erase(it);
            it = find(result.begin(), result.end(), '\r');
        }

        return result;
    } (label);
}

RetType Menu::Print()
{
    std::cout << _label << ":" << std::endl;
    for (std::pair<char, IMenu*> sm : _subMenus)
    {
        std::cout << sm.first << " - " << sm.second->GetLabel() << std::endl;
    }
    char option;
    IMenu* subMenu;

    do
    {
        option = ReadValue<char>("? ");
        subMenu = (*this)[option];
        if (subMenu == NULL)
            std::cout << "Invalid option. Please try again." << std::endl;
    } while (subMenu == NULL);

    ClearScreen();

    return subMenu->Print();
}

Menu* Menu::Load( ByteBuffer& bb )
{
    std::istringstream buffer(bb);

    std::string name; 
    std::getline(buffer, name, '\n');

    Menu* result = new Menu(name,0);

    uint16 level = 0;
    Menu* levelMenu = result;

    while (!buffer.eof())
    {
        auto position = buffer.tellg();
        uint16 levelTemp = 0;
        while (buffer.get() == '-') { levelTemp++; }
        buffer.unget();

        if (levelTemp == level + 1)
        {
            if (buffer.peek() == '*')
            {
                buffer.ignore(1,'*');
                char shortcut;
                buffer >> shortcut;
                buffer.ignore(1,'#');
                uint32 ret;
                buffer >> ret;
                buffer.ignore(1,'#');
                std::string label;
                std::getline(buffer, label, '\n');
                levelMenu->addMenu(shortcut, label, ret);
            }
            else
            {
                char shortcut;
                buffer >> shortcut;
                buffer.ignore(1,'#');
                std::string label;
                std::getline(buffer, label, '\n');
                levelMenu = (Menu*)levelMenu->addMenu(shortcut, label);
                level++;
            }
        }
        else if (level >= levelTemp)
        {
            buffer.seekg(position);
            level--;
            levelMenu = (Menu*)levelMenu->GetParent();
        }
    }

    return result;
}
