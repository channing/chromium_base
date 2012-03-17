#include "stdafx.h"
#include "mfc_process.h"

#include "cef_thread.h"


MfcProcess::~MfcProcess()
{
	m_IOThread.reset();
	m_FileThread.reset();

	//m_UIThread->CleanUp();
	// Terminate the UI thread.
	m_UIThread.reset();
	// Terminate the message loop.
	m_UIMessageLoop.reset();
}

void MfcProcess::CreateThreads()
{
	CreateUIThread();
	CreateFileThread();
	CreateIOThread();
}

void MfcProcess::RunMessageLoop()
{
	MessageLoopForUI::current()->RunWithDispatcher(NULL);
}

void MfcProcess::CreateUIThread()
{
	m_UIMessageLoop.reset(new MessageLoop(MessageLoop::TYPE_UI));
	m_UIThread.reset(new CefThread(CefThread::UI, MessageLoop::current()));
}

void MfcProcess::CreateFileThread()
{
	m_FileThread.reset(new CefThread(CefThread::FILE));
	base::Thread::Options options;
	//options.message_loop_type = MessageLoop::TYPE_DEFAULT;
	if (!m_FileThread->StartWithOptions(options))
		return;
}

void MfcProcess::CreateIOThread()
{
	m_IOThread.reset(new CefThread(CefThread::IO));
	base::Thread::Options options;
	//options.message_loop_type = MessageLoop::TYPE_DEFAULT;
	if (!m_IOThread->StartWithOptions(options))
		return;
}
