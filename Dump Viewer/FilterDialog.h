

#if !defined(AFX_FILTERDIALOG_H__INCLUDED_)
#define AFX_FILTERDIALOG_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class FilterDialog : public CDialog
{
// Construction
public:

	FilterDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(FilterDialog)
	enum { IDD = IDD_DIALOG_FILTER };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FilterDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(FilterDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

    CString GetFilterString()
    {
        return m_csData;
    }

    void SetFilterString( const CString& csString_i )
    {
        m_csData = csString_i;
    }

private:

    CString m_csData;
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_FILTERDIALOG_H__INCLUDED_)
