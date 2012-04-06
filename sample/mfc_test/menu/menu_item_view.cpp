#include "stdafx.h"
#include "menu_item_view.h"
#include "submenu_view.h"

const int MenuItemView::kMenuItemViewID = 1201;

MenuItemView::MenuItemView()
	: controller_(NULL),
	  parent_menu_item_(NULL),
	  selected_(false),
	  submenu_(NULL)
{
	set_id(kMenuItemViewID);
}

MenuItemView::~MenuItemView() {
	delete submenu_;
}

void MenuItemView::SetSelected(bool selected) {
	selected_ = selected;
}

void MenuItemView::AppendMenuItem(views::View* view) {
	if (!submenu_)
		CreateSubmenu();
	submenu_->AddChildView(view);
}

SubmenuView* MenuItemView::CreateSubmenu() {
	if (!submenu_)
		submenu_ = new SubmenuView(this);
	return submenu_;
}

SubmenuView* MenuItemView::GetSubmenu() const {
	return submenu_;
}

MenuItemView* MenuItemView::GetRootMenuItem() {
	return const_cast<MenuItemView*>(
		static_cast<const MenuItemView*>(this)->GetRootMenuItem());
}

const MenuItemView* MenuItemView::GetRootMenuItem() const {
	const MenuItemView* item = this;
	for (const MenuItemView* parent = GetParentMenuItem(); parent;
		parent = item->GetParentMenuItem())
		item = parent;
	return item;
}

MenuController* MenuItemView::GetMenuController() {
	return GetRootMenuItem()->controller_;
}

const MenuController* MenuItemView::GetMenuController() const {
	return GetRootMenuItem()->controller_;
}

void MenuItemView::SetMenuController(MenuController* controller) {
    controller_ = controller;
}

void MenuItemView::DestroyAllMenuHosts() {
    if (!HasSubmenu())
        return;

    submenu_->Close();
    for (int i = 0, item_count = submenu_->GetMenuItemCount(); i < item_count;
        ++i) {
            submenu_->GetMenuItemAt(i)->DestroyAllMenuHosts();
    }
}

bool MenuItemView::HasSubmenu() const {
    return (submenu_ != NULL);
}