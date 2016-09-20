

#if !defined(AFX_DUMPTREEDOC_H__INCLUDED_)
#define AFX_DUMPTREEDOC_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/** 
 * 
 * Document Class
 * 
 * @param       Nil
 * @return      class - 
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
class CDumpTreeDoc : public CDocument
{
protected: // create from serialization only

    CDumpTreeDoc();
    DECLARE_DYNCREATE(CDumpTreeDoc)
        
        // Attributes
public:
    
    // Operations
public:
    
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDumpTreeDoc)
public:

    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
	CString GetFileName();
    //}}AFX_VIRTUAL
    
    // Implementation
public:

    virtual ~CDumpTreeDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    
protected:
    
    // Generated message map functions
protected:
    
    //{{AFX_MSG(CDumpTreeDoc)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

	CString m_csFileName;
};

#endif // !defined(AFX_DUMPTREEDOC_H__INCLUDED_)
