
#include "stdafx.h"
#include "DumpTree.h"

#include "DumpTreeDoc.h"
#include "DumpTreeView.h"
#include <afxtempl.h>
#include "FilterDialog.h"
#include "Vc6AutoClasses.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef CArray<CString, CString> CHILDARRAY;

IMPLEMENT_DYNCREATE( CDumpTreeView, CTreeView )

// Register Find Dialog Message
const UINT CDumpTreeView::m_FindDialogMessage = RegisterWindowMessage( FINDMSGSTRING );


#pragma warning( disable : 4278 )
#pragma warning( disable : 4146 )
//The following #import imports EnvDTE based on its LIBID.
#import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("8.0") lcid("0") raw_interfaces_only named_guids
//The following #import imports EnvDTE80 based on its LIBID.
#import "libid:1A31287A-4D7D-413e-8E32-3B374931BD89" version("8.0") lcid("0") raw_interfaces_only named_guids
#pragma warning( default : 4146 )
#pragma warning( default : 4278 )

using namespace EnvDTE;
using namespace EnvDTE80;

BEGIN_MESSAGE_MAP( CDumpTreeView, CTreeView )
//{{AFX_MSG_MAP(CDumpTreeView)
    ON_COMMAND( ID_FILE_OPEN, OnFileOpen )
    ON_COMMAND( ID_FILE_SAVE_AS, OnFileSaveAs )
    ON_COMMAND(ID_REMOVE_ITEM, OnRemoveItem)
    ON_COMMAND(ID_EDIT_FIND_NEXT, OnFindNext)
    ON_COMMAND(ID_EDIT_FIND, OnFind)
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_EDIT_RELOAD, OnEditReload)
	ON_COMMAND(ID_EDIT_FILTER, OnEditFilter)
    ON_COMMAND(ID_EDIT_EXCLUDE, OnRemoveItem)
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
// Add Registered Find Dialog Message to Message Map
ON_REGISTERED_MESSAGE( m_FindDialogMessage, OnFindDialogMessage )
// Standard printing commands
ON_COMMAND(ID_FILE_PRINT, CTreeView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_DIRECT, CTreeView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_PREVIEW, CTreeView::OnFilePrintPreview)
END_MESSAGE_MAP()


bool OpenFile( _DTE* pDTE,CString csFileName_i, int nLineNo )
{
    HRESULT hRes = 0; 
    CComPtr<ItemOperations> Operations;
    // Get the ItemOperations pointer
    RETURN_ON_FAIL(pDTE->get_ItemOperations( &Operations ));
    CComPtr<Window> Wnd;
    // Open the file using ItemOperations pointer
    RETURN_ON_FAIL(Operations->OpenFile( CComBSTR( csFileName_i), CComBSTR(vsViewKindTextView), &Wnd ))
    
    // It might take some time to open the file. so we will wait in a loop
    CComPtr<Document> pDoc;
    for( int nIdx =0; nIdx < 5; nIdx ++ )
    {
        pDTE->get_ActiveDocument( &pDoc );
        //Sleep( 100 );
        if( pDoc )
        {
            break;
        }
        if( nIdx == 4 )
        {
            return false;
        }
        Sleep( 100 );
    }
    // Active the main window. 
    CComPtr<Window> pMainWnd;
    RETURN_ON_FAIL(pDTE->get_MainWindow( &pMainWnd ))
    pMainWnd->Activate();
    
    // Get the TextSelection pointer
    CComPtr<IDispatch> pDisp = 0;
    RETURN_ON_FAIL(pDoc->get_Selection( &pDisp ));
    CComQIPtr<TextSelection> pSelection = pDisp;
     //GotoLine with VARIANT_TRUE will make the line selected
    pSelection->GotoLine( nLineNo, VARIANT_TRUE);

    return true;
}


//////////////////////////////////////////////////////////////////////////
 struct __declspec(uuid("{EC1D73A1-8CC4-11cf-9BE9-00A0C90A632C}")) IApplication;
 const GUID IID_IApplication = { 0xEC1D73A1L,0x8CC4,0x11CF,0x9B,0xE9,0x00,0xA0,0xC9,0x0A,0x63,0x2C };
 
 struct __declspec(uuid("{FB7FDAE3-89B8-11CF-9BE8-00A0C90A632C}")) IDocuments;
 const GUID  IID_IDocuments = {0xFB7FDAE3L,0x89B8,0x11CF,0x9B,0xE8,0x00,0xA0,0xC9,0x0A,0x63,0x2C};
 
 // {2A6DF201-8240-11CF-AB59-00AA00C091A1}
 struct __declspec(uuid("{2A6DF201-8240-11CF-AB59-00AA00C091A1}")) ITextDocument;
 const GUID  IID_ITextDocument = { 0x2A6DF201L,0x8240,0x11CF,0xAB,0x59,0x00,0xAA,0x00,0xC0,0x91,0xA1 };
 
 // {05092F20-833F-11CF-AB59-00AA00C091A1}
 struct __declspec(uuid("{05092F20-833F-11CF-AB59-00AA00C091A1}")) ITextSelection;
 const GUID  IID_ITextSelection = { 0x05092F20L,0x833F,0x11CF,0xAB,0x59,0x00,0xAA,0x00,0xC0,0x91,0xA1 };
 
 //#include "APPAUTO.H"
 //#include "TEXTAUTO.H"
 
#define dsWindowStateMaximized 1
bool OpenFile(IApplication* m_spApplication,CString csFileName_i, int nLineNo )
 {
    CComPtr<IDispatch> pDispatch;
    // Make it visible just in case if it is not
    m_spApplication->put_Visible( VARIANT_TRUE );
    // Maximize the window
    m_spApplication->put_WindowState( dsWindowStateMaximized );
    // Get a pointer to the IDocument interface
    m_spApplication->get_Documents( &pDispatch );
    CComQIPtr<IDocuments> pDocs = pDispatch;
    CComVariant type="Auto";
    CComVariant read="False";
    pDispatch = 0;
    // The Open function of IDocument opens the file.
    pDocs->Open( CComBSTR(csFileName_i),type, read, &pDispatch );
    // Now for selecting a line, we need to get pointer to ITextSelection
    // Interface
    CComQIPtr<ITextDocument> pDoc = pDispatch;
    pDispatch = 0;
    pDoc->get_Selection( &pDispatch );
    CComQIPtr <ITextSelection> pTextSelection = pDispatch;
    CComVariant varReserved( FALSE );
    // now just set the cursor to the correct line and call SelectLine of ITextSelection
    pTextSelection->StartOfDocument( varReserved );
    pTextSelection->LineDown( varReserved, CComVariant(nLineNo-1));
    Sleep( 1000 );
    pTextSelection->SelectLine();
    return true;
 }
 
 void OnLaunch( IApplication* pApp )
 {
 	// Null implementation
 }

void OnLaunch( _DTE* pDTE )
{
	pDTE->put_UserControl( VARIANT_TRUE );
}

template<class T>
bool OpenUsingVS( IID riid, CString csProgID, CString csFileName_i, int nLineNo)
{
    HRESULT hRes; 
    CLSID clsid; 
    CComPtr<IUnknown> punk; 
    CComPtr<T> dte; 
    RETURN_ON_FAIL( ::CLSIDFromProgID( csProgID.operator LPCTSTR(), &clsid) ); 

    // Search through the Running Object Table for an instance of Visual Studio 
    // to use that either has the correct solution already open or does not have  
    // any solution open. 
    CComPtr<IRunningObjectTable> ROT; 
    RETURN_ON_FAIL( GetRunningObjectTable( 0, &ROT ) ); 

    CComPtr<IBindCtx> bindCtx; 
    RETURN_ON_FAIL( CreateBindCtx( 0, &bindCtx ) ); 

    CComPtr<IEnumMoniker> enumMoniker; 
    RETURN_ON_FAIL( ROT->EnumRunning( &enumMoniker ) ); 

    CComPtr<IMoniker> dteMoniker; 
    RETURN_ON_FAIL( CreateClassMoniker( clsid, &dteMoniker ) ); 

    CComPtr<IMoniker> moniker; 
    CComPtr<IMoniker> moniker2; 
    ULONG monikersFetched = 0; 
    while ( enumMoniker->Next( 1, &moniker, &monikersFetched ) == S_OK) 
    { 
        moniker2 =NULL;
        moniker->Reduce( bindCtx, MKRREDUCE_ALL, NULL, &moniker2 );
        IMoniker* pMon = (0 != moniker2)?moniker2:moniker;
        if ( moniker2->IsEqual( dteMoniker ) ) 
        { 
            hRes = ROT->GetObject( moniker, &punk ); 
            if ( hRes == S_OK ) 
            { 
                dte = punk; 
 
                if ( dte ) 
                { 
                    if( OpenFile( dte,csFileName_i, nLineNo ))
                        return true;
                    dte = 0;
                } 
            } 
            punk = NULL; 
        } 
        moniker = NULL; 
    } 

    if ( !dte ) 
    {                
        RETURN_ON_FAIL( ::CoCreateInstance( clsid, NULL, CLSCTX_LOCAL_SERVER, riid , (LPVOID*)&punk ) ); 
        dte = punk; 
        if ( !dte ) 
            return false; 
        OnLaunch( dte );
        OpenFile( dte,csFileName_i, nLineNo );
    }
    return true;
}

void OpenEditor( CString csFileName_i, int nLineNo )
{
    CString csAppName;
    if( !FindExecutable( csFileName_i, _T(""), csAppName.GetBuffer( MAX_PATH ) ))
    {
        csAppName.ReleaseBuffer();
        return;
    }
    csAppName.ReleaseBuffer();
    const LPCTSTR VC_6_APP_NAME = _T("msdev.exe");
    const LPCTSTR VC_8_APP_NAME = _T("devenv.exe");
    int nPos = csAppName.ReverseFind( _T('\\'));
    if( -1 == nPos )
    {
        return;
    }
    CString csExe = csAppName.Right( csAppName.GetLength() - nPos - 1 );
    if( 0 == csExe.CompareNoCase( VC_6_APP_NAME ))
    {
        // launch vc6
        OpenUsingVS<IApplication>( IID_IDispatch, L"MSDEV.Application",csFileName_i, nLineNo );
    }
    else if( 0 == csExe.CompareNoCase( VC_8_APP_NAME ) )
    {
        // launch in VS higher than VS6( VS7 to VS10 so far)
        OpenUsingVS<_DTE>( EnvDTE::IID__DTE, L"VisualStudio.DTE",csFileName_i, nLineNo );
    }

}


/** 
 * 
 * Constructor
 * 
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
CDumpTreeView::CDumpTreeView() : m_pFindDlg( 0 )
{
}


/** 
 * 
 * Destructor
 * 
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
CDumpTreeView::~CDumpTreeView()
{
    if( m_pFindDlg )
    {
        delete m_pFindDlg;
    }
}


/** 
 * 
 * PreCreateWindow function
 * 
 * @param       cs   - 
 * @return      BOOL - 
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
BOOL CDumpTreeView::PreCreateWindow( CREATESTRUCT& cs )
{
    // Set styles of Tree view
    cs.style |= TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS;
    return CTreeView::PreCreateWindow(cs);
}


/** 
 * 
 * OnDraw function
 * 
 * @param       pDC  - 
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::OnDraw( CDC* pDC )
{
    CDumpTreeDoc* pDoc = GetDocument();
    ASSERT_VALID( pDoc );
}


/** 
 * 
 * OnInitial Update function
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::OnInitialUpdate()
{
    
    CTreeView::OnInitialUpdate();
    m_TreeFont.CreatePointFont( 100, _T("Courier New") );
    m_TreeImgs.Create( 18, 18, ILC_COLOR32|ILC_MASK, 0, 2 );
    
    HICON hIcon = AfxGetApp()->LoadIcon( IDI_RIGHT );
    m_TreeImgs.Add( hIcon );
    hIcon = AfxGetApp()->LoadIcon( IDI_DOWN );
    m_TreeImgs.Add( hIcon );

    GetTreeCtrl().SetImageList( &m_TreeImgs, TVSIL_NORMAL );
    GetTreeCtrl().SetFont( &m_TreeFont );

    GetTreeCtrl().SetItemHeight( 20 );
    //GetTreeCtrl().SetIndent( 40 );

	m_csFileName = GetDocument()->GetFileName();
	if( !m_csFileName.IsEmpty())
	{
		LoadFile();
	}
	
}


#ifdef _DEBUG

/** 
 * 
 * AssertValid function
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::AssertValid() const
{
    CTreeView::AssertValid();
}


/** 
 * 
 * Dump function
 * 
 * @param       dc   - 
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::Dump( CDumpContext& dc ) const
{
    CTreeView::Dump(dc);
}


/** 
 * 
 * Return pointer to Document
 * 
 * @param       Nil
 * @return      CDumpTreeDoc* - 
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
CDumpTreeDoc* CDumpTreeView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDumpTreeDoc)));
    return (CDumpTreeDoc*)m_pDocument;
}
#endif //_DEBUG


/** 
 * 
 *  This function Opens a Dump file and create a tree view.
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::OnFileOpen()
{
    m_csFileName.Empty();
    CString csFileType = "Text File (*.txt)|*.txt||";
    CFileDialog ObjFileDlg( TRUE, NULL, NULL, OFN_FILEMUSTEXIST, csFileType );
    if( IDOK == ObjFileDlg.DoModal())
    {
        m_csFileName = ObjFileDlg.GetPathName();
    }
    else
    {
        return;
    }
    if( m_csFileName.IsEmpty())
    {
        AfxMessageBox( L"File name is empty" );
        return;
    }
    LoadFile();
}


/** 
 * 
 * This function is used to Load a file and creates it's tree view.
 * If filter is not Empty, it will only load the parents which contains the filter string.
 * 
 * @param       csFilter_i - 
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::LoadFile()
{
    CStdioFile ObjFile;
    if( !ObjFile.Open( m_csFileName, CFile::modeRead ))
    {
        AfxMessageBox( L"Unable to open File\n" );
        return;
    }
    // Set Title of the window.
    GetDocument()->SetTitle( m_csFileName );
    // Clear Tree View
    RemoveAllArrays();
    GetTreeCtrl().DeleteAllItems();
    CString csString;
    HTREEITEM hParent = 0;
    CHILDARRAY ChildArrayToLoad;// To Load Childs
    while( ObjFile.ReadString( csString ))
    {
        if( csString.IsEmpty() || -1 != csString.Find( L"-----------" ))
        {
            continue;
        }
        
        if( -1 == csString.Find( L"-->" ))
        {
            // Add to Load Array
            ChildArrayToLoad.Add( csString );
            continue;
        }
        //Display only Parent Items
        // Save Data of Previous Parent.
        if( hParent != NULL )
        {
            // Save Previous Childs.
            CHILDARRAY* pArraySave = new CHILDARRAY;
            pArraySave->Copy( ChildArrayToLoad );
            // Save Array poiter as Item Data
            GetTreeCtrl().SetItemData( hParent, ( DWORD )pArraySave );
            // Remove Previous Child Items.
            ChildArrayToLoad.RemoveAll();
        }
        hParent = GetTreeCtrl().InsertItem( csString );
        // To Display Button for Item to Expand
        GetTreeCtrl().InsertItem( L"1", hParent );
    }
    if( hParent != NULL )
    {
        // Save Previous Childs.
        CHILDARRAY* pArraySave = new CHILDARRAY;
        pArraySave->Copy( ChildArrayToLoad );
        // Save Array poiter as Item Data
        GetTreeCtrl().SetItemData( hParent, ( DWORD )pArraySave );
    }
    ObjFile.Close();    
}


/** 
 * 
 * This function Expand the Parent Item by reading child Items from file and
 * Inserting under the given Parent Item
 * 
 * @param       hItem_i - 
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::ExpandItem( const HTREEITEM& hItem_i )
{
    if( !GetTreeCtrl().ItemHasChildren( hItem_i ))
    {
        return;
    }
    // Remove the unwanted "1" child 
    HTREEITEM hChildRemove = GetTreeCtrl().GetChildItem( hItem_i );
    CString csDelete( "1" );
    if( csDelete != GetTreeCtrl().GetItemText( hChildRemove ))
    {
        return;// It is expanded already
    }
    GetTreeCtrl().DeleteItem( hChildRemove );
    CHILDARRAY* pChildArray = ( CHILDARRAY* ) GetTreeCtrl().GetItemData( hItem_i );
    if( 0 == pChildArray )
    {
        AfxMessageBox( L"Unable to Retrieve Child Array" );
        return;
    }
    int nSize = pChildArray->GetCount();
    for( int nIdx = 0; nSize > nIdx; ++nIdx )
    {
        CString csChild = pChildArray->GetAt( nIdx );
        // Insert child Items
        GetTreeCtrl().InsertItem( csChild, hItem_i );
    }
}


/** 
 * 
 * This function save the tree view as another file.
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::OnFileSaveAs()
{
    CString csFileName;
    CString csFileType = L"Text File (*.txt)|*.txt||";
    CFileDialog ObjFileDlg( FALSE, L"txt", L"Dump_Modified", 
                            OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | 
                            OFN_EXTENSIONDIFFERENT,
                            csFileType );
    if( IDOK == ObjFileDlg.DoModal())
    {
        csFileName = ObjFileDlg.GetPathName();
    }
    else
    {
        return;
    }
    if( m_csFileName == csFileName )
    {
        AfxMessageBox( L"The file is currently in use and cannot be overwritten" );
        return;
    }
    CStdioFile ObjWriteFile;
    if( !ObjWriteFile.Open( csFileName, CFile::modeWrite | CFile::modeCreate ))
    {
        AfxMessageBox( L"Failed to create the file" );
        return;
    }
    HTREEITEM hItem = GetTreeCtrl().GetRootItem();
    while( hItem != NULL )
    {
        CString csItemString = GetTreeCtrl().GetItemText( hItem ) + "\n";
        ObjWriteFile.WriteString( csItemString );
        // Write Child Items 
        WriteChildItems( ObjWriteFile, hItem );
        hItem = GetTreeCtrl().GetNextSiblingItem( hItem );
    }
    ObjWriteFile.Close();
}


/** 
* 
* This function will write the child Items to the file.
* 
* @param       ObjFile_io - 
* @param       hItem_i    - 
* @return      void
* @exception   Nil
* @see         Nil
* @since       1.0
*/
void CDumpTreeView::WriteChildItems( CStdioFile& ObjFile_io, const HTREEITEM& hItem_i )
{

    CHILDARRAY* pChildArray = ( CHILDARRAY* ) GetTreeCtrl().GetItemData( hItem_i );
    if( 0 == pChildArray )
    {
        AfxMessageBox( L"Unable to Retrieve Child Array" );
        return;
    }
    int nSize = pChildArray->GetSize();
    ObjFile_io.WriteString( L"\n" );
    for( int nIdx = 0; nSize > nIdx; ++nIdx )
    {
        CString csChild = pChildArray->GetAt( nIdx ) + "\n";
        ObjFile_io.WriteString( csChild );
    }
    ObjFile_io.WriteString( L"-----------------------------------------------------\n\n" );
}


/** 
 * 
 * This function removes Parent nodes if it's child Item contains selected item
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::OnRemoveItem()
{
    HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
    if( NULL == hItem )
    {
        AfxMessageBox( L"No Item Selected" );
        return;
    }
    CString csItemSelected =  GetTreeCtrl().GetItemText( hItem );
    csItemSelected.MakeLower();
    hItem = GetTreeCtrl().GetRootItem();
    while( hItem != NULL )
    {
        if( !FindChildInArray( hItem, csItemSelected ))// item has no selected child.
        {
            hItem = GetTreeCtrl().GetNextSiblingItem( hItem );
            continue;
        }
        // Finds  string in child, Delete Item
        HTREEITEM hItemDel = hItem;
        hItem = GetTreeCtrl().GetNextSiblingItem( hItem );
        CHILDARRAY *pChildArray = ( CHILDARRAY* ) GetTreeCtrl().GetItemData( hItemDel );
        delete pChildArray;
        GetTreeCtrl().DeleteItem( hItemDel );
    }
}


/** 
 * 
 * This function Removes all the arrays in the heap
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::RemoveAllArrays()
{
    HTREEITEM hItem = GetTreeCtrl().GetRootItem();
    while( hItem )
    {    
        CHILDARRAY *pChildArray = ( CHILDARRAY* ) GetTreeCtrl().GetItemData( hItem );
        delete pChildArray;
        hItem = GetTreeCtrl().GetNextSiblingItem( hItem );
    }
}


/** 
 * 
 * This function searches child items for the given string 
 * 
 * @param       hItem_i    - Parent Item to search
 * @param       csSearch_i - Search text
 * @return      bool       - true if it finds a child item with given string
 *                           else false
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool CDumpTreeView::FindChildInArray( const HTREEITEM& hItem_i, const CString& csSearch_i )
{
    CHILDARRAY* pChildArray = ( CHILDARRAY* ) GetTreeCtrl().GetItemData( hItem_i );
    if( 0 == pChildArray )
    {
        AfxMessageBox( L"Unable to Retrieve Child Array" );
        return false;
    }
    int nSize = pChildArray->GetSize();
    for( int nIdx = 0; nSize > nIdx; ++nIdx )
    {
        CString csChild = pChildArray->GetAt( nIdx );
        csChild.MakeLower();
        if( -1 != csChild.Find( csSearch_i ))
        {
            return true;
        }
    }
    return false;
}


/** 
 * 
 * This function Searches given string in Child Items in tree view, 
 * It first searches in file first and if it finds then expand the Parent Item
 * and highlights/select the item in the tree view. It incorporates searching both
 * down ward and upward.
 * 
 * @param       hItem_i     - Parent Item
 * @param       csSel_i     - Search text
 * @param       hSelChild_i - Contains Child if it is selected at that time.
 * @param       bDownWard_i - true if downward search else false
 * @return      bool        - true if it finds else false
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool CDumpTreeView::FindStringInChild( const HTREEITEM& hItem_i, const CString& csSel_i,
                                       const HTREEITEM& hSelChild_i, bool bDownWard_i )
{
    HTREEITEM hChildItem = hSelChild_i;
    if( 0 == hChildItem )
    {
        // Find the Child Item from file
        if( !FindChildInArray( hItem_i, csSel_i ))
        {
            return false;
        }
        // Expand the Item
        ExpandItem( hItem_i );
        // Get First Child
        hChildItem = GetTreeCtrl().GetChildItem( hItem_i );
        if( !bDownWard_i )// Start from Last child if search is upwards
        {
            HTREEITEM hLastItem = hChildItem;
            while( hChildItem != NULL )
            {
                hLastItem = hChildItem;
                hChildItem = GetTreeCtrl().GetNextSiblingItem( hChildItem );
            }
            hChildItem = hLastItem;
        }
    }
    else// If a child is selected,start from that child
    {
        hChildItem = bDownWard_i ? GetTreeCtrl().GetNextSiblingItem( hChildItem ):
        GetTreeCtrl().GetPrevSiblingItem( hChildItem );
    }
    while( hChildItem != NULL )
    {
        CString csItemString = GetTreeCtrl().GetItemText( hChildItem );
        csItemString.MakeLower();
        if( -1 != csItemString.Find( csSel_i ))
        {
            GetTreeCtrl().SelectItem( hChildItem );
            return true;
        }
        hChildItem = bDownWard_i ? GetTreeCtrl().GetNextSiblingItem( hChildItem ):
        GetTreeCtrl().GetPrevSiblingItem( hChildItem );
    }
    return false;
}


/** 
 * 
 * This function searches the already selected string to search.
 * It is called when Find Next or F3 is pressed.
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::OnFindNext()
{
    if( m_csFindString.IsEmpty())
    {
        return;
    }
    // Make Lower the find string.
    m_csFindString.MakeLower();
    // Get the Selected Item to start search from that item
    HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
    HTREEITEM hParentItem;
    if( NULL == hItem )// If no selected Item Take Root to start search
    {
        hParentItem = GetTreeCtrl().GetRootItem();
    }
    else
    {
        // If it is child node Take it's Parent to begin search.
        hParentItem = GetTreeCtrl().GetParentItem( hItem );// hItem contains
        if( 0 == hParentItem )// Selected item if it is child and start search from
        {                          // that child.
            hParentItem = hItem;
            hItem = 0;
        }
    }
    while( NULL != hParentItem )
    {
        if( !FindStringInChild( hParentItem, m_csFindString, hItem, m_bSearchDown ))// item has no selected child.
        {
            hParentItem = m_bSearchDown ? GetTreeCtrl().GetNextSiblingItem( hParentItem ):
            GetTreeCtrl().GetPrevSiblingItem( hParentItem );
            hItem = 0;
            continue;
        }
        // Finds  string in child
        break;
    }
}


/** 
 * 
 * This function is called to Find a string.
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::OnFind()
{
    if( m_pFindDlg )
    {
        delete m_pFindDlg;
        m_pFindDlg = 0;
    }
    m_pFindDlg = new CFindReplaceDialog;
    m_pFindDlg->Create( TRUE, m_csFindString, NULL, FR_DOWN, this );
}


/** 
 * 
 * This function is called when the Find Window sends a message
 * 
 * @param       wParam  - 
 * @param       lParam  - 
 * @return      LRESULT - 
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
LRESULT CDumpTreeView::OnFindDialogMessage( WPARAM wParam, LPARAM lParam )
{
    ASSERT( m_pFindDlg != NULL );
    
    // If the FR_DIALOGTERM flag is set,
    // invalidate the handle identifying the dialog box.
    if (m_pFindDlg->IsTerminating())
    {
        m_pFindDlg = NULL;
        return 0;
    }
    
    // If the FR_FINDNEXT flag is set,
    // call the application-defined search routine
    // to search for the requested string.
    if( !m_pFindDlg->FindNext())
    {
        return 0;        
    }
    //read data from dialog
    m_csFindString = m_pFindDlg->GetFindString();
    //bool bMatchCase = m_pFindDlg->MatchCase();
    //bool bMatchWholeWord = m_pFindDlg->MatchWholeWord();
	m_bSearchDown = m_pFindDlg->SearchDown()?true:false;
    OnFindNext();
    return 0;
}


/** 
 * 
 * This function is called when an item is going to expand.
 * 
 * @param       pNMHDR  - 
 * @param       pResult - 
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::OnItemexpanding( NMHDR* pNMHDR, LRESULT* pResult )
{
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
    HTREEITEM hItem =  pNMTreeView->itemNew.hItem;
    ExpandItem( hItem );

   
    *pResult = 0;
}


void CDumpTreeView::OnItemexpanded( NMHDR* pNMHDR, LRESULT* pResult )
{
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
    HTREEITEM hItem =  pNMTreeView->itemNew.hItem;    

    if( TVIS_EXPANDED  == (pNMTreeView->itemNew.state&TVIS_EXPANDED))
    {
        GetTreeCtrl().SetItemImage( hItem, 1, 1 );
    }
    else
    {
        GetTreeCtrl().SetItemImage( hItem, 0, 0 );
    }
    
    *pResult = 0;
}



/** 
 * 
 * This function is called to copy an item.
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::OnEditCopy()
{
    HTREEITEM hItemSelected = GetTreeCtrl().GetSelectedItem();
    CString csItem = GetTreeCtrl().GetItemText( hItemSelected );
    // Open Clipboard
    if ( !OpenClipboard() )
    {
        AfxMessageBox( L"Cannot open the Clipboard" );
        return;
    }
    // Remove the current Clipboard contents  
    if( !EmptyClipboard() )
    {
        AfxMessageBox( L"Cannot empty the Clipboard" );
        return;  
    }
    HGLOBAL hglbCopy;
    int nLength = csItem.GetLength();
    // Allocate a global memory object for the text. 
    hglbCopy = GlobalAlloc( GMEM_MOVEABLE, 
                            ( nLength + 1 ) * sizeof( TCHAR ));
    if (hglbCopy == NULL) 
    { 
        CloseClipboard();
        AfxMessageBox( L"Cannot Copy" );
        return; 
    } 
    // Lock the handle and copy the text to the buffer. 
    LPTSTR  lptstrCopy; 
    lptstrCopy = ( LPTSTR ) GlobalLock(hglbCopy); 
    memcpy( lptstrCopy, csItem.GetBuffer( nLength ), nLength * sizeof(TCHAR));
    lptstrCopy[nLength] = (TCHAR) 0;    // null character
    GlobalUnlock( hglbCopy );
    csItem.ReleaseBuffer(0);
    if ( ::SetClipboardData( CF_TEXT, hglbCopy ) == NULL )
    {
        AfxMessageBox( L"Unable to set Clipboard data" );    
        CloseClipboard();
        return;  
    }
    CloseClipboard();
}


/** 
 * 
 * This function Update the copy tool option.
 * 
 * @param       pCmdUI - 
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::OnUpdateEditCopy( CCmdUI* pCmdUI )
{
    if( GetTreeCtrl().GetSelectedItem())
    {
        pCmdUI->Enable( TRUE );
    }
    else
    {
        pCmdUI->Enable( FALSE );
    }
}


/** 
 * 
 * This function is called when an item is double clicked.
 * If the item contains a cpp file name, it will open that file and move to
 * the line number specified.
 * 
 * @param       nFlags - 
 * @param       point  - 
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    CTreeView ::OnLButtonDblClk( nFlags, point );
    HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
    CString csText = GetTreeCtrl().GetItemText( hItem );

    CStringArray lstFileTypes;
    lstFileTypes.Add( L".cpp" );
    lstFileTypes.Add( L".hpp" );
    lstFileTypes.Add( L".c" );
    lstFileTypes.Add( L".h" );

    int nListCount = lstFileTypes.GetCount();
    int nPosEnd = -1;
    for( int nIdx =0; nIdx < nListCount; nIdx++ )
    {
        nPosEnd = csText.Find( lstFileTypes[nIdx] );
        if( -1 != nPosEnd )
        {
            nPosEnd += lstFileTypes[nIdx].GetLength();
            break;
        }
    }

    if( -1 == nPosEnd )
    {
        return;
    }

    int nPosFirst = csText.Find( L":\\" );
    if( -1 == nPosFirst )
    {
        return;
    }
    nPosFirst -= 1;
    int nCount = nPosEnd - nPosFirst;
    CString csFileName = csText.Mid( nPosFirst, nCount );
    // Extract Line Number.
    CString csLineNum = csText.Mid( nPosEnd );
    csLineNum.TrimLeft( L"(" );
    int nPosLine = csLineNum.Find( L")" );
    csLineNum = csLineNum.Left( nPosLine );
    csLineNum.TrimRight();
    csLineNum.TrimLeft();
    int nLineNum = _ttoi( csLineNum );// Get Line number.
    CFileStatus ObjStatus;
    if( !CStdioFile::GetStatus( csFileName, ObjStatus ))
    {
        if( !FindInNewPath( csFileName ))
        {
            if( IDOK != AfxMessageBox( L"File Not Found, Click OK to Browse", MB_OKCANCEL ))
            {
                return;
            }
            CString csNewPath;
            CString csFileType = L"cpp file (*.cpp)|*.cpp||";
            CFileDialog ObjFileDlg( TRUE, NULL, NULL, OFN_FILEMUSTEXIST, csFileType );
            if( IDOK == ObjFileDlg.DoModal())
            {
                csNewPath = ObjFileDlg.GetPathName();
            }
            else
            {
                return;
            }
            ExtractNewPath( csFileName, csNewPath );
            csFileName = csNewPath;
        }        
    }
    AfxGetMainWnd()->ShowWindow( SW_MINIMIZE );

	OpenEditor( csFileName, nLineNum );

//    HINSTANCE hInst = ShellExecute( NULL, NULL, csFileName, 0, 0, SW_SHOWDEFAULT );
//    POINT WindowPoint;
//    WindowPoint.x = GetSystemMetrics( SM_CXSCREEN ) / 2;
//    WindowPoint.y = GetSystemMetrics( SM_CYSCREEN ) / 2;
//    CWnd* pWnd = WindowFromPoint( WindowPoint );
//    
//    // Ensure the Window
//    if( 0 == pWnd )
//    {
//        AfxMessageBox( L"Couldn't get Window Handle " );
//        return;
//    }
//    CWnd* pParent = pWnd->GetTopLevelParent();
//    CString csWndText;
//    if( 0 != pParent )
//    {
//        pParent->GetWindowText( csWndText );
//        pParent->ShowWindow( SW_SHOWMAXIMIZED );
//    }
//    else
//    {
//        pWnd->GetWindowText( csWndText );
//    }
//    // Extract the cpp file from title
//    CString csFileNameToSearch = csWndText;
//    csFileNameToSearch.TrimRight( "]" );
//    int nPos = csFileNameToSearch.ReverseFind( '\\' );
//    int nPosFile = csFileNameToSearch.ReverseFind( '[' );
//    if( nPosFile > nPos )
//    {
//        nPos = nPosFile;
//    }
//    csFileNameToSearch = csFileNameToSearch.Mid( nPos + 1 );
//    if( -1 == csFileName.Find( csFileNameToSearch ))// Ensure the Window
//    {
//        AfxMessageBox( "Unable to Find Line Number" );
//        return;
//    }
//    // Method1 - Set Position to Home ( Ctrl + Home ) and Move to Line Number.
////    keybd_event( VK_CONTROL, 0, 0, 0 );
////    keybd_event( VK_HOME, 0, 0, 0 );
////    keybd_event( VK_HOME, 0, KEYEVENTF_KEYUP, 0 );
////    keybd_event( VK_CONTROL, 0,  KEYEVENTF_KEYUP, 0 );
//    // Wait for Some time to process Keybd_event
//    // Sleep( 1000 );
//    // Move to Line number
////    for( int nIdx = 1; nIdx < nLineNum; ++nIdx )
////    {
////        //pWnd->PostMessage( WM_KEYDOWN, VK_DOWN, 1 );
////        keybd_event( VK_DOWN, 0, 0, 0 );
////        keybd_event( VK_DOWN, 0, KEYEVENTF_KEYUP, 0 );
////    }
//
//    // Method2 - Get Goto Window ( Ctrl + G ) and Enter Line Number.
//    keybd_event( VK_CONTROL, 0, 0, 0 );
////     keybd_event( VkKeyScan( 'G' ), 0, 0, 0 );
////     keybd_event( VkKeyScan( 'G' ), 0, KEYEVENTF_KEYUP, 0 );
//
//
//    keybd_event( 'G', 0, 0, 0 );
//    keybd_event( 'G', 0, KEYEVENTF_KEYUP, 0 );
//
//    keybd_event( VK_CONTROL, 0,  KEYEVENTF_KEYUP, 0 );
//    for( int nIdxStr = 0; nIdxStr < csLineNum.GetLength(); ++nIdxStr )
//    {
//        keybd_event( VkKeyScan( csLineNum[nIdxStr] ), 0, 0, 0 );
//        keybd_event( VkKeyScan( csLineNum[nIdxStr] ), 0, KEYEVENTF_KEYUP, 0 );
//    }
//    keybd_event( VK_RETURN, 0, 0, 0 );
//    keybd_event( VK_RETURN, 0, KEYEVENTF_KEYUP, 0 );
//    // Close GoTo Window( Alt +F4 )
//    keybd_event( VK_MENU, 0, 0, 0 );
//    keybd_event( VK_F4, 0, 0, 0 );
//    keybd_event( VK_F4, 0, KEYEVENTF_KEYUP, 0 );
//    keybd_event( VK_MENU, 0, KEYEVENTF_KEYUP, 0 );
}


/** 
 * 
 * This function will find file in the new Directory. If it finds
 * It return true and csPath_io contains new Path
 * 
 * @param       csPath_io - 
 * @return      bool      - True if success else false
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool CDumpTreeView::FindInNewPath( CString& csPath_io )
{
    if( m_csNewPath.IsEmpty() || m_csPathToReplace.IsEmpty())
    {
        return false;
    }
    if( -1 == csPath_io.Find( m_csPathToReplace ))
    {
        return false;
    }
    csPath_io.Replace( m_csPathToReplace, m_csNewPath );
    CFileStatus ObjStatus;
    if( !CStdioFile::GetStatus( csPath_io, ObjStatus ))
    {
        return false;
    }
    return true;
}


/** 
 * 
 * This function will find the new path and set the path to replace
 * and new path member variables.
 * 
 * @param       csPath_i - 
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::ExtractNewPath( const CString& csPathFirst_i, 
                                    const CString& csPathSecond_i )
{
    CString csFirstPath = csPathFirst_i;
    CString csSecondPath = csPathSecond_i;
    CString csTokenOfFirst;
    CString csTokenOfSecond;
    // Remove filename(.cpp ) from two paths
    int nPoscpp = csFirstPath.ReverseFind( '\\' );
    csFirstPath = csFirstPath.Left( nPoscpp );
    nPoscpp = csSecondPath.ReverseFind( '\\' );
    csSecondPath = csSecondPath.Left( nPoscpp );
    while( csTokenOfFirst == csTokenOfSecond )
    {
        int nPos1 = csFirstPath.ReverseFind( '\\' );
        if( -1 == nPos1 )
        {
            break;
        }
        csTokenOfFirst = csFirstPath.Mid( nPos1 + 1 );
        int nPos2 = csSecondPath.ReverseFind( '\\' );
        if( -1 == nPos2 )
        {
            break;
        }
        csTokenOfSecond = csSecondPath.Mid( nPos2 + 1 );
        if( csTokenOfFirst != csTokenOfSecond )
        {
            break;
        }
        csFirstPath = csFirstPath.Left( nPos1 );
        csSecondPath = csSecondPath.Left( nPos2 );
    }
    m_csNewPath = csSecondPath;
    m_csPathToReplace = csFirstPath;
}


/** 
 * 
 * This function is used to reload the file
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::OnEditReload()
{
    LoadFile();
}


/** 
 * 
 * This function is used to filter the parent Items containing the given string.
 * It removes all parent Items , which does not contains the given string in it's children.
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::OnEditFilter() 
{
    FilterDialog ObjFilterDlg;
    HTREEITEM hItemSelected = GetTreeCtrl().GetSelectedItem();
    if( 0 != hItemSelected )
    {
        ObjFilterDlg.SetFilterString( GetTreeCtrl().GetItemText( hItemSelected ));
    }
    if( IDOK != ObjFilterDlg.DoModal())
    {
        return;
    }
    CString csFilterString = ObjFilterDlg.GetFilterString();
    if( csFilterString.IsEmpty())
    {
        return;
    }
    HTREEITEM hItem = GetTreeCtrl().GetRootItem();
    csFilterString.MakeLower();
    while( hItem )
    {
        if( FindChildInArray( hItem, csFilterString ))
        {
            hItem = GetTreeCtrl().GetNextSiblingItem( hItem );
            continue;
        }
        CHILDARRAY* pArray = ( CHILDARRAY* ) GetTreeCtrl().GetItemData( hItem );
        delete pArray;
        HTREEITEM hItemToDelete = hItem;
        hItem = GetTreeCtrl().GetNextSiblingItem( hItem );
        GetTreeCtrl().DeleteItem( hItemToDelete );
    }
}


/** 
 * 
 * This function is used to populate context menu when an item is right clicked
 * 
 * @param       nFlags - 
 * @param       point  - 
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeView::OnRButtonDown(UINT nFlags, CPoint point) 
{
    CTreeView ::OnRButtonDown(nFlags, point);
    HTREEITEM hItemSelected = GetTreeCtrl().GetSelectedItem();
    if( NULL == hItemSelected )
    {
        return;
    }

	

	HTREEITEM hChild = GetTreeCtrl().GetChildItem( hItemSelected );
	DWORD dwExcludeMenuStyle = 0;
	if( hChild )
	{
		dwExcludeMenuStyle = MF_GRAYED;
	}

    CMenu ObjMenu;
    ObjMenu.CreatePopupMenu();
    ObjMenu.InsertMenu( MF_BYPOSITION, MF_STRING, ID_EDIT_COPY, L"Copy\tCtrl+C" );	
	ObjMenu.InsertMenu( MF_BYPOSITION, dwExcludeMenuStyle|MF_STRING, ID_EDIT_EXCLUDE, L"Exclude" );
    ObjMenu.InsertMenu( MF_BYPOSITION, MF_STRING, ID_EDIT_FILTER, L"Filter Criteria" );
	
    ClientToScreen( &point );
    ObjMenu.TrackPopupMenu( TPM_LEFTALIGN , point.x, point.y, this );
}