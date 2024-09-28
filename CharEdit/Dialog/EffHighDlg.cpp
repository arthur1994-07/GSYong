// EffHighDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../CharEdit.h"
#include "EffHighDlg.h"


// CEffHighDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CEffHighDlg, CDialog)
CEffHighDlg::CEffHighDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEffHighDlg::IDD, pParent)
{
	m_bMType = TRUE;
	m_bSType = TRUE;
	m_bKeyTime = TRUE;

	m_dwMType = 0;
	m_dwSType = 0;

	m_dwStartKey = 0;
	m_dwEndKey = 0;
}

CEffHighDlg::~CEffHighDlg()
{
}

void CEffHighDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MTYPE, m_pComboMType);
	DDX_Control(pDX, IDC_COMBO_STYPE, m_pComboSType);
}


BEGIN_MESSAGE_MAP(CEffHighDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_MTYPE, OnBnClickedCheckMtype)
	ON_BN_CLICKED(IDC_CHECK_STYPE, OnBnClickedCheckStype)
	ON_CBN_SELCHANGE(IDC_COMBO_MTYPE, OnCbnSelchangeComboMtype)
	ON_CBN_SELCHANGE(IDC_COMBO_STYPE, OnCbnSelchangeComboStype)
	ON_BN_CLICKED(IDC_CHECK_KEYTIME, OnBnClickedCheckKeytime)
	ON_EN_CHANGE(IDC_EDIT_STARTKEY, OnEnChangeEditStartkey)
	ON_EN_CHANGE(IDC_EDIT_ENDKEY, OnEnChangeEditEndkey)
END_MESSAGE_MAP()


// CEffHighDlg �޽��� ó�����Դϴ�.

BOOL CEffHighDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	int	nIndex;
	CString	strText;

	CWnd*		pWnd;
	CButton*	pButton;
	char		szValue[256];

	m_pComboMType.ResetContent();

	strText.Format ( "01.AN_IDLE" );
	nIndex = m_pComboMType.AddString ( strText );
	m_pComboMType.SetItemData ( nIndex, 0 );
	strText.Format ( "02.AN_GUARD" );
	nIndex = m_pComboMType.AddString ( strText );
	m_pComboMType.SetItemData ( nIndex, 1 );
	strText.Format ( "03.AN_WALK" );
	nIndex = m_pComboMType.AddString ( strText );
	m_pComboMType.SetItemData ( nIndex, 2 );
	strText.Format ( "04.AN_RUN" );
	nIndex = m_pComboMType.AddString ( strText );
	m_pComboMType.SetItemData ( nIndex, 3 );
	strText.Format ( "05.AN_ATTACK" );
	nIndex = m_pComboMType.AddString ( strText );
	m_pComboMType.SetItemData ( nIndex, 4 );
	strText.Format ( "06.AN_SHOCK" );
	nIndex = m_pComboMType.AddString ( strText );
	m_pComboMType.SetItemData ( nIndex, 5 );
	strText.Format ( "07.AN_DIE" );
	nIndex = m_pComboMType.AddString ( strText );
	m_pComboMType.SetItemData ( nIndex, 6 );
	strText.Format ( "08.AN_CREATE" );
	nIndex = m_pComboMType.AddString ( strText );
	m_pComboMType.SetItemData ( nIndex, 7 );
	strText.Format ( "09.AN_TALK" );
	nIndex = m_pComboMType.AddString ( strText );
	m_pComboMType.SetItemData ( nIndex, 8 );
	strText.Format ( "10.AN_SKILL" );
	nIndex = m_pComboMType.AddString ( strText );
	m_pComboMType.SetItemData ( nIndex, 9 );
	strText.Format ( "11.AN_GESTURE" );
	nIndex = m_pComboMType.AddString ( strText );
	m_pComboMType.SetItemData ( nIndex, 10 );

	m_pComboMType.SetCurSel ( m_dwMType );
	m_pComboSType.SetCurSel ( m_dwSType );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_MTYPE );
	if ( m_bMType )
	{
		pButton->SetCheck (TRUE);
		m_pComboMType.EnableWindow ( FALSE );
	}
	else
	{
		pButton->SetCheck (FALSE);
		m_pComboMType.EnableWindow ( TRUE );		
	}
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_STYPE );
	if ( m_bSType )	
	{
		pButton->SetCheck (TRUE);
		m_pComboSType.EnableWindow ( FALSE );
	}
	else
	{
		pButton->SetCheck (FALSE);
		m_pComboSType.EnableWindow ( TRUE );
	}


	pWnd = GetDlgItem ( IDC_EDIT_STARTKEY );
	_gcvt ( m_dwStartKey, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_ENDKEY );
	_gcvt ( m_dwEndKey, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_KEYTIME );
	if ( m_bKeyTime )
	{
		pButton->SetCheck ( TRUE );
		pWnd = GetDlgItem ( IDC_EDIT_STARTKEY );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem ( IDC_EDIT_ENDKEY );
		pWnd->EnableWindow ( FALSE );
	}
	else
	{
		pButton->SetCheck ( FALSE );
		pWnd = GetDlgItem ( IDC_EDIT_STARTKEY );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem ( IDC_EDIT_ENDKEY );
		pWnd->EnableWindow ( TRUE );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CEffHighDlg::OnBnClickedCheckMtype()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_MTYPE );

	if ( pButton->GetCheck() )
	{
		m_bMType = TRUE;
		m_pComboMType.EnableWindow ( FALSE );
	}
	else
	{
		m_bMType = FALSE;
		m_pComboMType.EnableWindow ( TRUE );
	}
}

void CEffHighDlg::OnCbnSelchangeComboMtype()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_dwMType = m_pComboMType.GetCurSel ();
}

void CEffHighDlg::OnBnClickedCheckStype()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_STYPE );

	if ( pButton->GetCheck() )
	{
		m_bSType = TRUE;
		m_pComboSType.EnableWindow ( FALSE );
	}
	else
	{
		m_bSType = FALSE;
		m_pComboSType.EnableWindow ( TRUE );
	}
}

void CEffHighDlg::OnCbnSelchangeComboStype()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_dwSType = m_pComboSType.GetCurSel ();
}

void CEffHighDlg::OnBnClickedCheckKeytime()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CWnd*		pWnd;
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_KEYTIME );

	if ( pButton->GetCheck() )
	{
		m_bKeyTime = TRUE;
		pWnd = GetDlgItem ( IDC_EDIT_STARTKEY );
		pWnd->EnableWindow ( FALSE );
		pWnd = GetDlgItem ( IDC_EDIT_ENDKEY );
		pWnd->EnableWindow ( FALSE );
	}
	else
	{
		m_bKeyTime = FALSE;
		pWnd = GetDlgItem ( IDC_EDIT_STARTKEY );
		pWnd->EnableWindow ( TRUE );
		pWnd = GetDlgItem ( IDC_EDIT_ENDKEY );
		pWnd->EnableWindow ( TRUE );
	}
}

void CEffHighDlg::OnEnChangeEditStartkey()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CWnd*		pWnd;
	CString		strText;

	pWnd = GetDlgItem ( IDC_EDIT_STARTKEY );
	pWnd->GetWindowText ( strText );
	m_dwStartKey = atoi ( strText.GetString() );
}

void CEffHighDlg::OnEnChangeEditEndkey()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CWnd*		pWnd;
	CString		strText;

	pWnd = GetDlgItem ( IDC_EDIT_ENDKEY );
	pWnd->GetWindowText ( strText );
	m_dwEndKey = atoi ( strText.GetString() );
}
