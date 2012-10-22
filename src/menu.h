#ifndef MENU_H_
#define MENU_H_

#include "consolereader.h"
#include <map>
#include <iostream>
#include <algorithm>

template <typename T>

class IMenu
{
public:
    IMenu(std::string label) : Label(label) { }
    virtual T Print() = 0;

    std::string Label;
private:
};

template <typename T>
class MenuItem : public IMenu<T>
{
public:
    MenuItem(std::string label, T val) : IMenu(label), value(val) { }
    T Print()
    {
        return value;
    }
private:
    T value;
};

template <typename T>
class Menu : public IMenu<T>
{
public:
	Menu(std::string label) : IMenu(label) { }

    T Print()
    {
        std::cout << Label << ":" << std::endl;
        for (std::pair<char, IMenu<T>*> sm : _subMenus)
        {
            std::cout << sm.first << " - " << sm.second->Label << std::endl;
        }
        char option;
        std::map<char, IMenu<T>*>::iterator subMenu;

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

	Menu& addMenu(char val, IMenu* m) { _subMenus[val] = m; return *this; }
    IMenu<T>* operator[](const char index) { return _subMenus[index]; }
private:
    std::map<char, IMenu<T>*> _subMenus;
};
#endif // MENU_H_
