// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "menu_host_root_view.h"

#include "menu_controller.h"
#include "submenu_view.h"

MenuHostRootView::MenuHostRootView(views::Widget* widget,
    SubmenuView* submenu)
    : views::internal::RootView(widget),
    submenu_(submenu),
    forward_drag_to_menu_controller_(true) {
}

bool MenuHostRootView::OnMousePressed(const views::MouseEvent& event) {
    forward_drag_to_menu_controller_ =
        !GetLocalBounds().Contains(event.location()) ||
        !RootView::OnMousePressed(event);
    if (forward_drag_to_menu_controller_ && GetMenuController())
        GetMenuController()->OnMousePressed(submenu_, event);
    return true;
}

//bool MenuHostRootView::OnMouseDragged(const views::MouseEvent& event) {
//  if (forward_drag_to_menu_controller_ && GetMenuController()) {
//    GetMenuController()->OnMouseDragged(submenu_, event);
//    return true;
//  }
//  return RootView::OnMouseDragged(event);
//}

void MenuHostRootView::OnMouseReleased(const views::MouseEvent& event) {
  RootView::OnMouseReleased(event);
  if (forward_drag_to_menu_controller_ && GetMenuController()) {
    forward_drag_to_menu_controller_ = false;
    GetMenuController()->OnMouseReleased(submenu_, event);
  }
}

void MenuHostRootView::OnMouseMoved(const views::MouseEvent& event) {
    RootView::OnMouseMoved(event);
    if (GetMenuController())
        GetMenuController()->OnMouseMoved(submenu_, event);
}

bool MenuHostRootView::OnMouseWheel(const views::MouseWheelEvent& event) {
    // Windows uses focus_util_win::RerouteMouseWheel to forward events to
    // the right menu.
    // RootView::OnMouseWheel forwards to the focused view. We don't have a
    // focused view, so we need to override this then forward to the menu.
    return submenu_->OnMouseWheel(event);
}

MenuController* MenuHostRootView::GetMenuController() {
    return submenu_ ? submenu_->GetMenuItem()->GetMenuController() : NULL;
}