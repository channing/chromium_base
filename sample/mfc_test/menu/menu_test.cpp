#include "stdafx.h"
#include "menu_test.h"
#include "menu_item_view_impl.h"
#include "submenu_view.h"
#include "menu_controller.h"

void ShowOwnTestMenu(int x, int y) {
	MenuItemView* root = new MenuItemView(NULL);
	root->AppendMenuItem(new TestMenuItemView(root));
	root->AppendMenuItem(new TestMenuItemView(root));
    MenuItemView* submenu1 = new TestMenuItemView(root);
    submenu1->AppendMenuItem(new TestMenuItemView(submenu1));
    submenu1->AppendMenuItem(new TestMenuItemView(submenu1));
    submenu1->AppendMenuItem(new TestMenuItemView(submenu1));
    submenu1->AppendMenuItem(new TestMenuItemView(submenu1));
    root->AppendMenuItem(submenu1);
	root->AppendMenuItem(new TestMenuItemView(root));

    scoped_ptr<MenuItemView> root_holder(root);
    scoped_ptr<MenuController> controller(new MenuController);
    controller->Run(NULL, root, gfx::Rect(x, y, 0, 0), MenuItemView::TOPLEFT);
}