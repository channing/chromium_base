#include "stdafx.h"
#include "submenu_view.h"

#include <algorithm>
#include "ui\gfx\insets.h"
#include "menu_host.h"
#include "menu_item_view.h"
#include "menu_controller.h"

namespace {
    const int kMenuWidth = 400;
}

const int SubmenuView::kSubmenuBorderSize = 3;
SubmenuView::SubmenuView(MenuItemView* parent)
    : parent_menu_item_(parent),
    host_(NULL)
{
    DCHECK(parent);
    set_parent_owned(false);
}

SubmenuView::~SubmenuView() {
    Close();
}

int SubmenuView::GetMenuItemCount() {
    int count = 0;
    for (int i = 0; i < child_count(); ++i) {
        if (child_at(i)->id() == MenuItemView::kMenuItemViewID)
            count++;
    }
    return count;
}

MenuItemView* SubmenuView::GetMenuItemAt(int index) {
    for (int i = 0, count = 0; i < child_count(); ++i) {
        if (child_at(i)->id() == MenuItemView::kMenuItemViewID &&
            count++ == index) {
                return static_cast<MenuItemView*>(child_at(i));
        }
    }
    NOTREACHED();
    return NULL;
}

void SubmenuView::Layout() {
    gfx::Insets insets = GetInsets();
    int x = insets.left();
    int y = insets.top();
    int menu_item_width = width() - insets.width();
    for (int i = 0; i < child_count(); ++i) {
        View* child = child_at(i);
        if (child->visible()) {
            gfx::Size child_pref_size = child->GetPreferredSize();
            child->SetBounds(x, y, menu_item_width, child_pref_size.height());
            y += child_pref_size.height();
        }
    }
}

gfx::Size SubmenuView::GetPreferredSize() {
    if (!has_children())
        return gfx::Size();

    int max_width = 0;
    int height = 0;
    for (int i = 0; i < child_count(); ++i) {
        View* child = child_at(i);
        gfx::Size child_pref_size = child->visible() ? child->GetPreferredSize()
            : gfx::Size();
        max_width = std::max(max_width, child_pref_size.width());
        height += child_pref_size.height();
    }
    gfx::Insets insets = GetInsets();
    return gfx::Size(
        max_width + insets.width(),
        height + insets.height());
}


bool SubmenuView::IsShowing() {
  return host_ && host_->IsMenuHostVisible();
}
void SubmenuView::ShowAt(views::Widget* parent, const gfx::Rect& bounds) {
    if (host_) {
        host_->Show();
    } else {
        host_ = new MenuHost(this);
        host_->InitMenuHost(parent, bounds, this);
    }
}

void SubmenuView::Reposition(const gfx::Rect& bounds) {
    if (host_)
        host_->SetMenuHostBounds(bounds);
}

void SubmenuView::Close() {
    if (host_) {
        host_->DestroyMenuHost();
        host_ = NULL;
    }
}

void SubmenuView::Hide() {
    if (host_)
        host_->HideMenuHost();
}

MenuItemView* SubmenuView::GetMenuItem() const {
    return parent_menu_item_;
}


//MenuScrollViewContainer* SubmenuView::GetScrollViewContainer() {
//  if (!scroll_view_container_) {
//    scroll_view_container_ = new MenuScrollViewContainer(this);
//    // Otherwise MenuHost would delete us.
//    scroll_view_container_->set_parent_owned(false);
//  }
//  return scroll_view_container_;
//}

void SubmenuView::MenuHostDestroyed() {
    host_ = NULL;
}

bool SubmenuView::OnMousePressed(const views::MouseEvent& event) {
    if (!views::View::OnMousePressed(event)) {
        GetMenuItem()->GetMenuController()->OnMousePressed(this, event);
    }
    return true;
}