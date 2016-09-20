#if !defined(AFX_ONFIGDLG_H__C97A22C8_2FBF_4D76_BD8E_8E0F794A0314__INCLUDED_)
#define AFX_ONFIGDLG_H__C97A22C8_2FBF_4D76_BD8E_8E0F794A0314__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// onfigDlg.h : header file
//
#include "EditEx.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// ConfigDlg dialog

extern bool g_IS_TEST_APP;

class ConfigDlg : public CDialog
{
// Construction
public:
	ConfigDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ConfigDlg)
	enum { IDD = IDD_DIALOG1 };
	CListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ConfigDlg)
	afx_msg void OnPath();
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	afx_msg void OnOk();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    void LoadSymbols();
public:

    bool m_bChanged;
    CString m_csPath;
	EditEx m_EditCtrl;
	CDialog m_ProgressDlg;
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	static UINT __cdecl ThreadEntry( LPVOID pParam );
	static BOOL CALLBACK SymRegisterCallbackProc64( HANDLE hProcess,
													ULONG ActionCode,
													ULONG64 CallbackData,
													ULONG64 UserContext );
	afx_msg void OnBnClickedButton3();
	void ShowWaitDialog();
public:
	afx_msg void OnBnClickedRadioMem();
	afx_msg void OnBnClickedRadioGdi();
	afx_msg void OnBnClickedRadioHandle();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ONFIGDLG_H__C97A22C8_2FBF_4D76_BD8E_8E0F794A0314__INCLUDED_)
