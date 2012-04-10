#pragma once

#include <vector>
#include "base\string16.h"

// Delegate for MenuItemView. MenuItemView doesn't own MenuItemDelegate

class MenuItemView;

class MenuItemModel {
public:
    virtual ~MenuItemModel();

    void SetView(MenuItemView* view);

    virtual string16 GetLable();

    virtual void Execute();
    virtual void WillShowMenu();

protected:
    typedef std::vector<MenuItemModel*> Models;

    MenuItemView* view_;
    Models children_;
};