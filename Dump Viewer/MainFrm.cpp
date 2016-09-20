

#include "stdafx.h"
#include "DumpTree.h"

#include "MainFrm.h"
#include "DumpTreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
//{{AFX_MSG_MAP(CMainFrame)
ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};


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
CMainFrame::CMainFrame()
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
CMainFrame::~CMainFrame()
{
}


/** 
 * 
 * OnCreate Function
 * 
 * @param       lpCreateStruct - 
 * @return      int            - 
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
        | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }
    
    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators,
        sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndToolBar);
    
    return 0;
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
BOOL CMainFrame::PreCreateWindow( CREATESTRUCT& cs )
{
    if( !CFrameWnd::PreCreateWindow( cs ))
        return FALSE;
    return TRUE;
}


#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::OnClose() 
{
    CDumpTreeView* pView = ( CDumpTreeView* ) GetActiveView();
    pView->RemoveAllArrays();
	CFrameWnd::OnClose();
}
