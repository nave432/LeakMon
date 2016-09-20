// Injector.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Injector.h"
#include "InjectorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInjectorApp

BEGIN_MESSAGE_MAP(CInjectorApp, CWinApp)
	//{{AFX_MSG_MAP(CInjectorApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInjectorApp construction

CInjectorApp::CInjectorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CInjectorApp object

CInjectorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CInjectorApp initialization

BOOL CInjectorApp::InitInstance()
{

    INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
   	
    AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
#if _MFC_VER < 0x800
    #ifdef _AFXDLL 
	    Enable3dControls();			// Call this when using MFC in a shared DLL
    #else
	    Enable3dControlsStatic();	// Call this when linking to MFC statically
    #endif
#endif

    HANDLE hMutex = CreateMutex( 0, 0, _T("leak_detector_injector"));
    if( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        AfxMessageBox( _T("Only one instance of the Injector can be run at a time"));
        CloseHandle( hMutex );
        return FALSE;
    }

	CInjectorDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
