// TexMultiDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "../SigmaCore/Util/Util.h"
#include "../EngineLib/DxTools/TextureManager.h"
#include "../SigmaCore/util/util.h"
#include "TexMultiDlg.h"


// CTexMultiDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CTexMultiDlg, CDialog)
CTexMultiDlg::CTexMultiDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTexMultiDlg::IDD, pParent)
{
}

CTexMultiDlg::~CTexMultiDlg()
{
}

void CTexMultiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTexMultiDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SRCTEX, OnBnClickedButtonSrctex)
	ON_BN_CLICKED(IDC_BUTTON_FLOWTEX, OnBnClickedButtonFlowtex)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_NEWUV, OnBnClickedCheckNewuv)
	ON_BN_CLICKED(IDC_CHECK_RANDOM_SPEED, OnBnClickedCheckRandomSpeed)
	ON_BN_CLICKED(IDC_CHECK_MODULATE2X, OnBnClickedCheckModulate2x)
END_MESSAGE_MAP()


// CTexMultiDlg �޽��� ó�����Դϴ�.

BOOL CTexMultiDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	if( m_strAlphaTex.size() )
	{
		GetDlgItem( IDC_EDIT_SRCTEX )->SetWindowText( m_strAlphaTex.c_str() );
	}
	else
	{
		std::string	strName;
		strName = sc::util::GetSpecularName( m_strBaseTex, "_e" );

		if( TextureManager::GetInstance().IsPathFile( strName ) )
		{
			m_strAlphaTex = strName.c_str();
			GetDlgItem( IDC_EDIT_SRCTEX )->SetWindowText( m_strAlphaTex.c_str() );
		}
		else
		{
			m_strAlphaTex = m_strBaseTex.c_str();
			GetDlgItem( IDC_EDIT_SRCTEX )->SetWindowText( m_strAlphaTex.c_str() );
		}
	}

	if( m_strFlowTex.size() )
	{
		GetDlgItem( IDC_EDIT_FLOWTEX )->SetWindowText( m_strFlowTex.c_str() );
	}
	else
	{
		m_strFlowTex = "_Wa_water1.bmp";
		GetDlgItem( IDC_EDIT_FLOWTEX )->SetWindowText( m_strFlowTex.c_str() );
	}

	CString Str;
	Str.Format ( "%4.3f", m_fSpeedX );
	GetDlgItem( IDC_EDIT_SPEED_X )->SetWindowText( Str );

	Str.Format ( "%4.3f", m_fSpeedY );
	GetDlgItem( IDC_EDIT_SPEED_Y )->SetWindowText( Str );

	Str.Format ( "%4.3f", m_fAlpha );
	GetDlgItem( IDC_EDIT_ALPHA )->SetWindowText( Str );

	Str.Format ( "%4.3f", m_fUVScale );
	GetDlgItem( IDC_EDIT_SCALE )->SetWindowText( Str );

	if( m_bNEWUV )		((CButton*)GetDlgItem( IDC_CHECK_NEWUV ))->SetCheck( TRUE );
	else				((CButton*)GetDlgItem( IDC_CHECK_NEWUV ))->SetCheck( FALSE );

	if( m_bRandomSpeed )	((CButton*)GetDlgItem( IDC_CHECK_RANDOM_SPEED ))->SetCheck( TRUE );
	else					((CButton*)GetDlgItem( IDC_CHECK_RANDOM_SPEED ))->SetCheck( FALSE );

	if( m_bModulate2X )	((CButton*)GetDlgItem( IDC_CHECK_MODULATE2X ))->SetCheck( TRUE );
	else				((CButton*)GetDlgItem( IDC_CHECK_MODULATE2X ))->SetCheck( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CTexMultiDlg::OnBnClickedButtonSrctex()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem( IDC_EDIT_SRCTEX );
		pWnd->SetWindowText( FilePath );

		m_strAlphaTex = FilePath;
	}
}

void CTexMultiDlg::OnBnClickedButtonFlowtex()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg( TRUE, ".BMP", NULL, OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem( IDC_EDIT_FLOWTEX );
		pWnd->SetWindowText( FilePath );

		m_strFlowTex = FilePath;
	}
}

void CTexMultiDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strText;

	CWnd* pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_SPEED_X );
	pWnd->GetWindowText ( strText );
	m_fSpeedX = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SPEED_Y );
	pWnd->GetWindowText ( strText );
	m_fSpeedY = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ALPHA );
	pWnd->GetWindowText ( strText );
	m_fAlpha = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE );
	pWnd->GetWindowText ( strText );
	m_fUVScale = (float)atof( strText.GetString() );

	OnOK();
}

void CTexMultiDlg::OnBnClickedCheckNewuv()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( ((CButton*)GetDlgItem( IDC_CHECK_NEWUV ))->GetCheck() )	m_bNEWUV = TRUE;
	else														m_bNEWUV = FALSE;
}

void CTexMultiDlg::OnBnClickedCheckRandomSpeed()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( ((CButton*)GetDlgItem( IDC_CHECK_RANDOM_SPEED ))->GetCheck() )	m_bRandomSpeed = TRUE;
	else																m_bRandomSpeed = FALSE;
}

void CTexMultiDlg::OnBnClickedCheckModulate2x()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( ((CButton*)GetDlgItem( IDC_CHECK_MODULATE2X ))->GetCheck() )	m_bModulate2X = TRUE;
	else																m_bModulate2X = FALSE;
}
