
#include "stdafx.h"
#include "dumptree.h"
#include "FilterDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** 
 * 
 * Constructor
 * 
 * @param       /=NULL/ - 
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
FilterDialog::FilterDialog(CWnd* pParent /*=NULL*/)
	: CDialog(FilterDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(FilterDialog)
	//}}AFX_DATA_INIT
}


/** 
 * 
 * DoDataExchange function
 * 
 * @param       pDX  - 
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void FilterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FilterDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FilterDialog, CDialog)
	//{{AFX_MSG_MAP(FilterDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void FilterDialog::OnOK() 
{
    GetDlgItemText( IDC_EDIT_FILTER, m_csData );
	CDialog::OnOK();
}

BOOL FilterDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetDlgItemText( IDC_EDIT_FILTER, m_csData );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
