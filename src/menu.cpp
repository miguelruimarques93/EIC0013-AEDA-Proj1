#include "menu.h"
#include "consolereader.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cctype>
#include <vector>
#include <algorithm>


IMenu::IMenu(const std::string& label, Menu* parent /*= NULL*/) : _parent(parent), _label(label)
{
    for (std::string::iterator it = std::find(_label.begin(), _label.end(), '\r'); 
         it != _label.end(); 
         it = std::find(_label.begin(), _label.end(), '\r'))
    {
        _label.erase(it);
        it = find(_label.begin(), _label.end(), '\r');
    }
}

uint32 Menu::Print() const
{
    Menu* parent = this->GetParent();
    std::cout << _label << ":" << std::endl;

    for (std::pair<char, IMenu*> sm : _subMenus)
        std::cout << std::left << std::setw(parent != NULL ? 6 : 1) << sm.first << " - " << sm.second->GetLabel() << std::endl;

    if (parent != NULL) std::cout << "CTRL-Z - Back" << std::endl;

    char option;
    IMenu* subMenu;

    do 
    {
        try
        {
            option = ReadValue<char>("? ");
            subMenu = this->operator[](toupper(option));
        }
        catch (EOFCharacterValue)
        {
        	subMenu = this->GetParent();
        }

        if (!subMenu) 
            std::cout << "Invalid option. Please try again." << std::endl;
    } while (!subMenu);

    ClearScreen();

    return subMenu->Print();
}

inline IMenu* Menu::addMenu(char indexer, const std::string& label)
{
    _subMenus.push_back(std::pair<char, IMenu*>(indexer, new Menu(label, this)));
    return GetLastSubMenu();
}

inline IMenu* Menu::addMenu(char indexer, const std::string& label, uint32 val)
{
    _subMenus.push_back(std::pair<char, IMenu*>(indexer, new Menu::Item(label, val, this)));
    return GetLastSubMenu();
}

inline IMenu* Menu::operator[](const char indexer)
{
    std::vector<std::pair<char, IMenu*>>::iterator it = std::find_if(_subMenus.begin(), _subMenus.end(), [indexer] (std::pair<char, IMenu*> elem){ return elem.first == indexer; });
    return (it == _subMenus.end() ? NULL : it->second);
}

inline IMenu* Menu::operator[](const char indexer) const
{
    std::vector<std::pair<char, IMenu*>>::const_iterator it = std::find_if(_subMenus.begin(), _subMenus.end(), [indexer] (std::pair<char, IMenu*> elem){ return elem.first == indexer; });
    return (it == _subMenus.end() ? NULL : it->second);
}

Menu* Menu::Load(ByteBuffer& bb)
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
                buffer.ignore(1, '*');
                char shortcut;
                buffer >> shortcut;
                buffer.ignore(1, '#');
                uint32 ret;
                buffer >> ret;
                buffer.ignore(1, '#');
                std::string label;
                std::getline(buffer, label, '\n');
                levelMenu->addMenu(shortcut, label, ret);
            }
            else
            {
                char shortcut;
                buffer >> shortcut;
                buffer.ignore(1, '#');
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
            levelMenu = levelMenu->GetParent();
        }
    }

    return result;
}
