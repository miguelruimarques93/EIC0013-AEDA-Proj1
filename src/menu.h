#ifndef MENU_H_
#define MENU_H_

#include "utils.h"
#include "bytebuffer.h"
#include <string>
#include <vector>

typedef uint32 RetType;

class Menu;

class IMenu
{
public:
    IMenu(const std::string& label, Menu* parent = NULL);
    virtual RetType Print() const = 0;

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
    RetType Print() const override { return _value; }

private:
    RetType _value;
};

class Menu : public IMenu
{
public:
    Menu(const std::string& label, Menu* parent = NULL) : IMenu(label, parent) { }
    RetType Print() const override;

    IMenu* AddMenu(char val, const std::string& label);

    IMenu* AddMenuItem(char val, const std::string& label, RetType value);

    IMenu* operator[](char index);
    IMenu* operator[](char index) const;

    IMenu* GetLastSubMenu() { return _subMenus[_subMenus.size()-1].second; }

    static Menu* Load(ByteBuffer& bb);

private:
    std::vector<std::pair<char, IMenu*>> _subMenus;
};

#endif // MENU_H_
