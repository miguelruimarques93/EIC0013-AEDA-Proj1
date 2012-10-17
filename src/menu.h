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
	virtual T Print() = 0;

	std::string Label;
private:
};

template <typename T>
class MenuItem : public IMenu<T>
{
public:
	MenuItem();
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
	Menu() { }

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

		return subMenu->second->Print();
	}

	void addMenu(IMenu* m) { _subMenus.push_back(m); }
private:
	std::map<char, IMenu<T>*> _subMenus;
};
#endif // MENU_H_
