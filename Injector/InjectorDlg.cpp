// InjectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Injector.h"
#include "InjectorDlg.h"
#include "..\Common\Common.h"
#include <TlHelp32.h>
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
        // No message handlers
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInjectorDlg dialog

CInjectorDlg::CInjectorDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CInjectorDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CInjectorDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_nColumnClick = -1;
    m_bSortAscending = true;
}

void CInjectorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CInjectorDlg)
    DDX_Control(pDX, IDC_LIST1, m_List);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInjectorDlg, CDialog)
    //{{AFX_MSG_MAP(CInjectorDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, OnInject)
    ON_BN_CLICKED(IDC_BUTTON2, OnDump)
    ON_BN_CLICKED(IDC_BUTTON3, ClearLeaks)
    ON_BN_CLICKED(IDC_BUTTON4, OnRefresh)
    ON_BN_CLICKED(IDC_BUTTON5, OnInfo)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnColumnclickList1)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void EnableDebugPriv( void )
{
    HANDLE hToken;
    LUID sedebugnameValue;
    TOKEN_PRIVILEGES tkp;

    if( ! OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
    {
        return;
    }
    if ( ! LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &sedebugnameValue ) )
    {
        CloseHandle( hToken );
        return;
    }

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = sedebugnameValue;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL );
    CloseHandle( hToken );
}

BOOL CInjectorDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon
    
    ListView_SetExtendedListViewStyleEx( m_List.m_hWnd, LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES,
                                                        LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES );
    m_List.InsertColumn( 0, _T("PID"), LVCFMT_LEFT, 75 );
    m_List.InsertColumn( 1, _T("Process"), LVCFMT_LEFT, 245 );

//    EnableDebugPriv();
    Refresh();
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInjectorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}

void CInjectorDlg::OnPaint() 
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

HCURSOR CInjectorDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

void CInjectorDlg::OnInject() 
{
    int nSelected = m_List.GetNextItem( -1, LVNI_SELECTED  );
    if( -1 == nSelected )
    {
        AfxMessageBox( _T("Please select a process"));
        return;
    }

    HANDLE hDuumpEvent = CreateEvent( 0, TRUE, FALSE, DUMP_EVENT );
    if( GetLastError() == ERROR_ALREADY_EXISTS)
    {
        AfxMessageBox( _T("The Dll is already injected in one application.\nPlease close that one before trying to inject again"));
        CloseHandle( hDuumpEvent );
        return;
    }
    CloseHandle( hDuumpEvent );

    

    CString csPath;
    LPSTR lpPath = csPath.GetBuffer( MAX_PATH );
    //GetCurrentDirectory( MAX_PATH, csPath.GetBuffer( MAX_PATH ));
    GetModuleFileName(0,lpPath, MAX_PATH );
    PathRemoveFileSpec( lpPath );
    csPath.ReleaseBuffer();    
    csPath += _T("\\hookdll.dll");

     if( !PathFileExists( csPath ))
     {
         AfxMessageBox( _T("Hookdll.dll not found in the path " + csPath + "\n\nCannot continue!"));
         return;
     }

    

    CString csPid = m_List.GetItemText( nSelected, 0 );
    DWORD dwPID = _ttoi( csPid );
    HANDLE hProcess =  OpenProcess( PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|
                                    PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ,
                                    FALSE, dwPID );    
    if( !hProcess)
    {
        AfxMessageBox( _T("Failed to open the process" ));
        return;
    }

    //csPath = _T("c:\\WINDOWS\\system32\\opengl32.dll");
    HINSTANCE hLib = GetModuleHandle( "Kernel32.dll" );    
    PROC pLoadLib = (PROC)GetProcAddress( hLib, "LoadLibraryA" );
    void* pLibRemote = ::VirtualAllocEx( hProcess, NULL, csPath.GetLength(),
                                         MEM_COMMIT, PAGE_READWRITE );
    ::WriteProcessMemory( hProcess, pLibRemote, (void*)csPath.operator LPCTSTR(),
                          csPath.GetLength(), NULL );
    if( !CreateRemoteThread( hProcess, 0, 0, (LPTHREAD_START_ROUTINE)pLoadLib, pLibRemote, 0, 0 ))
    {
        AfxMessageBox( "Create Remote thread Failed" );
        ::VirtualFreeEx( hProcess, pLibRemote,csPath.GetLength(), MEM_RELEASE );
    }
    
}

void CInjectorDlg::OnDump() 
{
    HANDLE hDuumpEvent = CreateEvent( 0, TRUE, FALSE, DUMP_EVENT );
    SetEvent( hDuumpEvent );
    CloseHandle( hDuumpEvent );
}

void CInjectorDlg::ClearLeaks() 
{
    HANDLE hMemRestEvent = CreateEvent( 0, TRUE, FALSE, CLEAR_LEAKS );
    SetEvent( hMemRestEvent );
    CloseHandle( hMemRestEvent );
}


void CInjectorDlg::Refresh()
{
    DWORD dwCurrentPid = GetCurrentProcessId();
    m_List.DeleteAllItems();
    HANDLE  hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    PROCESSENTRY32 stProcess = {0};
    stProcess.dwSize = sizeof( stProcess );
    Process32First( hSnapShot, &stProcess );
    bool bFound = false;
    do 
    {
        if( dwCurrentPid != stProcess.th32ProcessID )
        {
            CString csPid;
            csPid.Format( _T("%d"), stProcess.th32ProcessID );
            int nPos = m_List.InsertItem( 0, csPid );
            m_List.SetItemText( nPos, 1, stProcess.szExeFile );
        }
    } while( Process32Next( hSnapShot, &stProcess ));   
}

void CInjectorDlg::OnRefresh() 
{
    Refresh();
}

void CInjectorDlg::OnInfo() 
{
    HANDLE hMemRestEvent = CreateEvent( 0, TRUE, FALSE, SHOW_PDB_INFO );
    SetEvent( hMemRestEvent );
    CloseHandle( hMemRestEvent );
}

void CInjectorDlg::OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
    try
    {
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
        HDITEM stHItem = {0};
        stHItem.mask = HDI_FORMAT;
        stHItem.fmt = HDF_LEFT|HDF_STRING;
        if( -1 != m_nColumnClick )
        {
            m_List.GetHeaderCtrl()->SetItem( m_nColumnClick, &stHItem );
        }
        if( m_nColumnClick != pNMListView->iSubItem )
        {
            m_bSortAscending = true;
        }
        m_nColumnClick = pNMListView->iSubItem;
        ListView_SortItemsEx( m_List.m_hWnd, CompareFunc, (LPARAM)this );
        stHItem.fmt |= (m_bSortAscending)? HDF_SORTUP:HDF_SORTDOWN;
        m_List.GetHeaderCtrl()->SetItem( m_nColumnClick, &stHItem );
        m_bSortAscending = !m_bSortAscending;
    }
    catch(...)
    {		
    }  	
    *pResult = 0;
}

int CInjectorDlg::CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
    CInjectorDlg* pThis = (CInjectorDlg*)lParamSort;
    CString csText1 = pThis->m_List.GetItemText( lParam1, pThis->m_nColumnClick );
    CString csText2 = pThis->m_List.GetItemText( lParam2, pThis->m_nColumnClick );    
    int nReturn = 0;
    if( pThis->m_bSortAscending )
    {
        nReturn = csText1.Compare( csText2 );
    }
    else
    {
        nReturn = csText2.Compare( csText1 );
    }
    return nReturn;
}