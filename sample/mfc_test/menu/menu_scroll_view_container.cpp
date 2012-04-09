#include "menu_scroll_view_container.h"

#if defined(OS_WIN)
#include <windows.h>
#include <uxtheme.h>
#include <Vssym32.h>
#endif

#include "submenu_view.h"
#include "menu_item_view.h"
#include "menu_controller.h"

#include "ui\base\dragdrop\drag_drop_types.h"
#include "ui\gfx\canvas.h"
#include "ui\gfx\color_utils.h"


// Height of the scroll arrow.
// This goes up to 4 with large fonts, but this is close enough for now.
static const int scroll_arrow_height = 3;

static const int scroll_button_height = 20;

namespace {

    // MenuScrollButton ------------------------------------------------------------

    // MenuScrollButton is used for the scroll buttons when not all menu items fit
    // on screen. MenuScrollButton forwards appropriate events to the
    // MenuController.

    class MenuScrollButton : public views::View {
    public:
        MenuScrollButton(SubmenuView* host, bool is_up)
            : host_(host),
            is_up_(is_up),
            // Make our height the same as that of other MenuItemViews.
            pref_height_(scroll_button_height) {
        }

        virtual gfx::Size GetPreferredSize() {
            return gfx::Size(scroll_arrow_height * 2 - 1,
                pref_height_);
        }

        virtual bool CanDrop(const OSExchangeData& data) {
            DCHECK(host_->GetMenuItem()->GetMenuController());
            return true;  // Always return true so that drop events are targeted to us.
        }

        virtual void OnDragEntered(const views::DropTargetEvent& event) {
            DCHECK(host_->GetMenuItem()->GetMenuController());
            //host_->GetMenuItem()->GetMenuController()->OnDragEnteredScrollButton(
            //    host_, is_up_);
        }

        virtual int OnDragUpdated(const views::DropTargetEvent& event) {
            return ui::DragDropTypes::DRAG_NONE;
        }

        virtual void OnDragExited() {
            DCHECK(host_->GetMenuItem()->GetMenuController());
            //host_->GetMenuItem()->GetMenuController()->OnDragExitedScrollButton(host_);
        }

        virtual int OnPerformDrop(const views::DropTargetEvent& event) {
            return ui::DragDropTypes::DRAG_NONE;
        }

        virtual void OnPaint(gfx::Canvas* canvas) {
            SkColor arrow_color = color_utils::GetSysSkColor(COLOR_MENUTEXT);

            // Then the arrow.
            int x = width() / 2;
            int y = (height() - scroll_arrow_height) / 2;
            int delta_y = 1;
            if (!is_up_) {
                delta_y = -1;
                y += scroll_arrow_height;
            }
            for (int i = 0; i < scroll_arrow_height; ++i, --x, y += delta_y)
                canvas->FillRect(arrow_color, gfx::Rect(x, y, (i * 2) + 1, 1));
        }

        virtual void OnMouseMoved(const views::MouseEvent& event) {
            views::MouseEvent e(event, this, host_->GetScrollViewContainer());
            host_->GetMenuItem()->GetMenuController()->OnMouseMoved(host_, e);
        }

    private:
        // SubmenuView we were created for.
        SubmenuView* host_;

        // Direction of the button.
        bool is_up_;

        // Preferred height.
        int pref_height_;

        DISALLOW_COPY_AND_ASSIGN(MenuScrollButton);
    };

}  // namespace

// MenuScrollView --------------------------------------------------------------

// MenuScrollView is a viewport for the SubmenuView. It's reason to exist is so
// that ScrollRectToVisible works.
//
// NOTE: It is possible to use ScrollView directly (after making it deal with
// null scrollbars), but clicking on a child of ScrollView forces the window to
// become active, which we don't want. As we really only need a fraction of
// what ScrollView does, so we use a one off variant.

class MenuScrollViewContainer::MenuScrollView : public View {
public:
    explicit MenuScrollView(MenuScrollViewContainer* container, View* child) {
        container_ = container;
        AddChildView(child);
    }

    // Returns the contents, which is the SubmenuView.
    View* GetContents() {
        return child_at(0);
    }

private:
    MenuScrollViewContainer* container_;

    DISALLOW_COPY_AND_ASSIGN(MenuScrollView);
};

// MenuScrollViewContainer ----------------------------------------------------

MenuScrollViewContainer::MenuScrollViewContainer(SubmenuView* content_view)
    : content_view_(content_view)
{
    // make this at the bottom of z order
    scroll_view_ = new MenuScrollView(this, content_view);
    AddChildView(scroll_view_);

    scroll_up_button_ = new MenuScrollButton(content_view, true);
    scroll_down_button_ = new MenuScrollButton(content_view, false);
    AddChildView(scroll_up_button_);
    AddChildView(scroll_down_button_);

    set_border(views::Border::CreateEmptyBorder(
        SubmenuView::kSubmenuBorderSize,
        SubmenuView::kSubmenuBorderSize,
        SubmenuView::kSubmenuBorderSize,
        SubmenuView::kSubmenuBorderSize));
}

void MenuScrollViewContainer::OnPaintBackground(gfx::Canvas* canvas) {
    if (background()) {
        View::OnPaintBackground(canvas);
        return;
    }

    gfx::Rect bounds(0, 0, width(), height());

}

void MenuScrollViewContainer::Layout() {
    gfx::Insets insets = GetInsets();
    int x = insets.left();
    int y = insets.top();
    int width = View::width() - insets.width();
    int content_height = height() - insets.height();

    scroll_view_->SetBounds(x, y, width, content_height);
    scroll_view_->Layout();

    gfx::Size pref = scroll_up_button_->GetPreferredSize();
    scroll_up_button_->SetBounds(x, y, width, pref.height());

    pref = scroll_down_button_->GetPreferredSize();
    scroll_down_button_->SetBounds(x, height() - pref.height() - insets.bottom(),
        width, pref.height());
}

gfx::Size MenuScrollViewContainer::GetPreferredSize() {
    gfx::Size prefsize = scroll_view_->GetContents()->GetPreferredSize();
    gfx::Insets insets = GetInsets();
    prefsize.Enlarge(insets.width(), insets.height());
    return prefsize;
}

void MenuScrollViewContainer::OnBoundsChanged(
    const gfx::Rect& previous_bounds)
{
    gfx::Size content_pref = scroll_view_->GetContents()->GetPreferredSize();
    scroll_up_button_->SetVisible(false);
    scroll_down_button_->SetVisible(content_pref.height() > height());
    Layout();
}