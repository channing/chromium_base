
// MfcChromiumMsgLoopTestDlg.h : header file
//

#pragma once

#include "afxdialogex.h"
#include "ui/views/window/dialog_delegate.h"

// CMfcChromiumMsgLoopTestDlg dialog
class CMfcChromiumMsgLoopTestDlg : public CDialogEx
{
// Construction
public:
	CMfcChromiumMsgLoopTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFCCHROMIUMMSGLOOPTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedBtnShowdialog();
	afx_msg void OnBnClickedBtnShowmenu();
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
    afx_msg void OnBnClickedBtnLayouttest();
};


class TestDialogView : public views::DialogDelegateView {
public:
	virtual void DeleteDelegate() OVERRIDE;
	virtual gfx::Size GetPreferredSize() OVERRIDE;

	virtual views::View* GetContentsView() OVERRIDE;
};