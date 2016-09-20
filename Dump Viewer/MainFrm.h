
#if !defined(AFX_MAINFRM_H__INCLUDED_)
#define AFX_MAINFRM_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CFrameWnd
{
public:
	CMainFrame();
  
protected: // create from serialization only

    DECLARE_DYNCREATE(CMainFrame)
        // Attributes
public:
    
    // Operations
public:
    
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMainFrame)
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //}}AFX_VIRTUAL
    
    // Implementation
public:

    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    
protected:  // control bar embedded members

    CStatusBar  m_wndStatusBar;
    CToolBar    m_wndToolBar;
    
    // Generated message map functions
protected:

    //{{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_MAINFRM_H__INCLUDED_)
