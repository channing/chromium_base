#pragma once

#include "ui\views\widget\widget.h"

class SubmenuView;

// SubmenuView uses a MenuHost to house the SubmenuView.
//
// SubmenuView owns the MenuHost. When SubmenuView is done with the MenuHost
// |DestroyMenuHost| is invoked. The one exception to this is if the native
// OS destroys the widget out from under us, in which case |MenuHostDestroyed|
// is invoked back on the SubmenuView and the SubmenuView then drops references
// to the MenuHost.
class MenuHost : public views::Widget {
public:
    explicit MenuHost(SubmenuView* submenu);
    virtual ~MenuHost();

    // Initializes and shows the MenuHost.
    void InitMenuHost(Widget* parent,
        const gfx::Rect& bounds,
        views::View* contents_view);

    // Returns true if the menu host is visible.
    bool IsMenuHostVisible();

    // Hides the menu host.
    void HideMenuHost();
    // Destroys and deletes the menu host.
    void DestroyMenuHost();

    // Sets the bounds of the menu host.
    void SetMenuHostBounds(const gfx::Rect& bounds);

private:
    // Overridden from Widget:
    virtual views::internal::RootView* CreateRootView() OVERRIDE;
    virtual void OnNativeWidgetDestroyed() OVERRIDE;

    // The view we contain.
    SubmenuView* submenu_;

    // If true, DestroyMenuHost has been invoked.
    bool destroying_;

    DISALLOW_COPY_AND_ASSIGN(MenuHost);
};