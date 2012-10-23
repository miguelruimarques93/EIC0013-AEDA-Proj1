#ifndef MENU_H_
#define MENU_H_

#include "utils.h"
#include "bytebuffer.h"
#include <string>
#include <vector>
#include <algorithm>

typedef uint32 RetType;

class Menu;

class IMenu
{
public:
    IMenu(const std::string& label, Menu* Parent = NULL);
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
    Menu(const std::string& label, Menu* parent = NULL) : IMenu(label, parent) { }
    RetType Print() override;

    IMenu* addMenu(char val, std::string label)
    { 
        _subMenus.push_back(std::pair<char, IMenu*>(val, new Menu(label, this)));
        return GetLastSubMenu(); 
    }

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

    static Menu* Load(ByteBuffer& bb);

private:
    std::vector<std::pair<char, IMenu*>> _subMenus;
};

#endif // MENU_H_
