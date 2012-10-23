#include "menu.h"
#include "consolereader.h"
#include <iostream>
#include <sstream>
#include <algorithm>

IMenu::IMenu(const std::string& label, Menu* parent /*= NULL*/) :  _parent(parent), _label(label)
{
    std::string::iterator it = std::find(_label.begin(), _label.end(), '\r');
    while (it != _label.end())
    {
        _label.erase(it);
        it = find(_label.begin(), _label.end(), '\r');
    }
}

RetType Menu::Print()
{
    std::cout << _label << ":" << std::endl;
    for (std::pair<char, IMenu*> sm : _subMenus)
    {
        std::cout << sm.first << " - " << sm.second->GetLabel() << std::endl;
    }
    char option;
    std::map<char, IMenu*>::iterator subMenu;

    do
    {
        option = ReadValue<char>("? ");
        subMenu = _subMenus.find(option);
        if (subMenu == _subMenus.end())
            std::cout << "Invalid option. Please try again." << std::endl;
    } while (subMenu == _subMenus.end());

    ClearScreen();

    return subMenu->second->Print();
}

Menu* Menu::Load(ByteBuffer& bb)
{
    std::istringstream buffer(bb);

    std::string name; 
    std::getline(buffer, name, '\n');

    Menu* result = new Menu(name);

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

IMenu* Menu::addMenu(char val, const std::string& label)
{
    _subMenus[val] = new Menu(label, &(*this));
    _lastAddedKey = val;
    return _subMenus[val];
}

IMenu* Menu::addMenu(char val, const std::string& label, RetType value)
{
    _subMenus[val] = new MenuItem(label, value, &(*this));
    _lastAddedKey = val;
    return _subMenus[val];
}
