// HookDll.h : main header file for the HOOKDLL DLL
//

#if !defined(AFX_HOOKDLL_H__22E65FF0_2999_45C9_A726_5765A6D6DA0E__INCLUDED_)
#define AFX_HOOKDLL_H__22E65FF0_2999_45C9_A726_5765A6D6DA0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHookDllApp
// See HookDll.cpp for the implementation of this class
//

class CHookDllApp : public CWinApp
{
public:
	CHookDllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHookDllApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CHookDllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

enum HOOK_TYPE_e
{ 
	HT_UNKNOWN = 0,
	HT_MEMORY = 1,
	HT_GDI = 2,
	HT_HANDLE = 3,
	HT_NOTHING = 4
};

extern HOOK_TYPE_e g_HookType;
extern int g_StackDepth;

enum HANDLE_TYPES_e
{
	TYPE_EVENT_HANDLE,
	TYPE_MUTEX_HANDLE,
	TYPE_SEMAPHOR_HANDLE,
    TYPE_CRITICAL_SECTION_HANDLE,
	TYPE_WAIT_TIMER_HANDLE,
	TYPE_FILE_HANDLE,
	TYPE_TOKEN_HANDLE,
	TYPE_CHANGE_NOFICATION_HANDLE,
	TYPE_MEMEORY_MAPPED_FILE_HANDLE,
	TYPE_MEMORY_HANDLE,
	TYPE_PROCESS_HANDLE,
	TYPE_THREAD_HANDLE,
	TYPE_JOB_HANDLE,
	TYPE_MAIL_SLOT_HANDLE,
	TYPE_PIPE_HANDLE,
	TYPE_REGISTRY_HANDLE,
    TYPE_TIMER_QUEUE,
	TYPE_UNKNOWN
};

void DumpLeak();
void EmptyLeakMap();
void HookMemAlloc();
void HookGDIAlloc();
void HookHandleAlloc();
void CreateCallStack( LPVOID lpMem, SIZE_T dwBytes );
void RemovCallStack( LPVOID lpMem );


#ifdef HOOK_DLL_SRC
#define HOOK_DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define HOOK_DLL_EXPORT extern "C" __declspec(dllimport)
#endif

HOOK_DLL_EXPORT bool IsLeakDetected(void* pObject );
HOOK_DLL_EXPORT void SetHookType(HOOK_TYPE_e eType );

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOOKDLL_H__22E65FF0_2999_45C9_A726_5765A6D6DA0E__INCLUDED_)
