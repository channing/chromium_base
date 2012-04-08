#pragma once

#include "base/message_loop.h"
#include "menu_item_view.h"
#include "base/timer.h"

class MenuController : public MessageLoop::Dispatcher {
public:
    // Enumeration of how the menu should exit.
    enum ExitType {
        // Don't exit.
        EXIT_NONE,

        // All menus, including nested, should be exited.
        EXIT_ALL,

        // Only the outermost menu should be exited.
        EXIT_OUTERMOST,

        // This is set if the menu is being closed as the result of one of the menus
        // being destroyed.
        EXIT_DESTROYED
    };

    MenuController();
    virtual ~MenuController();

    // If a menu is currently active, this returns the controller for it.
    static MenuController* GetActiveInstance();

    void Run(views::Widget* parent,
        MenuItemView* root,
        const gfx::Rect& bounds,
        MenuItemView::AnchorPosition position);

    // Cancels the current Run. See ExitType for a description of what happens
    // with the various parameters.
    void Cancel(ExitType type);

    // An alternative to Cancel(EXIT_ALL) that can be used with a OneShotTimer.
    void CancelAll() { Cancel(EXIT_ALL); }


    // Various events, forwarded from the submenu.
    //
    // NOTE: the coordinates of the events are in that of the
    // MenuScrollViewContainer.
    void OnMousePressed(SubmenuView* source, const views::MouseEvent& event);
  void OnMouseMoved(SubmenuView* source, const views::MouseEvent& event);
private:

    class MenuScrollTask;

    // Values supplied to SetSelection.
    enum SetSelectionTypes {
        SELECTION_DEFAULT               = 0,

        // If set submenus are opened immediately, otherwise submenus are only
        // openned after a timer fires.
        SELECTION_UPDATE_IMMEDIATELY    = 1 << 0,

        // If set and the menu_item has a submenu, the submenu is shown.
        SELECTION_OPEN_SUBMENU          = 1 << 1,

        // SetSelection is being invoked as the result exiting or cancelling the
        // menu. This is used for debugging.
        SELECTION_EXIT                  = 1 << 2,
    };

    // Tracks selection information.
    struct State {
        State();
        ~State();

        // The selected menu item.
        MenuItemView* item;

        // If item has a submenu this indicates if the submenu is showing.
        bool submenu_open;

        // Bounds passed to the run menu. Used for positioning the first menu.
        gfx::Rect initial_bounds;

        // Position of the initial menu.
        MenuItemView::AnchorPosition anchor;

        // The direction child menus have opened in.
        std::list<bool> open_leading;

        // Bounds for the monitor we're showing on.
        gfx::Rect monitor_bounds;
    };

    // Used by GetMenuPart to indicate the menu part at a particular location.
    struct MenuPart {
        // Type of part.
        enum Type {
            NONE,
            MENU_ITEM,
            SCROLL_UP,
            SCROLL_DOWN
        };

        MenuPart() : type(NONE), menu(NULL), parent(NULL), submenu(NULL) {}

        // Convenience for testing type == SCROLL_DOWN or type == SCROLL_UP.
        bool is_scroll() const { return type == SCROLL_DOWN || type == SCROLL_UP; }

        // Type of part.
        Type type;

        // If type is MENU_ITEM, this is the menu item the mouse is over, otherwise
        // this is NULL.
        // NOTE: if type is MENU_ITEM and the mouse is not over a valid menu item
        //       but is over a menu (for example, the mouse is over a separator or
        //       empty menu), this is NULL and parent is the menu the mouse was
        //       clicked on.
        MenuItemView* menu;

        // If type is MENU_ITEM but the mouse is not over a menu item this is the
        // parent of the menu item the user clicked on. Otherwise this is NULL.
        MenuItemView* parent;

        // This is the submenu the mouse is over.
        SubmenuView* submenu;
    };

    // Sets the selection to menu_item a value of NULL unselects
    // everything. |types| is a bitmask of |SetSelectionTypes|.
    //
    // Internally this updates pending_state_ immediatley. state_ is only updated
    // immediately if SELECTION_UPDATE_IMMEDIATELY is set. If
    // SELECTION_UPDATE_IMMEDIATELY is not set CommitPendingSelection is invoked
    // to show/hide submenus and update state_.
    void SetSelection(MenuItemView* menu_item, int selection_types);

    virtual bool Dispatch(const MSG& msg) OVERRIDE;

    // Key processing. The return value of this is returned from Dispatch.
    // In other words, if this returns false (which happens if escape was
    // pressed, or a matching mnemonic was found) the message loop returns.
    bool OnKeyDown(ui::KeyboardCode key_code);

    void UpdateInitialLocation(const gfx::Rect& bounds,
        MenuItemView::AnchorPosition position);

    // Gets the enabled menu item at the specified location.
    // If over_any_menu is non-null it is set to indicate whether the location
    // is over any menu. It is possible for this to return NULL, but
    // over_any_menu to be true. For example, the user clicked on a separator.
    MenuItemView* GetMenuItemAt(views::View* menu, int x, int y);


    // Returns true if the coordinate is over the scroll buttons of the
    // SubmenuView's MenuScrollViewContainer. If true is returned, part is set to
    // indicate which scroll button the coordinate is.
    bool IsScrollButtonAt(SubmenuView* source,
        int x,
        int y,
        MenuPart::Type* part);

    // Returns the target for the mouse event. The coordinates are in terms of
    // source's scroll view container.
    MenuPart GetMenuPart(SubmenuView* source, const gfx::Point& source_loc);

    // Returns the target for mouse events. The search is done through |item| and
    // all its parents.
    MenuPart GetMenuPartByScreenCoordinateUsingMenu(MenuItemView* item,
        const gfx::Point& screen_loc);

    // Implementation of GetMenuPartByScreenCoordinate for a single menu. Returns
    // true if the supplied SubmenuView contains the location in terms of the
    // screen. If it does, part is set appropriately and true is returned.
    bool GetMenuPartByScreenCoordinateImpl(SubmenuView* menu,
        const gfx::Point& screen_loc,
        MenuPart* part);

    // Returns true if the SubmenuView contains the specified location. This does
    // NOT included the scroll buttons, only the submenu view.
    bool DoesSubmenuContainLocation(SubmenuView* submenu,
        const gfx::Point& screen_loc);

    // Opens/Closes the necessary menus such that state_ matches that of
    // pending_state_. This is invoked if submenus are not opened immediately,
    // but after a delay.
    void CommitPendingSelection();

    // If item has a submenu, it is closed. This does NOT update the selection
    // in anyway.
    void CloseMenu(MenuItemView* item);

    // If item has a submenu, it is opened. This does NOT update the selection
    // in anyway.
    void OpenMenu(MenuItemView* item);

    // Implementation of OpenMenu. If |show| is true, this invokes show on the
    // menu, otherwise Reposition is invoked.
    void OpenMenuImpl(MenuItemView* item, bool show);

    // Invoked when the children of a menu change and the menu is showing.
    // This closes any submenus and resizes the submenu.
    void MenuChildrenChanged(MenuItemView* item);

    // Builds the paths of the two menu items into the two paths, and
    // sets first_diff_at to the location of the first difference between the
    // two paths.
    void BuildPathsAndCalculateDiff(MenuItemView* old_item,
        MenuItemView* new_item,
        std::vector<MenuItemView*>* old_path,
        std::vector<MenuItemView*>* new_path,
        size_t* first_diff_at);

    // Builds the path for the specified item.
    void BuildMenuItemPath(MenuItemView* item, std::vector<MenuItemView*>* path);

    // Starts/stops the timer that commits the pending state to state
    // (opens/closes submenus).
    void StartShowTimer();
    void StopShowTimer();
    // Calculates the bounds of the menu to show. is_leading is set to match the
    // direction the menu opened in.
    gfx::Rect CalculateMenuBounds(MenuItemView* item,
        bool prefer_leading,
        bool* is_leading);

    // Returns the depth of the menu.
    static int MenuDepth(MenuItemView* item);

    // Selects the next/previous menu item.
    void IncrementSelection(int delta);

    // Returns the next selectable child menu item of |parent| starting at |index|
    // and incrementing index by |delta|. If there are no more selected menu items
    // NULL is returned.
    MenuItemView* FindNextSelectableMenuItem(MenuItemView* parent,
        int index,
        int delta);

    // If the selected item has a submenu and it isn't currently open, the
    // the selection is changed such that the menu opens immediately.
    void OpenSubmenuChangeSelectionIfCan();

    // If possible, closes the submenu.
    void CloseSubmenu();

    // Starts/stops scrolling as appropriate. part gives the part the mouse is
    // over.
    void UpdateScrolling(const MenuPart& part);

    // Stops scrolling.
    void StopScrolling();

    // Sets exit type.
    void SetExitType(ExitType type);
    // Handles the mouse location event on the submenu |source|.
    void HandleMouseLocation(SubmenuView* source,
        const gfx::Point& mouse_location);

    // We need this hook to catch some messages send by SendMessage
    static LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam);

    void OnDeactivateApplication();

    // The active instance.
    static MenuController* active_instance_;

    // If true, we're showing.
    bool showing_;
    // Indicates what to exit.
    ExitType exit_type_;

    // As the user drags the mouse around pending_state_ changes immediately.
    // When the user stops moving/dragging the mouse (or clicks the mouse)
    // pending_state_ is committed to state_, potentially resulting in
    // opening or closing submenus. This gives a slight delayed effect to
    // submenus as the user moves the mouse around. This is done so that as the
    // user moves the mouse all submenus don't immediately pop.
    State pending_state_;
    State state_;

    // As the mouse moves around submenus are not opened immediately. Instead
    // they open after this timer fires.
    base::OneShotTimer<MenuController> show_timer_;

    // Owner of child windows.
    views::Widget* owner_;
    // If true, we're in the middle of invoking ShowAt on a submenu.
    bool showing_submenu_;

    // Task for scrolling the menu. If non-null indicates a scroll is currently
    // underway.
    scoped_ptr<MenuScrollTask> scroll_task_;

};