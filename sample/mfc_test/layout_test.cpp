#include "stdafx.h"
#include "layout_test.h"

#include "ui\views\widget\widget.h"
#include "ui\views\layout\grid_layout.h"
#include "ui\views\layout\layout_constants.h"
#include "ui\views\controls\button\text_button.h"
#include "ui\views\controls\textfield\textfield.h"

using views::GridLayout;

views::View* LayoutTestWidget::GetContentsView() {
    return contents_;
}

bool LayoutTestWidget::CanResize() const {
    return true;
}

void LayoutTestWidget::Show() {
    LayoutTestWidget* widget = new LayoutTestWidget();
    views::Widget* window =
        views::Widget::CreateWindow(widget);

    window->Show();
}

LayoutTestWidget::LayoutTestWidget() : contents_(NULL) {
    Init();
}

views::Widget* LayoutTestWidget::GetWidget() {
    return contents_->GetWidget();
}

const views::Widget* LayoutTestWidget::GetWidget() const {
    return contents_->GetWidget();
}

void LayoutTestWidget::DeleteDelegate() {
    delete this;
}

void LayoutTestWidget::Init() {
    contents_ = new views::View();

    // Yummy layout code.
    views::GridLayout* layout = views::GridLayout::CreatePanel(contents_);
    contents_->SetLayoutManager(layout);

    const int labels_column_set_id = 0;
    const int single_column_view_set_id = 1;
    const int buttons_column_set_id = 2;

    views::ColumnSet* column_set = layout->AddColumnSet(0);
    column_set->AddColumn(GridLayout::LEADING, GridLayout::CENTER, 0,
        GridLayout::USE_PREF, 0, 0);
    column_set->AddColumn(GridLayout::LEADING, GridLayout::CENTER, 0,
        GridLayout::USE_PREF, 0, 0);
    column_set->AddPaddingColumn(0, views::kRelatedControlHorizontalSpacing);
    column_set->AddColumn(GridLayout::LEADING, GridLayout::CENTER, 1,
        GridLayout::USE_PREF, 0, 0);
    column_set->AddPaddingColumn(0, views::kRelatedControlHorizontalSpacing);
    column_set->AddColumn(GridLayout::CENTER, GridLayout::CENTER, 0,
        GridLayout::USE_PREF, 0, 0);

    layout->StartRow(0, 0);

    button1_ = new views::TextButton(this, L"Button 1");
    layout->AddView(button1_);
    button2_ = new views::TextButton(this, L"Button 2");
    layout->AddView(button2_);

    text_field_ = new views::Textfield();
    text_field_->SetText(L"Search here");
    layout->AddView(text_field_);

    close_button_ = new views::TextButton(this, L"Close");
    layout->AddView(close_button_);
}

void LayoutTestWidget::ButtonPressed(views::Button* sender, const views::Event& event) {
    if (sender == close_button_) {
        GetWidget()->Close();
    }
}
