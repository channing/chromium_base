#pragma once

#include "ui/views/view.h"

class MenuItemView;
class MenuHost;

class SubmenuView : public views::View {
public:
	// Creates a SubmenuView for the specified menu item.
	explicit SubmenuView(MenuItemView* parent);
	virtual ~SubmenuView();

	// Shows the menu at the specified location. Coordinates are in screen
	// coordinates. max_width gives the max width the view should be.
	void ShowAt(views::Widget* parent, const gfx::Rect& bounds);

	// Closes the menu, destroying the host.
	void Close();

	// Returns the parent menu item we're showing children for.
	MenuItemView* GetMenuItem() const;

	// Invoked if the menu is prematurely destroyed. This can happen if the window
	// closes while the menu is shown. If invoked the SubmenuView must drop all
	// references to the MenuHost as the MenuHost is about to be deleted.
	void MenuHostDestroyed();

    // Returns the number of child views that are MenuItemViews.
    // MenuItemViews are identified by ID.
    int GetMenuItemCount();

    // Returns the MenuItemView at the specified index.
    MenuItemView* GetMenuItemAt(int index);

	virtual void Layout() override;
	virtual gfx::Size GetPreferredSize() override;

private:
	// Parent menu item.
	MenuItemView* parent_menu_item_;

	// Widget subclass used to show the children. This is deleted when we invoke
	// |DestroyMenuHost|, or |MenuHostDestroyed| is invoked back on us.
	MenuHost* host_;

	DISALLOW_COPY_AND_ASSIGN(SubmenuView);
};