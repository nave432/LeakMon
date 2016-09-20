// Injector.h : main header file for the INJECTOR application
//

#if !defined(AFX_INJECTOR_H__19A8E889_329E_45CB_9B47_ACE56E19075E__INCLUDED_)
#define AFX_INJECTOR_H__19A8E889_329E_45CB_9B47_ACE56E19075E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CInjectorApp:
// See Injector.cpp for the implementation of this class
//

class CInjectorApp : public CWinApp
{
public:
	CInjectorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInjectorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CInjectorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INJECTOR_H__19A8E889_329E_45CB_9B47_ACE56E19075E__INCLUDED_)
