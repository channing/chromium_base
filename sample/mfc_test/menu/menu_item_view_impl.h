#pragma once

#include "menu_item_view.h"

class TestMenuItemView : public MenuItemView {
public:
    TestMenuItemView(MenuItemView* parent)
        : MenuItemView(parent)
    {
    }

    TestMenuItemView(MenuItemView* parent, CString label)
        : MenuItemView(parent),
        label_(label)
    {
    }

	virtual void OnPaint( gfx::Canvas* canvas ) OVERRIDE;
	virtual gfx::Size GetPreferredSize() OVERRIDE;

private:
    CString label_;
};