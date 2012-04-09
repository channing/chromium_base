#pragma once

// Delegate for MenuItemView. Owned by MenuItemView

class MenuItemDelegate {
public:
    virtual ~MenuItemDelegate() {}

    virtual void Execute() {};
    virtual void WillShowMenu() {};
};