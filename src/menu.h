#ifndef MENU_H_
#define MENU_H_

#include "utils.h"
#include "bytebuffer.h"

#include <string>
#include <vector>
#include <algorithm>

class Menu;

/// Exception thrown when a menu's parent is invalid
class ParentNULL : public std::runtime_error
{
public:
    ParentNULL() : runtime_error("") { }
};

//! IMenu abstract Class
/*!
    Represents a menu item that has a name/label.
*/
class IMenu
{
public:
    IMenu(const std::string& label, Menu* parent = NULL); ///> Constructor.
    virtual ~IMenu() { } ///> Virtual destructor.
    virtual uint32 Print() const = 0; ///> Prints the menu label and returns the Menu or Menu::Item chosen by the user of the menu.

    const std::string& GetLabel() const { return _label; } ///> Returns the Menu's label.
    Menu* GetParent() const { return _parent; } ///> Returns the Menu's parent. Can be null.
    void SetParent(Menu* val) { _parent = val; } ///> Updates the Menu's parent

protected:
    Menu* _parent; ///> Parent menu.
    std::string _label; ///> Menu.
};

//! Menu Class
/*!
    Represents a menu that can have multiple sub-menus and links to other Menus and Menu::Items
*/
class Menu : public IMenu
{
public:
    Menu(const std::string& label, Menu* parent = NULL) : IMenu(label, parent) { } ///> Constructor
    ~Menu() { for (auto elem: _subMenus) delete elem.second; } ///> Destructor

    uint32 Print() const override;

    IMenu* AddMenu(char indexer, const std::string& label); ///> Adds a submenu to this Menu
    IMenu* AddMenuItem(char indexer, const std::string& label, uint32 val); ///> Adds an Item to this Menu
    IMenu* operator[](char indexer); ///> Indexer
    IMenu* operator[](char indexer) const; ///> Const indexer
    IMenu* GetLastSubMenu() { return (_subMenus.end() - 1)->second; } ///> Ability to go to previous Menu

    static Menu* Load(ByteBuffer& bb); ///> Loads the Menu from a ByteBuffer

private:
    /// A Menu can have many Items
    class Item : public IMenu
    {
    public:
        /// Constructor
        Item(const std::string& label, uint32 val, Menu* parent = NULL) : IMenu(label, parent), _value(val) { }
        uint32 Print() const override { return _value; }

    private:
        uint32 _value; ///> Value linked with the Menu::Item
    };

    std::vector<std::pair<char, IMenu*>> _subMenus; ///> A vector of submenus
};

#endif // MENU_H_
