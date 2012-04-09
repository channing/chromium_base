#pragma once

#include "ui/views/view.h"
#include "menu_item_delegate.h"

class SubmenuView;
class MenuController;
class MenuItemDelegate;

class MenuItemView : public views::View {
public:
    friend class MenuController;
    // Where the menu should be anchored to for non-RTL languages.  The
    // opposite position will be used if base::i18n:IsRTL() is true.
    enum AnchorPosition {
        TOPLEFT,
        TOPRIGHT
    };

    // Where the menu should be drawn, above or below the bounds (when
    // the bounds is non-empty).  POSITION_BEST_FIT (default) positions
    // the menu below the bounds unless the menu does not fit on the
    // screen and the re is more space above.
    enum MenuPosition {
        POSITION_BEST_FIT,
        POSITION_ABOVE_BOUNDS,
        POSITION_BELOW_BOUNDS,
        POSITION_OVER_BOUNDS
    };
    // ID used to identify menu items.
    static const int kMenuItemViewID;

    MenuItemView(MenuItemView* parent, MenuItemDelegate* delegate);
    virtual ~MenuItemView();

    MenuItemDelegate* GetDelegate() {
        return delegate_.get();
    }

    void AppendMenuItem(views::View* view);

    // Returns the view that contains child menu items. If the submenu has
    // not been creates, this creates it.
    virtual SubmenuView* CreateSubmenu();
    // Returns true if this menu item has a submenu.
    virtual bool HasSubmenu() const;

    // Returns the view containing child menu items.
    virtual SubmenuView* GetSubmenu() const;

    // Returns the parent menu item.
    MenuItemView* GetParentMenuItem() { return parent_menu_item_; }
    const MenuItemView* GetParentMenuItem() const { return parent_menu_item_; }

    // Sets whether this item is selected. This is invoked as the user moves
    // the mouse around the menu while open.
    void SetSelected(bool selected);

    // Returns true if the item is selected.
    bool IsSelected() const { return selected_; }

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
    // Used by MenuController to cache the menu position in use by the
    // active menu.
    MenuPosition actual_menu_position() const { return actual_menu_position_; }
    void set_actual_menu_position(MenuPosition actual_menu_position) {
        actual_menu_position_ = actual_menu_position;
    }

    // Delegate
    scoped_ptr<MenuItemDelegate> delegate_;

    // The controller for the run operation, or NULL if the menu isn't showing.
    MenuController* controller_;

    // Our parent.
    MenuItemView* parent_menu_item_;

    // Whether we're selected.
    bool selected_;

    // Submenu, created via CreateSubmenu.
    SubmenuView* submenu_;

    // |menu_position_| is the requested position with respect to the bounds.
    // |actual_menu_position_| is used by the controller to cache the
    // position of the menu being shown.
    MenuPosition requested_menu_position_;
    MenuPosition actual_menu_position_;
};