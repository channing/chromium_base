#pragma once

#include "ui/views/view.h"


class SubmenuView;

// MenuScrollViewContainer contains the SubmenuView (through a MenuScrollView)
// and two scroll buttons. The scroll buttons are only visible and enabled if
// the preferred height of the SubmenuView is bigger than our bounds.
class MenuScrollViewContainer : public views::View {
 public:
  explicit MenuScrollViewContainer(SubmenuView* content_view);

  // Returns the buttons for scrolling up/down.
  views::View* scroll_down_button() const { return scroll_down_button_; }
  views::View* scroll_up_button() const { return scroll_up_button_; }

  // View overrides.
  virtual void OnPaintBackground(gfx::Canvas* canvas) OVERRIDE;
  virtual void Layout() OVERRIDE;
  virtual gfx::Size GetPreferredSize() OVERRIDE;

 protected:
  // View override.
  virtual void OnBoundsChanged(const gfx::Rect& previous_bounds) OVERRIDE;

 private:
  class MenuScrollView;

  // The scroll buttons.
  views::View* scroll_up_button_;
  views::View* scroll_down_button_;

  // The scroll view.
  MenuScrollView* scroll_view_;

  // The content view.
  SubmenuView* content_view_;

  DISALLOW_COPY_AND_ASSIGN(MenuScrollViewContainer);
};