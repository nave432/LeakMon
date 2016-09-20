

#include "DumpTreeDoc.h"
#if !defined(AFX_DUMPTREEVIEW_H__INCLUDED_)
#define AFX_DUMPTREEVIEW_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDumpTreeView : public CTreeView
{
protected: // create from serialization only
    
    CDumpTreeView();
    DECLARE_DYNCREATE(CDumpTreeView)
        
        // Attributes
public:
    
    CDumpTreeDoc* GetDocument();
    
    // Operations
public:
    
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDumpTreeView)
public:

    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:

    virtual void OnInitialUpdate(); // called first time after construct
    //}}AFX_VIRTUAL
    
    // Implementation
public:

    virtual ~CDumpTreeView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    
protected:
    
    // Generated message map functions
protected:
    
    //{{AFX_MSG(CDumpTreeView)
    afx_msg void OnFileOpen();
    afx_msg void OnFileSaveAs();
    afx_msg void OnRemoveItem();
    afx_msg void OnFindNext();
    afx_msg void OnFind();
    afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnEditReload();
	afx_msg void OnEditFilter();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
    
    afx_msg LRESULT OnFindDialogMessage(WPARAM wParam, LPARAM lParam);
    
    DECLARE_MESSAGE_MAP()

public:

    void RemoveAllArrays();
        
private:
    
    bool FindStringInChild( const HTREEITEM& hItem_i, const CString& csSel_i, 
                            const HTREEITEM& hSelChild_i, bool bDownWard_i = true );
    bool FindChildInArray( const HTREEITEM& hItem_i, const CString& csSearch_i );
    void WriteChildItems( CStdioFile& ObjFile_io, const HTREEITEM& hItem_i );
    void ExpandItem( const HTREEITEM& hItem_i );
    void LoadFile();
    void ExtractNewPath( const CString& csPathFirst_i, 
                         const CString& csPathSecond_i );
    bool FindInNewPath( CString& csPath_io );

private:
    
    CFindReplaceDialog* m_pFindDlg;
    static const UINT m_FindDialogMessage;
    CString m_csFindString;
    bool m_bSearchDown;
    CString m_csFileName;
    CString m_csNewPath;
    CString m_csPathToReplace;

    CFont m_TreeFont;
    CImageList m_TreeImgs;
};

#ifndef _DEBUG  // debug version in DumpTreeView.cpp
inline CDumpTreeDoc* CDumpTreeView::GetDocument()
{ return (CDumpTreeDoc*)m_pDocument; }
#endif

#define RETURN_ON_FAIL( expression ) \
	hRes = ( expression );        \
	if ( FAILED( hRes ) )         \
	return false;

#endif // !defined(AFX_DUMPTREEVIEW_H__INCLUDED_)
