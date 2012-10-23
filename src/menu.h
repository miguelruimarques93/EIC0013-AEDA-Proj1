#ifndef MENU_H_
#define MENU_H_

#include "consolereader.h"
#include "bytebuffer.h"
#include <map>
#include <iostream>
#include <algorithm>
#include <functional>
#include <sstream>

typedef uint32 RetType;

class Menu;

class IMenu
{
public:
    IMenu(std::string label, Menu* Parent = NULL) :  _parent(Parent)
    {
        std::string::iterator it = find(label.begin(), label.end(), '\r');
        while (it != label.end())
        {
            label.erase(it);
            it = find(label.begin(), label.end(), '\r');
        }

        Label = label;

    }
    virtual RetType Print() = 0;

    std::string Label;
    Menu* GetParent() const { return _parent; }
protected:
    Menu* _parent;
};

class MenuItem : public IMenu
{
public:
    MenuItem(std::string label, RetType val, Menu* Parent = NULL) : IMenu(label, Parent), value(val) { }
    RetType Print()
    {
        return value;
    }
private:
    RetType value;
};

class Menu : public IMenu
{
public:
    Menu(std::string label, Menu* Parent = NULL) : IMenu(label, Parent) { }

    RetType Print()
    {
        std::cout << Label << ":" << std::endl;
        for (std::pair<char, IMenu*> sm : _subMenus)
        {
            std::cout << sm.first << " - " << sm.second->Label << std::endl;
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

    IMenu* addMenu(char val, std::string label) { _subMenus[val] = new Menu(label, &(*this)); lastAddedKey = val; return _subMenus[val]; }
    IMenu* addMenu(char val, std::string label, RetType value) { _subMenus[val] = new MenuItem(label, value, &(*this)); lastAddedKey = val; return _subMenus[val]; }
    IMenu* operator[](const char index) { return _subMenus[index]; }
    IMenu* GetLastSubMenu() { return _subMenus[lastAddedKey]; }


    static Menu* Load(ByteBuffer& bb);
private:
    std::map<char, IMenu*> _subMenus;
    char lastAddedKey;
};

Menu* Menu::Load( ByteBuffer& bb )
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

#endif // MENU_H_
