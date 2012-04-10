#include "stdafx.h"
#include "menu_item_view.h"

#include "submenu_view.h"
#include "menu_controller.h"
#include "menu_scroll_view_container.h"
#include "base/stl_util.h"

const int MenuItemView::kMenuItemViewID = 1201;

MenuItemView::MenuItemView(MenuItemView* parent, MenuItemDelegate* delegate)
	: delegate_(delegate),
      controller_(NULL),
	  parent_menu_item_(parent),
	  selected_(false),
	  submenu_(NULL),
      requested_menu_position_(POSITION_BEST_FIT),
      actual_menu_position_(requested_menu_position_)
{
	set_id(kMenuItemViewID);
}

void MenuItemView::AppendMenuItem(views::View* view) {
    if (view->id() == kMenuItemViewID) {
        MenuItemView* menu_item = static_cast<MenuItemView*>(view);
        DCHECK(menu_item->GetParentMenuItem() == this);
    }
	if (!submenu_)
		CreateSubmenu();
	submenu_->AddChildView(view);
}

void MenuItemView::RemoveMenuItemAt(int index) {
    DCHECK(submenu_);
    DCHECK_LE(0, index);
    DCHECK_GT(submenu_->child_count(), index);

    View* item = submenu_->child_at(index);
    DCHECK(item);
    submenu_->RemoveChildView(item);

    // RemoveChildView() does not delete the item, which is a good thing
    // in case a submenu is being displayed while items are being removed.
    // Deletion will be done by ChildrenChanged() or at destruction.
    removed_items_.push_back(item);
}

SubmenuView* MenuItemView::CreateSubmenu() {
	if (!submenu_)
		submenu_ = new SubmenuView(this);
	return submenu_;
}

bool MenuItemView::HasSubmenu() const {
    return (submenu_ != NULL);
}
SubmenuView* MenuItemView::GetSubmenu() const {
	return submenu_;
}

void MenuItemView::SetSelected(bool selected) {
	selected_ = selected;
    SchedulePaint();
}


gfx::Size MenuItemView::GetPreferredSize() {
  if (pref_size_.IsEmpty())
    pref_size_ = CalculatePreferredSize();
  return pref_size_;
}

MenuController* MenuItemView::GetMenuController() {
	return GetRootMenuItem()->controller_;
}

const MenuController* MenuItemView::GetMenuController() const {
	return GetRootMenuItem()->controller_;
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

void MenuItemView::ChildrenChanged() {
  MenuController* controller = GetMenuController();
  if (controller) {
    //// Handles the case where we were empty and are no longer empty.
    //RemoveEmptyMenus();

    //// Handles the case where we were not empty, but now are.
    //AddEmptyMenus();

    controller->MenuChildrenChanged(this);

    if (submenu_) {
      // Force a paint and layout. This handles the case of the top
      // level window's size remaining the same, resulting in no
      // change to the submenu's size and no layout.
      submenu_->Layout();
      submenu_->SchedulePaint();
      // Update the menu selection after layout.
      controller->UpdateSubmenuSelection(submenu_);
    }
  }

  STLDeleteElements(&removed_items_);
}
void MenuItemView::SetMenuController(MenuController* controller) {
    controller_ = controller;
}

MenuItemView::~MenuItemView() {
	delete submenu_;
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

gfx::Size MenuItemView::CalculatePreferredSize() {
    return gfx::Size();
}
