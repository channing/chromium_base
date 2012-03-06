#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/process_util.h"
#include "base/stringprintf.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_paths.h"
#include "ui/gfx/canvas_skia.h"
#include "ui/views/focus/accelerator_handler.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/widget_delegate.h"
#include "ui/views/controls/label.h"
#include "ui/base/keycodes/keyboard_codes.h"

// in theory i include addition manifest in the gyp. but gyp have a bug no 
// generate the right vcxproj. so i add it here.

// if you don't include the Common-Controls you will see no tooltip. but no warm.
// microsoft is sucks. manifest is a nightmare.

#if defined(OS_WIN)
// Add Common Controls to the application manifest because it's required to
// support the default tooltip implementation.
#pragma comment(linker,"/manifestdependency:\"type='win32' \
                                              name='Microsoft.Windows.Common-Controls' \
                                              version='6.0.0.0' \
                                              processorArchitecture='*' \
                                              publicKeyToken='6595b64144ccf1df' \
                                              language='*'\"")
#endif

class tooltipView : public views::View {
public:
    tooltipView(int num);
    virtual ~tooltipView() {};

    virtual bool OnMousePressed(const views::MouseEvent& event) OVERRIDE;
    virtual gfx::Size GetPreferredSize() OVERRIDE;
    virtual bool GetTooltipText(const gfx::Point& p, string16* tooltip) const OVERRIDE;
    virtual void OnFocus() OVERRIDE;

    void SetTooltipText(const string16& tooltip_text);
    void SetTooltipText();
private:
    string16 tooltip_text_;
    int num_;
    int pressed_count_;
};

tooltipView::tooltipView(int num)
    : num_(num)
    , pressed_count_(0) {
    set_focusable(true);
    SetTooltipText();
    set_background(
        views::Background::CreateVerticalGradientBackground(
            SkColorSetRGB(0,0,0), SkColorSetRGB(255, 255, 255)));
}

void tooltipView::SetTooltipText(const string16& tooltip_text) {
    tooltip_text_ = tooltip_text;
}

void tooltipView::SetTooltipText() {
    string16 s;
    s = StringPrintf(L"tooltipView: %d\npressed: %d\n%ls\n", 
        num_, pressed_count_, HasFocus() ? L"be Focused" : L"No Focus");
    SetTooltipText(s);
    TooltipTextChanged();
}

bool tooltipView::GetTooltipText(const gfx::Point& p, string16* tooltip) const {
    DCHECK(tooltip);
    if (tooltip_text_.empty())
        return false;
    else  {
        *tooltip = tooltip_text_;
        return true;
    }
}
gfx::Size tooltipView::GetPreferredSize() {
    return gfx::Size(100, 100);
}

bool tooltipView::OnMousePressed(const views::MouseEvent& event) {
    RequestFocus();
    pressed_count_ ++;
    SetTooltipText();
    return true;
}

void tooltipView::OnFocus(){
    SetTooltipText();
}

class ExampleView : public views::WidgetDelegate {
public:
    ExampleView();
    virtual ~ExampleView() {};

    // Creates all the examples and shows the window.
    void Init();

private:
    // views::WidgetDelegate implementation:
    virtual bool CanResize() const OVERRIDE;
    virtual bool CanMaximize() const OVERRIDE;
    virtual std::wstring GetWindowTitle() const OVERRIDE;
    virtual views::View* GetContentsView() OVERRIDE;
    virtual void WindowClosing() OVERRIDE;
    virtual views::Widget* GetWidget() OVERRIDE;
    virtual const views::Widget* GetWidget() const OVERRIDE;

    views::View* contents_;

    DISALLOW_COPY_AND_ASSIGN(ExampleView);
};

ExampleView::ExampleView() : contents_(NULL) {}

void ExampleView::Init() {
    DCHECK(contents_ == NULL) << "Run called more than once.";
    tooltipView* view;
    contents_ = new views::View();
    contents_->set_background(
        views::Background::CreateSolidBackground(25, 25, 25));
    for (int i = 0; i < 5; i++)
    {
        views::View* hcontent =new views::View();
        contents_->AddChildView(hcontent);
        for (int j = 0; j < 5; j++)
        {
            view = new tooltipView(i*5 + j);
            hcontent->AddChildView(view);
        }
        hcontent->SetLayoutManager(new views::BoxLayout(
            views::BoxLayout::kHorizontal, 10, 10, 10));
    }

    contents_->SetLayoutManager(new views::BoxLayout(
        views::BoxLayout::kVertical, 10, 10, 10));
    views::Widget* window =
        views::Widget::CreateWindowWithBounds(this, gfx::Rect(0, 0, 850, 300));

    window->Show();
}

bool ExampleView::CanResize() const {
    return true;
}

bool ExampleView::CanMaximize() const {
    return true;
}

std::wstring ExampleView::GetWindowTitle() const {
    return L"Views Examples";
}

views::View* ExampleView::GetContentsView() {
    return contents_;
}

void ExampleView::WindowClosing() {
    MessageLoopForUI::current()->Quit();
}

views::Widget* ExampleView::GetWidget() {
    return contents_->GetWidget();
}

const views::Widget* ExampleView::GetWidget() const {
    return contents_->GetWidget();
}

int main(int argc, char** argv) {
#if defined(OS_WIN)
    OleInitialize(NULL);
#elif defined(OS_LINUX)
    // Initializes gtk stuff.
    g_thread_init(NULL);
    g_type_init();
    gtk_init(&argc, &argv);
#endif
    CommandLine::Init(argc, argv);

    base::EnableTerminationOnHeapCorruption();

    // The exit manager is in charge of calling the dtors of singleton objects.
    base::AtExitManager exit_manager;

    ui::RegisterPathProvider();
    ui::ResourceBundle::InitSharedInstance("en-US");

    MessageLoop main_message_loop(MessageLoop::TYPE_UI);

    // views::TestViewsDelegate delegate;

    // We do not use this header: chrome/common/chrome_switches.h
    // because that would create a bad dependency back on Chrome.
    views::Widget::SetPureViews(
        CommandLine::ForCurrentProcess()->HasSwitch("use-pure-views"));

    ExampleView view;
    view.Init();

    views::AcceleratorHandler accelerator_handler;
    MessageLoopForUI::current()->RunWithDispatcher(&accelerator_handler);

#if defined(OS_WIN)
    OleUninitialize();
#endif
    return 0;
}
