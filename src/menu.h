#ifndef MENU_H_
#define MENU_H_

#include "utils.h"
#include "bytebuffer.h"
#include <string>
#include <map>

typedef uint32 RetType;

class Menu;

class IMenu
{
public:
    IMenu(const std::string& label, Menu* parent = NULL);
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

    RetType Print() override;

    IMenu* addMenu(char val, const std::string& label);
    IMenu* addMenu(char val, const std::string& label, RetType value);
    IMenu* operator[](char index) { return _subMenus[index]; }
    IMenu* GetLastSubMenu() { return _subMenus[_lastAddedKey]; }

    static Menu* Load(ByteBuffer& bb);

private:
    std::map<char, IMenu*> _subMenus;
    char _lastAddedKey;
};

#endif // MENU_H_
