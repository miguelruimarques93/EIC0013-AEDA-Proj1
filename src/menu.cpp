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
    _label.erase(std::remove(_label.begin(), _label.end(), '\r'), _label.end());
}

uint32 Menu::Print() const
{
    Menu* parent = this->GetParent();
    std::cout << _label << ":" << std::endl;

    for (const auto& sm : _subMenus)
        std::cout << std::left << std::setw(parent != NULL ? 6 : 1) << sm.first << " - " << sm.second->GetLabel() << std::endl;

    if (parent != nullptr)
        std::cout << "CTRL-Z - Back" << std::endl;

    char option;
    IMenu* subMenu;

    do
    {
        try
        {
            option = ReadValue<char>("? ");
            subMenu = operator[]((char)toupper(option));
        }
        catch (EOFCharacterValue)
        {
            subMenu = GetParent();
            if (!subMenu) return 0;
        }

        if (!subMenu)
            std::cout << "Invalid option. Please try again." << std::endl;
    } while (!subMenu);

    ClearConsole();

    return subMenu->Print();
}

inline IMenu* Menu::AddMenu(char indexer, const std::string& label)
{
    _subMenus.push_back(std::make_pair(indexer, new Menu(label, this)));
    return GetLastSubMenu();
}

inline IMenu* Menu::AddMenuItem(char indexer, const std::string& label, uint32 val)
{
    _subMenus.push_back(std::make_pair(indexer, new Menu::Item(label, val, this)));
    return GetLastSubMenu();
}

inline IMenu* Menu::operator[](char indexer)
{
    return const_cast<IMenu*>(static_cast<const Menu&>(*this)[indexer]); // call const version
}

inline IMenu* Menu::operator[](char indexer) const
{
    auto it = std::find_if(_subMenus.begin(), _subMenus.end(), [indexer](std::pair<char, IMenu*> elem){ return elem.first == indexer; });
    return (it == _subMenus.end() ? nullptr : it->second);
}

Menu* Menu::Load(ByteBuffer& bb)
{
    std::istringstream buffer(bb);

    std::string name;
    std::getline(buffer, name, '\n');

    Menu* result = new Menu(name, 0);
    Menu* levelMenu = result;

    uint16 level = 0;

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
                levelMenu = static_cast<Menu*>(levelMenu->AddMenu(shortcut, label));
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
