// EffLiftTimeBar.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"
#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "EffLiftTimeBar.h"


// CEffLiftTimeBar ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CEffLiftTimeBar, CDialog)
CEffLiftTimeBar::CEffLiftTimeBar(CWnd* pParent /*=NULL*/)
	: CDialog(CEffLiftTimeBar::IDD, pParent),
	m_nTimer(0),
	m_fScale(0.005f)
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( FALSE );
}

CEffLiftTimeBar::~CEffLiftTimeBar()
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( TRUE );
}

void CEffLiftTimeBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_STATIC_EFFTIMELINE, m_stEffTimeLine); // --> SubclassDlgItem
	DDX_Control(pDX, IDC_SCROLLBAR_H, m_HScrollBar);
	DDX_Control(pDX, IDC_SCROLLBAR_V, m_VScrollBar);
}


BEGIN_MESSAGE_MAP(CEffLiftTimeBar, CDialog)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_TIMER()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SCALE, OnDeltaposSpinScale)
END_MESSAGE_MAP()


// CEffLiftTimeBar �޽��� ó�����Դϴ�.

void CEffLiftTimeBar::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	//	Note : ��Ʈ�ѵ��� Dlg�� ����� ����� ���� ������¡�� ������.
	//
	CRect rcWnd;
	CRect rcControl;

	CWnd *pWnd;

	GetWindowRect ( rcWnd );
	ScreenToClient ( &rcWnd );

	int nScrollH, nScrollH_top;
	pWnd = GetDlgItem ( IDC_SCROLLBAR_H );
	if ( pWnd )
	{
		pWnd->GetWindowRect ( &rcControl );
		ScreenToClient ( &rcControl );
		nScrollH = rcControl.bottom - rcControl.top;

		rcControl.bottom = rcWnd.bottom - 2;
		rcControl.top = rcControl.bottom - nScrollH;
		rcControl.right = rcWnd.right-2;

		pWnd->MoveWindow ( &rcControl );
	}
	nScrollH_top = rcControl.top;

	pWnd = GetDlgItem ( IDC_SCROLLBAR_V );
	if ( pWnd )
	{
		pWnd->GetWindowRect ( &rcControl );
		ScreenToClient ( &rcControl );

		rcControl.top = 2;
		rcControl.bottom = nScrollH_top-1;
		pWnd->MoveWindow ( &rcControl );
	}

	pWnd = GetDlgItem ( IDC_STATIC_EFFTIMELINE );
	if ( pWnd )
	{
		pWnd->GetWindowRect ( &rcControl );
		ScreenToClient ( &rcControl );

		rcControl.top = 2;
		rcControl.bottom = nScrollH_top-1;
		rcControl.right = rcWnd.right-2;
		pWnd->MoveWindow ( &rcControl );
	}
}

BOOL CEffLiftTimeBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_stEffTimeLine.SubclassDlgItem ( IDC_STATIC_EFFTIMELINE, this );

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_VScrollBar.SetScrollRange ( 0, 200, TRUE );
	m_HScrollBar.SetScrollRange ( 0, 200, TRUE );

	m_VScrollBar.SetScrollPos ( 200, TRUE );
	m_HScrollBar.SetScrollPos ( 0, TRUE );

	m_nTimer = SetTimer ( _TIMER_ID, 60, NULL );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CEffLiftTimeBar::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CDialog::PostNcDestroy();
}

void CEffLiftTimeBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	switch ( nSBCode )
	{
	case SB_LINELEFT:
		{
			int Pos = m_HScrollBar.GetScrollPos ();
			m_HScrollBar.SetScrollPos ( Pos-10, TRUE );
		}
		break;

	case SB_LINERIGHT:
		{
			int Pos = m_HScrollBar.GetScrollPos ();
			m_HScrollBar.SetScrollPos ( Pos+10, TRUE );
		}
		break;

	case SB_PAGELEFT:
		{
			int Pos = m_HScrollBar.GetScrollPos ();
			m_HScrollBar.SetScrollPos ( Pos-20, TRUE );
		}
		break;

	case SB_PAGERIGHT:
		{
			int Pos = m_HScrollBar.GetScrollPos ();
			m_HScrollBar.SetScrollPos ( Pos+20, TRUE );
		}
		break;

	case SB_THUMBTRACK:
		{
			m_HScrollBar.SetScrollPos ( nPos, TRUE );
		}
		break;

	case SB_THUMBPOSITION:
		{
			m_HScrollBar.SetScrollPos ( nPos, TRUE );
		}
		break;
	};
	
	m_stEffTimeLine.SetScrollPosX ( m_HScrollBar.GetScrollPos () );

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CEffLiftTimeBar::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	switch ( nSBCode )
	{
	case SB_LINEUP:
		{
			int Pos = m_VScrollBar.GetScrollPos ();
			m_VScrollBar.SetScrollPos ( Pos-10, TRUE );
		}
		break;

	case SB_LINEDOWN:
		{
			int Pos = m_VScrollBar.GetScrollPos ();
			m_VScrollBar.SetScrollPos ( Pos+10, TRUE );
		}
		break;

	case SB_PAGEUP:
		{
			int Pos = m_VScrollBar.GetScrollPos ();
			m_VScrollBar.SetScrollPos ( Pos-20, TRUE );
		}
		break;

	case SB_PAGEDOWN:
		{
			int Pos = m_VScrollBar.GetScrollPos ();
			m_VScrollBar.SetScrollPos ( Pos+20, TRUE );
		}
		break;

	case SB_THUMBTRACK:
		{
			m_VScrollBar.SetScrollPos ( nPos, TRUE );
		}
		break;

	//case SB_THUMBPOSITION:
	//	{
	//		m_VScrollBar.SetScrollPos ( nPos, TRUE );
	//	}
		break;
	};

	int SizeX, SizeY;
	m_VScrollBar.GetScrollRange ( &SizeX, &SizeY );
	m_stEffTimeLine.SetScrollPosY ( SizeY - m_VScrollBar.GetScrollPos () );

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CEffLiftTimeBar::ListingTimeLine ( EFF_PROPERTY* pParentProp )
{
	TIMELINE tLine;
	EFF_PROPERTY* pCurProp = pParentProp;
	while ( pCurProp )
	{
		tLine.IndexID = (DWORD) pCurProp;
		tLine.x1 = pCurProp->m_fGBeginTime;
		tLine.x2 = pCurProp->m_fGBeginTime+pCurProp->m_fGLifeTime;

		m_stEffTimeLine.AddTimeLine ( tLine );

		if ( pCurProp->m_pChild )
		{
			ListingTimeLine ( pCurProp->m_pChild );
		}

		pCurProp = pCurProp->m_pSibling;
	}

	return TRUE;
}

BOOL CEffLiftTimeBar::ListingTimeLine ( EFF_PROPGROUP *pEffPropGroup )
{
	m_stEffTimeLine.DelAllLines ();
	if ( !pEffPropGroup ) return FALSE;

	ListingTimeLine ( pEffPropGroup->m_pPropRoot );

	return TRUE;
}

void CEffLiftTimeBar::OnTimer(UINT nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	switch ( nIDEvent )
	{
	case _TIMER_ID:
		if ( DxEffGroupPlayer::GetInstance().GetAmount() > 0 )
		{
			m_stEffTimeLine.m_fTimeLine = DxEffGroupPlayer::GetInstance().GetTime ();
			m_stEffTimeLine.RedrawWindow ();
		}
		break;
	};

	CDialog::OnTimer(nIDEvent);
}

void CEffLiftTimeBar::OnDeltaposSpinScale(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	UpdateData( TRUE );

	if( pNMUpDown->iDelta > 0 )
		m_fScale *= 0.5f;
	else
		m_fScale *= 2.f;

	m_stEffTimeLine.SetScale ( m_fScale, 0.5f );

	UpdateData( FALSE );

	*pResult = 0;
}
