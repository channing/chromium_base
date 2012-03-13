
// MfcChromiumMsgLoopTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <vld.h>
#include "MfcChromiumMsgLoopTest.h"
#include "MfcChromiumMsgLoopTestDlg.h"
#include "base\at_exit.h"
#include "mfc_process.h"
#include "cef_thread.h"
#include "base/threading/thread.h"
#include "base/at_exit.h"
#include "base/logging.h"
#include "base/command_line.h"
#include "base/memory/scoped_ptr.h"
#include "base/bind.h"
#include "base/tracked_objects.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMfcChromiumMsgLoopTestApp

BEGIN_MESSAGE_MAP(CMfcChromiumMsgLoopTestApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMfcChromiumMsgLoopTestApp construction

CMfcChromiumMsgLoopTestApp::CMfcChromiumMsgLoopTestApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMfcChromiumMsgLoopTestApp object

CMfcChromiumMsgLoopTestApp theApp;


// CMfcChromiumMsgLoopTestApp initialization

class Foo : public base::RefCounted<Foo>
{
public:
	Foo() : test_count_(0) {}

	void Test0() {
		++test_count_;
	}
	void Test1(int arg) {
		LOG(INFO) << "current thread ID :" << base::PlatformThread::CurrentId()
			<< " arg :" << arg << " test_count_ :" << test_count_;
	}
private:
	~Foo() {}
	int test_count_;
	const std::string result_;
private:
	friend class base::RefCounted<Foo>;
};


class MyThread : public base::PlatformThread::Delegate
{
public:
	virtual void ThreadMain() 
	{
		base::PlatformThread::Sleep( 3000 );
	}
};

BOOL CMfcChromiumMsgLoopTestApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	//CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	//CMfcChromiumMsgLoopTestDlg dlg;
	//m_pMainWnd = &dlg;
	//dlg.Create( IDD_MFCCHROMIUMMSGLOOPTEST_DIALOG );
	//
	//dlg.ShowWindow( SW_SHOW );


	//base::AtExitManager exit_manager;

	//MfcProcess process;
	//process.CreateThreads();

	//scoped_ptr<base::Thread> thread(new base::Thread("my name"));
	//base::Thread::Options options;
	////options.message_loop_type = MessageLoop::TYPE_DEFAULT;
	//thread->StartWithOptions(options);

	//process.RunMessageLoop();


	using namespace logging;

	base::AtExitManager atexit;


	//// Platform thread test
	//base::PlatformThreadHandle thread_handle = NULL;
	//MyThread my_thread;
	//base::PlatformThread::Create( 0, &my_thread, &thread_handle );
	//ASSERT(thread_handle);
	//base::PlatformThread::Join(thread_handle);





	//CommandLine::Init(argc, argv);

	//InitLogging(L"debug.log", 
	//	LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG, 
	//	LOCK_LOG_FILE,
	//	DELETE_OLD_LOG_FILE,
	//	DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS);

	// base::Thread test
	//_CrtSetBreakAlloc(520);
	base::Thread thread("");
	thread.Start();
	thread.Stop();
	//int* p = new int(10);
	//tracked_objects::ThreadData::ShutdownSingleThreadedCleanup(false);

	//MessageLoop* messageloop = thread.message_loop();
	//scoped_refptr<Foo> foo(new Foo());
	//messageloop->PostTask(FROM_HERE, 
	//	base::Bind(&Foo::Test0, foo.get()));
	//messageloop->PostTask(FROM_HERE, 
	//	base::Bind(&Foo::Test1, foo.get(), 10));
	//base::PlatformThread::Sleep(2000);
	//thread.Stop();


	//MessageLoop main_message_loop(MessageLoop::TYPE_UI);
	//MessageLoopForUI::current()->RunWithDispatcher(NULL);

	//MSG msg;
	//while( GetMessage( &msg, NULL, 0, 0 ) ) {
	//	TranslateMessage( &msg );
	//	DispatchMessage( &msg );
	//}

	// Delete the shell manager created above.
	//if (pShellManager != NULL)
	//{
	//	delete pShellManager;
	//}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

