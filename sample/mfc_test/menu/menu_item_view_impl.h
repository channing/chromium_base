#pragma once

#include "menu_item_view.h"
#include "menu_item_delegate.h"

class TestMenuItemDelegate : public MenuItemDelegate {
public:
    TestMenuItemDelegate(CString label) : label_(label) {}

    virtual void Execute() {
        AfxMessageBox(L"You executed " + label_);
    }

private:
    CString label_;
};

class TestMenuItemView : public MenuItemView {
public:
    TestMenuItemView(MenuItemView* parent)
        : MenuItemView(parent, new TestMenuItemDelegate(L"Empty item"))
    {
    }

    TestMenuItemView(MenuItemView* parent, CString label)
        : MenuItemView(parent, new TestMenuItemDelegate(label)),
        label_(label)
    {
    }

	virtual void OnPaint( gfx::Canvas* canvas ) OVERRIDE;
	virtual gfx::Size GetPreferredSize() OVERRIDE;

private:
    CString label_;
};