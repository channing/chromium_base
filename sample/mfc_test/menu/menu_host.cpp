#include "stdafx.h"
#include "menu_host.h"

#include "ui/views/widget/native_widget_private.h"
#include "ui/views/widget/widget.h"
#include "submenu_view.h"


////////////////////////////////////////////////////////////////////////////////
// MenuHost, public:

MenuHost::MenuHost(SubmenuView* submenu)
    : submenu_(submenu),
    destroying_(false) {
}

MenuHost::~MenuHost() {
}

void MenuHost::InitMenuHost(Widget* parent,
    const gfx::Rect& bounds,
    views::View* contents_view) {
        Widget::InitParams params(Widget::InitParams::TYPE_MENU);
        params.has_dropshadow = true;
        params.parent_widget = parent;
        params.bounds = bounds;
        Init(params);
        SetContentsView(contents_view);
        Show();
}

bool MenuHost::IsMenuHostVisible() {
    return IsVisible();
}

void MenuHost::HideMenuHost() {
    Hide();
}

void MenuHost::DestroyMenuHost() {
    HideMenuHost();
    destroying_ = true;
    Close();
}

void MenuHost::SetMenuHostBounds(const gfx::Rect& bounds) {
    SetBounds(bounds);
}

////////////////////////////////////////////////////////////////////////////////
// MenuHost, Widget overrides:

void MenuHost::OnNativeWidgetDestroyed() {
    if (!destroying_) {
        // We weren't explicitly told to destroy ourselves, which means the menu was
        // deleted out from under us (the window we're parented to was closed). Tell
        // the SubmenuView to drop references to us.
        submenu_->MenuHostDestroyed();
    }
    Widget::OnNativeWidgetDestroyed();
}