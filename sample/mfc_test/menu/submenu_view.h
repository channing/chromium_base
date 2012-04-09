#pragma once

#include "ui/views/view.h"

class MenuItemView;
class MenuHost;
class MenuScrollViewContainer;

class SubmenuView : public views::View {
public:
    // Creates a SubmenuView for the specified menu item.
    explicit SubmenuView(MenuItemView* parent);
    virtual ~SubmenuView();

    // Returns the number of child views that are MenuItemViews.
    // MenuItemViews are identified by ID.
    int GetMenuItemCount();

    // Returns the MenuItemView at the specified index.
    MenuItemView* GetMenuItemAt(int index);

    // Positions and sizes the child views. This tiles the views vertically,
    // giving each child the available width.
    virtual void Layout() OVERRIDE;
    virtual gfx::Size GetPreferredSize() OVERRIDE;

    // Returns true if the menu is showing.
    bool IsShowing();
    // Shows the menu at the specified location. Coordinates are in screen
    // coordinates. max_width gives the max width the view should be.
    void ShowAt(views::Widget* parent, const gfx::Rect& bounds);

    // Resets the bounds of the submenu to |bounds|.
    void Reposition(const gfx::Rect& bounds);

    // Closes the menu, destroying the host.
    void Close();

    // Hides the hosting window.
    //
    // The hosting window is hidden first, then deleted (Close) when the menu is
    // done running. This is done to avoid deletion ordering dependencies. In
    // particular, during drag and drop (and when a modal dialog is shown as
    // a result of choosing a context menu) it is possible that an event is
    // being processed by the host, so that host is on the stack when we need to
    // close the window. If we closed the window immediately (and deleted it),
    // when control returned back to host we would crash as host was deleted.
    void Hide();
    // Returns the parent menu item we're showing children for.
    MenuItemView* GetMenuItem() const;



    // Returns the container for the SubmenuView.
    MenuScrollViewContainer* GetScrollViewContainer();

    // Invoked if the menu is prematurely destroyed. This can happen if the window
    // closes while the menu is shown. If invoked the SubmenuView must drop all
    // references to the MenuHost as the MenuHost is about to be deleted.
    void MenuHostDestroyed();

    void ScrollDown(int delta_y);
    virtual void ScrollRectToVisible(const gfx::Rect& rect) override;

    // Padding around the edges of the submenu.
    static const int kSubmenuBorderSize;

private:
    // Parent menu item.
    MenuItemView* parent_menu_item_;

    // Widget subclass used to show the children. This is deleted when we invoke
    // |DestroyMenuHost|, or |MenuHostDestroyed| is invoked back on us.
    MenuHost* host_;

    // Ancestor of the SubmenuView, lazily created.
    MenuScrollViewContainer* scroll_view_container_;

    DISALLOW_COPY_AND_ASSIGN(SubmenuView);

};