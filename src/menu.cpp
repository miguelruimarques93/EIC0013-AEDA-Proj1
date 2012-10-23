#include "menu.h"
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <sstream>
#include "consolereader.h"

IMenu::IMenu(const std::string& label, Menu* parent /*= NULL*/) :  _parent(parent), _label(label)
{
    std::string::const_iterator it = std::find(_label.begin(), _label.end(), '\r');
    while (it != _label.end())
    {
        _label.erase(it);
        it = std::find(_label.begin(), _label.end(), '\r');
    }
}

RetType Menu::Print() const
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
        subMenu = operator[](option);
        if (!subMenu)
            std::cout << "Invalid option. Please try again." << std::endl;
    } while (!subMenu);

    ClearScreen();

    return subMenu->Print();
}

Menu* Menu::Load(ByteBuffer& bb)
{
    std::istringstream buffer(bb);

    std::string name;
    std::getline(buffer, name, '\n');

    Menu* result = new Menu(name, 0);

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
                buffer.ignore(1, '*');
                char shortcut;
                buffer >> shortcut;
                buffer.ignore(1, '#');
                uint32 ret;
                buffer >> ret;
                buffer.ignore(1, '#');
                std::string label;
                std::getline(buffer, label, '\n');
                levelMenu->AddMenuItem(shortcut, label, ret);
            }
            else
            {
                char shortcut;
                buffer >> shortcut;
                buffer.ignore(1, '#');
                std::string label;
                std::getline(buffer, label, '\n');
                levelMenu = (Menu*)levelMenu->AddMenu(shortcut, label);
                level++;
            }
        }
        else if (level >= levelTemp)
        {
            buffer.seekg(position);
            level--;
            levelMenu = levelMenu->GetParent();
        }
    }

    return result;
}

IMenu* Menu::operator[](char index)
{
    return const_cast<IMenu*>(static_cast<const Menu&>(*this)[index]);
}

IMenu* Menu::operator[](char index) const
{
    auto it = std::find_if(_subMenus.begin(), _subMenus.end(), [index](std::pair<char, IMenu*> elem) { return elem.first == index; });
    return it == _subMenus.end() ? NULL: it->second;
}

IMenu* Menu::AddMenuItem(char val, const std::string& label, RetType value)
{
    _subMenus.push_back(std::pair<char, IMenu*>(val, new MenuItem(label, value, this)));
    return GetLastSubMenu();
}

IMenu* Menu::AddMenu(char val, const std::string& label)
{
    _subMenus.push_back(std::pair<char, IMenu*>(val, new Menu(label, this)));
    return GetLastSubMenu();
}
