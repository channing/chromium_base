// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/focus/accelerator_handler.h"

#include <bitset>
#include <gtk/gtk.h>
#include <X11/extensions/XInput2.h>

#include "ui/views/accelerator.h"
#include "ui/views/events/event.h"
#include "ui/views/focus/focus_manager.h"
#include "ui/views/ime/input_method.h"
#include "ui/views/touchui/touch_factory.h"
#include "ui/views/view.h"
#include "ui/views/widget/native_widget.h"

namespace views {

namespace {

Widget* FindWidgetForGdkWindow(GdkWindow* gdk_window) {
  gpointer data = NULL;
  gdk_window_get_user_data(gdk_window, &data);
  GtkWidget* gtk_widget = reinterpret_cast<GtkWidget*>(data);
  if (!gtk_widget || !GTK_IS_WIDGET(gtk_widget)) {
    DLOG(WARNING) << "no GtkWidget found for that GdkWindow";
    return NULL;
  }
  Widget* widget = Widget::GetWidgetForNativeView(gtk_widget);

  if (!widget) {
    DLOG(WARNING) << "no NativeWidgetGtk found for that GtkWidget";
    return NULL;
  }
  return widget;
}

}  // namespace

bool DispatchX2Event(Widget* widget, XEvent* xev) {
  XGenericEventCookie* cookie = &xev->xcookie;
  switch (cookie->evtype) {
    case XI_KeyPress:
    case XI_KeyRelease: {
      // TODO(sad): We don't capture XInput2 events from keyboard yet.
      break;
    }
    case XI_ButtonPress:
    case XI_ButtonRelease:
    case XI_Motion: {
      XIDeviceEvent* xievent = static_cast<XIDeviceEvent*>(cookie->data);
      Event::FromNativeEvent2 from_native;

      // Scrolling the wheel generates press/release events with button id's 4
      // and 5. In case of a wheelscroll, we do not want to show the cursor.
      if (xievent->detail == 4 || xievent->detail == 5) {
        MouseWheelEvent wheelev(xev, from_native);
        return widget->OnMouseEvent(wheelev);
      }

      // Is the event coming from a touch device?
      if (TouchFactory::GetInstance()->IsTouchDevice(xievent->sourceid)) {
        // Hide the cursor when a touch event comes in.
        TouchFactory::GetInstance()->SetCursorVisible(false, false);

        // With XInput 2.0, XI_ButtonPress and XI_ButtonRelease events are
        // ignored, as XI_Motion events contain enough data to detect finger
        // press and release. See more notes in TouchFactory::TouchParam.
        if (cookie->evtype == XI_ButtonPress ||
            cookie->evtype == XI_ButtonRelease)
          return false;

        // If the TouchEvent is processed by |root|, then return. Otherwise let
        // it fall through so it can be used as a MouseEvent, if desired.
        TouchEvent touch(xev, from_native);
        if (widget->OnTouchEvent(touch) != ui::TOUCH_STATUS_UNKNOWN)
          return true;

        // We do not want to generate a mouse event for an unprocessed touch
        // event here. That is already done by the gesture manager in
        // RootView::OnTouchEvent.
        return false;
      } else {
        MouseEvent mouseev(xev, from_native);

        // Show the cursor. Start a timer to hide the cursor after a delay on
        // move (not drag) events, or if the only button pressed is released.
        bool start_timer = mouseev.type() == ui::ET_MOUSE_MOVED;
        start_timer |= mouseev.type() == ui::ET_MOUSE_RELEASED &&
                       (mouseev.IsOnlyLeftMouseButton() ||
                        mouseev.IsOnlyMiddleMouseButton() ||
                        mouseev.IsOnlyRightMouseButton());
        TouchFactory::GetInstance()->SetCursorVisible(true, start_timer);

        return widget->OnMouseEvent(mouseev);
      }
    }
  }
  return false;
}

bool DispatchXEvent(XEvent* xev) {
  GdkDisplay* gdisp = gdk_display_get_default();
  XID xwindow = xev->xany.window;

  if (xev->type == GenericEvent) {
    if (!TouchFactory::GetInstance()->ShouldProcessXI2Event(xev))
      return true;  // Consume the event.

    XGenericEventCookie* cookie = &xev->xcookie;
    if (cookie->evtype == XI_HierarchyChanged) {
      TouchFactory::GetInstance()->UpdateDeviceList(cookie->display);
      return true;
    }

    XIDeviceEvent* xiev = static_cast<XIDeviceEvent*>(cookie->data);
    xwindow = xiev->event;
  }

  GdkWindow* gwind = gdk_window_lookup_for_display(gdisp, xwindow);
  Widget* widget = FindWidgetForGdkWindow(gwind);
  if (widget) {
    Event::FromNativeEvent2 from_native;
    switch (xev->type) {
      case KeyPress:
      case KeyRelease: {
        KeyEvent keyev(xev, from_native);
        InputMethod* ime = widget->GetInputMethod();
        // Always dispatch key events to the input method first, to make sure
        // that the input method's hotkeys work all time.
        if (ime) {
          ime->DispatchKeyEvent(keyev);
          return true;
        }
        return widget->OnKeyEvent(keyev);
      }
      case ButtonPress:
      case ButtonRelease:
        if (xev->xbutton.button == 4 || xev->xbutton.button == 5) {
          // Scrolling the wheel triggers button press/release events.
          MouseWheelEvent wheelev(xev, from_native);
          return widget->OnMouseEvent(wheelev);
        }
        // fallthrough
      case MotionNotify: {
        MouseEvent mouseev(xev, from_native);
        return widget->OnMouseEvent(mouseev);
      }

      case GenericEvent: {
        return DispatchX2Event(widget, xev);
      }
    }
  }

  return false;
}

void SetTouchDeviceList(std::vector<unsigned int>& devices) {
  TouchFactory::GetInstance()->SetTouchDeviceList(devices);
}

AcceleratorHandler::AcceleratorHandler() {}

base::MessagePumpDispatcher::DispatchStatus
    AcceleratorHandler::Dispatch(XEvent* xev) {
  return DispatchXEvent(xev) ?
      base::MessagePumpDispatcher::EVENT_PROCESSED :
      base::MessagePumpDispatcher::EVENT_IGNORED;
}

}  // namespace views
