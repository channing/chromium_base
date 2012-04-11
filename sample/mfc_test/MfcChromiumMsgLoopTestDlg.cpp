
// MfcChromiumMsgLoopTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MfcChromiumMsgLoopTest.h"
#include "MfcChromiumMsgLoopTestDlg.h"
#include "afxdialogex.h"
#include "base\memory\ref_counted.h"
#include "base\threading\platform_thread.h"
#include "cef_thread.h"
#include "base/bind.h"
#include "ui/views/widget/widget.h"
#include "ui/views/controls/menu/menu_item_view.h"
#include "ui/views/controls/menu/menu_delegate.h"
#include "ui/views/controls/menu/menu_runner.h"
#include "menu/menu_test.h"
#include "layout_test.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMfcChromiumMsgLoopTestDlg dialog




CMfcChromiumMsgLoopTestDlg::CMfcChromiumMsgLoopTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMfcChromiumMsgLoopTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMfcChromiumMsgLoopTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMfcChromiumMsgLoopTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CMfcChromiumMsgLoopTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN_SHOWDIALOG, &CMfcChromiumMsgLoopTestDlg::OnBnClickedBtnShowdialog)
	ON_BN_CLICKED(IDC_BTN_SHOWMENU, &CMfcChromiumMsgLoopTestDlg::OnBnClickedBtnShowmenu)
	ON_WM_HOTKEY()
    ON_BN_CLICKED(IDC_BTN_LAYOUTTEST, &CMfcChromiumMsgLoopTestDlg::OnBnClickedBtnLayouttest)
END_MESSAGE_MAP()


// CMfcChromiumMsgLoopTestDlg message handlers

BOOL CMfcChromiumMsgLoopTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	RegisterHotKey(GetSafeHwnd(), 0, MOD_CONTROL | MOD_SHIFT, 'W');

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMfcChromiumMsgLoopTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMfcChromiumMsgLoopTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMfcChromiumMsgLoopTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMfcChromiumMsgLoopTestDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	//PostQuitMessage(0);
	MessageLoop::current()->Quit();
}


void CMfcChromiumMsgLoopTestDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnOK();

	DestroyWindow();
}


void CMfcChromiumMsgLoopTestDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnCancel();

	DestroyWindow();
}

class Task2 : public base::RefCountedThreadSafe<Task2>
{
public:
	Task2(int thread_id, std::string name)
		:thread_id_(thread_id),
		name_(name)
	{

	}

	void Run() {
		CString msg;
		CString sub(name_.c_str());
		msg += sub;
		sub.Format(L"\n%d\n%d\n", thread_id_, base::PlatformThread::CurrentId());
		msg += sub;
		AfxMessageBox(msg);
	}

private:
	~Task2() {}
	int thread_id_;
	std::string name_;

private:
	friend class base::RefCountedThreadSafe<Task2>;
};

class Task1 : public base::RefCountedThreadSafe<Task1>
{
public:
	Task1(){

	}

	void Run() {
		scoped_refptr<Task2> task2 = new Task2(base::PlatformThread::CurrentId(), base::PlatformThread::GetName());
		CefThread::PostTask(CefThread::UI, FROM_HERE, base::Bind(&Task2::Run, task2));
	}

private:
	~Task1() {}

private:
	friend class base::RefCountedThreadSafe<Task1>;
};

class TaskFoo : public base::RefCountedThreadSafe<TaskFoo>
{
public:
	TaskFoo(){

	}

	void Run() {
		++count_;
		if (count_ == 99999) {
			AfxMessageBox(L"99999");
		}
	}

private:
	~TaskFoo() {}

	static int count_;

private:
	friend class base::RefCountedThreadSafe<TaskFoo>;
};

int TaskFoo::count_ = 0;

void CMfcChromiumMsgLoopTestDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	CString msg;
	msg.Format(L"Current thread id: %d", base::PlatformThread::CurrentId());
	AfxMessageBox(msg);

	scoped_refptr<Task1> task = new Task1;
	CefThread::PostTask(CefThread::FILE, FROM_HERE, base::Bind(&Task1::Run, task));

	for (int i = 0; i < 100000; ++i) {
		scoped_refptr<TaskFoo> task = new TaskFoo;
		CefThread::PostTask(CefThread::IO, FROM_HERE, base::Bind(&TaskFoo::Run, task));
	}
}

void TestDialogView::DeleteDelegate()
{
	delete this;
}

gfx::Size TestDialogView::GetPreferredSize()
{
	return gfx::Size(300, 200);
}

views::View* TestDialogView::GetContentsView()
{
	return this;
}

void CMfcChromiumMsgLoopTestDlg::OnBnClickedBtnShowdialog()
{
	// TODO: Add your control notification handler code here
	views::DialogDelegateView* default_dialog = new TestDialogView;
	views::Widget* test_dlg = views::Widget::CreateWindowWithParent(default_dialog, m_hWnd);
	test_dlg->Show();
}

class TestMenuDelegate : public views::MenuDelegate {
public:

	virtual void ExecuteCommand(int id)  {
		CString msg;
		msg.Format(L"You selected item %d", id);
		AfxMessageBox(msg);
	}

};

namespace {
	void ShowTestMenu() {
		scoped_ptr<TestMenuDelegate> menu_delegate;
		menu_delegate.reset(new TestMenuDelegate());
		views::MenuItemView* root_item = new views::MenuItemView(menu_delegate.get());
		int id = 100;
		root_item->AppendMenuItem(id++, L"first item", views::MenuItemView::NORMAL);
		root_item->AppendMenuItem(id++, L"second item", views::MenuItemView::NORMAL);
		root_item->AppendSeparator();
		root_item->AppendSubMenu(id, L"submenu");


		scoped_ptr<views::MenuRunner> menu_runner;
		menu_runner.reset(new views::MenuRunner(root_item));
		menu_runner->RunMenuAt(NULL, NULL, gfx::Rect(500, 500, 500, 500),
			views::MenuItemView::TOPRIGHT, views::MenuRunner::HAS_MNEMONICS);
	}
}

void CMfcChromiumMsgLoopTestDlg::OnBnClickedBtnShowmenu()
{
	// TODO: Add your control notification handler code here
    CRect btn;
    ::GetClientRect(GetDlgItem(IDC_BTN_SHOWMENU)->GetSafeHwnd(), &btn);
    CPoint pt(btn.left, btn.bottom);
    ::ClientToScreen(GetDlgItem(IDC_BTN_SHOWMENU)->GetSafeHwnd(), &pt);
	ShowOwnTestMenu(pt.x, pt.y);

    //ShowTestMenu();
}

static BOOL ForceForegroundWindow( HWND hwnd )
{
    HWND hwndForeground = GetForegroundWindow();
    if ( hwnd == hwndForeground ) {
        return TRUE;
    }

    if ( IsIconic( hwnd ) ) {
        ShowWindow( hwnd, SW_RESTORE );
    }

    //Important:try the simplest way first
    SetForegroundWindow( hwnd );
    hwndForeground = GetForegroundWindow();
    if ( hwnd == hwndForeground ) {
        return TRUE;
    }

    DWORD dwThreadHwnd = GetWindowThreadProcessId( hwnd, NULL );
    DWORD dwThreadForeground = GetWindowThreadProcessId( hwndForeground, NULL );

    BOOL bRet = FALSE;
    if ( dwThreadHwnd != dwThreadForeground ) {
        if ( AttachThreadInput( dwThreadForeground, dwThreadHwnd, TRUE ) ) {
            bRet = SetForegroundWindow( hwnd );
            AttachThreadInput( dwThreadForeground, dwThreadHwnd, FALSE );
        }
    }
    else {
        bRet = SetForegroundWindow( hwnd );
    }

    return bRet;
}

void CMfcChromiumMsgLoopTestDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: Add your message handler code here and/or call default
    //ShowWindow(SW_HIDE);
    //ShowWindow(SW_SHOW);
    //ForceForegroundWindow(GetSafeHwnd());
	ShowTestMenu();

	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}


void CMfcChromiumMsgLoopTestDlg::OnBnClickedBtnLayouttest()
{
    // TODO: Add your control notification handler code here
    LayoutTestWidget::Show();
}
