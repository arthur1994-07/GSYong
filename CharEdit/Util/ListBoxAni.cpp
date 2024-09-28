// ListBoxAni.cpp : implementation file
//

#include "stdafx.h"
#include "../charedit.h"
#include "ListBoxAni.h"

#include "../Properties/AnimPage.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CListBoxAni

CListBoxAni::CListBoxAni()
: m_dwIDRPopup(-1)
, m_dwNotifyMsg(-1)
{
}

CListBoxAni::~CListBoxAni()
{
}


BEGIN_MESSAGE_MAP(CListBoxAni, CListBox)
	//{{AFX_MSG_MAP(CListBoxAni)
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxAni message handlers

void CListBoxAni::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CMenu Menu;
	int pMenuID = 0;

	DWORD SelectionMade;
	VERIFY ( Menu.LoadMenu ( m_dwIDRPopup ) );

	CMenu* pPopup = Menu.GetSubMenu ( pMenuID );
	ASSERT ( pPopup != NULL );

	POINT pp;
	GetCursorPos (&pp);
	SelectionMade = pPopup->TrackPopupMenu (
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
		pp.x, pp.y, this );

	pPopup->DestroyMenu ();

	CWnd *pWnd = this->GetParent();
	pWnd->SendMessage ( m_dwNotifyMsg, SelectionMade, 0L );

	CListBox::OnRButtonUp(nFlags, point);
}
