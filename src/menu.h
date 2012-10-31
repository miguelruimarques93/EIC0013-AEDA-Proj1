#ifndef MENU_H_
#define MENU_H_

#include "utils.h"
#include "bytebuffer.h"

#include <string>
#include <vector>
#include <algorithm>

class Menu;

class ParentNULL : public std::runtime_error
{
public:
    ParentNULL() : runtime_error("") { }
};

class IMenu
{
public:
    IMenu(const std::string& label, Menu* parent = NULL);
    virtual ~IMenu() { }
    virtual uint32 Print() const = 0;

    const std::string& GetLabel() const { return _label; }
    Menu* GetParent() const { return _parent; }
    void SetParent(Menu* val) { _parent = val; }

protected:
    Menu* _parent;
    std::string _label;
};

class Menu : public IMenu
{
public:
    Menu(const std::string& label, Menu* parent = NULL) : IMenu(label, parent) { }
    ~Menu() { for (auto elem: _subMenus) delete elem.second; }

    uint32 Print() const override;

    IMenu* AddMenu(char indexer, const std::string& label);
    IMenu* AddMenuItem(char indexer, const std::string& label, uint32 val);
    IMenu* operator[](char indexer);
    IMenu* operator[](char indexer) const;
    IMenu* GetLastSubMenu() { return (_subMenus.end() - 1)->second; }

    static Menu* Load(ByteBuffer& bb);

private:
    class Item : public IMenu
    {
    public:
        Item(const std::string& label, uint32 val, Menu* parent = NULL) : IMenu(label, parent), _value(val) { }
        uint32 Print() const override { return _value; }

    private:
        uint32 _value;
    };

    std::vector<std::pair<char, IMenu*>> _subMenus;
};

#endif // MENU_H_
