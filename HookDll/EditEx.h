#pragma once

#define WM_EDIT_MSG WM_USER +  2000
// EditEx

class EditEx : public CEdit
{
	DECLARE_DYNAMIC(EditEx)

public:
	EditEx(CWnd* pWndMsg );
	virtual ~EditEx();

protected:
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	CWnd* m_pMsgWnd;
};


