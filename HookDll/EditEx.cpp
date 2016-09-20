// EditEx.cpp : implementation file
//

#include "stdafx.h"
#include "HookDll.h"
#include "EditEx.h"


// EditEx

IMPLEMENT_DYNAMIC(EditEx, CEdit)

EditEx::EditEx(CWnd* pWndMsg):
m_pMsgWnd( pWndMsg )
{
}

EditEx::~EditEx()
{
}


BEGIN_MESSAGE_MAP(EditEx, CEdit)
END_MESSAGE_MAP()



// EditEx message handlers



LRESULT EditEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( WM_KILLFOCUS == message)
	{
		m_pMsgWnd->SendMessage( WM_EDIT_MSG, 0, 0 );
	}
	else if( WM_KEYDOWN == message && VK_RETURN == wParam )
	{
		m_pMsgWnd->SendMessage( WM_EDIT_MSG, 0, 0 );
	}
	return CEdit::WindowProc(message, wParam, lParam);
}
