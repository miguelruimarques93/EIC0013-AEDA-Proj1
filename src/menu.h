#ifndef MENU_H_
#define MENU_H_

#include "utils.h"
#include "bytebuffer.h"
#include <string>
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
    IMenu(const std::string& label, Menu* Parent = NULL) :  _parent(Parent)
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

    const std::string& GetLabel() const { return _label; }

    Menu* GetParent() const { return _parent; }

protected:
    Menu* _parent;
    std::string _label;
};

class MenuItem : public IMenu
{
public:
    MenuItem(const std::string& label, RetType value, Menu* parent = NULL) : IMenu(label, parent), _value(value) { }
    RetType Print() override { return _value; }
 
private:
    RetType _value;
};

class Menu : public IMenu
{
public:
    Menu(const std::string& label, Menu* parent = NULL) : IMenu(label, parent), _lastAddedKey(0) { }
    RetType Print() override
    {
        std::cout << Label << ":" << std::endl;
        for (std::pair<char, IMenu*> sm : _subMenus)
        {
            std::cout << sm.first << " - " << sm.second->Label << std::endl;
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

    IMenu* addMenu(char val, std::string label)
    { 
        _subMenus.push_back(std::pair<char, IMenu*>(val, new Menu(label, this)));
        return GetLastSubMenu(); }
    IMenu* addMenu(char val, std::string label, RetType value)
    { 
        _subMenus.push_back(std::pair<char, IMenu*>(val, new MenuItem(label, value, this)));
        return GetLastSubMenu();
    }
    IMenu* operator[](const char index)
    {
        auto it = std::find_if(_subMenus.begin(), _subMenus.end(), [index] (std::pair<char, IMenu*> elem) { return elem.first == index; });
        return (it == _subMenus.end() ? NULL: it->second);
    }
    IMenu* GetLastSubMenu() { return _subMenus[_subMenus.size()-1].second; }

    IMenu* addMenu(char val, const std::string& label);
    IMenu* addMenu(char val, const std::string& label, RetType value);
    IMenu* operator[](char index) { return _subMenus[index]; }
    IMenu* GetLastSubMenu() { return _subMenus[_lastAddedKey]; }

    static Menu* Load(ByteBuffer& bb);

private:
    std::vector<std::pair<char, IMenu*>> _subMenus;
    char _lastAddedKey;
};

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

#endif // MENU_H_
