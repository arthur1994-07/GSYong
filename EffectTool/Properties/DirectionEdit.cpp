// DirectionEdit.cpp : ���� �����Դϴ�.
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

// DirectionEdit ��ȭ �����Դϴ�.

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


// DirectionEdit �޽��� ó�����Դϴ�.

BOOL DirectionEdit::OnInitDialog()
{
	assert(m_pvDirection);

	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_stDirection.SubclassDlgItem ( IDC_STATIC_POS, this );


	UpdateContrl ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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

	//	Note : �� �ǵ����� �ֵ��� �ϱ� ����
	//
	m_fOldScale = m_fScale;
	m_pvOldDirection = *m_pvDirection;
}

void DirectionEdit::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_pSheetTab->SetActivePage ( m_dwOldPageIndex );
}

void DirectionEdit::OnBnClickedButtonCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_fScale = m_fOldScale;
	*m_pvDirection = m_pvOldDirection;
	m_pSheetTab->SetActivePage ( m_dwOldPageIndex );
}

void DirectionEdit::OnEnChangeEditX()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CPropertyPage::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CPropertyPage::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CPropertyPage::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
