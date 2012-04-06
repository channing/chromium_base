#pragma once

#include "base\message_loop.h"
#include "menu_item_view.h"

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

    virtual bool Dispatch( const MSG& msg );

    // Cancels the current Run. See ExitType for a description of what happens
    // with the various parameters.
    void Cancel(ExitType type);

    // An alternative to Cancel(EXIT_ALL) that can be used with a OneShotTimer.
    void CancelAll() { Cancel(EXIT_ALL); }

    // Sets exit type.
    void SetExitType(ExitType type);

private:
    // We need this hook to catch some messages send by SendMessage
    static LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam);

    void OnDeactivateApplication();

	// The active instance.
	static MenuController* active_instance_;

    // Indicates what to exit.
    ExitType exit_type_;
};