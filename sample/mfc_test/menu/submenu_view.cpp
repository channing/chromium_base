#include "stdafx.h"
#include "submenu_view.h"

#include <algorithm>
#include "ui\gfx\insets.h"
#include "menu_host.h"
#include "menu_item_view.h"
#include "menu_controller.h"
#include "menu_scroll_view_container.h"

namespace {
    const int kMenuWidth = 400;
}

const int SubmenuView::kSubmenuBorderSize = 3;

SubmenuView::SubmenuView(MenuItemView* parent)
    : parent_menu_item_(parent),
    host_(NULL),
    scroll_view_container_(NULL)
{
    DCHECK(parent);
    set_parent_owned(false);
}

SubmenuView::~SubmenuView() {
  // The menu may not have been closed yet (it will be hidden, but not
  // necessarily closed).
  Close();

  delete scroll_view_container_;
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
    // We're in a ScrollView, and need to set our width/height ourselves.
    if (!parent())
        return;

    // Use our current y, unless it means part of the menu isn't visible anymore.
    gfx::Size pref = GetPreferredSize();
    SetBounds(x(), y(), parent()->width(), pref.height());

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
        // Force construction of the scroll view container.
        GetScrollViewContainer();
        // Make sure the first row is visible.
        ScrollRectToVisible(gfx::Rect(gfx::Size(1, 1)));
        host_->InitMenuHost(parent, bounds, scroll_view_container_);
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

MenuScrollViewContainer* SubmenuView::GetScrollViewContainer() {
  if (!scroll_view_container_) {
    scroll_view_container_ = new MenuScrollViewContainer(this);
    // Otherwise MenuHost would delete us.
    scroll_view_container_->set_parent_owned(false);
  }
  return scroll_view_container_;
}

void SubmenuView::MenuHostDestroyed() {
    host_ = NULL;
}

bool SubmenuView::OnMousePressed(const views::MouseEvent& event) {
    if (!views::View::OnMousePressed(event)) {
        // convert mouse location
        views::MouseEvent e(event, this, scroll_view_container_);
        GetMenuItem()->GetMenuController()->OnMousePressed(this, e);
    }
    return true;
}

void SubmenuView::ScrollDown(int delta_y) {
    int new_y = y() - delta_y;
    // height() - parent()->height() <= y <= 0
    new_y = std::min(0, std::max(new_y, parent()->height() - height()));
    SetY(new_y);
    GetScrollViewContainer()->scroll_up_button()->SetVisible(new_y < 0);
    GetScrollViewContainer()->scroll_down_button()->SetVisible(height() + new_y > parent()->height());
}

void SubmenuView::ScrollRectToVisible(const gfx::Rect& rect) {
    views::View* scroll_up = GetScrollViewContainer()->scroll_up_button();
    int top_visible = scroll_up->visible() ? scroll_up->height() : 0;
    top_visible -= y();
    if (rect.y() < top_visible) {
        ScrollDown(rect.y() - top_visible);
    }

    views::View* scroll_down = GetScrollViewContainer()->scroll_down_button();
    int bottom_visible = scroll_down->visible() ? parent()->height() - scroll_down->height() : parent()->height();
    bottom_visible -= y();
    if (rect.bottom() > bottom_visible) {
        ScrollDown(rect.bottom() - bottom_visible);
    }
}