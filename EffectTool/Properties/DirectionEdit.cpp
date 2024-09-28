// DirectionEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"
#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "EffsheetWithTab.h"
#include "DirectionEdit.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// DirectionEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(DirectionEdit, CPropertyPage)
DirectionEdit::DirectionEdit()
	: CPropertyPage(DirectionEdit::IDD)
{
	m_dwOldPageIndex = 0;
	m_fScale = 1.f;

	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( FALSE );
}

DirectionEdit::~DirectionEdit()
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( TRUE );
}

void DirectionEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCROLLBAR_Z, m_ZScroll);
	DDX_Control(pDX, IDC_SCROLLBAR_Y, m_YScroll);
	DDX_Control(pDX, IDC_SCROLLBAR_X, m_XScroll);
}


BEGIN_MESSAGE_MAP(DirectionEdit, CPropertyPage)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_OK2, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_EN_CHANGE(IDC_EDIT_X, OnEnChangeEditX)
	ON_EN_CHANGE(IDC_EDIT_Y, OnEnChangeEditY)
	ON_EN_CHANGE(IDC_EDIT_Z, OnEnChangeEditZ)
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

	if ( m_fScale < 1.f )	m_fScale = 1.f;

	m_ToScale = 400.0f;
	m_ToScale /= m_fScale;

	m_XScroll.SetScrollRange ( 0, +800, TRUE );
	m_YScroll.SetScrollRange ( 0, +800, TRUE );
	m_ZScroll.SetScrollRange ( 0, +800, TRUE );

	m_XScroll.SetScrollPos ( int(400+m_pvDirection->x*m_ToScale), TRUE );
	m_YScroll.SetScrollPos ( int(400-m_pvDirection->y*m_ToScale), TRUE );
	m_ZScroll.SetScrollPos ( int(400-m_pvDirection->z*m_ToScale), TRUE );

	CString Str;
	Str.Format ( "%3.2f", m_pvDirection->x );
	GetDlgItem(IDC_EDIT_X)->SetWindowText ( Str );

	Str.Format ( "%3.2f", m_pvDirection->y );
	GetDlgItem(IDC_EDIT_Y)->SetWindowText ( Str );

	Str.Format ( "%3.2f", m_pvDirection->z );
	GetDlgItem(IDC_EDIT_Z)->SetWindowText ( Str );

	//	Note : 값 되돌릴수 있도록 하기 위해
	//
	m_fOldScale = m_fScale;
	m_pvOldDirection = *m_pvDirection;
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

	m_stDirection.m_vtCreatePos = *m_pvDirection;
	m_stDirection.UpdateContrl ( m_fScale );
	m_stDirection.RedrawWindow ();


	CString Str;
	Str.Format ( "%3.2f", m_pvDirection->x );
	GetDlgItem(IDC_EDIT_X)->SetWindowText ( Str );

	Str.Format ( "%3.2f", m_pvDirection->y );
	GetDlgItem(IDC_EDIT_Y)->SetWindowText ( Str );

	Str.Format ( "%3.2f", m_pvDirection->z );
	GetDlgItem(IDC_EDIT_Z)->SetWindowText ( Str );


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

	m_stDirection.m_vtCreatePos = *m_pvDirection;
	m_stDirection.RedrawWindow ();

	CString Str;
	Str.Format ( "%3.2f", m_pvDirection->x );
	GetDlgItem(IDC_EDIT_X)->SetWindowText ( Str );

	Str.Format ( "%3.2f", m_pvDirection->y );
	GetDlgItem(IDC_EDIT_Y)->SetWindowText ( Str );

	Str.Format ( "%3.2f", m_pvDirection->z );
	GetDlgItem(IDC_EDIT_Z)->SetWindowText ( Str );


	CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

void DirectionEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_pSheetTab->SetActivePage ( m_dwOldPageIndex );
}

void DirectionEdit::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_fScale = m_fOldScale;
	*m_pvDirection = m_pvOldDirection;
	m_pSheetTab->SetActivePage ( m_dwOldPageIndex );
}

void DirectionEdit::OnEnChangeEditX()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*	pWnd;
	CString strText;

	pWnd = GetDlgItem ( IDC_EDIT_X );
	pWnd->GetWindowText ( strText );
	m_pvDirection->x = (float) atof ( strText.GetString() );

	m_stDirection.m_vtCreatePos = *m_pvDirection;

	if ( m_fScale < 1.f )	m_fScale = 1.f;

	m_ToScale = 400.0f;
	m_ToScale /= m_fScale;

	m_XScroll.SetScrollRange ( 0, +800, TRUE );
	m_YScroll.SetScrollRange ( 0, +800, TRUE );
	m_ZScroll.SetScrollRange ( 0, +800, TRUE );

	m_XScroll.SetScrollPos ( int(400+m_pvDirection->x*m_ToScale), TRUE );
	m_YScroll.SetScrollPos ( int(400-m_pvDirection->y*m_ToScale), TRUE );
	m_ZScroll.SetScrollPos ( int(400-m_pvDirection->z*m_ToScale), TRUE );

	UpdateScreen ();
}

void DirectionEdit::OnEnChangeEditY()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*	pWnd;
	CString strText;

	pWnd = GetDlgItem ( IDC_EDIT_Y );
	pWnd->GetWindowText ( strText );
	m_pvDirection->y = (float) atof ( strText.GetString() );

	m_stDirection.m_vtCreatePos = *m_pvDirection;

	if ( m_fScale < 1.f )	m_fScale = 1.f;

	m_ToScale = 400.0f;
	m_ToScale /= m_fScale;

	m_XScroll.SetScrollRange ( 0, +800, TRUE );
	m_YScroll.SetScrollRange ( 0, +800, TRUE );
	m_ZScroll.SetScrollRange ( 0, +800, TRUE );

	m_XScroll.SetScrollPos ( int(400+m_pvDirection->x*m_ToScale), TRUE );
	m_YScroll.SetScrollPos ( int(400-m_pvDirection->y*m_ToScale), TRUE );
	m_ZScroll.SetScrollPos ( int(400-m_pvDirection->z*m_ToScale), TRUE );

	UpdateScreen ();
}

void DirectionEdit::OnEnChangeEditZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*	pWnd;
	CString strText;

	pWnd = GetDlgItem ( IDC_EDIT_Z );
	pWnd->GetWindowText ( strText );
	m_pvDirection->z = (float) atof ( strText.GetString() );

	
	UpdateScreen ();
}

void DirectionEdit::UpdateScreen()
{
	m_stDirection.m_vtCreatePos = *m_pvDirection;

	if ( m_fScale < 1.f )	m_fScale = 1.f;

	m_ToScale = 400.0f;
	m_ToScale /= m_fScale;

	m_XScroll.SetScrollRange ( 0, +800, TRUE );
	m_YScroll.SetScrollRange ( 0, +800, TRUE );
	m_ZScroll.SetScrollRange ( 0, +800, TRUE );

	m_XScroll.SetScrollPos ( int(400+m_pvDirection->x*m_ToScale), TRUE );
	m_YScroll.SetScrollPos ( int(400-m_pvDirection->y*m_ToScale), TRUE );
	m_ZScroll.SetScrollPos ( int(400-m_pvDirection->z*m_ToScale), TRUE );

	m_stDirection.m_vtCreatePos = *m_pvDirection;
	m_stDirection.UpdateContrl ( m_fScale );
	m_stDirection.RedrawWindow ();
}
