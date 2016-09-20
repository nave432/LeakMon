


#include "stdafx.h"
#include "DumpTree.h"

#include "DumpTreeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CDumpTreeDoc, CDocument)

BEGIN_MESSAGE_MAP(CDumpTreeDoc, CDocument)
//{{AFX_MSG_MAP(CDumpTreeDoc)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


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
CDumpTreeDoc::CDumpTreeDoc()
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
CDumpTreeDoc::~CDumpTreeDoc()
{
}


/** 
 * OnNewDocument
 * 
 * 
 * @param       Nil
 * @return      BOOL - 
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
BOOL CDumpTreeDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;
    return TRUE;
}


/** 
 * 
 * Serialize function
 * 
 * @param       ar   - 
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CDumpTreeDoc::Serialize( CArchive& ar )
{
    if (ar.IsStoring())
    {
    }
    else
    {
    }
}


#ifdef _DEBUG
void CDumpTreeDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CDumpTreeDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

BOOL CDumpTreeDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	/*if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;*/

	m_csFileName = lpszPathName;
	return TRUE;
}


CString CDumpTreeDoc::GetFileName()
{
	return m_csFileName;
}