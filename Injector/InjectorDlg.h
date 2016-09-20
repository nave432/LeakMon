// InjectorDlg.h : header file
//

#include "afxwin.h"
#if !defined(AFX_INJECTORDLG_H__8132A123_B235_452C_9AD7_3AB54DA58826__INCLUDED_)
#define AFX_INJECTORDLG_H__8132A123_B235_452C_9AD7_3AB54DA58826__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CInjectorDlg dialog

class CInjectorDlg : public CDialog
{
// Construction
public:
	CInjectorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CInjectorDlg)
	enum { IDD = IDD_INJECTOR_DIALOG };
	CListCtrl	m_List;
	//}}AFX_DATA

    void Refresh();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInjectorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

    static int CALLBACK CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CInjectorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnInject();
	afx_msg void OnDump();
	afx_msg void ClearLeaks();
	afx_msg void OnRefresh();
	afx_msg void OnInfo();
	afx_msg void OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    int m_nColumnClick;
    bool m_bSortAscending;

public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INJECTORDLG_H__8132A123_B235_452C_9AD7_3AB54DA58826__INCLUDED_)
