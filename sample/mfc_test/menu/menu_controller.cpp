#include "stdafx.h"
#include "menu_controller.h"
#include "menu_item_view.h"
#include "submenu_view.h"

MenuController* MenuController::active_instance_ = NULL;


MenuController::MenuController()
    : exit_type_(EXIT_NONE)
{
    active_instance_ = this;
}

MenuController::~MenuController() {
    if (active_instance_ == this) {
        active_instance_ = NULL;
    }
}

void MenuController::Run(views::Widget* parent, MenuItemView* root, const gfx::Rect& bounds, MenuItemView::AnchorPosition position) {
    HHOOK send_message_hook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, 0, GetCurrentThreadId());

    root->SetMenuController(this);
    gfx::Rect menu_bounds(bounds);
    menu_bounds.set_size(root->GetSubmenu()->GetPreferredSize());
    root->GetSubmenu()->ShowAt(parent, menu_bounds);

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
    //case WM_KEYDOWN: {
    //    bool result = OnKeyDown(ui::KeyboardCodeFromNative(msg));
    //    TranslateMessage(&msg);
    //    return result;
    //                 }
    //case WM_CHAR:
    //    return !SelectByChar(static_cast<char16>(msg.wParam));
    //    
    case WM_ACTIVATEAPP:
        if (!msg.wParam) {
            SetExitType(EXIT_ALL);
        }
        break;

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

void MenuController::SetExitType(ExitType type) {
    exit_type_ = type;
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

MenuController* MenuController::GetActiveInstance() {
    return active_instance_;
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
