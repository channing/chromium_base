#pragma once

#include "ui/views/view.h"

class SubmenuView;
class MenuController;

class MenuItemView : public views::View {
public:
    // Where the menu should be anchored to for non-RTL languages.  The
    // opposite position will be used if base::i18n:IsRTL() is true.
    enum AnchorPosition {
        TOPLEFT,
        TOPRIGHT
    };

    // ID used to identify menu items.
    static const int kMenuItemViewID;

    MenuItemView();
    virtual ~MenuItemView();

    void AppendMenuItem(views::View* view);

    // Sets whether this item is selected. This is invoked as the user moves
    // the mouse around the menu while open.
    void SetSelected(bool selected);

    // Returns true if the item is selected.
    bool IsSelected() const { return selected_; }

    SubmenuView* CreateSubmenu();

    // Returns true if this menu item has a submenu.
    virtual bool HasSubmenu() const;

    // Returns the view containing child menu items.
    virtual SubmenuView* GetSubmenu() const;

    // Returns the parent menu item.
    MenuItemView* GetParentMenuItem() { return parent_menu_item_; }
    const MenuItemView* GetParentMenuItem() const { return parent_menu_item_; }

    // Returns the object responsible for controlling showing the menu.
    MenuController* GetMenuController();
    const MenuController* GetMenuController() const;

    void SetMenuController(MenuController* controller);

    // Returns the root parent, or this if this has no parent.
    MenuItemView* GetRootMenuItem();
    const MenuItemView* GetRootMenuItem() const;

    // Destroys the window used to display this menu and recursively destroys
    // the windows used to display all descendants.
    void DestroyAllMenuHosts();

protected:
    // The controller for the run operation, or NULL if the menu isn't showing.
    MenuController* controller_;

    // Our parent.
    MenuItemView* parent_menu_item_;

    // Whether we're selected.
    bool selected_;

    // Submenu, created via CreateSubmenu.
    SubmenuView* submenu_;
};