#pragma once

// Delegate for MenuItemView. MenuItemView doesn't own MenuItemDelegate

class MenuItemView;

class MenuItemDelegate {
public:
    virtual ~MenuItemDelegate() {}

    virtual string16 GetLable() {
        return string16();
    }

    virtual void Execute() {};
    virtual void WillShowMenu() {};
};