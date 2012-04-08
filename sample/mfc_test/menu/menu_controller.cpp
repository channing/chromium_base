#include "stdafx.h"
#include "menu_controller.h"
#include "menu_item_view.h"
#include "submenu_view.h"
#include "ui\gfx\screen.h"
#include "ui\base\events.h"

using base::Time;
using base::TimeDelta;
using ui::OSExchangeData;

// Period of the scroll timer (in milliseconds).
static const int kScrollTimerMS = 30;

// Delay, in ms, between when menus are selected are moused over and the menu
// appears.
static const int kShowDelay = 400;

// Amount to inset submenus.
static const int kSubmenuHorizontalInset = 3;

static const int kMaxMenuWidth = 500;

// Recurses through the child views of |view| returning the first view starting
// at |start| that is focusable. A value of -1 for |start| indicates to start at
// the first view (if |forward| is false, iterating starts at the last view). If
// |forward| is true the children are considered first to last, otherwise last
// to first.
static views::View* GetFirstFocusableView(views::View* view, int start, bool forward) {
    if (forward) {
        for (int i = start == -1 ? 0 : start; i < view->child_count(); ++i) {
            views::View* deepest = GetFirstFocusableView(view->child_at(i), -1, forward);
            if (deepest)
                return deepest;
        }
    } else {
        for (int i = start == -1 ? view->child_count() - 1 : start; i >= 0; --i) {
            views::View* deepest = GetFirstFocusableView(view->child_at(i), -1, forward);
            if (deepest)
                return deepest;
        }
    }
    return view->IsFocusable() ? view : NULL;
}

// Returns the first child of |start| that is focusable.
static views::View* GetInitialFocusableView(views::View* start, bool forward) {
    return GetFirstFocusableView(start, -1, forward);
}

// Returns the next view after |start_at| that is focusable. Returns NULL if
// there are no focusable children of |ancestor| after |start_at|.
static views::View* GetNextFocusableView(
    views::View* ancestor,
    views::View* start_at,
    bool forward)
{
    DCHECK(ancestor->Contains(start_at));
    views::View* parent = start_at;
    do {
        views::View* new_parent = parent->parent();
        int index = new_parent->GetIndexOf(parent);
        index += forward ? 1 : -1;
        if (forward || index != -1) {
            views::View* next = GetFirstFocusableView(new_parent, index, forward);
            if (next)
                return next;
        }
        parent = new_parent;
    } while (parent != ancestor);
    return NULL;
}

// MenuController:State ------------------------------------------------------

MenuController::State::State()
    : item(NULL),
    submenu_open(false),
    anchor(MenuItemView::TOPLEFT) {}

MenuController::State::~State() {}
MenuController* MenuController::active_instance_ = NULL;


MenuController* MenuController::GetActiveInstance() {
    return active_instance_;
}

void MenuController::Run(views::Widget* parent, MenuItemView* root, const gfx::Rect& bounds, MenuItemView::AnchorPosition position) {
    HHOOK send_message_hook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, 0, GetCurrentThreadId());

    root->SetMenuController(this);

    // Reset current state.
    pending_state_ = State();
    state_ = State();
    UpdateInitialLocation(bounds, position);
    owner_ = parent;

    // Set the selection, which opens the initial menu.
    SetSelection(root, SELECTION_OPEN_SUBMENU | SELECTION_UPDATE_IMMEDIATELY);

    {
        MessageLoopForUI* loop = MessageLoopForUI::current();
        MessageLoop::ScopedNestableTaskAllower allow(loop);
        loop->RunWithDispatcher(this);
    }

    root->SetMenuController(NULL);

    if (send_message_hook) {
        UnhookWindowsHookEx(send_message_hook);
    }
}

void MenuController::Cancel(ExitType type) {
    // If the menu has already been destroyed, no further cancellation is
    // needed.  We especially don't want to set the |exit_type_| to a lesser
    // value.
    if (exit_type_ == EXIT_DESTROYED || exit_type_ == type)
        return;

    SetExitType(type);

    // TODO: Hide menu
}

void MenuController::OnMousePressed(SubmenuView* source,
    const views::MouseEvent& event)
{
    MenuPart part = GetMenuPart(source, event.location());
    if (part.is_scroll())
        return;  // Ignore presses on scroll buttons.

    // On a press we immediately commit the selection, that way a submenu
    // pops up immediately rather than after a delay.
    int selection_types = SELECTION_UPDATE_IMMEDIATELY;
    if (!part.menu) {
        part.menu = part.parent;
        selection_types |= SELECTION_OPEN_SUBMENU;
    } else {
        //if (part.menu->GetDelegate()->CanDrag(part.menu)) {
        //  possible_drag_ = true;
        //  press_pt_ = event.location();
        //}
        if (part.menu->HasSubmenu())
            selection_types |= SELECTION_OPEN_SUBMENU;
    }
    SetSelection(part.menu, selection_types);
}

void MenuController::SetSelection(MenuItemView* menu_item, int selection_types) {
    size_t paths_differ_at = 0;
    std::vector<MenuItemView*> current_path;
    std::vector<MenuItemView*> new_path;
    BuildPathsAndCalculateDiff(pending_state_.item, menu_item, &current_path,
        &new_path, &paths_differ_at);

    size_t current_size = current_path.size();
    size_t new_size = new_path.size();

    for (size_t i = paths_differ_at; i < current_size; ++i) {
        current_path[i]->SetSelected(false);
    }

    // Notify the new path it is selected.
    for (size_t i = paths_differ_at; i < new_size; ++i)
        new_path[i]->SetSelected(true);

    // TODO(sky): convert back to DCHECK when figure out 93471.
    DCHECK(menu_item || (selection_types & SELECTION_EXIT) != 0);

    pending_state_.item = menu_item;
    pending_state_.submenu_open = (selection_types & SELECTION_OPEN_SUBMENU) != 0;

    // Stop timers.
    StopShowTimer();
    //StopCancelAllTimer();

    if (selection_types & SELECTION_UPDATE_IMMEDIATELY)
        CommitPendingSelection();
    else
        StartShowTimer();
}

bool MenuController::Dispatch(const MSG& msg) {
    if (exit_type_ == EXIT_ALL || exit_type_ == EXIT_DESTROYED) {
        // We must translate/dispatch the message here, otherwise we would drop
        // the message on the floor.
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        return false;
    }

    // NOTE: we don't get WM_ACTIVATE or anything else interesting in here.
    switch (msg.message) {
        //case WM_CONTEXTMENU: {
        //    MenuItemView* item = pending_state_.item;
        //    if (item && item->GetRootMenuItem() != item) {
        //        gfx::Point screen_loc(0, item->height());
        //        View::ConvertPointToScreen(item, &screen_loc);
        //        item->GetDelegate()->ShowContextMenu(item, item->GetCommand(),
        //            screen_loc, false);
        //    }
        //    return true;
        //                     }

        //                     // NOTE: focus wasn't changed when the menu was shown. As such, don't
        //                     // dispatch key events otherwise the focused window will get the events.
        //                     
    case WM_LBUTTONDOWN: {
        POINT native_point = {GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam)};
        ClientToScreen(msg.hwnd, &native_point);
        gfx::Point screen_loc(native_point);
        MenuPart part = GetMenuPartByScreenCoordinateUsingMenu(state_.item, screen_loc);
        if (part.type == MenuPart::NONE ||
            (part.type == MenuPart::MENU_ITEM && part.menu &&
            part.menu->GetRootMenuItem() != state_.item->GetRootMenuItem())) {
                // Mouse wasn't pressed over any menu, or the active menu, cancel.
                Cancel(EXIT_ALL);
        }
        break;

                         }
    case WM_KEYDOWN: {
        bool result = OnKeyDown(ui::KeyboardCodeFromNative(msg));
        TranslateMessage(&msg);
        return result;
                     }
                     //case WM_CHAR:
                     //    return !SelectByChar(static_cast<char16>(msg.wParam));
                     //    

    case WM_KEYUP:
        return true;

    case WM_SYSKEYUP:
        // We may have been shown on a system key, as such don't do anything
        // here. If another system key is pushed we'll get a WM_SYSKEYDOWN and
        // close the menu.
        return true;

    case WM_CANCELMODE:
    case WM_SYSKEYDOWN:
        // Exit immediately on system keys.
        Cancel(EXIT_ALL);
        return false;

    default:
        break;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    return exit_type_ == EXIT_NONE;
}


bool MenuController::OnKeyDown(ui::KeyboardCode key_code) {
    switch (key_code) {
    case ui::VKEY_UP:
        IncrementSelection(-1);
        break;

    case ui::VKEY_DOWN:
        IncrementSelection(1);
        break;

    case ui::VKEY_RIGHT:
        OpenSubmenuChangeSelectionIfCan();
        break;

    case ui::VKEY_LEFT:
        CloseSubmenu();
        break;

        //case ui::VKEY_SPACE:
        //  if (SendAcceleratorToHotTrackedView() == ACCELERATOR_PROCESSED_EXIT)
        //    return false;
        //  break;

        //case ui::VKEY_RETURN:
        //  if (pending_state_.item) {
        //    if (pending_state_.item->HasSubmenu()) {
        //      OpenSubmenuChangeSelectionIfCan();
        //    } else {
        //      SendAcceleratorResultType result = SendAcceleratorToHotTrackedView();
        //      if (result == ACCELERATOR_NOT_PROCESSED &&
        //          pending_state_.item->enabled()) {
        //        Accept(pending_state_.item, 0);
        //        return false;
        //      } else if (result == ACCELERATOR_PROCESSED_EXIT) {
        //        return false;
        //      }
        //    }
        //  }
        //  break;

    case ui::VKEY_ESCAPE:
        if (!state_.item->GetParentMenuItem() ||
            (!state_.item->GetParentMenuItem()->GetParentMenuItem() &&
            (!state_.item->HasSubmenu() ||
            !state_.item->GetSubmenu()->IsShowing()))) {
                // User pressed escape and only one menu is shown, cancel it.
                Cancel(EXIT_OUTERMOST);
                return false;
        }
        CloseSubmenu();
        break;

    case VK_APPS:
        break;

    default:
        break;
    }
    return true;
}
MenuController::MenuController() :
showing_(false),
    exit_type_(EXIT_NONE),
    owner_(NULL),
    showing_submenu_(false)
{
    active_instance_ = this;
}

MenuController::~MenuController() {
    DCHECK(!showing_);
    if (active_instance_ == this) {
        active_instance_ = NULL;
    }
}

void MenuController::UpdateInitialLocation(
    const gfx::Rect& bounds,
    MenuItemView::AnchorPosition position) {
        pending_state_.initial_bounds = bounds;
        if (bounds.height() > 1) {
            // Inset the bounds slightly, otherwise drag coordinates don't line up
            // nicely and menus close prematurely.
            pending_state_.initial_bounds.Inset(0, 1);
        }

        pending_state_.anchor = position;

        // Calculate the bounds of the monitor we'll show menus on. Do this once to
        // avoid repeated system queries for the info.
        pending_state_.monitor_bounds = gfx::Screen::GetMonitorWorkAreaNearestPoint(
            bounds.origin());
}

MenuItemView* MenuController::GetMenuItemAt(views::View* source, int x, int y) {
    // Walk the view hierarchy until we find a menu item (or the root).
    views::View* child_under_mouse = source->GetEventHandlerForPoint(gfx::Point(x, y));
    while (child_under_mouse &&
        child_under_mouse->id() != MenuItemView::kMenuItemViewID) {
            child_under_mouse = child_under_mouse->parent();
    }
    if (child_under_mouse && child_under_mouse->enabled() &&
        child_under_mouse->id() == MenuItemView::kMenuItemViewID) {
            return static_cast<MenuItemView*>(child_under_mouse);
    }
    return NULL;
}

MenuController::MenuPart MenuController::GetMenuPart(
    SubmenuView* source,
    const gfx::Point& source_loc) {
        gfx::Point screen_loc(source_loc);
        views::View::ConvertPointToScreen(source, &screen_loc);
        return GetMenuPartByScreenCoordinateUsingMenu(state_.item, screen_loc);
}

MenuController::MenuPart MenuController::GetMenuPartByScreenCoordinateUsingMenu(
    MenuItemView* item,
    const gfx::Point& screen_loc) {
        MenuPart part;
        for (; item; item = item->GetParentMenuItem()) {
            if (item->HasSubmenu() && item->GetSubmenu()->IsShowing() &&
                GetMenuPartByScreenCoordinateImpl(item->GetSubmenu(), screen_loc,
                &part)) {
                    return part;
            }
        }
        return part;
}

bool MenuController::GetMenuPartByScreenCoordinateImpl(
    SubmenuView* menu,
    const gfx::Point& screen_loc,
    MenuPart* part) {
        // Is the mouse over the scroll buttons?
        gfx::Point scroll_view_loc = screen_loc;
        //View* scroll_view_container = menu->GetScrollViewContainer();
        views::View* scroll_view_container = menu;
        views::View::ConvertPointToView(NULL, scroll_view_container, &scroll_view_loc);
        if (scroll_view_loc.x() < 0 ||
            scroll_view_loc.x() >= scroll_view_container->width() ||
            scroll_view_loc.y() < 0 ||
            scroll_view_loc.y() >= scroll_view_container->height()) {
                // Point isn't contained in menu.
                return false;
        }
        //if (IsScrollButtonAt(menu, scroll_view_loc.x(), scroll_view_loc.y(),
        //                     &(part->type))) {
        //  part->submenu = menu;
        //  return true;
        //}

        // Not over the scroll button. Check the actual menu.
        if (DoesSubmenuContainLocation(menu, screen_loc)) {
            gfx::Point menu_loc = screen_loc;
            views::View::ConvertPointToView(NULL, menu, &menu_loc);
            part->menu = GetMenuItemAt(menu, menu_loc.x(), menu_loc.y());
            part->type = MenuPart::MENU_ITEM;
            part->submenu = menu;
            if (!part->menu)
                part->parent = menu->GetMenuItem();
            return true;
        }

        // While the mouse isn't over a menu item or the scroll buttons of menu, it
        // is contained by menu and so we return true. If we didn't return true other
        // menus would be searched, even though they are likely obscured by us.
        return true;
}

bool MenuController::DoesSubmenuContainLocation(SubmenuView* submenu,
    const gfx::Point& screen_loc) {
        gfx::Point view_loc = screen_loc;
        views::View::ConvertPointToView(NULL, submenu, &view_loc);
        gfx::Rect vis_rect = submenu->GetVisibleBounds();
        return vis_rect.Contains(view_loc.x(), view_loc.y());
}

void MenuController::CommitPendingSelection() {
    StopShowTimer();

    size_t paths_differ_at = 0;
    std::vector<MenuItemView*> current_path;
    std::vector<MenuItemView*> new_path;
    BuildPathsAndCalculateDiff(state_.item, pending_state_.item, &current_path,
        &new_path, &paths_differ_at);

    // Hide the old menu.
    for (size_t i = paths_differ_at; i < current_path.size(); ++i) {
        if (current_path[i]->HasSubmenu()) {
            current_path[i]->GetSubmenu()->Hide();
        }
    }

    // Copy pending to state_, making sure to preserve the direction menus were
    // opened.
    std::list<bool> pending_open_direction;
    state_.open_leading.swap(pending_open_direction);
    state_ = pending_state_;
    state_.open_leading.swap(pending_open_direction);

    int menu_depth = MenuDepth(state_.item);
    if (menu_depth == 0) {
        state_.open_leading.clear();
    } else {
        int cached_size = static_cast<int>(state_.open_leading.size());
        DCHECK_GE(menu_depth, 0);
        while (cached_size-- >= menu_depth)
            state_.open_leading.pop_back();
    }

    if (!state_.item) {
        // Nothing to select.
        //StopScrolling();
        return;
    }

    // Open all the submenus preceeding the last menu item (last menu item is
    // handled next).
    if (new_path.size() > 1) {
        for (std::vector<MenuItemView*>::iterator i = new_path.begin();
            i != new_path.end() - 1; ++i) {
                OpenMenu(*i);
        }
    }

    if (state_.submenu_open) {
        // The submenu should be open, open the submenu if the item has a submenu.
        if (state_.item->HasSubmenu()) {
            OpenMenu(state_.item);
        } else {
            state_.submenu_open = false;
        }
    } else if (state_.item->HasSubmenu() &&
        state_.item->GetSubmenu()->IsShowing()) {
            state_.item->GetSubmenu()->Hide();
    }

    //if (scroll_task_.get() && scroll_task_->submenu()) {
    //  // Stop the scrolling if none of the elements of the selection contain
    //  // the menu being scrolled.
    //  bool found = false;
    //  for (MenuItemView* item = state_.item; item && !found;
    //       item = item->GetParentMenuItem()) {
    //    found = (item->HasSubmenu() && item->GetSubmenu()->IsShowing() &&
    //             item->GetSubmenu() == scroll_task_->submenu());
    //  }
    //  if (!found)
    //    StopScrolling();
    //}
}

void MenuController::CloseMenu(MenuItemView* item) {
    DCHECK(item);
    if (!item->HasSubmenu())
        return;
    item->GetSubmenu()->Hide();
}

void MenuController::OpenMenu(MenuItemView* item) {
    DCHECK(item);
    if (item->GetSubmenu()->IsShowing()) {
        return;
    }

    OpenMenuImpl(item, true);
}

void MenuController::OpenMenuImpl(MenuItemView* item, bool show) {
    // TODO(oshima|sky): Don't show the menu if drag is in progress and
    // this menu doesn't support drag drop. See crbug.com/110495.
    if (show) {
        //item->GetDelegate()->WillShowMenu(item);
    }
    bool prefer_leading =
        state_.open_leading.empty() ? true : state_.open_leading.back();
    bool resulting_direction;
    gfx::Rect bounds =
        CalculateMenuBounds(item, prefer_leading, &resulting_direction);
    state_.open_leading.push_back(resulting_direction);
    showing_submenu_ = true;
    if (show)
        item->GetSubmenu()->ShowAt(owner_, bounds);
    else
        item->GetSubmenu()->Reposition(bounds);
    showing_submenu_ = false;
}

void MenuController::MenuChildrenChanged(MenuItemView* item) {
    DCHECK(item);

    // If the current item or pending item is a descendant of the item
    // that changed, move the selection back to the changed item.
    const MenuItemView* ancestor = state_.item;
    while (ancestor && ancestor != item)
        ancestor = ancestor->GetParentMenuItem();
    if (!ancestor) {
        ancestor = pending_state_.item;
        while (ancestor && ancestor != item)
            ancestor = ancestor->GetParentMenuItem();
        if (!ancestor)
            return;
    }
    SetSelection(item, SELECTION_OPEN_SUBMENU | SELECTION_UPDATE_IMMEDIATELY);
    if (item->HasSubmenu())
        OpenMenuImpl(item, false);
}

void MenuController::BuildPathsAndCalculateDiff(
    MenuItemView* old_item,
    MenuItemView* new_item,
    std::vector<MenuItemView*>* old_path,
    std::vector<MenuItemView*>* new_path,
    size_t* first_diff_at)
{
    DCHECK(old_path && new_path && first_diff_at);
    BuildMenuItemPath(old_item, old_path);
    BuildMenuItemPath(new_item, new_path);

    size_t common_size = std::min(old_path->size(), new_path->size());

    // Find the first difference between the two paths, when the loop
    // returns, diff_i is the first index where the two paths differ.
    for (size_t i = 0; i < common_size; ++i) {
        if ((*old_path)[i] != (*new_path)[i]) {
            *first_diff_at = i;
            return;
        }
    }

    *first_diff_at = common_size;
}

void MenuController::BuildMenuItemPath(MenuItemView* item,
    std::vector<MenuItemView*>* path)
{
    if (!item)
        return;
    BuildMenuItemPath(item->GetParentMenuItem(), path);
    path->push_back(item);
}
void MenuController::StartShowTimer() {
    show_timer_.Start(FROM_HERE, TimeDelta::FromMilliseconds(kShowDelay), this,
        &MenuController::CommitPendingSelection);
}

void MenuController::StopShowTimer() {
    show_timer_.Stop();
}

gfx::Rect MenuController::CalculateMenuBounds(MenuItemView* item,
    bool prefer_leading,
    bool* is_leading) {
        DCHECK(item);

        SubmenuView* submenu = item->GetSubmenu();
        DCHECK(submenu);

        gfx::Size pref = submenu->GetPreferredSize();

        // Don't let the menu go too wide.
        if (item->actual_menu_position() != MenuItemView::POSITION_OVER_BOUNDS)
            pref.set_width(std::min(pref.width(), kMaxMenuWidth));
        //item->GetDelegate()->GetMaxWidthForMenu(item)));
        if (!state_.monitor_bounds.IsEmpty())
            pref.set_width(std::min(pref.width(), state_.monitor_bounds.width()));

        // Assume we can honor prefer_leading.
        *is_leading = prefer_leading;

        int x, y;

        if (!item->GetParentMenuItem()) {
            // First item, position relative to initial location.
            x = state_.initial_bounds.x();
            if (item->actual_menu_position() == MenuItemView::POSITION_OVER_BOUNDS)
                y = state_.initial_bounds.y();
            else
                y = state_.initial_bounds.bottom();
            if (state_.anchor == MenuItemView::TOPRIGHT)
                x = x + state_.initial_bounds.width() - pref.width();

            if (!state_.monitor_bounds.IsEmpty() &&
                pref.height() > state_.monitor_bounds.height() &&
                item->actual_menu_position() == MenuItemView::POSITION_OVER_BOUNDS) {
                    // Handle very tall menus.
                    pref.set_height(state_.monitor_bounds.height());
                    y = state_.monitor_bounds.y();
            } else if (!state_.monitor_bounds.IsEmpty() &&
                y + pref.height() > state_.monitor_bounds.bottom() &&
                item->actual_menu_position() != MenuItemView::POSITION_OVER_BOUNDS) {
                    // The menu doesn't fit on screen. The menu position with
                    // respect to the bounds will be preserved if it has already
                    // been drawn. On the first drawing if the first location is
                    // above the half way point then show from the mouse location to
                    // bottom of screen, otherwise show from the top of the screen
                    // to the location of the mouse.  While odd, this behavior
                    // matches IE.
                    if (item->actual_menu_position() == MenuItemView::POSITION_BELOW_BOUNDS ||
                        (item->actual_menu_position() == MenuItemView::POSITION_BEST_FIT &&
                        y < (state_.monitor_bounds.y() +
                        state_.monitor_bounds.height() / 2))) {
                            pref.set_height(std::min(pref.height(),
                                state_.monitor_bounds.bottom() - y));
                            item->set_actual_menu_position(MenuItemView::POSITION_BELOW_BOUNDS);
                    } else {
                        pref.set_height(std::min(pref.height(),
                            state_.initial_bounds.y() - state_.monitor_bounds.y()));
                        y = state_.initial_bounds.y() - pref.height();
                        item->set_actual_menu_position(MenuItemView::POSITION_ABOVE_BOUNDS);
                    }
            } else if (item->actual_menu_position() ==
                MenuItemView::POSITION_ABOVE_BOUNDS) {
                    pref.set_height(std::min(pref.height(),
                        state_.initial_bounds.y() - state_.monitor_bounds.y()));
                    y = state_.initial_bounds.y() - pref.height();
            } else if (item->actual_menu_position() ==
                MenuItemView::POSITION_OVER_BOUNDS) {
                    // Center vertically assuming all items have the same height.
                    int middle = state_.initial_bounds.y() - pref.height() / 2;
                    if (submenu->GetMenuItemCount() > 0)
                        middle += submenu->GetMenuItemAt(0)->GetPreferredSize().height() / 2;
                    y = std::max(state_.monitor_bounds.y(), middle);
                    if (y + pref.height() > state_.monitor_bounds.bottom())
                        y = state_.monitor_bounds.bottom() - pref.height();
            } else {
                item->set_actual_menu_position(MenuItemView::POSITION_BELOW_BOUNDS);
            }
        } else {
            // Not the first menu; position it relative to the bounds of the menu
            // item.
            gfx::Point item_loc;
            views::View::ConvertPointToScreen(item, &item_loc);

            // We must make sure we take into account the UI layout. If the layout is
            // RTL, then a 'leading' menu is positioned to the left of the parent menu
            // item and not to the right.
            bool layout_is_rtl = base::i18n::IsRTL();
            bool create_on_the_right = (prefer_leading && !layout_is_rtl) ||
                (!prefer_leading && layout_is_rtl);

            if (create_on_the_right) {
                x = item_loc.x() + item->width() - kSubmenuHorizontalInset;
                if (state_.monitor_bounds.width() != 0 &&
                    x + pref.width() > state_.monitor_bounds.right()) {
                        if (layout_is_rtl)
                            *is_leading = true;
                        else
                            *is_leading = false;
                        x = item_loc.x() - pref.width() + kSubmenuHorizontalInset;
                }
            } else {
                x = item_loc.x() - pref.width() + kSubmenuHorizontalInset;
                if (state_.monitor_bounds.width() != 0 && x < state_.monitor_bounds.x()) {
                    if (layout_is_rtl)
                        *is_leading = false;
                    else
                        *is_leading = true;
                    x = item_loc.x() + item->width() - kSubmenuHorizontalInset;
                }
            }
            y = item_loc.y() - SubmenuView::kSubmenuBorderSize;
            if (state_.monitor_bounds.width() != 0) {
                pref.set_height(std::min(pref.height(), state_.monitor_bounds.height()));
                if (y + pref.height() > state_.monitor_bounds.bottom())
                    y = state_.monitor_bounds.bottom() - pref.height();
                if (y < state_.monitor_bounds.y())
                    y = state_.monitor_bounds.y();
            }
        }

        if (state_.monitor_bounds.width() != 0) {
            if (x + pref.width() > state_.monitor_bounds.right())
                x = state_.monitor_bounds.right() - pref.width();
            if (x < state_.monitor_bounds.x())
                x = state_.monitor_bounds.x();
        }
        return gfx::Rect(x, y, pref.width(), pref.height());
}

// static
int MenuController::MenuDepth(MenuItemView* item) {
    return item ? (MenuDepth(item->GetParentMenuItem()) + 1) : 0;
}

void MenuController::IncrementSelection(int delta) {
    MenuItemView* item = pending_state_.item;
    DCHECK(item);
    if (pending_state_.submenu_open && item->HasSubmenu() &&
        item->GetSubmenu()->IsShowing()) {
            // A menu is selected and open, but none of its children are selected,
            // select the first menu item.
            if (item->GetSubmenu()->GetMenuItemCount()) {
                SetSelection(item->GetSubmenu()->GetMenuItemAt(0), SELECTION_DEFAULT);
                //ScrollToVisible(item->GetSubmenu()->GetMenuItemAt(0));
                return;
            }
    }

    MenuItemView* parent = item->GetParentMenuItem();
    if (parent) {
        int parent_count = parent->GetSubmenu()->GetMenuItemCount();
        if (parent_count > 1) {
            for (int i = 0; i < parent_count; ++i) {
                if (parent->GetSubmenu()->GetMenuItemAt(i) == item) {
                    MenuItemView* to_select =
                        FindNextSelectableMenuItem(parent, i, delta);
                    if (!to_select)
                        break;
                    //ScrollToVisible(to_select);
                    SetSelection(to_select, SELECTION_DEFAULT);
                    break;
                }
            }
        }
    }
}

MenuItemView* MenuController::FindNextSelectableMenuItem(MenuItemView* parent,
    int index,
    int delta) {
        int start_index = index;
        int parent_count = parent->GetSubmenu()->GetMenuItemCount();
        // Loop through the menu items skipping any invisible menus. The loop stops
        // when we wrap or find a visible child.
        do {
            index = (index + delta + parent_count) % parent_count;
            if (index == start_index)
                return NULL;
            MenuItemView* child = parent->GetSubmenu()->GetMenuItemAt(index);
            if (child->visible())
                return child;
        } while (index != start_index);
        return NULL;
}

void MenuController::OpenSubmenuChangeSelectionIfCan() {
    MenuItemView* item = pending_state_.item;
    if (item->HasSubmenu() && item->enabled()) {
        if (item->GetSubmenu()->GetMenuItemCount() > 0) {
            SetSelection(item->GetSubmenu()->GetMenuItemAt(0),
                SELECTION_UPDATE_IMMEDIATELY);
        } else {
            // No menu items, just show the sub-menu.
            SetSelection(item, SELECTION_OPEN_SUBMENU | SELECTION_UPDATE_IMMEDIATELY);
        }
    }
}

void MenuController::CloseSubmenu() {
    MenuItemView* item = state_.item;
    DCHECK(item);
    if (!item->GetParentMenuItem())
        return;
    if (item->HasSubmenu() && item->GetSubmenu()->IsShowing())
        SetSelection(item, SELECTION_UPDATE_IMMEDIATELY);
    else if (item->GetParentMenuItem()->GetParentMenuItem())
        SetSelection(item->GetParentMenuItem(), SELECTION_UPDATE_IMMEDIATELY);
}

void MenuController::SetExitType(ExitType type) {
    exit_type_ = type;
}

LRESULT CALLBACK MenuController::CallWndProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        CWPSTRUCT* cwp = (CWPSTRUCT*) lParam;
        // application deactivate
        if (cwp->message == WM_ACTIVATEAPP && !cwp->wParam) {
            MenuController* menu_controller = MenuController::GetActiveInstance();
            if (menu_controller) {
                menu_controller->OnDeactivateApplication();
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void MenuController::OnDeactivateApplication() {
    Cancel(EXIT_ALL);
}
