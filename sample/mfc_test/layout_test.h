#pragma once

#include "ui\views\widget\widget_delegate.h"
#include "ui\views\controls\button\button.h"

namespace views {
    class TextButton;
    class Textfield;
}

class LayoutTestWidget :
    public views::WidgetDelegate,
    public views::ButtonListener
{
public:
    static void Show();

    LayoutTestWidget();
    void Init();

    virtual views::View* GetContentsView() override;
    virtual bool CanResize() const;

    virtual views::Widget* GetWidget();
    virtual const views::Widget* GetWidget() const;

    virtual void DeleteDelegate();

    virtual void ButtonPressed(views::Button* sender, const views::Event& event);

private:
    views::View* contents_;
    views::TextButton* button1_;
    views::TextButton* button2_;
    views::TextButton* close_button_;
    views::Textfield* text_field_;
};