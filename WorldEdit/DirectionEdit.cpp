//#include "..\effecttool\directionedit.h"
// DirectionEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "sheetWithTab.h"
#include "DirectionEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// DirectionEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(DirectionEdit, CPropertyPage)
DirectionEdit::DirectionEdit()
	: CPropertyPage(DirectionEdit::IDD)
{
	m_dwOldPageIndex = 0;
	m_fScale = 1.0f;

	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( FALSE );
}

DirectionEdit::~DirectionEdit()
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( TRUE );
}

void DirectionEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_STATIC_POS, m_stDirection);
	DDX_Control(pDX, IDC_SCROLLBAR_Z, m_ZScroll);
	DDX_Control(pDX, IDC_SCROLLBAR_Y, m_YScroll);
	DDX_Control(pDX, IDC_SCROLLBAR_X, m_XScroll);
}


BEGIN_MESSAGE_MAP(DirectionEdit, CPropertyPage)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_EN_CHANGE(IDC_EDIT_X, &DirectionEdit::OnEnChangeEditX)
	ON_EN_CHANGE(IDC_EDIT_Y, &DirectionEdit::OnEnChangeEditY)
	ON_EN_CHANGE(IDC_EDIT_Z, &DirectionEdit::OnEnChangeEditZ)
END_MESSAGE_MAP()


// DirectionEdit 메시지 처리기입니다.

BOOL DirectionEdit::OnInitDialog()
{
	assert(m_pvDirection);

	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_stDirection.SubclassDlgItem ( IDC_STATIC_POS, this );

	UpdateContrl ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void DirectionEdit::UpdateContrl ()
{
	m_stDirection.m_vtCreatePos = *m_pvDirection;

	m_ToScale = 200.0f;
	m_ToScale *= m_fScale;

	m_XScroll.SetScrollRange ( 0, +800, TRUE );
	m_YScroll.SetScrollRange ( 0, +800, TRUE );
	m_ZScroll.SetScrollRange ( 0, +800, TRUE );

	m_XScroll.SetScrollPos ( int(400+m_pvDirection->x*m_ToScale), TRUE );
	m_YScroll.SetScrollPos ( int(400-m_pvDirection->y*m_ToScale), TRUE );
	m_ZScroll.SetScrollPos ( int(400-m_pvDirection->z*m_ToScale), TRUE );

	CString Str;
	Str.Format ( _T("%3.3f"), m_pvDirection->x );
	GetDlgItem(IDC_EDIT_X)->SetWindowText ( Str );

	Str.Format ( _T("%3.3f"), m_pvDirection->y );
	GetDlgItem(IDC_EDIT_Y)->SetWindowText ( Str );

	Str.Format ( _T("%3.3f"), m_pvDirection->z );
	GetDlgItem(IDC_EDIT_Z)->SetWindowText ( Str );

	//	Note : 값 되돌릴수 있도록 하기 위해
	//
	m_fOldScale = m_fScale;
	m_pvOldDirection = *m_pvDirection;

	// Note : 방향 렌더링을 위함.
	NSDirectionRender::Update( *m_pvDirection );
}

void DirectionEdit::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int nPos = uPos;

	// TODO: Add your message handler code here and/or call default
	switch ( nSBCode )
	{
	case SB_LINELEFT:
		nPos = m_XScroll.GetScrollPos();
		nPos -= 1;
		m_pvDirection->x = (nPos-400) /m_ToScale;
		m_XScroll.SetScrollPos ( nPos, TRUE );
		break;

	case SB_LINERIGHT:
		nPos = m_XScroll.GetScrollPos();
		nPos += 1;
		m_pvDirection->x = (nPos-400) /m_ToScale;
		m_XScroll.SetScrollPos ( nPos, TRUE );
		break;

	case SB_PAGELEFT:
		nPos = m_XScroll.GetScrollPos();
		nPos -= 20;
		m_pvDirection->x = (nPos-400) /m_ToScale;
		m_XScroll.SetScrollPos ( nPos, TRUE );
		break;

	case SB_PAGERIGHT:
		nPos = m_XScroll.GetScrollPos();
		nPos += 20;
		m_pvDirection->x = (nPos-400) /m_ToScale;
		m_XScroll.SetScrollPos ( nPos, TRUE );
		break;

	case SB_THUMBPOSITION:
		m_pvDirection->x = (nPos-400) /m_ToScale;
		m_XScroll.SetScrollPos ( nPos, TRUE );
		break;

	case SB_THUMBTRACK:
		m_pvDirection->x = (nPos-400) /m_ToScale;
		break;
	};

	CString Str;
	Str.Format ( _T("%3.3f"), m_pvDirection->x );
	GetDlgItem(IDC_EDIT_X)->SetWindowText ( Str );

	Str.Format ( _T("%3.3f"), m_pvDirection->y );
	GetDlgItem(IDC_EDIT_Y)->SetWindowText ( Str );

	Str.Format ( _T("%3.3f"), m_pvDirection->z );
	GetDlgItem(IDC_EDIT_Z)->SetWindowText ( Str );

	// 재차 그린다.
	ReDraw();

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void DirectionEdit::OnVScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int nPos = uPos;

	// TODO: Add your message handler code here and/or call default
	if ( pScrollBar == GetDlgItem ( IDC_SCROLLBAR_Z ) )
	{
		switch ( nSBCode )
		{
		case SB_LINEUP:
			nPos = m_ZScroll.GetScrollPos();
			nPos -= 1;
			m_pvDirection->z = -(nPos-400) /m_ToScale;
			m_ZScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_LINEDOWN:
			nPos = m_ZScroll.GetScrollPos();
			nPos += 1;
			m_pvDirection->z = -(nPos-400) /m_ToScale;
			m_ZScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_PAGEUP:
			nPos = m_ZScroll.GetScrollPos();
			nPos -= 20;
			m_pvDirection->z = -(nPos-400) /m_ToScale;
			m_ZScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_PAGEDOWN:
			nPos = m_ZScroll.GetScrollPos();
			nPos += 20;
			m_pvDirection->z = -(nPos-400) /m_ToScale;
			m_ZScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_THUMBPOSITION:
			m_pvDirection->z = -(nPos-400) /m_ToScale;
			m_ZScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_THUMBTRACK:
			m_pvDirection->z = -(nPos-400) /m_ToScale;
			break;
		};
	}
	else if ( pScrollBar == GetDlgItem ( IDC_SCROLLBAR_Y ) )
	{
		switch ( nSBCode )
		{
		case SB_LINEUP:
			nPos = m_YScroll.GetScrollPos();
			nPos -= 1;
			m_pvDirection->y = -(nPos-400) /m_ToScale;
			m_YScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_LINEDOWN:
			nPos = m_YScroll.GetScrollPos();
			nPos += 1;
			m_pvDirection->y = -(nPos-400) /m_ToScale;
			m_YScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_PAGEUP:
			nPos = m_YScroll.GetScrollPos();
			nPos -= 20;
			m_pvDirection->y = -(nPos-400) /m_ToScale;
			m_YScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_PAGEDOWN:
			nPos = m_YScroll.GetScrollPos();
			nPos += 20;
			m_pvDirection->y = -(nPos-400) /m_ToScale;
			m_YScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_THUMBPOSITION:
			m_pvDirection->y = -(nPos-400) /m_ToScale;
			m_YScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_THUMBTRACK:
			m_pvDirection->y = -(nPos-400) /m_ToScale;
			break;
		};
	}

	CString Str;
	Str.Format ( _T("%3.3f"), m_pvDirection->x );
	GetDlgItem(IDC_EDIT_X)->SetWindowText ( Str );

	Str.Format ( _T("%3.3f"), m_pvDirection->y );
	GetDlgItem(IDC_EDIT_Y)->SetWindowText ( Str );

	Str.Format ( _T("%3.3f"), m_pvDirection->z );
	GetDlgItem(IDC_EDIT_Z)->SetWindowText ( Str );

	// 재차 그린다.
	ReDraw();

	CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

void DirectionEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	NSDirectionRender::SetActive( FALSE );

	m_pSheetTab->SetActivePage ( m_dwOldPageIndex );
}

void DirectionEdit::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	NSDirectionRender::SetActive( FALSE );

	m_fScale = m_fOldScale;
	*m_pvDirection = m_pvOldDirection;
	m_pSheetTab->SetActivePage ( m_dwOldPageIndex );
}

BOOL DirectionEdit::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	NSDirectionRender::SetActive( TRUE );

	return CPropertyPage::OnSetActive();
}

BOOL DirectionEdit::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	NSDirectionRender::SetActive( FALSE );

	return CPropertyPage::OnKillActive();
}

void DirectionEdit::OnEnChangeEditX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString Str;
	GetDlgItem(IDC_EDIT_X)->GetWindowText( Str );
	m_pvDirection->x = (float) _tstof( Str.GetString() );

	// 재차 그린다.
	ReDraw();
}

void DirectionEdit::OnEnChangeEditY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString Str;
	GetDlgItem(IDC_EDIT_Y)->GetWindowText( Str );
	m_pvDirection->y = (float) _tstof( Str.GetString() );

	// 재차 그린다.
	ReDraw();
}

void DirectionEdit::OnEnChangeEditZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString Str;
	GetDlgItem(IDC_EDIT_Z)->GetWindowText( Str );
	m_pvDirection->z = (float) _tstof( Str.GetString() );

	// 재차 그린다.
	ReDraw();
}

void DirectionEdit::ReDraw()
{
	// m_stDirection 에 그린다.
	m_stDirection.m_vtCreatePos = *m_pvDirection;
	m_stDirection.RedrawWindow ();

	// Note : 방향 렌더링을 위함.
	NSDirectionRender::Update( *m_pvDirection );
}

#include "../EngineLib/DxTools/EditMeshs.h"

namespace NSDirectionRender
{
	BOOL		g_bActiveRender(FALSE);
	D3DXVECTOR3	g_vDir;

	void SetActive( BOOL bActive )
	{
		g_bActiveRender = bActive;
	}

	void Update( const D3DXVECTOR3& vDir )
	{
		g_vDir = vDir;
	}

	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vLookat )
	{
		if( !g_bActiveRender )
			return;

		DWORD dwFogEnable;
		pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		{
			D3DXVECTOR3	vPos1 = vLookat + g_vDir * 10000.f;
			D3DXVECTOR3	vPos2 = vLookat - g_vDir * 10000.f;
			EDITMESHS::RENDERLINE( pd3dDevice, vPos1, vPos2, 0xffffffff );
		}
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );
	}
};
