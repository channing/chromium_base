#pragma once

#include "ui/views/widget/root_view.h"


class MenuController;
class SubmenuView;

// MenuHostRootView is the RootView of the window showing the menu.
// SubmenuView's scroll view is added as a child of MenuHostRootView.
// MenuHostRootView forwards relevant events to the MenuController.
//
// As all the menu items are owned by the root menu item, care must be taken
// such that when MenuHostRootView is deleted it doesn't delete the menu items.
class MenuHostRootView : public views::internal::RootView {
public:
    MenuHostRootView(views::Widget* widget, SubmenuView* submenu);

    void ClearSubmenu() { submenu_ = NULL; }

    // Overridden from View:
    virtual bool OnMousePressed(const views::MouseEvent& event) OVERRIDE;
    //virtual bool OnMouseDragged(const views::MouseEvent& event) OVERRIDE;
    virtual void OnMouseReleased(const views::MouseEvent& event) OVERRIDE;
    virtual void OnMouseMoved(const views::MouseEvent& event) OVERRIDE;
    virtual bool OnMouseWheel(const views::MouseWheelEvent& event) OVERRIDE;

private:
    // Returns the MenuController for this MenuHostRootView.
    MenuController* GetMenuController();

    // The SubmenuView we contain.
    SubmenuView* submenu_;

    // Whether mouse dragged/released should be forwarded to the MenuController.
    bool forward_drag_to_menu_controller_;

    DISALLOW_COPY_AND_ASSIGN(MenuHostRootView);
};