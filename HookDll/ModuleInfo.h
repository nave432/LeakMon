#pragma once
#include "afxcmn.h"


// ModuleInfo dialog

class ModuleInfo : public CDialog
{
	DECLARE_DYNAMIC(ModuleInfo)

public:
	ModuleInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~ModuleInfo();

// Dialog Data
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	static BOOL CALLBACK MYSymEnumerateModulesProc64(  PCSTR ModuleName,
														DWORD64 BaseOfDll,
														PVOID UserContext );
	static int CALLBACK CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );
	afx_msg void OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult);

	CListCtrl m_List;
	int m_nColumnClick;
	bool m_bSortAscending;

};
