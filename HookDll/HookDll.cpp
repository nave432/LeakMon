// HookDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "HookDll.h"
#include "Dbghelp.h"
#include "ConfigDlg.h"
#include "ModuleInfo.h"
#include "HookFunctions.h"
#include "..\Common\Common.h"
#include "ArrayEx.h"
//#include <afxtempl.h>
#include <afxmt.h>
#include <new.h>

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

BEGIN_MESSAGE_MAP(CHookDllApp, CWinApp)
    //{{AFX_MSG_MAP(CHookDllApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//#define ENABLE_LOG
/////////////////////////////////////////////////////////////////////////////
// helper macro
#define REGISTER_HOOK(Function, Dll) \
{ \
    pOrg##Function = (Function##Def)GetProcAddress(Dll, #Function); \
    if ( pOrg##Function != NULL )\
    {\
    HOOKFUNCDESC entry; \
    entry.pProc = (PROC)My##Function; \
    entry.pOrigProc = (PROC)pOrg##Function; \
    entry.szFunc = #Function; \
    entry.lpszDllName = _T(#Dll)_T(".dll"); \
    entry.dwOrdinal = 0; \
    g_arrHookedFunctions.SetAt(nIndex, entry); \
    nIndex++;\
    } \
    else{ \
    OutputDebugStringA( "#Leakmon: Failed to get address of function"  #Function ); \
    }\
}
//////////////////////////////////////////////////////////////////////////////////

CHookDllApp::CHookDllApp()
{
}

//////////////////////////////////////////////////////////////
//////////global variables and declarations//////////////////
typedef CArrayEx<DWORD64,DWORD64> STACK_ARRAY;
struct MEM_INFO
{
    STACK_ARRAY *parCallStack;
    SIZE_T nMemSize;
};

#define STACKWALK_MAX_NAMELEN 1024
CHookDllApp theApp;
CMapEx<LPVOID,LPVOID,MEM_INFO,MEM_INFO> m_MemMap;
bool g_bTrack = true;
bool g_bHooked = false;
HOOK_TYPE_e g_HookType = HT_UNKNOWN;
int g_StackDepth = 20;
CCriticalSection SyncObj;
CArrayEx<HOOKFUNCDESC,HOOKFUNCDESC&> g_arrHookedFunctions;



#ifdef _M_IX86
void StackDump( LPVOID pMem, DWORD dwBytes)
{
        STACKFRAME64 stStackFrame = {0};
        CONTEXT stContext = {0};
        stContext.ContextFlags = CONTEXT_ALL;    
        __asm    call x
        __asm x: pop eax
        __asm    mov stContext.Eip, eax
        __asm    mov stContext.Ebp, ebp
        __asm    mov stContext.Esp, esp

        stStackFrame.AddrPC.Offset = stContext.Eip;
        stStackFrame.AddrPC.Mode = AddrModeFlat;
        stStackFrame.AddrFrame.Offset = stContext.Ebp;
        stStackFrame.AddrFrame.Mode = AddrModeFlat;
        stStackFrame.AddrStack.Offset = stContext.Esp;
        stStackFrame.AddrStack.Mode = AddrModeFlat;
 
//         BYTE SymBol[ sizeof(SYMBOL_INFO) + STACKWALK_MAX_NAMELEN ] = {0};
// 
//         SYMBOL_INFO* pSymbol = (SYMBOL_INFO*)SymBol;
//         pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
//         pSymbol->MaxNameLen = STACKWALK_MAX_NAMELEN;
// 
//         IMAGEHLP_LINE64 Line = {0};
//         Line.SizeOfStruct = sizeof( IMAGEHLP_LINE64 );
        
        HANDLE hProcess = GetCurrentProcess();
        MEM_INFO stInfo;
        //stInfo.parCallStack = new STACK_ARRAY;
        
        void * p = AllocMem(sizeof(STACK_ARRAY));
        stInfo.parCallStack = new( (void*)p ) STACK_ARRAY;
        
        stInfo.nMemSize = dwBytes;
        for( int i =0; i < g_StackDepth ; i++ )// only retrieve 40 functions
        {
            BOOL b = StackWalk64( IMAGE_FILE_MACHINE_I386, hProcess, GetCurrentThread(), 
                              &stStackFrame ,&stContext, 0, 
                              SymFunctionTableAccess64 , SymGetModuleBase64, NULL );
            if ( !b )
            {
               break;
            }
            DWORD64 dwDisplacement = 0;
            if (stStackFrame.AddrPC.Offset == stStackFrame.AddrReturn.Offset)
            {
              break;
            }

//////////////////////////////////////////////////////////////////////////
        //if( SymFromAddr( hProcess, stStackFrame.AddrPC.Offset, &dwDisplacement, pSymbol ))
        //{
        //		CString cs = "Ordinal823";
        //	if( cs == pSymbol->Name)
        //		{
        //			break;
        //		}
        //			
        //}
//////////////////////////////////////////////////////////////////////////

            if( i <= 1 )// ignore the functions on the top of stack which is our own.
            {
                continue;
            }
            stInfo.parCallStack->Add( stStackFrame.AddrPC.Offset );
        }        
        m_MemMap[pMem] = stInfo;        
}

#else
void StackDump( LPVOID pMem, SIZE_T dwBytes)
{
        
    CONTEXT                       Context;
    //KNONVOLATILE_CONTEXT_POINTERS NvContext;
    //UNWIND_HISTORY_TABLE          UnwindHistoryTable;
    PRUNTIME_FUNCTION             RuntimeFunction;
    PVOID                         HandlerData;
    ULONG64                       EstablisherFrame;
    ULONG64                       ImageBase;

    //OutputDebugString(L"StackTrace64: Executing stack trace...\n");

    //
    // First, we'll get the caller's context.
    //

    RtlCaptureContext(&Context);

    //
    // Initialize the (optional) unwind history table.
    //

    /*RtlZeroMemory(
        &UnwindHistoryTable,
        sizeof(UNWIND_HISTORY_TABLE));*/

    
        //BYTE SymBol[ sizeof(SYMBOL_INFO) + STACKWALK_MAX_NAMELEN ] = {0};
        //SYMBOL_INFO* pSymbol = (SYMBOL_INFO*)SymBol;
        //DWORD64 dwDisplacement;
     
        HANDLE hProcess = GetCurrentProcess();
        MEM_INFO stInfo;
        //stInfo.parCallStack = new STACK_ARRAY;
        
        void * p = AllocMem(sizeof(STACK_ARRAY));
        stInfo.parCallStack = new( (void*)p ) STACK_ARRAY;
        
        stInfo.nMemSize = dwBytes;
        for( int i =0; i < g_StackDepth ; i++ )// only retrieve 40 functions
        {
            //
        // Try to look up unwind metadata for the current function.
        //

        RuntimeFunction = RtlLookupFunctionEntry(
            Context.Rip,
            &ImageBase,
            NULL
            );

        /*RtlZeroMemory(
            &NvContext,
            sizeof(KNONVOLATILE_CONTEXT_POINTERS));*/

        if (!RuntimeFunction)
        {
            //
            // If we don't have a RUNTIME_FUNCTION, then we've encountered
            // a leaf function.  Adjust the stack approprately.
            //

            Context.Rip  = (ULONG64)(*(PULONG64)Context.Rsp);
            Context.Rsp += 8;
        }
        else
        {
            //
            // Otherwise, call upon RtlVirtualUnwind to execute the unwind for
            // us.
            //

            RtlVirtualUnwind(
                0, //UNW_FLAG_NHANDLER,
                ImageBase,
                Context.Rip,
                RuntimeFunction,
                &Context,
                &HandlerData,
                &EstablisherFrame,
                NULL );
        }

        //
        // If we reach an RIP of zero, this means that we've walked off the end
        // of the call stack and are done.
        //

        if (!Context.Rip)
            break;

//////////////////////////////////////////////////////////////////////////
         
                 //if( SymFromAddr( hProcess, Context.Rip, &dwDisplacement, pSymbol ))
                 //{
                 //    CString cs = "Ordinal823";
                 //     if( cs == pSymbol->Name)
                 //    {
                 //        break;
                 //    }
                 //   
                 //}
//////////////////////////////////////////////////////////////////////////

            if( i <= 1 )// ignore the functions on the top of stack which is our own.
            {
                continue;
            }
            stInfo.parCallStack->Add( Context.Rip );
        }        
        m_MemMap[pMem] = stInfo;        
}
#endif

void CreateCallStack( LPVOID lpMem, SIZE_T dwBytes )
{
    if( !lpMem )
    {
        return;
    }
    try
    {		
        CSingleLock lockObj( &SyncObj, TRUE );
        if( g_bHooked && g_bTrack )
        {
            g_bTrack = false;
            StackDump( lpMem, dwBytes );
#ifdef ENABLE_LOG
            CString cs;
            cs.Format( "Allocating    %x" ,(UINT)lpMem);
            OutputDebugString(cs);
#endif
            g_bTrack = true;
        }
    }
    catch(...)
    {
    }
    
}

void RemovCallStack( LPVOID lpMem )
{
    try
    {
        if( !lpMem )
        {
            return;
        }

        CSingleLock lockObj( &SyncObj, TRUE );
        if( g_bHooked && g_bTrack )
        {
            g_bTrack = false;
            MEM_INFO stInfo;
            if( m_MemMap.Lookup( lpMem, stInfo ))
            {
                //delete stInfo.parCallStack;
                stInfo.parCallStack->~STACK_ARRAY();
                DeleteMem( stInfo.parCallStack);
                m_MemMap.RemoveKey( lpMem );
#ifdef ENABLE_LOG
                CString cs;
                cs.Format( "De-allocating %x" ,(UINT)lpMem);
                OutputDebugString(cs);
#endif
            }
            g_bTrack = true;
        }
    }
    catch(...)
    {
    }	
}

void CopyStack(LPVOID lpExisting, LPVOID lpNew, int nType )
{
    CSingleLock lockObj( &SyncObj, TRUE );
    if( g_bHooked && g_bTrack )
    {
        MEM_INFO stInfo;
        if( m_MemMap.Lookup( lpExisting, stInfo ))
        {
            MEM_INFO stNew;
            //stNew.parCallStack = new STACK_ARRAY;
            void * p = AllocMem(sizeof(STACK_ARRAY));
            stNew.parCallStack = new( (void*)p ) STACK_ARRAY;
            
            stNew.parCallStack->Copy( *stInfo.parCallStack );
            stNew.nMemSize = nType;
        }

    }
}


LPVOID WINAPI MyHeapAlloc( IN HANDLE hHeap,
                           IN DWORD dwFlags,
                           IN SIZE_T dwBytes )
{
    LPVOID lMem =  pOrgHeapAlloc( hHeap, dwFlags, dwBytes );
    CreateCallStack( lMem, dwBytes );
    return lMem;
}

LPVOID WINAPI MyHeapReAlloc( HANDLE hHeap,
                           DWORD dwFlags,
                           LPVOID lpMem,
                           SIZE_T dwBytes )
{
    LPVOID lpNewMem = pOrgHeapReAlloc( hHeap, dwFlags, lpMem, dwBytes );
    try
    {
        CSingleLock lockObj( &SyncObj, TRUE );
        if( g_bHooked && g_bTrack )
        {
            g_bTrack = false;
            MEM_INFO stInfo;
            if( m_MemMap.Lookup( lpMem, stInfo ))
            {				
                m_MemMap.RemoveKey( lpMem );
                m_MemMap[lpNewMem] = stInfo;

            }
            g_bTrack = true;
        }
    }
    catch (...)
    {
    }
    return lpNewMem;
}

LPVOID WINAPI MyVirtualAlloc( LPVOID lpAddress,
                              SIZE_T dwSize, DWORD flAllocationType,
                              DWORD flProtect )
{
    LPVOID lMem =  pOrgVirtualAlloc( lpAddress, dwSize, flAllocationType, flProtect );
    CreateCallStack( lMem, dwSize );
    return lMem;
}

BOOL WINAPI MyVirtualFree( LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType )
{
    RemovCallStack( lpAddress );
    return pOrgVirtualFree( lpAddress, dwSize, dwFreeType );
}

LPVOID WINAPI MyVirtualAllocEx( HANDLE hProcess, LPVOID lpAddress,
                                SIZE_T dwSize, DWORD flAllocationType,
                                DWORD flProtect )
{
    LPVOID lMem =  pOrgVirtualAllocEx( hProcess, lpAddress, dwSize, flAllocationType, flProtect );
    CreateCallStack( lMem, dwSize );
    return lMem;
}

BOOL WINAPI MyVirtualFreeEx( HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType )
{
    RemovCallStack( lpAddress );
    return pOrgVirtualFreeEx( hProcess, lpAddress, dwSize, dwFreeType );
}

BOOL WINAPI MyHeapFree(  HANDLE hHeap,  DWORD dwFlags,  LPVOID lpMem )
{
    RemovCallStack( lpMem );
    return pOrgHeapFree( hHeap, dwFlags, lpMem );
}


HANDLE WINAPI MyGlobalAlloc( UINT uFlags, SIZE_T dwBytes )
{
    HANDLE hHandle = pOrgGlobalAlloc( uFlags,  dwBytes );
    if ( g_HookType == HT_MEMORY )
        CreateCallStack(hHandle, dwBytes);
    else
        CreateCallStack(hHandle, TYPE_MEMORY_HANDLE );
    return hHandle;
}

HANDLE WINAPI MyGlobalReAlloc( HGLOBAL hMem, SIZE_T dwBytes, UINT uFlags )
{
    HANDLE hHandle = pOrgGlobalReAlloc( hMem, dwBytes, uFlags  );
    if( hHandle )
    {
        if ( g_HookType == HT_MEMORY )
            CreateCallStack(hHandle, dwBytes);
        else
            CreateCallStack(hHandle, TYPE_MEMORY_HANDLE );
        RemovCallStack( hMem );
    }
    return hHandle;
}

HANDLE WINAPI MyGlobalFree( HGLOBAL hMem )
{
    HANDLE hHandle = pOrgGlobalFree( hMem );
    if ( hHandle == NULL )
        RemovCallStack( hMem );
    return hHandle;

}
HLOCAL WINAPI MyLocalAlloc( UINT uFlags, SIZE_T uBytes )
{
    HLOCAL hHandle = pOrgLocalAlloc( uFlags,  uBytes );
    if ( g_HookType == HT_MEMORY )
        CreateCallStack(hHandle, uBytes);
    else
        CreateCallStack(hHandle, TYPE_MEMORY_HANDLE );
    return hHandle;
}

HLOCAL WINAPI MyLocalReAlloc( HLOCAL hMem, SIZE_T uBytes, UINT uFlags )
{
    HLOCAL hHandle = pOrgLocalReAlloc( hMem, uBytes, uFlags);
    if( hHandle )
    {
        if ( g_HookType == HT_MEMORY )
            CreateCallStack(hHandle, uBytes);
        else
            CreateCallStack(hHandle, TYPE_MEMORY_HANDLE );
        RemovCallStack( hMem );
    }
    return hHandle;
}

HLOCAL WINAPI MyLocalFree(HLOCAL hMem )
{
    HLOCAL hHandle = pOrgLocalFree(hMem );
    if ( hHandle == NULL )
        RemovCallStack( hMem );
    return hHandle;
}

LPVOID WINAPI MyCoTaskMemAlloc( SIZE_T cb)
{
    LPVOID lpMem = pOrgCoTaskMemAlloc( cb );
    CreateCallStack( lpMem, cb );
    return lpMem;
}

LPVOID WINAPI MyCoTaskMemRealloc(LPVOID pv, SIZE_T cb)
{
    LPVOID lpMem = pOrgCoTaskMemRealloc(pv, cb );
    if( lpMem )
    {
        CreateCallStack( lpMem, cb );
        RemovCallStack( pv );
    }
    return lpMem;
}

void   WINAPI MyCoTaskMemFree( LPVOID pv )
{
    RemovCallStack( pv );
}

LPVOID WINAPI MyMapViewOfFile( HANDLE hFileMappingObject, DWORD dwDesiredAccess,
    DWORD dwFileOffsetHigh, DWORD dwFileOffsetLow, SIZE_T dwNumberOfBytesToMap )
{
    LPVOID lMem = pOrgMapViewOfFile( hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh,
    dwFileOffsetLow, dwNumberOfBytesToMap);
    CreateCallStack( lMem, dwNumberOfBytesToMap );
    return lMem;
}

LPVOID WINAPI MyMapViewOfFileEx( HANDLE hFileMappingObject, DWORD dwDesiredAccess,
    DWORD dwFileOffsetHigh, DWORD dwFileOffsetLow, SIZE_T dwNumberOfBytesToMap, LPVOID lpBaseAddress )
{
    LPVOID lMem = pOrgMapViewOfFileEx( hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh,
    dwFileOffsetLow, dwNumberOfBytesToMap, lpBaseAddress);
    CreateCallStack( lMem, dwNumberOfBytesToMap );
    return lMem;
}

BOOL WINAPI MyUnmapViewOfFile( LPCVOID lpBaseAddress )
{
    RemovCallStack( (LPVOID)lpBaseAddress );
    return pOrgUnmapViewOfFile( lpBaseAddress );
}

INT MyNtMapViewOfSection( HANDLE SectionHandle, HANDLE ProcessHandle, PVOID *BaseAddress,
    ULONG_PTR ZeroBits, SIZE_T CommitSize, PLARGE_INTEGER SectionOffset, PSIZE_T ViewSize,
    SECTION_INHERIT InheritDisposition, ULONG AllocationType, ULONG Win32Protect )
{
    INT s = pOrgNtMapViewOfSection( SectionHandle, ProcessHandle, BaseAddress, ZeroBits, CommitSize,
    SectionOffset, ViewSize, InheritDisposition, AllocationType, Win32Protect );
    CreateCallStack( BaseAddress, *ViewSize );
    return s;
}

INT MyNtUnmapViewOfSection( HANDLE ProcessHandle, PVOID BaseAddress )
{
    INT s = pOrgNtUnmapViewOfSection( ProcessHandle, BaseAddress );
    RemovCallStack( (LPVOID)BaseAddress );
    return s;
}


const int MEMORY_FUN_NO = 19;
void HookMemAlloc()
{
    g_arrHookedFunctions.SetSize( MEMORY_FUN_NO ); 
    int nIndex = 0;
    HMODULE Kernel32 = GetModuleHandle( "Kernel32.dll" );
    HMODULE Ole32 = GetModuleHandle( "ole32.dll" );
    HMODULE ntdll = GetModuleHandle( "ntdll.dll" );

    REGISTER_HOOK(HeapAlloc, Kernel32 );
    REGISTER_HOOK(HeapFree,Kernel32 );
    REGISTER_HOOK(HeapReAlloc,Kernel32 );
    REGISTER_HOOK(VirtualAlloc,Kernel32 );    
    REGISTER_HOOK(VirtualFree,Kernel32 );
    REGISTER_HOOK(VirtualAllocEx,Kernel32 );    
    REGISTER_HOOK(VirtualFreeEx,Kernel32 );
    REGISTER_HOOK(GlobalAlloc,Kernel32 );
    REGISTER_HOOK(GlobalReAlloc,Kernel32 );
    REGISTER_HOOK(GlobalFree,Kernel32 );
    REGISTER_HOOK(LocalAlloc,Kernel32 );
    REGISTER_HOOK(LocalReAlloc,Kernel32 );
    REGISTER_HOOK(LocalFree,Kernel32 );
    REGISTER_HOOK(MapViewOfFile,Kernel32 );
    REGISTER_HOOK(MapViewOfFileEx,Kernel32 );
    REGISTER_HOOK(UnmapViewOfFile,Kernel32 );
    REGISTER_HOOK(CoTaskMemAlloc,Ole32 );
    REGISTER_HOOK(CoTaskMemRealloc,Ole32 );
    REGISTER_HOOK(CoTaskMemFree,Ole32 );
    ///*REGISTER_HOOK( NtMapViewOfSection, ntdll);
    //REGISTER_HOOK( NtUnmapViewOfSection, ntdll );*/

    ASSERT( nIndex == MEMORY_FUN_NO );
    HookDynamicLoadedFun( MEMORY_FUN_NO, g_arrHookedFunctions.GetData());
}

void RestoreHooks()
{
    HookDynamicLoadedFun( g_arrHookedFunctions.GetCount(), g_arrHookedFunctions.GetData(), true );
}

//////////////////////////////////////////////////////////////////////////
                 //gdi functions
//////////////////////////////////////////////////////////////////////////

// bitmap

HANDLE WINAPI MyLoadImageA(HINSTANCE hInst,LPCSTR name,UINT type,int cx,int cy,UINT fuLoad)
{
    HANDLE hImg = pOrgLoadImageA(hInst,name,type,cx,cy,fuLoad);
    CreateCallStack( hImg , 0 );
    return hImg;
}

HANDLE WINAPI MyLoadImageW( HINSTANCE hInst,LPCWSTR name,UINT type,int cx,int cy,UINT fuLoad)
{
    HANDLE hImg = pOrgLoadImageW(hInst,name,type,cx,cy,fuLoad);
    CreateCallStack( hImg , 0 );
    return hImg;
}

HBITMAP WINAPI MyLoadBitmapA( HINSTANCE hInstance, LPCSTR lpBitmapName)
{
    HBITMAP hBmp = pOrgLoadBitmapA( hInstance, lpBitmapName );
    CreateCallStack( hBmp , 0 );
    return hBmp;
}
HBITMAP WINAPI MyLoadBitmapW( HINSTANCE hInstance, LPCWSTR lpBitmapName )
{
    HBITMAP hBmp =  pOrgLoadBitmapW( hInstance, lpBitmapName );
    CreateCallStack( hBmp , 0 );
    return hBmp;
}

HANDLE  WINAPI MyLoadImageADef( HINSTANCE hInst, LPCSTR name, UINT type,int cx,int cy, UINT fuLoad)
{
    HANDLE hBmp =  pOrgLoadImageA( hInst, name, type,cx,cy, fuLoad );
    CreateCallStack( hBmp , type );
    return hBmp;
}

HANDLE  WINAPI MyLoadImageWDef( HINSTANCE hInst, LPCWSTR name, UINT type, int cx, int cy, UINT fuLoad)
{
    HANDLE hBmp =  pOrgLoadImageW( hInst, name, type,cx,cy, fuLoad );
    CreateCallStack( hBmp , type );
    return hBmp;
}

HBITMAP WINAPI MyCreateBitmap(  int nWidth,  int nHeight, UINT nPlanes,  UINT nBitCount,  CONST VOID *lpBits)
{
    HBITMAP hBmp =  pOrgCreateBitmap( nWidth,  nHeight, nPlanes,   nBitCount,  lpBits );
    CreateCallStack( hBmp , 0 );
    return hBmp;
}

HBITMAP WINAPI MyCreateBitmapIndirect(  CONST BITMAP *pbm )
{
    HBITMAP hBmp =  pOrgCreateBitmapIndirect( pbm );
    CreateCallStack( hBmp , 0 );
    return hBmp;
}

HBITMAP WINAPI MyCreateCompatibleBitmap(  HDC hdc,  int cx,  int cy)
{
    HBITMAP hBmp =  pOrgCreateCompatibleBitmap( hdc,   cx,   cy);
    CreateCallStack( hBmp , 0 );
    return hBmp;
}

HBITMAP WINAPI MyCreateDIBitmap(  HDC hdc,  CONST BITMAPINFOHEADER *pbmih,  DWORD flInit,  CONST VOID *pjBits,  CONST BITMAPINFO *pbmi,  UINT iUsage)
{
    HBITMAP hBmp =  pOrgCreateDIBitmap( hdc,   pbmih,   flInit,  pjBits,   pbmi,   iUsage);
    CreateCallStack( hBmp , 0 );
    return hBmp;
}

HBITMAP WINAPI MyCreateDIBSection( HDC hdc,  CONST BITMAPINFO *lpbmi,  UINT usage, VOID **ppvBits,  HANDLE hSection,  DWORD offset)
{
    HBITMAP hBmp =  pOrgCreateDIBSection( hdc,  lpbmi,  usage, ppvBits,  hSection,  offset );
    CreateCallStack( hBmp , 0 );
    return hBmp;
}


HBITMAP WINAPI MyCreateDiscardableBitmap( HDC hdc, int cx, int cy)
{
    HBITMAP hBmp =  pOrgCreateDiscardableBitmap( hdc, cx, cy );
    CreateCallStack( hBmp , 0 );
    return hBmp;
}

HANDLE  WINAPI MyCopyImage( HANDLE h, UINT type, int cx, int cy, UINT flags)
{
    HANDLE hBmp =  pOrgCopyImage(h, type, cx, cy, flags);
    int nType = 0;
    CreateCallStack( hBmp , type );
    return hBmp;
}

BOOL WINAPI MyGetIconInfo( HICON hIcon, PICONINFO piconinfo)
{
    BOOL bRet = pOrgGetIconInfo( hIcon, piconinfo );
    if( bRet )
    {
        CreateCallStack( piconinfo->hbmColor, 0 );
        if( piconinfo->hbmMask )
        {
            CSingleLock lockObj( &SyncObj, TRUE );
            if( g_bHooked && g_bTrack )
            {
                g_bTrack = false;
                MEM_INFO stInfo;
                if( m_MemMap.Lookup( piconinfo->hbmColor, stInfo ))
                {
                    MEM_INFO stInfo2;
                    stInfo2.nMemSize = stInfo.nMemSize;
                    //stInfo2.parCallStack = new STACK_ARRAY;
                    void * p = AllocMem(sizeof(STACK_ARRAY));
                    stInfo2.parCallStack = new( (void*)p ) STACK_ARRAY;
                    stInfo2.parCallStack->Copy( *stInfo.parCallStack );
                    m_MemMap[piconinfo->hbmMask] = stInfo2;
                }
                g_bTrack = true;
            }
        }
        
    }
    return bRet;
}

BOOL WINAPI MyGetIconInfoExA( HICON hicon, PICONINFOEXA piconinfo)
{
    BOOL bRet = pOrgGetIconInfoExA( hicon, piconinfo );
    if( bRet )
    {
        CreateCallStack( piconinfo->hbmColor, 0 );
        if( piconinfo->hbmMask )
        {
            CSingleLock lockObj( &SyncObj, TRUE );
            if( g_bHooked && g_bTrack )
            {
                g_bTrack = false;
                MEM_INFO stInfo;
                if( m_MemMap.Lookup( piconinfo->hbmColor, stInfo ))
                {
                    MEM_INFO stInfo2;
                    stInfo2.nMemSize = stInfo.nMemSize;
                    //stInfo2.parCallStack = new STACK_ARRAY;
                    void * p = AllocMem(sizeof(STACK_ARRAY));
                    stInfo2.parCallStack = new( (void*)p ) STACK_ARRAY;
                    stInfo2.parCallStack->Copy( *stInfo.parCallStack );
                    m_MemMap[piconinfo->hbmMask] = stInfo2;
                }
                g_bTrack = true;
            }
        }

    }
    return bRet;
}

BOOL WINAPI MyGetIconInfoExW( HICON hicon,PICONINFOEXW piconinfo)
{
    BOOL bRet = pOrgGetIconInfoExW( hicon, piconinfo );
    if( bRet )
    {
        CreateCallStack( piconinfo->hbmColor, 0 );
        if( piconinfo->hbmMask )
        {
            CSingleLock lockObj( &SyncObj, TRUE );
            if( g_bHooked && g_bTrack )
            {
                g_bTrack = false;
                MEM_INFO stInfo;
                if( m_MemMap.Lookup( piconinfo->hbmColor, stInfo ))
                {
                    MEM_INFO stInfo2;
                    stInfo2.nMemSize = stInfo.nMemSize;
                    //stInfo2.parCallStack = new STACK_ARRAY;
                    void * p = AllocMem(sizeof(STACK_ARRAY));
                    stInfo2.parCallStack = new( (void*)p ) STACK_ARRAY;
                    stInfo2.parCallStack->Copy( *stInfo.parCallStack );
                    m_MemMap[piconinfo->hbmMask] = stInfo2;
                }
                g_bTrack = true;
            }
        }
    }
    return bRet;
}


BOOL WINAPI MyDeleteObject(  HGDIOBJ ho)
{
    BOOL bRet =  pOrgDeleteObject( ho );
    if( bRet )
        RemovCallStack( ho );
    return bRet;
}

//////////////////////////////////////////////////////////////////////////
//ICONS
HICON WINAPI MyCopyIcon( HICON hIcon)
{
    HICON hIconnew = pOrgCopyIcon( hIcon );
    CreateCallStack( hIconnew, IMAGE_ICON );
    return hIconnew;
}

HICON WINAPI MyCreateIcon(HINSTANCE hInstance,int nWidth,int nHeight,BYTE cPlanes,BYTE cBitsPixel,CONST BYTE *lpbANDbits,CONST BYTE *lpbXORbits)
{
    HICON hIcon = pOrgCreateIcon( hInstance,nWidth,nHeight,cPlanes,cBitsPixel,lpbANDbits,lpbXORbits) ;
    CreateCallStack( hIcon, IMAGE_ICON );
    return hIcon;
}

HICON WINAPI MyCreateIconFromResource( PBYTE presbits, DWORD dwResSize, BOOL fIcon, DWORD dwVer)
{
    HICON hIcon = pOrgCreateIconFromResource(presbits, dwResSize, fIcon, dwVer);
    CreateCallStack( hIcon, IMAGE_ICON );
    return hIcon;
}

HICON WINAPI MyCreateIconFromResourceEx( PBYTE presbits, DWORD dwResSize,BOOL fIcon,DWORD dwVer,int cxDesired,int cyDesired,UINT Flags )
{
    HICON hIcon = pOrgCreateIconFromResourceEx(presbits, dwResSize,fIcon,dwVer,cxDesired,cyDesired,Flags );
    CreateCallStack( hIcon, IMAGE_ICON );
    return hIcon;
}

HICON WINAPI MyCreateIconIndirect( PICONINFO piconinfo )
{
    HICON hIcon = pOrgCreateIconIndirect(piconinfo );
    CreateCallStack( hIcon, IMAGE_ICON );
    return hIcon;
}

BOOL  WINAPI MyDestroyIcon(HICON hIcon)
{
    BOOL bRet = pOrgDestroyIcon( hIcon );
    if( bRet )
        RemovCallStack( hIcon );
    return bRet;
}

HICON WINAPI MyDuplicateIcon(HINSTANCE hInst, HICON hIcon)
{
    HICON hIconnew = pOrgDuplicateIcon( hInst, hIcon );
    CreateCallStack( hIconnew, IMAGE_ICON );
    return hIconnew;
}

HICON WINAPI MyExtractAssociatedIconA(HINSTANCE hInst,  LPSTR lpIconPath,  LPWORD lpiIcon)
{
    HICON hIcon = pOrgExtractAssociatedIconA(hInst,  lpIconPath,  lpiIcon);
    CreateCallStack( hIcon, IMAGE_ICON );
    return hIcon;
}

HICON WINAPI MyExtractAssociatedIconW(HINSTANCE hInst,  LPWSTR lpIconPath,  LPWORD lpiIcon)
{
    HICON hIcon = pOrgExtractAssociatedIconW(hInst,  lpIconPath,  lpiIcon);
    CreateCallStack( hIcon, IMAGE_ICON );
    return hIcon;
}

HICON WINAPI MyExtractAssociatedIconExA(HINSTANCE hInst,LPSTR lpIconPath,  LPWORD lpiIconIndex,  LPWORD lpiIconId)
{
    HICON hIcon = pOrgExtractAssociatedIconExA(hInst,lpIconPath,  lpiIconIndex,  lpiIconId);
    CreateCallStack( hIcon, IMAGE_ICON );
    return hIcon;
}

HICON WINAPI MyExtractAssociatedIconExW(HINSTANCE hInst,LPWSTR lpIconPath,  LPWORD lpiIconIndex,  LPWORD lpiIconId)
{
    HICON hIcon = pOrgExtractAssociatedIconExW(hInst,lpIconPath,  lpiIconIndex,  lpiIconId);
    CreateCallStack( hIcon, IMAGE_ICON );
    return hIcon;
}

HICON WINAPI MyExtractIconA(HINSTANCE hInst, LPCSTR lpszExeFileName, UINT nIconIndex)
{
    HICON hIcon = pOrgExtractIconA(hInst, lpszExeFileName, nIconIndex);
    CreateCallStack( hIcon, IMAGE_ICON );
    return hIcon;
}

HICON WINAPI MyExtractIconW(HINSTANCE hInst, LPCWSTR lpszExeFileName, UINT nIconIndex)
{
    HICON hIcon = pOrgExtractIconW(hInst, lpszExeFileName, nIconIndex);
    CreateCallStack( hIcon, IMAGE_ICON );
    return hIcon;
}

UINT  WINAPI MyExtractIconExA(LPCSTR lpszFile, int nIconIndex, HICON *phiconLarge, HICON *phiconSmall, UINT nIcons)
{
    UINT uRet = pOrgExtractIconExA(lpszFile, nIconIndex, phiconLarge, phiconSmall, nIcons);
    if( uRet <= 0 || ( -1 == nIconIndex && !phiconLarge && !phiconSmall))
    {
        return uRet;
    }
    MEM_INFO stInfo;	
    CSingleLock lockObj( &SyncObj, TRUE );
    bool bFirst = true;
    if( g_bHooked && g_bTrack )
    {
        g_bTrack = false;			
        for( UINT uIdx = 1; uIdx < uRet; uIdx++ )
        {
            if( phiconLarge[uIdx] )
            {				
                if( bFirst )
                {
                    CreateCallStack( phiconLarge[uIdx], IMAGE_ICON );
                    if( m_MemMap.Lookup( phiconLarge[uIdx], stInfo ))
                        bFirst = false;
                }
                else
                {
                    MEM_INFO stInfo2;
                    stInfo2.nMemSize = stInfo.nMemSize;
                    //stInfo2.parCallStack = new STACK_ARRAY;
                    void * p = AllocMem(sizeof(STACK_ARRAY));
                    stInfo2.parCallStack = new( (void*)p ) STACK_ARRAY;
                    stInfo2.parCallStack->Copy( *stInfo.parCallStack );
                    m_MemMap[phiconLarge[uIdx]] = stInfo2;
                }					
            }

            if( phiconSmall[uIdx] )
            {				
                if( bFirst )
                {
                    CreateCallStack( phiconSmall[uIdx], IMAGE_ICON );
                    if( m_MemMap.Lookup( phiconLarge[uIdx], stInfo ))
                        bFirst = false;
                }
                else
                {
                    MEM_INFO stInfo2;
                    stInfo2.nMemSize = stInfo.nMemSize;
                    //stInfo2.parCallStack = new STACK_ARRAY;
                    void * p = AllocMem(sizeof(STACK_ARRAY));
                    stInfo2.parCallStack = new( (void*)p ) STACK_ARRAY;
                    stInfo2.parCallStack->Copy( *stInfo.parCallStack );
                    m_MemMap[phiconSmall[uIdx]] = stInfo2;
                }					
            }
        }
        g_bTrack = true;
    }		
    return uRet;
}

UINT  WINAPI MyExtractIconExW(LPCWSTR lpszFile, int nIconIndex,  HICON *phiconLarge, HICON *phiconSmall, UINT nIcons)
{
    UINT uRet = pOrgExtractIconExW(lpszFile, nIconIndex, phiconLarge, phiconSmall, nIcons);
    if( uRet <= 0 || ( -1 == nIconIndex && !phiconLarge && !phiconSmall))
    {
        return uRet;
    }
    MEM_INFO stInfo;	
    CSingleLock lockObj( &SyncObj, TRUE );
    bool bFirst = true;
    if( g_bHooked && g_bTrack )
    {
        g_bTrack = false;			
        for( UINT uIdx = 1; uIdx < uRet; uIdx++ )
        {
            if( phiconLarge[uIdx] )
            {				
                if( bFirst )
                {
                    CreateCallStack( phiconLarge[uIdx], IMAGE_ICON );
                    if( m_MemMap.Lookup( phiconLarge[0], stInfo ))
                        bFirst = false;
                }
                else
                {
                    MEM_INFO stInfo2;
                    stInfo2.nMemSize = stInfo.nMemSize;
                    //stInfo2.parCallStack = new STACK_ARRAY;
                    void * p = AllocMem(sizeof(STACK_ARRAY));
                    stInfo2.parCallStack = new( (void*)p ) STACK_ARRAY;
                    stInfo2.parCallStack->Copy( *stInfo.parCallStack );
                    m_MemMap[phiconLarge[uIdx]] = stInfo2;
                }					
            }

            if( phiconSmall[uIdx] )
            {				
                if( bFirst )
                {
                    CreateCallStack( phiconSmall[uIdx], IMAGE_ICON );
                    if( m_MemMap.Lookup( phiconLarge[0], stInfo ))
                        bFirst = false;
                }
                else
                {
                    MEM_INFO stInfo2;
                    stInfo2.nMemSize = stInfo.nMemSize;
                    //stInfo2.parCallStack = new STACK_ARRAY;
                    void * p = AllocMem(sizeof(STACK_ARRAY));
                    stInfo2.parCallStack = new( (void*)p ) STACK_ARRAY;
                    stInfo2.parCallStack->Copy( *stInfo.parCallStack );
                    m_MemMap[phiconSmall[uIdx]] = stInfo2;
                }					
            }
        }
        g_bTrack = true;
    }		
    return uRet;
}

HICON WINAPI MyLoadIconA( HINSTANCE hInstance, LPCSTR lpIconName )
{
    HICON hIcon = pOrgLoadIconA(hInstance, lpIconName );
    if( hIcon && hInstance )
    {
        CreateCallStack( hIcon, IMAGE_ICON );
    }
    return hIcon;
}

HICON WINAPI MyLoadIconW( HINSTANCE hInstance, LPCWSTR lpIconName )
{
    HICON hIcon = pOrgLoadIconW(hInstance, lpIconName );
    if( hIcon && hInstance )
    {
        CreateCallStack( hIcon, IMAGE_ICON );
    }
    return hIcon;
}

UINT  WINAPI MyPrivateExtractIconsA( LPCSTR szFileName, int nIconIndex, int cxIcon, int cyIcon, HICON *phicon, UINT *piconid, UINT nIcons, UINT flags)
{
    UINT uRet = pOrgPrivateExtractIconsA(szFileName, nIconIndex, cxIcon, cyIcon, phicon, piconid, nIcons, flags);
    if( uRet <= 0 || !phicon )
    {
        return uRet;
    }
    MEM_INFO stInfo;	
    CSingleLock lockObj( &SyncObj, TRUE );
    bool bFirst = true;
    if( g_bHooked && g_bTrack )
    {
        g_bTrack = false;			
        for( UINT uIdx = 1; uIdx < uRet; uIdx++ )
        {
            if( phicon[uIdx] )
            {				
                if( bFirst )
                {
                    CreateCallStack( phicon[uIdx], IMAGE_ICON );
                    if( m_MemMap.Lookup( phicon[uIdx], stInfo ))
                        bFirst = false;
                }
                else
                {
                    MEM_INFO stInfo2;
                    stInfo2.nMemSize = stInfo.nMemSize;
                    //stInfo2.parCallStack = new STACK_ARRAY;
                    void * p = AllocMem(sizeof(STACK_ARRAY));
                    stInfo2.parCallStack = new( (void*)p ) STACK_ARRAY;
                    stInfo2.parCallStack->Copy( *stInfo.parCallStack );
                    m_MemMap[phicon[uIdx]] = stInfo2;
                }					
            }
        }
        g_bTrack = true;
    }		
    return uRet;
}

UINT  WINAPI MyPrivateExtractIconsW( LPCWSTR szFileName, int nIconIndex, int cxIcon, int cyIcon, HICON *phicon, UINT *piconid,UINT nIcons,UINT flags)
{
    UINT uRet = pOrgPrivateExtractIconsW(szFileName, nIconIndex, cxIcon, cyIcon, phicon, piconid, nIcons, flags);
    if( uRet <= 0 || !phicon )
    {
        return uRet;
    }
    MEM_INFO stInfo;	
    CSingleLock lockObj( &SyncObj, TRUE );
    bool bFirst = true;
    if( g_bHooked && g_bTrack )
    {
        g_bTrack = false;			
        for( UINT uIdx = 1; uIdx < uRet; uIdx++ )
        {
            if( phicon[uIdx] )
            {				
                if( bFirst )
                {
                    CreateCallStack( phicon[uIdx], IMAGE_ICON );
                    if( m_MemMap.Lookup( phicon[uIdx], stInfo ))
                        bFirst = false;
                }
                else
                {
                    MEM_INFO stInfo2;
                    stInfo2.nMemSize = stInfo.nMemSize;
                    //stInfo2.parCallStack = new STACK_ARRAY;
                    void * p = AllocMem(sizeof(STACK_ARRAY));
                    stInfo2.parCallStack = new( (void*)p ) STACK_ARRAY;
                    stInfo2.parCallStack->Copy( *stInfo.parCallStack );
                    m_MemMap[phicon[uIdx]] = stInfo2;
                }					
            }
        }
        g_bTrack = true;
    }		
    return uRet;
}


//////////////////////////////////////////////////////////////////////////
// Cursors
HCURSOR WINAPI MyCreateCursor( HINSTANCE hInst, int xHotSpot, int yHotSpot,int nWidth, int nHeight, CONST VOID *pvANDPlane,CONST VOID *pvXORPlane)
{
    HCURSOR hCur = pOrgCreateCursor(hInst, xHotSpot, yHotSpot,nWidth, nHeight, pvANDPlane,pvXORPlane);
    CreateCallStack( hCur, IMAGE_CURSOR);
    return hCur;
}

HCURSOR WINAPI MyLoadCursorA( HINSTANCE hInstance, LPCSTR lpCursorName)
{
    HCURSOR hCur = pOrgLoadCursorA( hInstance, lpCursorName );
    if( hInstance )
    {
        CreateCallStack( hCur, IMAGE_CURSOR );
    }	
    return hCur;
}

HCURSOR WINAPI MyLoadCursorW( HINSTANCE hInstance, LPCWSTR lpCursorName)
{
    HCURSOR hCur = pOrgLoadCursorW( hInstance, lpCursorName );
    if( hInstance )
    {
        CreateCallStack( hCur, IMAGE_CURSOR );
    }	
    return hCur;
}

HCURSOR WINAPI MyLoadCursorFromFileA( LPCSTR lpFileName )
{
    HCURSOR hCur = pOrgLoadCursorFromFileA(lpFileName);
    CreateCallStack( hCur,IMAGE_CURSOR );
    return hCur;
}

HCURSOR WINAPI MyLoadCursorFromFileW( LPCWSTR lpFileName )
{
    HCURSOR hCur = pOrgLoadCursorFromFileW(lpFileName);
    CreateCallStack( hCur, IMAGE_CURSOR );
    return hCur;
}

BOOL WINAPI MyDestroyCursor( HCURSOR hCursor )
{
    BOOL bRet = pOrgDestroyCursor( hCursor );
    if( bRet )
    {
        RemovCallStack( hCursor );
    }
    return bRet;
}

//brush
HBRUSH  WINAPI MyCreateBrushIndirect(  CONST LOGBRUSH *plbrush)
{
    HBRUSH hBr = pOrgCreateBrushIndirect(plbrush);
    CreateCallStack( hBr, 0 );
    return hBr;
}

HBRUSH  WINAPI MyCreateSolidBrush(  COLORREF color)
{
    HBRUSH hBr = pOrgCreateSolidBrush(color);
    CreateCallStack( hBr, 0 );
    return hBr;
}

HBRUSH  WINAPI MyCreatePatternBrush(  HBITMAP hbm)
{
    HBRUSH hBr = pOrgCreatePatternBrush(hbm);
    CreateCallStack( hBr, 0 );
    return hBr;
}

HBRUSH  WINAPI MyCreateDIBPatternBrush(  HGLOBAL h,  UINT iUsage)
{
    HBRUSH hBr = pOrgCreateDIBPatternBrush(h, iUsage );
    CreateCallStack( hBr, 0 );
    return hBr;
}

HBRUSH  WINAPI MyCreateDIBPatternBrushPt(  CONST VOID *lpPackedDIB,  UINT iUsage)
{
    HBRUSH hBr = pOrgCreateDIBPatternBrushPt(lpPackedDIB,iUsage);
    CreateCallStack( hBr, 0 );
    return hBr;
}

HBRUSH  WINAPI MyCreateHatchBrush(  int iHatch,  COLORREF color)
{
    HBRUSH hBr = pOrgCreateHatchBrush( iHatch, color );
    CreateCallStack( hBr, 0 );
    return hBr;
}

// DC functions
 HDC WINAPI MyCreateCompatibleDC( HDC hdc )
 {
    HDC hDC = pOrgCreateCompatibleDC( hdc );
    CreateCallStack( hDC, 0 );
    return hDC;
 }
 
 HDC WINAPI MyCreateDCA( LPCSTR pwszDriver,  LPCSTR pwszDevice,  LPCSTR pszPort,  CONST DEVMODEA * pdm )
 {
    HDC hDC = pOrgCreateDCA( pwszDriver, pwszDevice, pszPort,  pdm );
    CreateCallStack( hDC , 0 );
    return hDC;
 }

 HDC WINAPI MyCreateDCW( LPCWSTR pwszDriver,  LPCWSTR pwszDevice,  LPCWSTR pszPort,  CONST DEVMODEW * pdm )
 {
    HDC hDC = pOrgCreateDCW( pwszDriver, pwszDevice, pszPort,  pdm );
    CreateCallStack( hDC, 0 );
    return hDC;
 }

 HDC WINAPI MyCreateICA( LPCSTR pszDriver,  LPCSTR pszDevice,  LPCSTR pszPort,  CONST DEVMODEA * pdm )
 {
    HDC hDC = pOrgCreateICA( pszDriver, pszDevice, pszPort,  pdm );
    CreateCallStack( hDC, 0 );
    return hDC;
 }

 HDC WINAPI MyCreateICW( LPCWSTR pszDriver,  LPCWSTR pszDevice,  LPCWSTR pszPort,  CONST DEVMODEW * pdm )
 {
    HDC hDC = pOrgCreateICW( pszDriver, pszDevice, pszPort,  pdm );
    CreateCallStack( hDC, 0 );
    return hDC;
 }

 HDC WINAPI MyGetDC( HWND hWnd )
 {
    HDC hDC = pOrgGetDC( hWnd );
    CreateCallStack( hDC, 0 );
    return hDC;
 }

 HDC WINAPI MyGetDCEx( HWND hWnd, HRGN hrgnClip, DWORD flags )
 {
    HDC hDC = pOrgGetDCEx(  hWnd, hrgnClip, flags );
    CreateCallStack( hDC, 0 );
    return hDC;
 }

 HDC WINAPI MyGetWindowDC( HWND hWnd )
 {
    HDC hDC = pOrgGetWindowDC( hWnd );
    CreateCallStack( hDC, 0 );
    return hDC;
 }


int WINAPI MyReleaseDC( HWND hWnd, HDC hDC)
{
    int nRet = pOrgReleaseDC( hWnd, hDC );
    if( nRet )
        RemovCallStack( hDC );
    return nRet;
}

BOOL WINAPI MyDeleteDC( HDC hdc)
{
    BOOL bRet = pOrgDeleteDC( hdc );
    if( bRet )
    {
        RemovCallStack( hdc );
    }	
    return bRet;
}

 // font functions
 HFONT WINAPI MyCreateFontA(  int cHeight,  int cWidth,  int cEscapement,  int cOrientation,  int cWeight,  DWORD bItalic,
     DWORD bUnderline,  DWORD bStrikeOut,  DWORD iCharSet,  DWORD iOutPrecision,  DWORD iClipPrecision,
     DWORD iQuality,  DWORD iPitchAndFamily, LPCSTR pszFaceName)
 {
    HFONT hFont = pOrgCreateFontA( cHeight,  cWidth,  cEscapement,  cOrientation,   cWeight,   bItalic,
          bUnderline,   bStrikeOut,   iCharSet,   iOutPrecision,   iClipPrecision,
          iQuality,   iPitchAndFamily,  pszFaceName );
    CreateCallStack( hFont, 0 );
    return hFont;
 }

 HFONT WINAPI MyCreateFontW(  int cHeight,  int cWidth,  int cEscapement,  int cOrientation,  int cWeight,  DWORD bItalic,
     DWORD bUnderline,  DWORD bStrikeOut,  DWORD iCharSet,  DWORD iOutPrecision,  DWORD iClipPrecision,
     DWORD iQuality,  DWORD iPitchAndFamily, LPCWSTR pszFaceName)
 {

     HFONT hFont = pOrgCreateFontW( cHeight,  cWidth,  cEscapement,  cOrientation,   cWeight,   bItalic,
         bUnderline,   bStrikeOut,   iCharSet,   iOutPrecision,   iClipPrecision,
         iQuality,   iPitchAndFamily,  pszFaceName );
     CreateCallStack( hFont, 0 );
     return hFont;
 }

 HFONT WINAPI MyCreateFontIndirectA(  CONST LOGFONTA *lplf)
 {
     HFONT hFont = pOrgCreateFontIndirectA( lplf );
     CreateCallStack( hFont, 0 );
     return hFont;
 }

 HFONT WINAPI MyCreateFontIndirectW( CONST LOGFONTW *lplf)
 {
     HFONT hFont = pOrgCreateFontIndirectW( lplf );
     CreateCallStack( hFont, 0 );
     return hFont;
 }

 // Meta File
HDC WINAPI MyCreateMetaFileA(  LPCSTR pszFile )
{
    HDC hDC = pOrgCreateMetaFileA( pszFile );
    CreateCallStack( hDC, 0 );
    return hDC;
}

HDC WINAPI MyCreateMetaFileW(  LPCWSTR pszFile )
{
    HDC hDC = pOrgCreateMetaFileW( pszFile );
    CreateCallStack( hDC, 0 );
    return hDC;
}

HDC WINAPI MyCreateEnhMetaFileA(  HDC hdc,  LPCSTR lpFilename,  CONST RECT *lprc,  LPCSTR lpDesc)
{
    HDC hDC = pOrgCreateEnhMetaFileA( hdc, lpFilename, lprc, lpDesc );
    CreateCallStack( hDC, 0 );
    return hDC;
}

HDC WINAPI MyCreateEnhMetaFileW(  HDC hdc,  LPCWSTR lpFilename,  CONST RECT *lprc,  LPCWSTR lpDesc)
{
    HDC hDC = pOrgCreateEnhMetaFileW( hdc, lpFilename, lprc, lpDesc );
    CreateCallStack( hDC, 0 );
    return hDC;

}
HENHMETAFILE WINAPI MyGetEnhMetaFileA(  LPCSTR lpName )
{
    HENHMETAFILE hMetaFile = pOrgGetEnhMetaFileA( lpName );
    CreateCallStack( hMetaFile, 0 );
    return hMetaFile;
}

HENHMETAFILE WINAPI MyGetEnhMetaFileW(  LPCWSTR lpName )
{
    HENHMETAFILE hMetaFile = pOrgGetEnhMetaFileW( lpName );
    CreateCallStack( hMetaFile, 0 );
    return hMetaFile;

}
HMETAFILE WINAPI MyGetMetaFileA(  LPCSTR lpName)
{
    HMETAFILE hMetaFile = pOrgGetMetaFileA( lpName );
    CreateCallStack( hMetaFile, 0 );
    return hMetaFile;
}

HMETAFILE WINAPI MyGetMetaFileW( LPCWSTR lpName )
{
    HMETAFILE hMetaFile = pOrgGetMetaFileW( lpName );
    CreateCallStack( hMetaFile, 0 );
    return hMetaFile ;

}
BOOL WINAPI MyDeleteMetaFile( HMETAFILE hmf )
{
    BOOL bRet = pOrgDeleteMetaFile( hmf );
    if( bRet )
    RemovCallStack( hmf );
    return bRet;
}

BOOL WINAPI MyDeleteEnhMetaFile( HENHMETAFILE hmf )
{
    BOOL bRet = pOrgDeleteEnhMetaFile( hmf );
    if( bRet )
    {
        RemovCallStack( hmf );
    }	
    return bRet;

}
HENHMETAFILE WINAPI MyCopyEnhMetaFileA( HENHMETAFILE hEnh, LPCSTR lpFileName)
{
    HENHMETAFILE hMetaFile = pOrgCopyEnhMetaFileA( hEnh, lpFileName );
    CreateCallStack( hMetaFile , 0 );
    return hMetaFile;
}

HENHMETAFILE WINAPI MyCopyEnhMetaFileW( HENHMETAFILE hEnh, LPCWSTR lpFileName)
{
    HENHMETAFILE hMetaFile = pOrgCopyEnhMetaFileW( hEnh, lpFileName );
    CreateCallStack( hMetaFile, 0 );
    return hMetaFile;
}

HENHMETAFILE WINAPI MyCloseEnhMetaFile( HDC hdc)
{
    HENHMETAFILE hMetaFile = pOrgCloseEnhMetaFile( hdc );
    if( hMetaFile )
    {
        RemovCallStack( hdc );
        CreateCallStack( hMetaFile, 0 );
    }
    return hMetaFile;
}

HMETAFILE WINAPI MyCloseMetaFile( HDC hdc)
{
    HMETAFILE hMetaFile = pOrgCloseMetaFile( hdc );
    if( hMetaFile )
    {
        RemovCallStack( hdc );
        CreateCallStack( hMetaFile, 0 );
    }	
    return hMetaFile;
}


// Pen
HPEN WINAPI MyCreatePen(  int iStyle,  int cWidth,  COLORREF color)
{
    HPEN hGDIObj = pOrgCreatePen( iStyle, cWidth, color );
    CreateCallStack( hGDIObj, 0 );
    return hGDIObj;
}

HPEN WINAPI MyCreatePenIndirect(  CONST LOGPEN *plpen)
{

    HPEN hGDIObj = pOrgCreatePenIndirect( plpen );
    CreateCallStack( hGDIObj, 0 );
    return hGDIObj;
}

HPEN WINAPI MyExtCreatePen( DWORD iPenStyle, DWORD cWidth, CONST LOGBRUSH *plbrush, DWORD cStyle, CONST DWORD *pstyle)
{
    HPEN hGDIObj = pOrgExtCreatePen( iPenStyle, cWidth, plbrush, cStyle, pstyle );
    CreateCallStack( hGDIObj, 0 );
    return hGDIObj;
}


// region 
HRGN WINAPI MyPathToRegion( HDC hdc)
{
    HRGN hGDIObj = pOrgPathToRegion(hdc);
    CreateCallStack( hGDIObj, 0 );
    return hGDIObj;
}

HRGN WINAPI MyCreateEllipticRgn(  int x1,  int y1,  int x2, int y2)
{
    HRGN hGDIObj = pOrgCreateEllipticRgn( x1, y1, x2, y2 );
    CreateCallStack( hGDIObj, 0 );
    return hGDIObj;
}

HRGN WINAPI MyCreateEllipticRgnIndirect(  CONST RECT *lprect)
{
    HRGN hGDIObj = pOrgCreateEllipticRgnIndirect(lprect);
    CreateCallStack( hGDIObj, 0 );
    return hGDIObj;
}

HRGN WINAPI MyCreatePolygonRgn( CONST POINT *pptl, int cPoint, int iMode)
{
    HRGN hGDIObj = pOrgCreatePolygonRgn(pptl,cPoint,iMode);
    CreateCallStack( hGDIObj, 0 );
    return hGDIObj;
}

HRGN WINAPI MyCreatePolyPolygonRgn( CONST POINT *pptl, CONST INT  *pc, int cPoly, int iMode)
{
    HRGN hGDIObj = pOrgCreatePolyPolygonRgn(pptl,pc,cPoly,iMode);
    CreateCallStack( hGDIObj, 0 );
    return hGDIObj;
}

HRGN WINAPI MyCreateRectRgn(  int x1,  int y1,  int x2,  int y2)
{
    HRGN hGDIObj = pOrgCreateRectRgn(x1,y1,x2,y2);
    CreateCallStack(hGDIObj, 0 );
    return hGDIObj;
}

HRGN WINAPI MyCreateRectRgnIndirect(  CONST RECT *lprect)
{
    HRGN hGDIObj = pOrgCreateRectRgnIndirect(lprect);
    CreateCallStack( hGDIObj, 0 );
    return hGDIObj;
}

HRGN WINAPI MyCreateRoundRectRgn(  int x1,  int y1,  int x2,  int y2,  int w,  int h)
{
    HRGN hGDIObj = pOrgCreateRoundRectRgn(x1,y1,x2,y2,w,h);
    CreateCallStack( hGDIObj, 0 );
    return hGDIObj;
}

HRGN WINAPI MyExtCreateRegion( CONST XFORM * lpx,  DWORD nCount, CONST RGNDATA * lpData)
{
    HRGN hGDIObj = pOrgExtCreateRegion(lpx,nCount,lpData);
    CreateCallStack( hGDIObj, 0 );
    return hGDIObj;
}

 
//palette 
HPALETTE WINAPI MyCreateHalftonePalette(  HDC hdc)
{
    HPALETTE hGDIObj = pOrgCreateHalftonePalette(hdc);
    CreateCallStack( hGDIObj, 0 );
    return hGDIObj;
}

HPALETTE WINAPI MyCreatePalette( CONST LOGPALETTE * plpal )
{
    HPALETTE hGDIObj = pOrgCreatePalette(plpal);
    CreateCallStack( hGDIObj, 0 );
    return hGDIObj;
}


const int GDI_FUNC_NO = 84;
void HookGDIAlloc()
{
    HMODULE Gdi32 = LoadLibrary( "Gdi32.dll" );
    HMODULE User32 = LoadLibrary( "User32.dll" );
    HMODULE Shell32 = LoadLibrary( "Shell32.dll" );
    
    g_arrHookedFunctions.SetSize( GDI_FUNC_NO ); 
    int nIndex = 0;
    // Bitmap functions
    REGISTER_HOOK(LoadBitmapA,User32 );
    REGISTER_HOOK(LoadBitmapW,User32 );
    REGISTER_HOOK(LoadImageA,User32 );
    REGISTER_HOOK(LoadImageW,User32 );
    REGISTER_HOOK(CreateBitmap,Gdi32 );
    REGISTER_HOOK(CreateBitmapIndirect,Gdi32 );
    REGISTER_HOOK(CreateCompatibleBitmap,Gdi32 );
    REGISTER_HOOK(CreateDIBitmap,Gdi32 );
    REGISTER_HOOK(CreateDIBSection,Gdi32 );
    REGISTER_HOOK(CreateDiscardableBitmap,Gdi32 );
    REGISTER_HOOK(CopyImage,User32 );
    REGISTER_HOOK(GetIconInfo,User32 );
    REGISTER_HOOK(GetIconInfoExA,User32 );
    REGISTER_HOOK(GetIconInfoExA,User32 );
    REGISTER_HOOK(DeleteObject,Gdi32 );
    
    //ICONS
    REGISTER_HOOK(CopyIcon,User32 );
    REGISTER_HOOK(CreateIcon,User32 );
    REGISTER_HOOK(CreateIconFromResource,User32 );
    REGISTER_HOOK(CreateIconFromResourceEx,User32 );
    REGISTER_HOOK(CreateIconIndirect,User32 );
    REGISTER_HOOK(DestroyIcon,User32 );
    REGISTER_HOOK(DuplicateIcon,Shell32 );
    REGISTER_HOOK(ExtractAssociatedIconA,Shell32 );
    REGISTER_HOOK(ExtractAssociatedIconW,Shell32 );
    REGISTER_HOOK(ExtractAssociatedIconExA,Shell32 );
    REGISTER_HOOK(ExtractAssociatedIconExW,Shell32 );
    REGISTER_HOOK(ExtractIconA,Shell32 );
    REGISTER_HOOK(ExtractIconW,Shell32 );
    REGISTER_HOOK(ExtractIconExA,Shell32 );
    REGISTER_HOOK(ExtractIconExW,Shell32 );
    REGISTER_HOOK(LoadIconA,User32 );
    REGISTER_HOOK(LoadIconW,User32 );
    REGISTER_HOOK(PrivateExtractIconsA,User32 );
    REGISTER_HOOK(PrivateExtractIconsW,User32 );

    // Cursor
    REGISTER_HOOK(CreateCursor,User32 );
    REGISTER_HOOK(LoadCursorA,User32 );
    REGISTER_HOOK(LoadCursorW,User32 );
    REGISTER_HOOK(LoadCursorFromFileA,User32 );
    REGISTER_HOOK(LoadCursorFromFileW,User32 );
    REGISTER_HOOK(DestroyCursor,User32 );


    // brush
    REGISTER_HOOK(CreateBrushIndirect,Gdi32 );
    REGISTER_HOOK(CreateSolidBrush,Gdi32 );
    REGISTER_HOOK(CreatePatternBrush,Gdi32 );
    REGISTER_HOOK(CreateDIBPatternBrush,Gdi32 );
    REGISTER_HOOK(CreateDIBPatternBrushPt,Gdi32 );
    REGISTER_HOOK(CreateHatchBrush,Gdi32 );

    // DC
    REGISTER_HOOK(CreateCompatibleDC,Gdi32 );
    REGISTER_HOOK(CreateDCA,Gdi32 );
    REGISTER_HOOK(CreateDCW,Gdi32 );
    REGISTER_HOOK(CreateICA,Gdi32 );
    REGISTER_HOOK(CreateICW,Gdi32 );
    REGISTER_HOOK(GetDC,User32 );
    REGISTER_HOOK(GetDCEx,User32 );
    REGISTER_HOOK(GetWindowDC,User32 );
    REGISTER_HOOK(ReleaseDC,User32 );
    REGISTER_HOOK(DeleteDC,Gdi32 );
    

    // FONT
    REGISTER_HOOK(CreateFontA,Gdi32 );
    REGISTER_HOOK(CreateFontW,Gdi32 );
    REGISTER_HOOK(CreateFontIndirectA,Gdi32 );
    REGISTER_HOOK(CreateFontIndirectW,Gdi32 );

    // Metafile
    REGISTER_HOOK(CreateMetaFileA,Gdi32 );
    REGISTER_HOOK(CreateMetaFileW,Gdi32 );
    REGISTER_HOOK(CreateEnhMetaFileA,Gdi32 );
    REGISTER_HOOK(CreateEnhMetaFileW,Gdi32 );
    REGISTER_HOOK(GetEnhMetaFileA,Gdi32 );
    REGISTER_HOOK(GetEnhMetaFileW,Gdi32 );
    REGISTER_HOOK(GetMetaFileA,Gdi32 );
    REGISTER_HOOK(GetMetaFileW,Gdi32 );
    REGISTER_HOOK(DeleteMetaFile,Gdi32 );
    REGISTER_HOOK(DeleteEnhMetaFile,Gdi32 );
    REGISTER_HOOK(CopyEnhMetaFileA,Gdi32 );
    REGISTER_HOOK(CopyEnhMetaFileW,Gdi32 );
    REGISTER_HOOK(CloseEnhMetaFile,Gdi32 );
    REGISTER_HOOK(CloseMetaFile,Gdi32 );

    //Pen
    REGISTER_HOOK(CreatePen,Gdi32 );
    REGISTER_HOOK(CreatePenIndirect,Gdi32 );
    REGISTER_HOOK(ExtCreatePen,Gdi32 );

    //region
    REGISTER_HOOK(PathToRegion,Gdi32 );
    REGISTER_HOOK(CreateEllipticRgn,Gdi32 );
    REGISTER_HOOK(CreateEllipticRgnIndirect,Gdi32 );
    REGISTER_HOOK(CreatePolygonRgn,Gdi32 );
    REGISTER_HOOK(CreatePolyPolygonRgn,Gdi32 );
    REGISTER_HOOK(CreateRectRgn,Gdi32 );
    REGISTER_HOOK(CreateRectRgnIndirect,Gdi32 );
    REGISTER_HOOK(CreateRoundRectRgn,Gdi32 );
    REGISTER_HOOK(ExtCreateRegion,Gdi32 );

    //palette
    REGISTER_HOOK(CreateHalftonePalette,Gdi32 );
    REGISTER_HOOK(CreatePalette,Gdi32 );

    

    ASSERT( nIndex == GDI_FUNC_NO );
    HookDynamicLoadedFun( GDI_FUNC_NO, g_arrHookedFunctions.GetData());

}

//////////////////////////////////////////////////////////////////////////
// Handle functions
//////////////////////////////////////////////////////////////////////////
HANDLE WINAPI MyCreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes,BOOL bManualReset,BOOL bInitialState,LPCSTR lpName)
{
    HANDLE hHandle = pOrgCreateEventA(lpEventAttributes, bManualReset, bInitialState, lpName);
    CreateCallStack( hHandle, TYPE_EVENT_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateEventW( LPSECURITY_ATTRIBUTES lpEventAttributes,BOOL bManualReset,BOOL bInitialState,LPCWSTR lpName)
{
    HANDLE hHandle = pOrgCreateEventW( lpEventAttributes, bManualReset, bInitialState, lpName);
    CreateCallStack( hHandle, TYPE_EVENT_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateEventExA( LPSECURITY_ATTRIBUTES lpEventAttributes, LPCSTR lpName, DWORD dwFlags,DWORD dwDesiredAccess)
{
    HANDLE hHandle = pOrgCreateEventExA( lpEventAttributes,  lpName,  dwFlags, dwDesiredAccess);
    CreateCallStack( hHandle, TYPE_EVENT_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateEventExW( LPSECURITY_ATTRIBUTES lpEventAttributes, LPCWSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess )
{
    HANDLE hHandle = pOrgCreateEventExW( lpEventAttributes,  lpName,  dwFlags,  dwDesiredAccess );
    CreateCallStack( hHandle, TYPE_EVENT_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyOpenEventA( DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName)
{
    HANDLE hHandle = pOrgOpenEventA( dwDesiredAccess,  bInheritHandle,  lpName);
    CreateCallStack( hHandle, TYPE_EVENT_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyOpenEventW( DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpName )
{
    HANDLE hHandle = pOrgOpenEventW( dwDesiredAccess,  bInheritHandle,  lpName );
    CreateCallStack( hHandle, TYPE_EVENT_HANDLE );
    return hHandle;
}

HANDLE WINAPI MyCreateMutexA(LPSECURITY_ATTRIBUTES lpMutexAttributes,BOOL bInitialOwner, LPCSTR lpName )
{
    HANDLE hHandle = pOrgCreateMutexA(lpMutexAttributes, bInitialOwner,  lpName );
    CreateCallStack( hHandle, TYPE_MUTEX_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateMutexW(LPSECURITY_ATTRIBUTES lpMutexAttributes,BOOL bInitialOwner,LPCWSTR lpName)
{
    HANDLE hHandle = pOrgCreateMutexW(lpMutexAttributes, bInitialOwner, lpName);
    CreateCallStack( hHandle, TYPE_MUTEX_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateMutexExA(LPSECURITY_ATTRIBUTES lpEventAttributes,LPCSTR lpName,DWORD dwFlags,DWORD dwDesiredAccess)
{
    HANDLE hHandle = pOrgCreateMutexExA(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
    CreateCallStack( hHandle, TYPE_MUTEX_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateMutexExW(LPSECURITY_ATTRIBUTES lpEventAttributes,LPCWSTR lpName,DWORD dwFlags,DWORD dwDesiredAccess)
{
    HANDLE hHandle = pOrgCreateMutexExW(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
    CreateCallStack( hHandle, TYPE_MUTEX_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyOpenMutexA(DWORD dwDesiredAccess,BOOL bInheritHandle,LPCSTR lpName)
{
    HANDLE hHandle = pOrgOpenMutexA(dwDesiredAccess, bInheritHandle, lpName);
    CreateCallStack( hHandle, TYPE_MUTEX_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyOpenMutexW(DWORD dwDesiredAccess,BOOL bInheritHandle,LPCWSTR lpName)
{
    HANDLE hHandle = pOrgOpenMutexW(dwDesiredAccess, bInheritHandle, lpName);
    CreateCallStack( hHandle, TYPE_MUTEX_HANDLE);
    return hHandle;
}

HANDLE WINAPI MyCreateSemaphoreA( LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount,LPCSTR lpName )
{
    HANDLE hHandle = pOrgCreateSemaphoreA( lpSemaphoreAttributes,  lInitialCount,  lMaximumCount, lpName );
    CreateCallStack( hHandle, TYPE_SEMAPHOR_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateSemaphoreW(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,LONG lInitialCount,LONG lMaximumCount,LPCWSTR lpName)
{
    HANDLE hHandle = pOrgCreateSemaphoreW(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName);
    CreateCallStack( hHandle, TYPE_SEMAPHOR_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateSemaphoreExA(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,LONG lInitialCount,LONG lMaximumCount,LPCSTR lpName,DWORD dwFlags,DWORD dwDesiredAccess)
{
    HANDLE hHandle = pOrgCreateSemaphoreExA(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, dwFlags, dwDesiredAccess);
    CreateCallStack( hHandle, TYPE_SEMAPHOR_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateSemaphoreExW(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,LONG lInitialCount,LONG lMaximumCount,LPCWSTR lpName,DWORD dwFlags,DWORD dwDesiredAccess)
{
    HANDLE hHandle = pOrgCreateSemaphoreExW(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, dwFlags, dwDesiredAccess);
    CreateCallStack( hHandle, TYPE_SEMAPHOR_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyOpenSemaphoreA(DWORD dwDesiredAccess,BOOL bInheritHandle,LPCSTR lpName)
{
    HANDLE hHandle = pOrgOpenSemaphoreA(dwDesiredAccess, bInheritHandle, lpName);
    CreateCallStack( hHandle, TYPE_SEMAPHOR_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyOpenSemaphoreW(DWORD dwDesiredAccess,BOOL bInheritHandle,LPWSTR lpName)
{
    HANDLE hHandle = pOrgOpenSemaphoreW(dwDesiredAccess, bInheritHandle, lpName);
    CreateCallStack( hHandle, TYPE_SEMAPHOR_HANDLE );
    return hHandle;
}

HANDLE WINAPI MyCreateWaitableTimerA( LPSECURITY_ATTRIBUTES lpTimerAttributes, BOOL bManualReset, LPCSTR lpTimerName)
{
    HANDLE hHandle = pOrgCreateWaitableTimerA( lpTimerAttributes,  bManualReset,  lpTimerName);
    CreateCallStack( hHandle, TYPE_WAIT_TIMER_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateWaitableTimerW(LPSECURITY_ATTRIBUTES lpTimerAttributes,BOOL bManualReset,LPCWSTR lpTimerName)
{
    HANDLE hHandle = pOrgCreateWaitableTimerW(lpTimerAttributes, bManualReset, lpTimerName);
    CreateCallStack( hHandle, TYPE_WAIT_TIMER_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateWaitableTimerExA(LPSECURITY_ATTRIBUTES lpTimerAttributes,LPCSTR lpTimerName,DWORD dwFlags,DWORD dwDesiredAccess)
{
    HANDLE hHandle = pOrgCreateWaitableTimerExA(lpTimerAttributes, lpTimerName, dwFlags, dwDesiredAccess);
    CreateCallStack( hHandle, TYPE_WAIT_TIMER_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateWaitableTimerExW(LPSECURITY_ATTRIBUTES lpTimerAttributes,LPCWSTR lpTimerName,DWORD dwFlags,DWORD dwDesiredAccess)
{
    HANDLE hHandle = pOrgCreateWaitableTimerExW(lpTimerAttributes, lpTimerName, dwFlags, dwDesiredAccess); 
    CreateCallStack( hHandle, TYPE_WAIT_TIMER_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyOpenWaitableTimerA(DWORD dwDesiredAccess,BOOL bInheritHandle,LPCSTR lpTimerName)
{
    HANDLE hHandle = pOrgOpenWaitableTimerA(dwDesiredAccess, bInheritHandle, lpTimerName);
    CreateCallStack( hHandle, TYPE_WAIT_TIMER_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyOpenWaitableTimerW(DWORD dwDesiredAccess,BOOL bInheritHandle,LPCWSTR lpTimerName)
{
    HANDLE hHandle = pOrgOpenWaitableTimerW(dwDesiredAccess, bInheritHandle, lpTimerName);
    CreateCallStack( hHandle, TYPE_WAIT_TIMER_HANDLE );
    return hHandle;
}

// file function
HANDLE WINAPI MyCreateFileA(LPCSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile)
{
    HANDLE hHandle = pOrgCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateFileW(LPCWSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile)
{
    HANDLE hHandle = pOrgCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile); 
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateFileTransactedA(LPCSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile,HANDLE hTransaction,PUSHORT pusMiniVersion,PVOID  lpExtendedParameter)
{
    HANDLE hHandle = pOrgCreateFileTransactedA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile, hTransaction, pusMiniVersion,  lpExtendedParameter);
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateFileTransactedW(LPCWSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile,HANDLE hTransaction,PUSHORT pusMiniVersion,PVOID  lpExtendedParameter )
{
    HANDLE hHandle = pOrgCreateFileTransactedW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile, hTransaction, pusMiniVersion,  lpExtendedParameter );
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyFindFirstFileA(LPCSTR lpFileName,LPWIN32_FIND_DATAA lpFindFileData)
{
    HANDLE hHandle = pOrgFindFirstFileA(lpFileName, lpFindFileData);
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyFindFirstFileW(LPCWSTR lpFileName,LPWIN32_FIND_DATAW lpFindFileData)
{
    HANDLE hHandle = pOrgFindFirstFileW(lpFileName, lpFindFileData); 
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyFindFirstFileExA(LPCSTR lpFileName,FINDEX_INFO_LEVELS fInfoLevelId,LPVOID lpFindFileData,FINDEX_SEARCH_OPS fSearchOp,LPVOID lpSearchFilter,DWORD dwAdditionalFlags)
{
    HANDLE hHandle = pOrgFindFirstFileExA(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyFindFirstFileExW(LPCWSTR lpFileName,FINDEX_INFO_LEVELS fInfoLevelId,LPVOID lpFindFileData,FINDEX_SEARCH_OPS fSearchOp,LPVOID lpSearchFilter,DWORD dwAdditionalFlags)
{
    HANDLE hHandle = pOrgFindFirstFileExW(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags); 
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyFindFirstFileNameTransactedW (LPCWSTR lpFileName,DWORD dwFlags,LPDWORD StringLength,PWCHAR LinkName,HANDLE hTransaction)
{
    HANDLE hHandle = pOrgFindFirstFileNameTransactedW (lpFileName, dwFlags, StringLength, LinkName, hTransaction);
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyFindFirstFileNameW (LPCWSTR lpFileName,DWORD dwFlags,LPDWORD StringLength,PWCHAR LinkName)
{
    HANDLE hHandle = pOrgFindFirstFileNameW (lpFileName, dwFlags, StringLength, LinkName); 
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyFindFirstFileTransactedA(LPCSTR lpFileName,FINDEX_INFO_LEVELS fInfoLevelId,LPVOID lpFindFileData,FINDEX_SEARCH_OPS fSearchOp,LPVOID lpSearchFilter,DWORD dwAdditionalFlags,HANDLE hTransaction)
{
    HANDLE hHandle = pOrgFindFirstFileTransactedA(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags, hTransaction);
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyFindFirstFileTransactedW(LPCWSTR lpFileName,FINDEX_INFO_LEVELS fInfoLevelId,LPVOID lpFindFileData,FINDEX_SEARCH_OPS fSearchOp,LPVOID lpSearchFilter,DWORD dwAdditionalFlags,HANDLE hTransaction)
{
    HANDLE hHandle = pOrgFindFirstFileTransactedW(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags, hTransaction);
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyFindFirstStreamTransactedW(LPCWSTR lpFileName,STREAM_INFO_LEVELS InfoLevel,LPVOID lpFindStreamData,DWORD dwFlags,HANDLE hTransaction)
{
    HANDLE hHandle = pOrgFindFirstStreamTransactedW(lpFileName, InfoLevel, lpFindStreamData, dwFlags, hTransaction);
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyFindFirstStreamW( LPCWSTR lpFileName,STREAM_INFO_LEVELS InfoLevel,LPVOID lpFindStreamData,DWORD dwFlags)
{
    HANDLE hHandle = pOrgFindFirstStreamW( lpFileName, InfoLevel, lpFindStreamData, dwFlags);
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
BOOL WINAPI MyFindClose( HANDLE hFindFile)
{
    BOOL bRet  = pOrgFindClose( hFindFile);
    if( bRet )
        RemovCallStack( hFindFile );
    return bRet;
}
HANDLE WINAPI MyOpenFileById(HANDLE hFile,LPFILE_ID_DESCRIPTOR lpFileID,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwFlags)
{
    HANDLE hHandle = pOrgOpenFileById(hFile, lpFileID, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwFlags);
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyReOpenFile(HANDLE hOriginalFile,DWORD dwDesiredAccess,DWORD dwShareMode,DWORD dwFlags)
{
    HANDLE hHandle = pOrgReOpenFile(hOriginalFile, dwDesiredAccess, dwShareMode, dwFlags);
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateIoCompletionPort(HANDLE FileHandle,HANDLE ExistingCompletionPort,ULONG_PTR CompletionKey,DWORD NumberOfConcurrentThreads)
{
    HANDLE hHandle = pOrgCreateIoCompletionPort(FileHandle, ExistingCompletionPort, CompletionKey, NumberOfConcurrentThreads);
    CreateCallStack( hHandle, TYPE_FILE_HANDLE );
    return hHandle;
}

//Authorization function
BOOL   WINAPI MyCreateRestrictedToken(HANDLE ExistingTokenHandle,DWORD Flags,DWORD DisableSidCount,PSID_AND_ATTRIBUTES SidsToDisable,DWORD DeletePrivilegeCount,PLUID_AND_ATTRIBUTES PrivilegesToDelete,DWORD RestrictedSidCount,PSID_AND_ATTRIBUTES SidsToRestrict,PHANDLE NewTokenHandle)
{
    BOOL   bret = pOrgCreateRestrictedToken(ExistingTokenHandle, Flags, DisableSidCount, SidsToDisable, DeletePrivilegeCount, PrivilegesToDelete, RestrictedSidCount, SidsToRestrict, NewTokenHandle);
    if( bret )
        CreateCallStack( *NewTokenHandle, TYPE_TOKEN_HANDLE );
    return bret;
}
BOOL   WINAPI MyDuplicateToken(HANDLE ExistingTokenHandle,SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,PHANDLE DuplicateTokenHandle)
{
    BOOL   bret = pOrgDuplicateToken(ExistingTokenHandle, ImpersonationLevel, DuplicateTokenHandle);
    if( bret )
        CreateCallStack( *DuplicateTokenHandle, TYPE_TOKEN_HANDLE );
    return bret;
}
BOOL   WINAPI MyDuplicateTokenEx(HANDLE hExistingToken,DWORD dwDesiredAccess,LPSECURITY_ATTRIBUTES lpTokenAttributes,SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,TOKEN_TYPE TokenType,PHANDLE phNewToken)
{
    BOOL   bret = pOrgDuplicateTokenEx(hExistingToken, dwDesiredAccess, lpTokenAttributes, ImpersonationLevel, TokenType, phNewToken);
    if( bret )
        CreateCallStack( *phNewToken, TYPE_TOKEN_HANDLE );
    return bret;
}
BOOL   WINAPI MyOpenProcessToken(HANDLE ProcessHandle,DWORD DesiredAccess,PHANDLE TokenHandle)
{
    BOOL   bret = pOrgOpenProcessToken(ProcessHandle, DesiredAccess, TokenHandle);
    if( bret )
        CreateCallStack( *TokenHandle, TYPE_TOKEN_HANDLE );
    return bret;
}
BOOL   WINAPI MyOpenThreadToken(HANDLE ThreadHandle,DWORD DesiredAccess,BOOL OpenAsSelf,PHANDLE TokenHandle)
{
    BOOL   bret = pOrgOpenThreadToken(ThreadHandle, DesiredAccess, OpenAsSelf, TokenHandle); 
    if( bret )
        CreateCallStack( *TokenHandle, TYPE_TOKEN_HANDLE );
    return bret;
}

//Directory management
HANDLE WINAPI MyFindFirstChangeNotificationA(LPCSTR lpPathName,BOOL bWatchSubtree,DWORD dwNotifyFilter)
{
    HANDLE hHandle = pOrgFindFirstChangeNotificationA(lpPathName, bWatchSubtree, dwNotifyFilter);
    CreateCallStack( hHandle, TYPE_CHANGE_NOFICATION_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyFindFirstChangeNotificationW(LPCWSTR lpPathName,BOOL bWatchSubtree,DWORD dwNotifyFilter)
{
    HANDLE hHandle = pOrgFindFirstChangeNotificationW(lpPathName, bWatchSubtree, dwNotifyFilter); 
    CreateCallStack( hHandle, TYPE_CHANGE_NOFICATION_HANDLE );
    return hHandle;
}
BOOL   WINAPI MyFindCloseChangeNotification(HANDLE hChangeHandle)
{
    BOOL   bRet = pOrgFindCloseChangeNotification(hChangeHandle); 
    if( bRet )
        RemovCallStack( hChangeHandle );
    return bRet;

}

// File mapping
HANDLE WINAPI MyCreateMemoryResourceNotification( MEMORY_RESOURCE_NOTIFICATION_TYPE NotificationType )
{
    HANDLE hHandle = pOrgCreateMemoryResourceNotification( NotificationType );
    CreateCallStack( hHandle, TYPE_MEMEORY_MAPPED_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateFileMappingA( HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName )
{
    HANDLE hHandle = pOrgCreateFileMappingA( hFile,  lpFileMappingAttributes,  flProtect,  dwMaximumSizeHigh,  dwMaximumSizeLow,  lpName );
    CreateCallStack( hHandle, TYPE_MEMEORY_MAPPED_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateFileMappingW( HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCWSTR lpName )
{
    HANDLE hHandle = pOrgCreateFileMappingW( hFile, lpFileMappingAttributes, flProtect,  dwMaximumSizeHigh,  dwMaximumSizeLow,  lpName );
    CreateCallStack( hHandle, TYPE_MEMEORY_MAPPED_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateFileMappingNumaA( HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName, DWORD nndPreferred )
{
    HANDLE hHandle = pOrgCreateFileMappingNumaA( hFile,  lpFileMappingAttributes,  flProtect,  dwMaximumSizeHigh,  dwMaximumSizeLow,  lpName,  nndPreferred );
    CreateCallStack( hHandle, TYPE_MEMEORY_MAPPED_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateFileMappingNumaW( HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCWSTR lpName, DWORD nndPreferred )
{
    HANDLE hHandle = pOrgCreateFileMappingNumaW( hFile,  lpFileMappingAttributes,  flProtect,  dwMaximumSizeHigh,  dwMaximumSizeLow,  lpName,  nndPreferred );
    CreateCallStack( hHandle, TYPE_MEMEORY_MAPPED_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyOpenFileMappingA( DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName )
{
    HANDLE hHandle = pOrgOpenFileMappingA( dwDesiredAccess,  bInheritHandle,  lpName );
    CreateCallStack( hHandle, TYPE_MEMEORY_MAPPED_FILE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyOpenFileMappingW( DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpName )
{
    HANDLE hHandle = pOrgOpenFileMappingW( dwDesiredAccess,  bInheritHandle,  lpName );
    CreateCallStack( hHandle, TYPE_MEMEORY_MAPPED_FILE_HANDLE );
    return hHandle;
}

//Memory
HANDLE WINAPI MyHeapCreate( DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize )
{
    HANDLE hHandle = pOrgHeapCreate( flOptions,  dwInitialSize,  dwMaximumSize );
    CreateCallStack( hHandle, TYPE_MEMORY_HANDLE );
    return hHandle;
}
BOOL   WINAPI MyHeapDestroy(HANDLE hHeap )
{
    BOOL   bRet = pOrgHeapDestroy(hHeap );
    if( bRet )
        RemovCallStack( hHeap );
    return bRet;

}

//Process and thread
BOOL   WINAPI MyCreateProcessA( LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation )
{
    BOOL   bret = pOrgCreateProcessA( lpApplicationName,  lpCommandLine,  lpProcessAttributes,  lpThreadAttributes,  bInheritHandles,  dwCreationFlags,  lpEnvironment,  lpCurrentDirectory,  lpStartupInfo,  lpProcessInformation );
    if( bret && lpProcessInformation )
    {
        CreateCallStack( lpProcessInformation->hProcess, TYPE_PROCESS_HANDLE );
        CopyStack( lpProcessInformation->hProcess, lpProcessInformation->hThread, TYPE_THREAD_HANDLE );
    }
    return bret;
}
BOOL   WINAPI MyCreateProcessW( LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation )
{
    BOOL   bret = pOrgCreateProcessW( lpApplicationName,  lpCommandLine,  lpProcessAttributes,  lpThreadAttributes,  bInheritHandles,  dwCreationFlags,  lpEnvironment,  lpCurrentDirectory,  lpStartupInfo,  lpProcessInformation );
    if( bret && lpProcessInformation )
    {
        CreateCallStack( lpProcessInformation->hProcess, TYPE_PROCESS_HANDLE );
        CopyStack( lpProcessInformation->hProcess, lpProcessInformation->hThread, TYPE_THREAD_HANDLE );
    }
    return bret;
}
BOOL   WINAPI MyCreateProcessAsUserA(HANDLE hToken,LPCSTR lpApplicationName,LPSTR lpCommandLine,LPSECURITY_ATTRIBUTES lpProcessAttributes,LPSECURITY_ATTRIBUTES lpThreadAttributes,BOOL bInheritHandles,DWORD dwCreationFlags,LPVOID lpEnvironment,LPCSTR lpCurrentDirectory,LPSTARTUPINFOA lpStartupInfo,LPPROCESS_INFORMATION lpProcessInformation)
{
    BOOL   bret = pOrgCreateProcessAsUserA(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    if( bret && lpProcessInformation )
    {
        CreateCallStack( lpProcessInformation->hProcess, TYPE_PROCESS_HANDLE );
        CopyStack( lpProcessInformation->hProcess, lpProcessInformation->hThread, TYPE_THREAD_HANDLE );
    }
    return bret;
}
BOOL   WINAPI MyCreateProcessAsUserW(HANDLE hToken,LPWSTR lpApplicationName,LPWSTR lpCommandLine,LPSECURITY_ATTRIBUTES lpProcessAttributes,LPSECURITY_ATTRIBUTES lpThreadAttributes,BOOL bInheritHandles,DWORD dwCreationFlags,LPVOID lpEnvironment,LPWSTR lpCurrentDirectory,LPSTARTUPINFOW lpStartupInfo,LPPROCESS_INFORMATION lpProcessInformation)
{
    BOOL   bret = pOrgCreateProcessAsUserW(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    if( bret && lpProcessInformation )
    {
        CreateCallStack( lpProcessInformation->hProcess, TYPE_PROCESS_HANDLE );
        CopyStack( lpProcessInformation->hProcess, lpProcessInformation->hThread, TYPE_THREAD_HANDLE );
    }
    return bret;
}
BOOL   WINAPI MyCreateProcessWithLogonW(LPCWSTR lpUsername,LPCWSTR lpDomain,LPCWSTR lpPassword,DWORD dwLogonFlags,LPCWSTR lpApplicationName,LPWSTR lpCommandLine,DWORD dwCreationFlags,LPVOID lpEnvironment,LPCWSTR lpCurrentDirectory,LPSTARTUPINFOW lpStartupInfo,LPPROCESS_INFORMATION lpProcessInformation)
{
    BOOL   bret = pOrgCreateProcessWithLogonW(lpUsername, lpDomain, lpPassword, dwLogonFlags, lpApplicationName, lpCommandLine, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    if( bret && lpProcessInformation )
    {
        CreateCallStack( lpProcessInformation->hProcess, TYPE_PROCESS_HANDLE );
        CopyStack( lpProcessInformation->hProcess, lpProcessInformation->hThread, TYPE_THREAD_HANDLE );
    }
    return bret;
}
BOOL   WINAPI MyCreateProcessWithTokenW(HANDLE hToken,DWORD dwLogonFlags,LPCWSTR lpApplicationName,LPWSTR lpCommandLine,DWORD dwCreationFlags,LPVOID lpEnvironment,LPCWSTR lpCurrentDirectory,LPSTARTUPINFOW lpStartupInfo,LPPROCESS_INFORMATION lpProcessInformation)
{
    BOOL   bret = pOrgCreateProcessWithTokenW(hToken, dwLogonFlags, lpApplicationName, lpCommandLine, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation); 
    if( bret && lpProcessInformation )
    {
        CreateCallStack( lpProcessInformation->hProcess, TYPE_PROCESS_HANDLE );
        CopyStack( lpProcessInformation->hProcess, lpProcessInformation->hThread, TYPE_THREAD_HANDLE );
    }
    return bret;
}
HANDLE WINAPI MyOpenProcess( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId )
{
    HANDLE hHandle = pOrgOpenProcess( dwDesiredAccess,  bInheritHandle,  dwProcessId );
    CreateCallStack( hHandle, TYPE_PROCESS_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateThread( LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId )
{
    HANDLE hHandle = pOrgCreateThread( lpThreadAttributes,  dwStackSize,  lpStartAddress,  lpParameter,  dwCreationFlags,  lpThreadId );
    CreateCallStack( hHandle, TYPE_THREAD_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateRemoteThread( HANDLE hProcess, LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId )
{
    HANDLE hHandle = pOrgCreateRemoteThread( hProcess,  lpThreadAttributes,  dwStackSize,  lpStartAddress,  lpParameter,  dwCreationFlags,  lpThreadId );
    CreateCallStack( hHandle, TYPE_THREAD_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyOpenThread( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId )
{
    HANDLE hHandle = pOrgOpenThread( dwDesiredAccess,  bInheritHandle,  dwThreadId );
    CreateCallStack( hHandle, TYPE_THREAD_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateJobObjectA( LPSECURITY_ATTRIBUTES lpJobAttributes, LPCSTR lpName )
{
    HANDLE hHandle = pOrgCreateJobObjectA( lpJobAttributes, lpName );
    CreateCallStack( hHandle, TYPE_JOB_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateJobObjectW( LPSECURITY_ATTRIBUTES lpJobAttributes, LPCWSTR lpName )
{
    HANDLE hHandle = pOrgCreateJobObjectW( lpJobAttributes, lpName );
    CreateCallStack( hHandle, TYPE_JOB_HANDLE );
    return hHandle;
}

// Mail slot
HANDLE WINAPI MyCreateMailslotA( LPCSTR lpName, DWORD nMaxMessageSize, DWORD lReadTimeout, LPSECURITY_ATTRIBUTES lpSecurityAttributes )
{
    HANDLE hHandle = pOrgCreateMailslotA( lpName,  nMaxMessageSize,  lReadTimeout,  lpSecurityAttributes );
    CreateCallStack( hHandle, TYPE_MAIL_SLOT_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateMailslotW( LPCWSTR lpName, DWORD nMaxMessageSize, DWORD lReadTimeout, LPSECURITY_ATTRIBUTES lpSecurityAttributes )
{
    HANDLE hHandle = pOrgCreateMailslotW( lpName,  nMaxMessageSize,  lReadTimeout,  lpSecurityAttributes );
    CreateCallStack( hHandle, TYPE_MAIL_SLOT_HANDLE );
    return hHandle;
}

// pipe
BOOL   WINAPI MyCreatePipe( PHANDLE hReadPipe, PHANDLE hWritePipe, LPSECURITY_ATTRIBUTES lpPipeAttributes, DWORD nSize )
{
    BOOL   bret = pOrgCreatePipe( hReadPipe,  hWritePipe,  lpPipeAttributes,  nSize ); 
    if( bret )
    {
        CreateCallStack( *hReadPipe, TYPE_PIPE_HANDLE );
        CreateCallStack( *hWritePipe, TYPE_PIPE_HANDLE );
    }
    return bret;
}
HANDLE WINAPI MyCreateNamedPipeA( LPCSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode, DWORD nMaxInstances, DWORD nOutBufferSize, DWORD nInBufferSize, DWORD nDefaultTimeOut, LPSECURITY_ATTRIBUTES lpSecurityAttributes )
{
    HANDLE hHandle = pOrgCreateNamedPipeA( lpName,  dwOpenMode,  dwPipeMode,  nMaxInstances,  nOutBufferSize,  nInBufferSize,  nDefaultTimeOut,  lpSecurityAttributes );
    CreateCallStack( hHandle, TYPE_PIPE_HANDLE );
    return hHandle;
}
HANDLE WINAPI MyCreateNamedPipeW( LPCWSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode, DWORD nMaxInstances, DWORD nOutBufferSize, DWORD nInBufferSize, DWORD nDefaultTimeOut, LPSECURITY_ATTRIBUTES lpSecurityAttributes )
{
    HANDLE hHandle = pOrgCreateNamedPipeW( lpName, dwOpenMode,  dwPipeMode,  nMaxInstances,  nOutBufferSize,  nInBufferSize,  nDefaultTimeOut,  lpSecurityAttributes );
    CreateCallStack( hHandle, TYPE_PIPE_HANDLE );
    return hHandle;
}

//Registry
LSTATUS WINAPI MyRegCreateKeyExA( HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired, CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition )
{
    LSTATUS hHandle = pOrgRegCreateKeyExA( hKey, lpSubKey,  Reserved,  lpClass,  dwOptions,  samDesired,   lpSecurityAttributes,  phkResult,  lpdwDisposition );
    if( phkResult )
        CreateCallStack( *phkResult, TYPE_REGISTRY_HANDLE );
    return hHandle;
}
LSTATUS WINAPI MyRegCreateKeyExW ( HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired, CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition )
{
    LSTATUS hHandle = pOrgRegCreateKeyExW ( hKey,  lpSubKey,  Reserved,  lpClass,  dwOptions,  samDesired,   lpSecurityAttributes,  phkResult,  lpdwDisposition );
    if( phkResult )
        CreateCallStack( *phkResult, TYPE_REGISTRY_HANDLE );
    return hHandle;
}
LSTATUS WINAPI MyRegCreateKeyTransactedA( HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired, CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition, HANDLE hTransaction, PVOID  pExtendedParemeter )
{
    LSTATUS hHandle = pOrgRegCreateKeyTransactedA( hKey,  lpSubKey,  Reserved,  lpClass,  dwOptions,  samDesired,   lpSecurityAttributes,  phkResult,  lpdwDisposition, hTransaction,   pExtendedParemeter );
    if( phkResult )
        CreateCallStack( *phkResult, TYPE_REGISTRY_HANDLE );
    return hHandle;
}
LSTATUS WINAPI MyRegCreateKeyTransactedW( HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired, CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition, HANDLE hTransaction, PVOID  pExtendedParemeter )
{
    LSTATUS hHandle = pOrgRegCreateKeyTransactedW( hKey, lpSubKey, Reserved,  lpClass,  dwOptions,  samDesired,   lpSecurityAttributes,  phkResult,  lpdwDisposition,  hTransaction,   pExtendedParemeter ); 
    if( phkResult )
        CreateCallStack( *phkResult, TYPE_REGISTRY_HANDLE );
    return hHandle;
}
LSTATUS WINAPI MyRegOpenCurrentUser( REGSAM samDesired, PHKEY phkResult )
{
    LSTATUS hHandle = pOrgRegOpenCurrentUser( samDesired,  phkResult ); 
    if( phkResult )
        CreateCallStack( *phkResult, TYPE_REGISTRY_HANDLE );
    return hHandle;
}
LSTATUS WINAPI MyRegOpenKeyA ( HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult )
{
    LSTATUS hHandle = pOrgRegOpenKeyA ( hKey,  lpSubKey,  phkResult );
    if( phkResult )
        CreateCallStack( *phkResult, TYPE_REGISTRY_HANDLE );
    return hHandle;
}
LSTATUS WINAPI MyRegOpenKeyW ( HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult )
{
    LSTATUS hHandle = pOrgRegOpenKeyW ( hKey,  lpSubKey,  phkResult );
    if( phkResult )
        CreateCallStack( *phkResult, TYPE_REGISTRY_HANDLE );
    return hHandle;
}
LSTATUS WINAPI MyRegOpenKeyExA ( HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult )
{
    LSTATUS hHandle = pOrgRegOpenKeyExA ( hKey,  lpSubKey,  ulOptions,  samDesired,  phkResult );
    if( phkResult )
        CreateCallStack( *phkResult, TYPE_REGISTRY_HANDLE );
    return hHandle;
}
LSTATUS WINAPI MyRegOpenKeyExW ( HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult )
{
    LSTATUS hHandle = pOrgRegOpenKeyExW ( hKey,  lpSubKey,  ulOptions,  samDesired,  phkResult ); 
    if( phkResult )
        CreateCallStack( *phkResult, TYPE_REGISTRY_HANDLE );
    return hHandle;
}
LSTATUS WINAPI MyRegOpenKeyTransactedA ( HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult, HANDLE hTransaction, PVOID  pExtendedParemeter )
{
    LSTATUS hHandle = pOrgRegOpenKeyTransactedA ( hKey,  lpSubKey,  ulOptions,  samDesired,  phkResult,  hTransaction,   pExtendedParemeter );
    if( phkResult )
        CreateCallStack( *phkResult, TYPE_REGISTRY_HANDLE );
    return hHandle;
}
LSTATUS WINAPI MyRegOpenKeyTransactedW ( HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult, HANDLE hTransaction, PVOID  pExtendedParemeter )
{
    LSTATUS hHandle = pOrgRegOpenKeyTransactedW ( hKey,  lpSubKey,  ulOptions,  samDesired,  phkResult,  hTransaction,   pExtendedParemeter );
    if( phkResult )
        CreateCallStack( *phkResult, TYPE_REGISTRY_HANDLE );
    return hHandle;
}
LSTATUS WINAPI MyRegOpenUserClassesRoot( HANDLE hToken, DWORD dwOptions, REGSAM samDesired, PHKEY  phkResult )
{
    LSTATUS hHandle = pOrgRegOpenUserClassesRoot( hToken,  dwOptions,  samDesired,   phkResult ); 
    if( phkResult )
        CreateCallStack( *phkResult, TYPE_REGISTRY_HANDLE );
    return hHandle;
}
LSTATUS WINAPI MyRegCreateKeyA ( HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult )
{
    LSTATUS hHandle = pOrgRegCreateKeyA ( hKey,  lpSubKey,  phkResult );
    if( phkResult )
        CreateCallStack( *phkResult, TYPE_REGISTRY_HANDLE );
    return hHandle;
}
LSTATUS WINAPI MyRegCreateKeyW ( HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult )
{
    LSTATUS hHandle = pOrgRegCreateKeyW ( hKey,  lpSubKey,  phkResult );
    if( phkResult )
        CreateCallStack( *phkResult, TYPE_REGISTRY_HANDLE );
    return hHandle;
}
LSTATUS WINAPI MyRegCloseKey ( HKEY hKey )
{
    LSTATUS bRet = pOrgRegCloseKey ( hKey ); 
    if( bRet )
        RemovCallStack( hKey );
    return bRet;
}

////////////////////////////////start - v3 additions//////////////////////////////////////////////////
// Timers
HANDLE WINAPI MyCreateTimerQueue(void)
{
    HANDLE hHandle = pOrgCreateTimerQueue();
    if( hHandle )
        CreateCallStack( hHandle, TYPE_TIMER_QUEUE );
    return hHandle;
}

BOOL   WINAPI MyCreateTimerQueueTimer(PHANDLE phNewTimer,HANDLE TimerQueue,WAITORTIMERCALLBACK Callback,PVOID Parameter,DWORD DueTime,DWORD Period,ULONG Flags)
{
    BOOL bRet = pOrgCreateTimerQueueTimer(phNewTimer,TimerQueue,Callback,Parameter,DueTime,Period,Flags);
    if( bRet && phNewTimer && *phNewTimer )
        CreateCallStack( *phNewTimer, TYPE_TIMER_QUEUE );
    return bRet;
}

BOOL   WINAPI MyDeleteTimerQueueTimer(HANDLE TimerQueue,HANDLE Timer,HANDLE CompletionEvent)
{
    BOOL bRet = pOrgDeleteTimerQueueTimer(TimerQueue,Timer,CompletionEvent);
    if( bRet )
        RemovCallStack( Timer );
    return bRet;
}

BOOL   WINAPI MyDeleteTimerQueueEx(HANDLE TimerQueue,HANDLE CompletionEvent)
{
    BOOL bRet = pOrgDeleteTimerQueueEx(TimerQueue,CompletionEvent);
    if( bRet )
        RemovCallStack( TimerQueue );
    return bRet;
}

BOOL WINAPI MyDeleteTimerQueue(HANDLE TimerQueue)
{
    BOOL bRet = pOrgDeleteTimerQueue(TimerQueue);
    if( bRet )
        RemovCallStack( TimerQueue );
    return bRet;
}

//Critical section
void WINAPI MyInitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    pOrgInitializeCriticalSection( lpCriticalSection );
    CreateCallStack( lpCriticalSection, TYPE_CRITICAL_SECTION_HANDLE );
}
BOOL WINAPI MyInitializeCriticalSectionEx(LPCRITICAL_SECTION lpCriticalSection,DWORD dwSpinCount,DWORD Flags)
{
    BOOL bRet = pOrgInitializeCriticalSectionEx(lpCriticalSection, dwSpinCount, Flags);
    if( bRet )
        CreateCallStack( lpCriticalSection, TYPE_CRITICAL_SECTION_HANDLE );
    return bRet;
}

BOOL WINAPI MyInitializeCriticalSectionAndSpinCount(LPCRITICAL_SECTION lpCriticalSection,DWORD dwSpinCount)
{
    BOOL bRet = pOrgInitializeCriticalSectionAndSpinCount(lpCriticalSection, dwSpinCount);
    if( bRet )
        CreateCallStack( lpCriticalSection, TYPE_CRITICAL_SECTION_HANDLE );
    return bRet;
}
void WINAPI MyDeleteCriticalSection( LPCRITICAL_SECTION lpCriticalSection)
{
    pOrgDeleteCriticalSection(lpCriticalSection);
    RemovCallStack( lpCriticalSection );
    
}

////////////////////////////////end - v3 additions//////////////////////////////////////////////////

BOOL   WINAPI MyDuplicateHandle(HANDLE hSourceProcessHandle,HANDLE hSourceHandle,HANDLE hTargetProcessHandle,LPHANDLE lpTargetHandle,DWORD dwDesiredAccess,BOOL bInheritHandle,DWORD dwOptions)
{
    BOOL   bret = pOrgDuplicateHandle(hSourceProcessHandle,hSourceHandle,hTargetProcessHandle, lpTargetHandle,dwDesiredAccess,bInheritHandle,dwOptions);
    
    if(DUPLICATE_CLOSE_SOURCE&dwOptions)
    {
        RemovCallStack( hSourceHandle );
    }
    
    if( bret )
        CreateCallStack( *lpTargetHandle, TYPE_UNKNOWN );
    return bret;
}

BOOL   WINAPI MyCloseHandle( HANDLE hObject )
{
    BOOL   bRet = pOrgCloseHandle( hObject );
    if( bRet )
        RemovCallStack( hObject );
    return bRet;

}


const int HANDLE_FUNC_COUNT = 108;
void HookHandleAlloc()
{
    HMODULE Kernel32 = LoadLibrary( "Kernel32.dll" );
    HMODULE Advapi32 = LoadLibrary( "Advapi32.dll" );
    g_arrHookedFunctions.SetSize( HANDLE_FUNC_COUNT ); 
    int nIndex = 0;

    REGISTER_HOOK(CreateEventA,Kernel32 );
    REGISTER_HOOK(CreateEventW,Kernel32 );
    REGISTER_HOOK(CreateEventExA,Kernel32 );
    REGISTER_HOOK(CreateEventExW,Kernel32 );
    REGISTER_HOOK(OpenEventA,Kernel32 );
    REGISTER_HOOK(OpenEventW,Kernel32 );
    REGISTER_HOOK(CreateMutexA,Kernel32 );
    REGISTER_HOOK(CreateMutexW,Kernel32 );
    REGISTER_HOOK(CreateMutexExA,Kernel32 );
    REGISTER_HOOK(CreateMutexExW,Kernel32 );
    REGISTER_HOOK(OpenMutexA,Kernel32 );
    REGISTER_HOOK(OpenMutexW,Kernel32 );
    REGISTER_HOOK(CreateSemaphoreA,Kernel32 );
    REGISTER_HOOK(CreateSemaphoreW,Kernel32 );
    REGISTER_HOOK(CreateSemaphoreExA,Kernel32 );
    REGISTER_HOOK(CreateSemaphoreExW,Kernel32 );
    REGISTER_HOOK(OpenSemaphoreA,Kernel32 );
    REGISTER_HOOK(OpenSemaphoreW,Kernel32 );
    REGISTER_HOOK(CreateWaitableTimerA,Kernel32 );
    REGISTER_HOOK(CreateWaitableTimerW,Kernel32 );
    REGISTER_HOOK(CreateWaitableTimerExA,Kernel32 );
    REGISTER_HOOK(CreateWaitableTimerExW,Kernel32 );
    REGISTER_HOOK(OpenWaitableTimerA,Kernel32 );
    REGISTER_HOOK(OpenWaitableTimerW,Kernel32 );
    REGISTER_HOOK(CreateFileA,Kernel32 );
    REGISTER_HOOK(CreateFileW,Kernel32 );
    REGISTER_HOOK(CreateFileTransactedA,Kernel32 );
    REGISTER_HOOK(CreateFileTransactedW,Kernel32 );
    REGISTER_HOOK(FindFirstFileA,Kernel32 );
    REGISTER_HOOK(FindFirstFileW,Kernel32 );
    REGISTER_HOOK(FindFirstFileExA,Kernel32 );
    REGISTER_HOOK(FindFirstFileExW,Kernel32 );
    REGISTER_HOOK(FindFirstFileExW,Kernel32 );
    REGISTER_HOOK(FindFirstFileNameW,Kernel32 );
    REGISTER_HOOK(FindFirstFileTransactedA,Kernel32 );
    REGISTER_HOOK(FindFirstFileTransactedW,Kernel32 );
    REGISTER_HOOK(FindFirstStreamTransactedW,Kernel32 );
    REGISTER_HOOK(FindFirstStreamW,Kernel32 );
    REGISTER_HOOK(FindClose,Kernel32 );
    REGISTER_HOOK(OpenFileById,Kernel32 );
    REGISTER_HOOK(ReOpenFile,Kernel32 );
    REGISTER_HOOK(CreateIoCompletionPort,Kernel32 );
    REGISTER_HOOK(CreateRestrictedToken,Advapi32 );
    REGISTER_HOOK(DuplicateToken,Advapi32 );
    REGISTER_HOOK(DuplicateTokenEx,Advapi32 );
    REGISTER_HOOK(OpenProcessToken,Advapi32 );
    REGISTER_HOOK(OpenThreadToken,Advapi32 );
    REGISTER_HOOK(FindFirstChangeNotificationA,Kernel32 );
    REGISTER_HOOK(FindFirstChangeNotificationW,Kernel32 );
    REGISTER_HOOK(FindCloseChangeNotification,Kernel32 );
    REGISTER_HOOK(CreateMemoryResourceNotification,Kernel32 );
    REGISTER_HOOK(CreateFileMappingA,Kernel32 );
    REGISTER_HOOK(CreateFileMappingW,Kernel32 );
    REGISTER_HOOK(CreateFileMappingNumaA,Kernel32 );
    REGISTER_HOOK(CreateFileMappingNumaW,Kernel32 );
    REGISTER_HOOK(OpenFileMappingA,Kernel32 );
    REGISTER_HOOK(OpenFileMappingW,Kernel32 );
    REGISTER_HOOK(HeapCreate,Kernel32 );
    REGISTER_HOOK(HeapDestroy,Kernel32 );
    REGISTER_HOOK(GlobalAlloc,Kernel32 );
    REGISTER_HOOK(GlobalReAlloc,Kernel32 );
    REGISTER_HOOK(GlobalFree,Kernel32 );
    REGISTER_HOOK(LocalAlloc,Kernel32 );
    REGISTER_HOOK(LocalReAlloc,Kernel32 );
    REGISTER_HOOK(LocalFree,Kernel32 );
    REGISTER_HOOK(CreateProcessA,Kernel32 );
    REGISTER_HOOK(CreateProcessW,Kernel32 );
    REGISTER_HOOK(CreateProcessAsUserA,Advapi32 );
    REGISTER_HOOK(CreateProcessAsUserW,Advapi32 );
    REGISTER_HOOK(CreateProcessWithLogonW,Advapi32 );
    REGISTER_HOOK(CreateProcessWithTokenW,Advapi32 );
    REGISTER_HOOK(OpenProcess,Kernel32 );
    REGISTER_HOOK(CreateThread,Kernel32 );
    REGISTER_HOOK(CreateRemoteThread,Kernel32 );
    REGISTER_HOOK(OpenThread,Kernel32 );
    REGISTER_HOOK(CreateJobObjectA,Kernel32 );
    REGISTER_HOOK(CreateJobObjectW,Kernel32 );
    REGISTER_HOOK(CreateMailslotA,Kernel32 );
    REGISTER_HOOK(CreateMailslotW,Kernel32 );
    REGISTER_HOOK(CreatePipe,Kernel32 );
    REGISTER_HOOK(CreateNamedPipeA,Kernel32 );
    REGISTER_HOOK(CreateNamedPipeW,Kernel32 );
    REGISTER_HOOK(RegCreateKeyExA,Advapi32 );
    REGISTER_HOOK(RegCreateKeyExW,Advapi32 );
    REGISTER_HOOK(RegCreateKeyTransactedA,Kernel32 );
    REGISTER_HOOK(RegCreateKeyTransactedW,Kernel32 );
    REGISTER_HOOK(RegOpenCurrentUser,Kernel32 );
    REGISTER_HOOK(RegOpenKeyA,Kernel32 );
    REGISTER_HOOK(RegOpenKeyW,Kernel32 );
    REGISTER_HOOK(RegOpenKeyExA,Kernel32 );
    REGISTER_HOOK(RegOpenKeyExW,Kernel32 );
    REGISTER_HOOK(RegOpenKeyTransactedA,Kernel32 );
    REGISTER_HOOK(RegOpenKeyTransactedW,Kernel32 );
    REGISTER_HOOK(RegOpenUserClassesRoot,Kernel32 );
    REGISTER_HOOK(RegCreateKeyA,Kernel32 );
    REGISTER_HOOK(RegCreateKeyW,Kernel32 );
    REGISTER_HOOK(RegCloseKey,Kernel32 );
    REGISTER_HOOK(DuplicateHandle,Kernel32 );
    REGISTER_HOOK(CloseHandle,Kernel32 );

    ////////////////////////////////v3 additions//////////////////////////////////////////////////
    // Timers
    REGISTER_HOOK(CreateTimerQueue,Kernel32 );
    REGISTER_HOOK(CreateTimerQueueTimer,Kernel32 );
    REGISTER_HOOK(DeleteTimerQueueTimer,Kernel32 );
    REGISTER_HOOK(DeleteTimerQueueEx,Kernel32 );
    REGISTER_HOOK(DeleteTimerQueue,Kernel32 );

    REGISTER_HOOK(InitializeCriticalSection,Kernel32 );
    REGISTER_HOOK(InitializeCriticalSectionEx,Kernel32 );
    REGISTER_HOOK(InitializeCriticalSectionAndSpinCount,Kernel32 );
    REGISTER_HOOK(DeleteCriticalSection,Kernel32 );

   
    ////////////////////////////////end v3 additions//////////////////////////////////////////////////

    ASSERT( HANDLE_FUNC_COUNT == nIndex );
    HookDynamicLoadedFun( HANDLE_FUNC_COUNT, g_arrHookedFunctions.GetData() );

}


DWORD WINAPI DumpController( LPVOID pParam )
{
    AFX_MANAGE_STATE( AfxGetStaticModuleState());
    ConfigDlg dlg;
    dlg.Create( ConfigDlg::IDD );
    dlg.ShowWindow( SW_SHOW );
    if( IDOK != dlg.RunModalLoop() && !g_IS_TEST_APP )
    {
// 		HMODULE hHookDll = GetModuleHandle( _T("HookDll.dll"));
// 		FreeLibrary( hHookDll );
        return 0;
    }

    dlg.DestroyWindow();

    m_MemMap.InitHashTable( 2001 );

    //g_HookType = 3;
    if( HT_MEMORY == g_HookType )
    {
        HookMemAlloc();
    }
    else if( HT_GDI == g_HookType )
    {
        HookGDIAlloc();
    }
    else if( HT_HANDLE == g_HookType )
    {
        HookHandleAlloc();
    }
    else
    {
        //error
    }
 
    
    HANDLE hDumpEvent	 = CreateEvent( 0, TRUE, FALSE, DUMP_EVENT );
    HANDLE hMemRestEvent = CreateEvent( 0, TRUE, FALSE, CLEAR_LEAKS );
    HANDLE hSymBolInfo   = CreateEvent( 0, TRUE, FALSE, SHOW_PDB_INFO );
    HANDLE hArray[3] = { hDumpEvent, hMemRestEvent, hSymBolInfo };
    g_bHooked = true; 
    while( 1 )
    {
        DWORD dwWait = WaitForMultipleObjects( 3, hArray, FALSE, INFINITE );
        CSingleLock lockObj( &SyncObj, TRUE );
        g_bTrack = false;
        lockObj.Unlock();
        if( dwWait == WAIT_OBJECT_0 )
        {
            ResetEvent( hDumpEvent );
            DumpLeak();
        }
        else if( dwWait == WAIT_OBJECT_0 + 1)
        {
            lockObj.Lock();
            EmptyLeakMap();
            lockObj.Unlock();
            ResetEvent( hMemRestEvent );
            
        }
        else if( dwWait == WAIT_OBJECT_0 + 2)
        {
            ModuleInfo dlg;
            dlg.DoModal();
            ResetEvent( hSymBolInfo );
        }
        else if( dwWait == WAIT_OBJECT_0 + 3)// exit event
        {
            break;
        }
        lockObj.Lock();
        g_bTrack = true;
        lockObj.Unlock();
    }
    CloseHandle( hDumpEvent );
    CloseHandle( hMemRestEvent );
    CloseHandle( hSymBolInfo );
    return 0;
}

BOOL CHookDllApp::InitInstance() 
{
    HANDLE hThread = ::CreateThread( 0,0,DumpController, 0,0, 0 );
    CloseHandle(  hThread );    
    return CWinApp::InitInstance();
}

CString GetGDIHandleType( HGDIOBJ hObj, SIZE_T nType )
{
    CString csType;
    if( nType == IMAGE_ICON ) 
    {
        csType = _T("Icon");
        return csType;
    }
    else if( nType == IMAGE_CURSOR )
    {
        csType = _T("Cursor");
        return csType;
    }

    DWORD dwType = GetObjectType( hObj );
    switch( dwType )
    {
    case OBJ_BITMAP:
        csType = _T("Bitmap");
        break;
    case OBJ_BRUSH:
        csType = _T("Brush");
        break;
    case OBJ_COLORSPACE:
        csType = _T("Color space");
        break;
    case OBJ_DC:
        csType = _T( "Device context");
        break;
    case OBJ_ENHMETADC:
        csType = _T("Enhanced metafile DC");
        break;
    case OBJ_ENHMETAFILE:
        csType = _T("Enhanced metafile");
        break;
    case OBJ_EXTPEN:
        csType = _T("Extended pen");
        break;
    case OBJ_FONT:
        csType = _T("Font");
        break;
    case OBJ_MEMDC:
        csType = _T("Memory DC");
        break;
    case OBJ_METAFILE:
        csType = _T("Metafile");
        break;
    case OBJ_METADC:
        csType = _T("Metafile DC");
        break;
    case OBJ_PAL:
        csType = _T("Palette");
        break;
    case OBJ_PEN:
        csType = _T("Pen");
        break;
    case OBJ_REGION:
        csType = _T("Region");
        break;
    default:
        csType = _T("Unknown");
        break;
    }
    return csType;

}

CString GetHandleType( HGDIOBJ hObj, SIZE_T nType )
{
    CString csType;
    switch( nType)
    {
    case TYPE_EVENT_HANDLE:
        csType = _T("Event HANDLE");
        break;
    case TYPE_MUTEX_HANDLE:
        csType = _T("Mutex HANDLE");
        break;
    case TYPE_SEMAPHOR_HANDLE:
        csType = _T("Semaphore HANDLE");
        break;
    case TYPE_CRITICAL_SECTION_HANDLE:
        csType = _T("Critical section object");
        break;
    case TYPE_WAIT_TIMER_HANDLE:
        csType = _T("Waitable timer HANDLE");
        break;
    case TYPE_FILE_HANDLE:
        csType = _T("File HANDLE");
        break;
    case TYPE_TOKEN_HANDLE:
        csType = _T("Token HANDLE");
        break;
    case TYPE_CHANGE_NOFICATION_HANDLE:
        csType = _T("Change Notification HANDLE");
        break;
    case TYPE_MEMEORY_MAPPED_FILE_HANDLE:
        csType = _T("Memory mapped file HANDLE");
        break;
    case TYPE_MEMORY_HANDLE:
        csType = _T("Memory HANDLE");
        break;
    case TYPE_PROCESS_HANDLE:
        csType = _T("Process HANDLE");
        break;
    case TYPE_THREAD_HANDLE:
        csType = _T("Thread HANDLE");
        break;
    case TYPE_JOB_HANDLE:
        csType = _T("Job HANDLE");
        break;
    case TYPE_MAIL_SLOT_HANDLE:
        csType = _T("Mail Slot HANDLE");
        break;
    case TYPE_PIPE_HANDLE:
        csType = _T("Pipe HANDLE");
        break;
    case TYPE_REGISTRY_HANDLE:
        csType = _T("Registry HANDLE");
        break;
    case TYPE_TIMER_QUEUE:
        csType = _T("Timer queue HANDLE");
        break;
    default:
        csType = _T("unknown type");
        break;
    }
    return csType;
}

void DumpLeak()
{
    if( 0 == m_MemMap.GetCount())
    {
        AfxMessageBox( "No leak detected" );
        return;
    }
    CFileDialog dlg( FALSE, _T(".txt"), _T("Dump.txt"));
    if( IDOK != dlg.DoModal())
    {
        return;
    }
    CFile File;
    //if( !File.Open( _T("D:\\Dump.txt"), CFile::modeCreate|CFile::modeWrite ))
    if( !File.Open( dlg.GetPathName(), CFile::modeCreate|CFile::modeWrite ))
    {
        AfxMessageBox( "Failed to create file" );
        return;
    }
    HANDLE hProcess = GetCurrentProcess();
    DWORD64 dwDisplacement;

    BYTE SymBol[ sizeof(SYMBOL_INFO) + STACKWALK_MAX_NAMELEN ] = {0};
    SYMBOL_INFO* pSymbol = (SYMBOL_INFO*)SymBol;
    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = STACKWALK_MAX_NAMELEN;

    IMAGEHLP_LINE64 Line = {0};
    Line.SizeOfStruct = sizeof( IMAGEHLP_LINE64 );

    MEM_INFO stInfo;
    LPVOID lMem;
    POSITION pos = m_MemMap.GetStartPosition();
    while( pos )
    {
        m_MemMap.GetNextAssoc( pos, lMem, stInfo );
        CString csLength;
        if( HT_MEMORY == g_HookType )
        {
            csLength.Format( "-->Bytes allocated -- %d\r\n\r\n", stInfo.nMemSize );
        }
        else if( HT_GDI == g_HookType )
        {
            CString csType = GetGDIHandleType( lMem, stInfo.nMemSize );
            csLength.Format( "-->%s -- 0x%x\r\n\r\n", csType, lMem );
            //csLength.Format( "Bytes allocated -- %d\r\n\r\n", stInfo.nMemSize );
        }
        else if( HT_HANDLE == g_HookType )
        {
            CString csType = GetHandleType( lMem, stInfo.nMemSize );
            csLength.Format( "-->%s -- 0x%x\r\n\r\n", csType, lMem );
        }
        
        File.Write( csLength, csLength.GetLength());
        int nCount = (int)stInfo.parCallStack->GetCount();
        for( int nIdx =1;nIdx< nCount;nIdx++ )
        {
            DWORD64 dwOffset = (*(stInfo.parCallStack)).GetAt( nIdx );

            CString cs;
            CString csFunctionName;
            
            if( !SymFromAddr( hProcess, dwOffset, &dwDisplacement, pSymbol ))
            {
                /*csFunctionName = "Unknown";*/                
                MEMORY_BASIC_INFORMATION stMemoryInfo;                 
                HMODULE hModule = 0;
                // Get the information about the virtual address space of the calling process
                if( VirtualQuery( (void*)dwOffset, &stMemoryInfo, sizeof( stMemoryInfo ))
                                                                            != 0 )
                {            
                    hModule = reinterpret_cast<HMODULE>( 
                                                    stMemoryInfo.AllocationBase);
                }
                // Get the exe's or ddl's file name
                DWORD dwFileNameLength = GetModuleFileName( hModule, csFunctionName.GetBuffer( MAX_PATH ), MAX_PATH );
                csFunctionName.ReleaseBuffer();
            }
            else
            {
                csFunctionName = pSymbol->Name;
            }
            DWORD dwLine = 0;
            if( SymGetLineFromAddr64( hProcess, dwOffset, &dwLine, &Line ))
            {
                CString csFormatString;
                int n = 40 - csFunctionName.GetLength();
                csFormatString.Format( _T("%s%d%s"), _T("%s%"), n, _T("s%s(%d)"));
                cs.Format( csFormatString, csFunctionName, _T(" "), Line.FileName, Line.LineNumber );
            }
            else
            {
                cs = csFunctionName;
            }
//            CString cs = (*(stInfo.parCallStack)).GetAt( nIdx);
            cs += _T("\r\n");
            File.Write( cs, cs.GetLength());
        }        
        TCHAR tc[] = {"------------------------------------------------\r\n\r\n\r\n\r\n"};
        File.Write( tc, sizeof(tc) - 1);
    }
    File.Close();

    if( IDYES == AfxMessageBox( _T("Dump saved.\nDo you want to open it?" ) , MB_YESNO ))
    {
        CString csDllPath;
        HMODULE hHookDll = GetModuleHandle( _T("HookDll.dll"));
        if( !GetModuleFileName( hHookDll, csDllPath.GetBuffer( MAX_PATH), MAX_PATH ))
        {
            return;
        }
        csDllPath.ReleaseBuffer();
        int nPos = csDllPath.ReverseFind( _T('\\'));
        if( 0 >= nPos )
        {
            return;
        }
        csDllPath = csDllPath.Left( nPos + 1 );
        csDllPath += _T("DumpViewer.exe");

        CString csFileName = _T("\"") + dlg.GetPathName();
        csFileName += _T("\"");
        ShellExecute( NULL, _T("open"), csDllPath, csFileName, 0, SW_SHOWDEFAULT );
    }
    

}
void EmptyLeakMap()
{
    POSITION pos = m_MemMap.GetStartPosition();
    while( pos )
    {
        LPVOID lpMem = 0;
        MEM_INFO stInfo;
        m_MemMap.GetNextAssoc( pos, lpMem, stInfo );
        //delete stInfo.parCallStack;
        
        stInfo.parCallStack->~STACK_ARRAY();
        DeleteMem( stInfo.parCallStack);
    }
    m_MemMap.RemoveAll();
}


int CHookDllApp::ExitInstance() 
{
    try
    {   
        // Restore the hooks       
        g_bHooked = false;
        EmptyLeakMap();
        RestoreHooks();
    }
    catch (...)
    {
        
    }
    //DumpLeak();
    return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////////////

HOOK_DLL_EXPORT bool IsLeakDetected( void* pObject )
{
    try
    {
        if( !pObject )
        {
            return false;
        }
        MEM_INFO stInfo;
        if( m_MemMap.Lookup( pObject, stInfo ))
        {
            return true;
        }
    }
    catch(...)
    {
    }
    return false;
}

HOOK_DLL_EXPORT void SetHookType(HOOK_TYPE_e eType )
{
    g_HookType = eType;
}