
#include "stdafx.h"
#include "../CharEdit.h"
#include "./SheetWithTabAnim.h"
#include "./AnimPage.h"
#include "./MultianimPage.h"
#include "./AnimTabControl.h"

CAnimTabControl::CAnimTabControl(void)
{
	ZeroMemory(m_tabPages, sizeof(CDialog*) * ATC_NUMBEROFPAGE);
	m_tabPages[ATC_ANIMPAGE] = new CAnimPage;
	m_tabPages[ATC_MULTIANIMPAGE] = new CMultianimPage;
}

CAnimTabControl::~CAnimTabControl(void)
{
	for( INT i = 0; i < ATC_NUMBEROFPAGE; ++i )
		SAFE_DELETE(m_tabPages[i]);
}

BEGIN_MESSAGE_MAP(CAnimTabControl, CTabCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CAnimTabControl::Init(void)
{
	m_tabCurrent = ATC_ANIMPAGE;

	INT nTabDlgID[] = { IDD_DIALOG_ANIM, IDD_DIALOG_MULTIANIM };

	for ( INT i = 0; i < ATC_NUMBEROFPAGE; ++i )
	{
		if ( m_tabPages[i] )
		{
			m_tabPages[i]->DestroyWindow();
			m_tabPages[i]->Create( nTabDlgID[i], this );
			m_tabPages[i]->ShowWindow(SW_HIDE);
		}
	}

	((CAnimPage*)m_tabPages[ATC_ANIMPAGE])->SetSheetTab(m_pSheetTab);
	((CMultianimPage*)m_tabPages[ATC_MULTIANIMPAGE])->SetSheetTab(m_pSheetTab);
	
	m_tabPages[m_tabCurrent]->ShowWindow(SW_SHOW);
	m_tabPages[m_tabCurrent]->SetFocus();
	SetRectangle();
}

void CAnimTabControl::SetRectangle(void)
{
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;

	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);

	nX=itemRect.left;
	nY=itemRect.bottom+1;
	nXc=tabRect.right-itemRect.left-1;
	nYc=tabRect.bottom-nY-1; 

	m_tabPages[ATC_ANIMPAGE]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	for( INT i = 1; i < ATC_NUMBEROFPAGE; ++i )
	{
		m_tabPages[i]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);
	}
}

void CAnimTabControl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTabCtrl::OnLButtonDown(nFlags, point);

	if(m_tabCurrent != GetCurFocus())
	{
		m_tabPages[m_tabCurrent]->ShowWindow(SW_HIDE);
		m_tabCurrent=GetCurFocus();
		m_tabPages[m_tabCurrent]->ShowWindow(SW_SHOW);
		m_tabPages[m_tabCurrent]->SetFocus();
	}
}

CAnimPage* CAnimTabControl::GetAnimPage(void)
{
	return (CAnimPage*)m_tabPages[ATC_ANIMPAGE];
}

CMultianimPage*	CAnimTabControl::GetMultiAnimPage(void)
{
	return (CMultianimPage*)m_tabPages[ATC_MULTIANIMPAGE];
}