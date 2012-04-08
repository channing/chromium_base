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
    submenu1->AppendMenuItem(new TestMenuItemView(submenu1, L"main 1"));
    submenu1->AppendMenuItem(new TestMenuItemView(submenu1, L"main 2"));
    submenu1->AppendMenuItem(new TestMenuItemView(submenu1, L"main 3"));
    submenu1->AppendMenuItem(new TestMenuItemView(submenu1, L"main 4"));
    root->AppendMenuItem(submenu1);
	root->AppendMenuItem(new TestMenuItemView(root));

    MenuItemView* submenu2 = new TestMenuItemView(root);
    root->AppendMenuItem(submenu2);
    for (int i = 0; i < 40; ++i) {
        CString label;
        label.Format(L"sub %d", i);
        submenu2->AppendMenuItem(new TestMenuItemView(submenu2, label));
    }

    scoped_ptr<MenuItemView> root_holder(root);
    scoped_ptr<MenuController> controller(new MenuController);
    controller->Run(NULL, root, gfx::Rect(x, y, 0, 0), MenuItemView::TOPLEFT);
}