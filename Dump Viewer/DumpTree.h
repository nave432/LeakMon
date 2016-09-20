

#if !defined(AFX_DUMPTREE_H__INCLUDED_)
#define AFX_DUMPTREE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CDumpTreeApp : public CWinApp
{
public:

    CDumpTreeApp();
    
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDumpTreeApp)
public:
    
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL
    
    // Implementation
    //{{AFX_MSG(CDumpTreeApp)
    afx_msg void OnAppAbout();
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
};

#endif // !defined(AFX_DUMPTREE_H__INCLUDED_)
