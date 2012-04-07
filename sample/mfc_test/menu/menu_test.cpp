#include "stdafx.h"
#include "menu_test.h"
#include "menu_item_view_impl.h"
#include "submenu_view.h"
#include "menu_controller.h"

void ShowOwnTestMenu(int x, int y) {
	scoped_ptr<MenuItemView> root(new MenuItemView());
	root->AppendMenuItem(new TestMenuItemView());
	root->AppendMenuItem(new TestMenuItemView());
	root->AppendMenuItem(new TestMenuItemView());
    scoped_ptr<MenuController> controller(new MenuController);
    controller->Run(NULL, root.get(), gfx::Rect(x, y, 0, 0), MenuItemView::TOPLEFT);
}

