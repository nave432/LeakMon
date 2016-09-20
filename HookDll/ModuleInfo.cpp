// ModuleInfo.cpp : implementation file
//

#include "stdafx.h"
#include "HookDll.h"
#include "ModuleInfo.h"
#include <Dbghelp.h>

// ModuleInfo dialog

IMPLEMENT_DYNAMIC(ModuleInfo, CDialog)

ModuleInfo::ModuleInfo(CWnd* pParent /*=NULL*/)
	: CDialog(ModuleInfo::IDD, pParent)
{
	m_nColumnClick = -1;
	m_bSortAscending = true;

}

ModuleInfo::~ModuleInfo()
{
}

void ModuleInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
}


BEGIN_MESSAGE_MAP(ModuleInfo, CDialog)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, ModuleInfo::OnLvnColumnclickList1)
END_MESSAGE_MAP()


// ModuleInfo message handlers

BOOL ModuleInfo::OnInitDialog()
{
	CDialog::OnInitDialog();
	ListView_SetExtendedListViewStyleEx( m_List, LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
												 LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT );
	m_List.InsertColumn( 0, "Module", LVCFMT_LEFT, 100 );
	m_List.InsertColumn( 1, "PDB", LVCFMT_LEFT, 500 );
	SymEnumerateModules64( GetCurrentProcess(),MYSymEnumerateModulesProc64, this );

	return TRUE;  // return TRUE unless you set the focus to a control
}


BOOL CALLBACK ModuleInfo::MYSymEnumerateModulesProc64(  PCSTR ModuleName,
										  DWORD64 BaseOfDll,
										  PVOID UserContext )
{
	ModuleInfo *pDlg = (ModuleInfo*)UserContext;
	IMAGEHLP_MODULE64 stInfo = {0};
	stInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
	int nPos = pDlg->m_List.InsertItem( pDlg->m_List.GetItemCount(), ModuleName );
	if( SymGetModuleInfo64( GetCurrentProcess(), BaseOfDll, &stInfo ))
	{   
		if( stInfo.LoadedPdbName[0] != 0)
		{
			pDlg->m_List.SetItemText( nPos, 1, stInfo.LoadedPdbName );

		}
		//         DWORD dwTimeStamp = GetTimestampForLoadedLibrary( (HMODULE)BaseOfDll );
		//         if( stInfo.TimeDateStamp !=  dwTimeStamp )
		//         {
		//             pDlg->m_List.SetItemText( nPos, 2, "No" );
		//         }
		// 		else
		// 		{
		// 			pDlg->m_List.SetItemText( nPos, 2, "Yes" );
		// 		}
	}
	return TRUE;
}
void ModuleInfo::OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
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

int ModuleInfo::CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
	ModuleInfo* pThis = (ModuleInfo*)lParamSort;
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
