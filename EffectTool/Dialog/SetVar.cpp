// SetVar.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"
#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "SetVar.h"


// CSetVar ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSetVar, CDialog)
CSetVar::CSetVar(CWnd* pParent /*=NULL*/)
	: CDialog(CSetVar::IDD, pParent)
{
	m_dwFlag = USECHECK | USERADIO | USESTARTSCROLL | USESTARTVARSCROLL | USEENDSCROLL | USEEND4SCROLL;

	m_bUse = FALSE;
	m_iRadio = 0;
	m_fStart = 0.0f;
	m_fStartVar = 0.0f;
	m_fEnd = 0.0f;
	m_fEnd4 = 0.f;

	m_bZeroBase = FALSE;
	m_ToScale = 20.0f;
	m_Zero = 400;

	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( FALSE );
}

CSetVar::~CSetVar()
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( TRUE );
}

void CSetVar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCROLLBAR_START, m_ScrollStart);
	DDX_Control(pDX, IDC_SCROLLBAR_STARTVAR, m_ScrollVar);
	DDX_Control(pDX, IDC_SCROLLBAR_END, m_ScrollEnd);
	DDX_Control(pDX, IDC_SCROLLBAR_END4, m_ScrollEnd4);
}


BEGIN_MESSAGE_MAP(CSetVar, CDialog)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_START_F, OnEnChangeEditStartF)
	ON_EN_CHANGE(IDC_EDIT_STARTVAR_F, OnEnChangeEditStartvarF)
	ON_EN_CHANGE(IDC_EDIT_END_F, OnEnChangeEditEndF)
	ON_BN_CLICKED(IDC_CHECK_USE, OnBnClickedCheckUse)
	ON_EN_CHANGE(IDC_EDIT_END4_F, OnEnChangeEditEnd4F)
	ON_BN_CLICKED(IDC_RADIO_UP, OnBnClickedRadioUp)
	ON_BN_CLICKED(IDC_RADIO_MID, OnBnClickedRadioMid)
	ON_BN_CLICKED(IDC_RADIO_DOWN, OnBnClickedRadioDown)
END_MESSAGE_MAP()


// CSetVar �޽��� ó�����Դϴ�.
BOOL CSetVar::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	if ( m_bZeroBase )
	{
		m_Zero = 0;
	}


	// TODO: Add extra initialization here
	if ( !( m_dwFlag & USECHECK ) )
	{
		GetDlgItem ( IDC_CHECK_USE ) -> ShowWindow ( FALSE );
	}
	if ( !( m_dwFlag & USERADIO ) )
	{
		GetDlgItem ( IDC_STATIC_CENTER ) -> ShowWindow ( FALSE );
		GetDlgItem ( IDC_RADIO_UP ) -> ShowWindow ( FALSE );
		GetDlgItem ( IDC_RADIO_MID ) -> ShowWindow ( FALSE );
	}
	if ( !( m_dwFlag & USEDOWN ) )
	{
		GetDlgItem ( IDC_RADIO_DOWN ) -> ShowWindow ( FALSE );
	}
	if ( !( m_dwFlag & USESTARTSCROLL ) )
	{
		GetDlgItem ( IDC_STATIC_START ) -> ShowWindow ( FALSE );
		GetDlgItem ( IDC_EDIT_START_F ) -> ShowWindow ( FALSE );
		m_ScrollStart.ShowWindow ( FALSE );
	}
	if ( !( m_dwFlag & USESTARTVARSCROLL ) )
	{
		GetDlgItem ( IDC_STATIC_STARTVAR ) -> ShowWindow ( FALSE );
		GetDlgItem ( IDC_EDIT_STARTVAR_F ) -> ShowWindow ( FALSE );
		m_ScrollVar.ShowWindow ( FALSE );
	}
	if ( !( m_dwFlag & USEENDSCROLL ) )
	{
		GetDlgItem ( IDC_STATIC_END ) -> ShowWindow ( FALSE );
		GetDlgItem ( IDC_EDIT_END_F ) -> ShowWindow ( FALSE );
		m_ScrollEnd.ShowWindow ( FALSE );
	}
	if ( !( m_dwFlag & USEEND4SCROLL ) )
	{
		GetDlgItem ( IDC_STATIC_END4 ) -> ShowWindow ( FALSE );
		GetDlgItem ( IDC_EDIT_END4_F ) -> ShowWindow ( FALSE );
		m_ScrollEnd4.ShowWindow ( FALSE );
	}

	CButton *pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_USE );

	if ( m_bUse )	pButton->SetCheck( TRUE );
	else			pButton->SetCheck( FALSE );

	//	Note : ���� ��ư
	//
	if ( m_iRadio == 0 )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_UP );
		pButton->SetCheck ( 1 );

		pButton = (CButton*) GetDlgItem ( IDC_RADIO_MID );
		pButton->SetCheck ( 0 );

		pButton = (CButton*) GetDlgItem ( IDC_RADIO_DOWN );
		pButton->SetCheck ( 0 );
	}
	else if ( m_iRadio == 1 )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_UP );
		pButton->SetCheck ( 0 );

		pButton = (CButton*) GetDlgItem ( IDC_RADIO_MID );
		pButton->SetCheck ( 1 );

		pButton = (CButton*) GetDlgItem ( IDC_RADIO_DOWN );
		pButton->SetCheck ( 0 );
	}
	else if ( m_iRadio == 2 )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_UP );
		pButton->SetCheck ( 0 );

		pButton = (CButton*) GetDlgItem ( IDC_RADIO_MID );
		pButton->SetCheck ( 0 );

		pButton = (CButton*) GetDlgItem ( IDC_RADIO_DOWN );
		pButton->SetCheck ( 1 );
	}

	m_ScrollStart.SetScrollRange ( 0, +800, TRUE );
	m_ScrollVar.SetScrollRange ( 0, +800, TRUE );
	m_ScrollEnd.SetScrollRange ( 0, +800, TRUE );
	m_ScrollEnd4.SetScrollRange ( 0, +800, TRUE );

	m_ScrollStart.SetScrollPos ( int(m_Zero+m_fStart*m_ToScale), TRUE );
	m_ScrollVar.SetScrollPos ( int(m_Zero+m_fStartVar*m_ToScale), TRUE );
	m_ScrollEnd.SetScrollPos ( int(m_Zero+m_fEnd*m_ToScale), TRUE );
	m_ScrollEnd4.SetScrollPos ( int(m_Zero+m_fEnd4*m_ToScale), TRUE );

	CString Str;
	Str.Format ( "%3.3f", m_fStart );
	GetDlgItem ( IDC_EDIT_START_F ) -> SetWindowText ( Str );
	Str.Format ( "%3.3f", m_fStartVar );
	GetDlgItem ( IDC_EDIT_STARTVAR_F ) -> SetWindowText ( Str );
	Str.Format ( "%3.3f", m_fEnd );
	GetDlgItem ( IDC_EDIT_END_F ) -> SetWindowText ( Str );
	Str.Format ( "%3.3f", m_fEnd4 );
	GetDlgItem ( IDC_EDIT_END4_F ) -> SetWindowText ( Str );


	if ( !m_StrTitle.IsEmpty() )		SetWindowText ( m_StrTitle );

	if ( !m_StrUse.IsEmpty() )
		GetDlgItem(IDC_CHECK_USE)->SetWindowText(m_StrUse);

	if ( !m_StrRadio1.IsEmpty() )
		GetDlgItem(IDC_RADIO_UP)->SetWindowText(m_StrRadio1);

	if ( !m_StrRadio2.IsEmpty() )
		GetDlgItem(IDC_RADIO_MID)->SetWindowText(m_StrRadio2);

	if ( !m_StrComm1.IsEmpty() )
		GetDlgItem(IDC_STATIC_START)->SetWindowText(m_StrComm1);

	if ( !m_StrComm2.IsEmpty() )
		GetDlgItem(IDC_STATIC_STARTVAR)->SetWindowText(m_StrComm2);

	if ( !m_StrComm3.IsEmpty() )
		GetDlgItem(IDC_STATIC_END)->SetWindowText(m_StrComm3);

	if ( !m_StrComm4.IsEmpty() )
		GetDlgItem(IDC_STATIC_END4)->SetWindowText(m_StrComm4);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CSetVar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	int iPos = nPos;

	if ( pScrollBar == &m_ScrollStart )
	{
		switch ( nSBCode )
		{
		case SB_LINELEFT:
			iPos = m_ScrollStart.GetScrollPos();
			iPos -= 1;
			m_fStart = (iPos-m_Zero) /m_ToScale;
			m_ScrollStart.SetScrollPos ( iPos, TRUE );
			break;

		case SB_LINERIGHT:
			iPos = m_ScrollStart.GetScrollPos();
			iPos += 1;
			m_fStart = (iPos-m_Zero) /m_ToScale;
			m_ScrollStart.SetScrollPos ( iPos, TRUE );
			break;

		case SB_PAGELEFT:
			iPos = m_ScrollStart.GetScrollPos();
			iPos -= 20;
			m_fStart = (iPos-m_Zero) /m_ToScale;
			m_ScrollStart.SetScrollPos ( iPos, TRUE );
			break;

		case SB_PAGERIGHT:
			iPos = m_ScrollStart.GetScrollPos();
			iPos += 20;
			m_fStart = (iPos-m_Zero) /m_ToScale;
			m_ScrollStart.SetScrollPos ( iPos, TRUE );
			break;

		case SB_THUMBPOSITION:
			m_fStart = (iPos-m_Zero) /m_ToScale;
			m_ScrollStart.SetScrollPos ( iPos, TRUE );
			break;

		case SB_THUMBTRACK:
			m_fStart = (iPos-m_Zero) /m_ToScale;
			break;
		};
	}
	else if ( pScrollBar == &m_ScrollVar )
	{
		switch ( nSBCode )
		{
		case SB_LINELEFT:
			iPos = m_ScrollVar.GetScrollPos();
			iPos -= 1;
			m_fStartVar = (iPos-m_Zero) /m_ToScale;
			m_ScrollVar.SetScrollPos ( iPos, TRUE );
			break;

		case SB_LINERIGHT:
			iPos = m_ScrollVar.GetScrollPos();
			iPos += 1;
			m_fStartVar = (iPos-m_Zero) /m_ToScale;
			m_ScrollVar.SetScrollPos ( iPos, TRUE );
			break;

		case SB_PAGELEFT:
			iPos = m_ScrollVar.GetScrollPos();
			iPos -= 20;
			m_fStartVar = (iPos-m_Zero) /m_ToScale;
			m_ScrollVar.SetScrollPos ( iPos, TRUE );
			break;

		case SB_PAGERIGHT:
			iPos = m_ScrollVar.GetScrollPos();
			iPos += 20;
			m_fStartVar = (iPos-m_Zero) /m_ToScale;
			m_ScrollVar.SetScrollPos ( iPos, TRUE );
			break;

		case SB_THUMBPOSITION:
			m_fStartVar = (iPos-m_Zero) /m_ToScale;
			m_ScrollVar.SetScrollPos ( iPos, TRUE );
			break;

		case SB_THUMBTRACK:
			m_fStartVar = (iPos-m_Zero) /m_ToScale;
			break;
		};
	}
	else if ( pScrollBar == &m_ScrollEnd )
	{
		switch ( nSBCode )
		{
		case SB_LINELEFT:
			iPos = m_ScrollEnd.GetScrollPos();
			iPos -= 1;
			m_fEnd = (iPos-m_Zero) /m_ToScale;
			m_ScrollEnd.SetScrollPos ( iPos, TRUE );
			break;

		case SB_LINERIGHT:
			iPos = m_ScrollEnd.GetScrollPos();
			iPos += 1;
			m_fEnd = (iPos-m_Zero) /m_ToScale;
			m_ScrollEnd.SetScrollPos ( iPos, TRUE );
			break;

		case SB_PAGELEFT:
			iPos = m_ScrollEnd.GetScrollPos();
			iPos -= 20;
			m_fEnd = (iPos-m_Zero) /m_ToScale;
			m_ScrollEnd.SetScrollPos ( iPos, TRUE );
			break;

		case SB_PAGERIGHT:
			iPos = m_ScrollEnd.GetScrollPos();
			iPos += 20;
			m_fEnd = (iPos-m_Zero) /m_ToScale;
			m_ScrollEnd.SetScrollPos ( iPos, TRUE );
			break;

		case SB_THUMBPOSITION:
			m_fEnd = (iPos-m_Zero) /m_ToScale;
			m_ScrollEnd.SetScrollPos ( iPos, TRUE );
			break;

		case SB_THUMBTRACK:
			m_fEnd = (iPos-m_Zero) /m_ToScale;
			break;
		};
	}
	else if ( pScrollBar == &m_ScrollEnd4 )
	{
		switch ( nSBCode )
		{
		case SB_LINELEFT:
			iPos = m_ScrollEnd4.GetScrollPos();
			iPos -= 1;
			m_fEnd4 = (iPos-m_Zero) /m_ToScale;
			m_ScrollEnd4.SetScrollPos ( iPos, TRUE );
			break;

		case SB_LINERIGHT:
			iPos = m_ScrollEnd4.GetScrollPos();
			iPos += 1;
			m_fEnd4 = (iPos-m_Zero) /m_ToScale;
			m_ScrollEnd4.SetScrollPos ( iPos, TRUE );
			break;

		case SB_PAGELEFT:
			iPos = m_ScrollEnd4.GetScrollPos();
			iPos -= 20;
			m_fEnd4 = (iPos-m_Zero) /m_ToScale;
			m_ScrollEnd4.SetScrollPos ( iPos, TRUE );
			break;

		case SB_PAGERIGHT:
			iPos = m_ScrollEnd4.GetScrollPos();
			iPos += 20;
			m_fEnd4 = (iPos-m_Zero) /m_ToScale;
			m_ScrollEnd4.SetScrollPos ( iPos, TRUE );
			break;

		case SB_THUMBPOSITION:
			m_fEnd4 = (iPos-m_Zero) /m_ToScale;
			m_ScrollEnd4.SetScrollPos ( iPos, TRUE );
			break;

		case SB_THUMBTRACK:
			m_fEnd4 = (iPos-m_Zero) /m_ToScale;
			break;
		};
	}

	CString Str;
	Str.Format ( "%3.3f", m_fStart );
	GetDlgItem(IDC_EDIT_START_F)->SetWindowText ( Str );
	Str.Format ( "%3.3f", m_fStartVar );
	GetDlgItem(IDC_EDIT_STARTVAR_F)->SetWindowText ( Str );
	Str.Format ( "%3.3f", m_fEnd );
	GetDlgItem(IDC_EDIT_END_F)->SetWindowText ( Str );
	Str.Format ( "%3.3f", m_fEnd4 );
	GetDlgItem(IDC_EDIT_END4_F)->SetWindowText ( Str );

	CDialog::OnHScroll(nSBCode, iPos, pScrollBar);
}

void CSetVar::OnEnChangeEditStartF()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString Str;
	GetDlgItem(IDC_EDIT_START_F)->GetWindowText ( Str );
	m_fStart = (float) atof ( Str.GetString() );

	m_ScrollStart.SetScrollPos ( int(m_Zero+m_fStart*m_ToScale), TRUE );
}

void CSetVar::OnEnChangeEditStartvarF()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString Str;
	GetDlgItem(IDC_EDIT_STARTVAR_F)->GetWindowText ( Str );
	m_fStartVar = (float) atof ( Str.GetString() );

	m_ScrollVar.SetScrollPos ( int(m_Zero+m_fStartVar*m_ToScale), TRUE );
}

void CSetVar::OnEnChangeEditEndF()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString Str;
	GetDlgItem(IDC_EDIT_END_F)->GetWindowText ( Str );
	m_fEnd = (float) atof ( Str.GetString() );

	m_ScrollEnd.SetScrollPos ( int(m_Zero+m_fEnd*m_ToScale), TRUE );
}

void CSetVar::OnEnChangeEditEnd4F()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString Str;
	GetDlgItem(IDC_EDIT_END4_F)->GetWindowText ( Str );
	m_fEnd4 = (float) atof ( Str.GetString() );

	m_ScrollEnd4.SetScrollPos ( int(m_Zero+m_fEnd4*m_ToScale), TRUE );
}

void CSetVar::OnBnClickedCheckUse()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_USE );

	if ( m_bUse )
	{
		m_bUse = FALSE;
		pButton->SetCheck( FALSE );
	}
	else
	{
		m_bUse = TRUE;
		pButton->SetCheck( TRUE );
	}
}

void CSetVar::OnBnClickedRadioUp()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iRadio = 0;
}

void CSetVar::OnBnClickedRadioMid()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iRadio = 1;
}

void CSetVar::OnBnClickedRadioDown()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_iRadio = 2;
}
