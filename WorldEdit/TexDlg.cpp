// TexDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "../SigmaCore/Util/Util.h"
#include "../EngineLib/DxTools/TextureManager.h"
#include "../SigmaCore/util/util.h"
#include "TexDlg.h"


// CTexDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTexDlg, CDialog)
CTexDlg::CTexDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTexDlg::IDD, pParent),
	m_emMode( EMTESPECULAR )
{
}

CTexDlg::~CTexDlg()
{
}

void CTexDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTexDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SRCTEX, OnBnClickedButtonSrctex)
	ON_BN_CLICKED(IDC_BUTTON_FLOWTEX, OnBnClickedButtonFlowtex)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_SPEED, OnBnClickedCheckSpeed)
	ON_BN_CLICKED(IDC_CHECK_NIGHTLIGHT, OnBnClickedCheckNightlight)
END_MESSAGE_MAP()


// CTexDlg 메시지 처리기입니다.

BOOL CTexDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if( m_emMode == EMTESPECULAR )
	{
		GetDlgItem( IDC_STATIC_FLOWTEX )->ShowWindow( FALSE );
		GetDlgItem( IDC_EDIT_FLOWTEX )->ShowWindow( FALSE );
		GetDlgItem( IDC_BUTTON_FLOWTEX )->ShowWindow( FALSE );

		CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_SPEED );
		pButton->ShowWindow( FALSE );
		GetDlgItem( IDC_STATIC_SPEED )->ShowWindow( FALSE );
		GetDlgItem( IDC_EDIT_SPEED )->ShowWindow( FALSE );

		GetDlgItem( IDC_STATIC_ALPHA )->ShowWindow( FALSE );
		GetDlgItem( IDC_EDIT_ALPHA )->ShowWindow( FALSE );		
	}
	else if( m_emMode == EMTEDIFFUSE )
	{
		GetDlgItem( IDC_STATIC_FLOWTEX )->ShowWindow( FALSE );
		GetDlgItem( IDC_EDIT_FLOWTEX )->ShowWindow( FALSE );
		GetDlgItem( IDC_BUTTON_FLOWTEX )->ShowWindow( FALSE );

		CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_SPEED );
		if( m_bSpeed )	pButton->SetCheck( TRUE );
		else			pButton->SetCheck( FALSE );

		pButton = (CButton*)GetDlgItem( IDC_CHECK_NIGHTLIGHT );
		if( m_bNightLight )	pButton->SetCheck( TRUE );
		else				pButton->SetCheck( FALSE );
	}


	if( m_strAlphaTex.size() )
	{
		GetDlgItem( IDC_EDIT_SRCTEX )->SetWindowText( m_strAlphaTex.c_str() );
	}
	else
	{
		std::string	strName;

		if( m_emMode == EMTESPECULAR )
		{
			strName = sc::util::GetSpecularName( m_strBaseTex, "_s" );
		}
		else if( m_emMode == EMTEDIFFUSE )
		{
			strName = sc::util::GetSpecularName( m_strBaseTex, "_e" );
		}

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
	Str.Format ( "%3.3f", m_fSpeed );
	GetDlgItem( IDC_EDIT_SPEED )->SetWindowText( Str );

	Str.Format ( "%3.3f", m_fAlpha );
	GetDlgItem( IDC_EDIT_ALPHA )->SetWindowText( Str );

	// Speed 체크 박스를 확인 후 Edit 박스를 Enable 할 것인가 ? 결정.
	OnBnClickedCheckSpeed();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTexDlg::OnBnClickedButtonSrctex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		char szName[255];
		strcpy( szName, FilePath.GetString() );
		m_strAlphaTex = szName;

		CWnd *pWnd;
		pWnd = GetDlgItem( IDC_EDIT_SRCTEX );
		pWnd->SetWindowText( m_strAlphaTex.c_str() );
	}
}

void CTexDlg::OnBnClickedButtonFlowtex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

		m_strFlowTex = FilePath.GetString();
	}
}

void CTexDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strText;

	CWnd* pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_SPEED );
	pWnd->GetWindowText ( strText );
	m_fSpeed = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ALPHA );
	pWnd->GetWindowText ( strText );
	m_fAlpha = (float)atof( strText.GetString() );

	OnOK();
}

void CTexDlg::OnBnClickedCheckSpeed()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_SPEED );
	if( pButton->GetCheck() )
	{
		m_bSpeed = TRUE;
		GetDlgItem( IDC_EDIT_SPEED )->EnableWindow( TRUE );
	}
	else	
	{
		m_bSpeed = FALSE;
		GetDlgItem( IDC_EDIT_SPEED )->EnableWindow( FALSE );
	}

	
}

void CTexDlg::OnBnClickedCheckNightlight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_NIGHTLIGHT );
	if( pButton->GetCheck() )
	{
		m_bNightLight = TRUE;
	}
	else	
	{
		m_bNightLight = FALSE;
	}

}
