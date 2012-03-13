#pragma once

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop.h"
#include "base/threading/non_thread_safe.h"
#include "cef_thread.h"

class MfcProcess : public base::NonThreadSafe {
public:
	~MfcProcess();

	void CreateThreads();

	void RunMessageLoop();

private:
	void CreateUIThread();
	void CreateFileThread();
	void CreateIOThread();

	scoped_ptr<MessageLoop> m_UIMessageLoop;
	scoped_ptr<CefThread> m_UIThread;
	scoped_ptr<CefThread> m_FileThread;
	scoped_ptr<CefThread> m_IOThread;
};