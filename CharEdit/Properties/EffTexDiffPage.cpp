// EffTexDiffPage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"

#include "../SigmaCore/Util/Util.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "EffTexDiffPage.h"
#include ".\efftexdiffpage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffTexDiffPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffTexDiffPage, CPropertyPage)
CEffTexDiffPage::CEffTexDiffPage()
	: CEffCharPage(CEffTexDiffPage::IDD)
{
}

CEffTexDiffPage::~CEffTexDiffPage()
{
}

void CEffTexDiffPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MATRIAL, m_ComboMatrials);
}


BEGIN_MESSAGE_MAP(CEffTexDiffPage, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_MATRIAL, OnCbnSelchangeComboMatrial)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX, OnBnClickedCheckSrctex)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_1X, OnBnClickedRadio1x)
	ON_BN_CLICKED(IDC_RADIO_2X, OnBnClickedRadio2x)
	ON_BN_CLICKED(IDC_RADIO_4X, OnBnClickedRadio4x)
	ON_BN_CLICKED(IDC_CHECK_UPDOWN, OnBnClickedCheckUpdown)
	ON_EN_CHANGE(IDC_EDIT_ALPHA, OnEnChangeEditAlpha)
END_MESSAGE_MAP()


// CEffTexDiffPage 메시지 처리기입니다.
void CEffTexDiffPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_TEXDIFF*)pProp);

	CString	Str;

	CWnd*		pWnd;
	CButton*	pButton;
	int		nIndex;

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharTexDiff::TYPEID,
			&m_Property,
			NULL,
			pSkinPiece
		);

		if ( !m_pEffChar )
		{
			MessageBox ( "이팩트 생성에 실패하였습니다.", "ERROR", MB_OK );
			return;
		}

		pSkinPiece->AddEffList ( m_pEffChar );

		m_Property = *((EFFCHAR_PROPERTY_TEXDIFF*)m_pEffChar->GetProperty ());
	}

	DWORD	dwMaterials = m_Property.m_dwMaterials;

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	m_ComboMatrials.ResetContent();
	for ( DWORD i=0; i<dwMaterials; i++ )
	{
		Str.Format ( "%s",pMatrials[i].szTexture );
		nIndex = m_ComboMatrials.AddString ( Str );
		m_ComboMatrials.SetItemData ( nIndex, i );
	}

	int nCurSel = 0;
	m_ComboMatrials.SetCurSel( nCurSel );			// 초기값.. !!

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].IsType( EMCET_BASE ) )	pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );

	char	szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_ALPHA );
	_gcvt ( pMatrials[nCurSel].d3dMaterial.Power, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem( IDC_EDIT_SPEED );
	_gcvt ( m_Property.m_fSpeed, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pButton = (CButton*)GetDlgItem( IDC_CHECK_UPDOWN );
	if( m_Property.m_dwFlag & NO_UPDOWN )	pButton->SetCheck( TRUE );
	else									pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem( IDC_RADIO_1X );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_RADIO_2X );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_RADIO_4X );
	pButton->SetCheck( FALSE );
	switch( m_Property.m_dwIntensity )
	{
	case 0:
		pButton = (CButton*)GetDlgItem( IDC_RADIO_1X );
		pButton->SetCheck( TRUE );
		break;
	case 1:
		pButton = (CButton*)GetDlgItem( IDC_RADIO_2X );
		pButton->SetCheck( TRUE );
		break;
	case 2:
		pButton = (CButton*)GetDlgItem( IDC_RADIO_4X );
		pButton->SetCheck( TRUE );
		break;
	};

	// = 0;
	//m_pEffChar->SetProperty( &m_Property );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( pMatrials[nCurSel].szEffTex );

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CEffTexDiffPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString		strTexture;
	CWnd		*pWnd;
	const char*	szTexture;

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	int	nCurSel = m_ComboMatrials.GetCurSel();

	pWnd = GetDlgItem ( IDC_EDIT_SPEED );
	pWnd->GetWindowText ( strTexture );
	m_Property.m_fSpeed = (float) atof ( strTexture.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strTexture );
	szTexture = strTexture.GetString();
	strcpy ( pMatrials[nCurSel].szEffTex, szTexture );

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

    EffectApply();
}

void CEffTexDiffPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffTexDiffPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();
}

void CEffTexDiffPage::OnBnClickedButtonTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
		pWnd->SetWindowText ( FilePath );
	}
}

void CEffTexDiffPage::OnBnClickedCheckSrctex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	int nCurSel = m_ComboMatrials.GetCurSel();

	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pButton->GetCheck() )		pMatrials[nCurSel].SetType( EMCET_BASE );
	else							pMatrials[nCurSel].SetType( EMCET_NULL );

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CEffTexDiffPage::OnCbnSelchangeComboMatrial()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd*		pWnd;
	CButton*	pButton;

	int nCurSel = m_ComboMatrials.GetCurSel();

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].IsType( EMCET_BASE ) )	pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );

	char	szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_ALPHA );
	_gcvt ( pMatrials[nCurSel].d3dMaterial.Power, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( pMatrials[nCurSel].szEffTex );

	//	EnableWindow
	CheckEnableWindow ( nCurSel );
}

void CEffTexDiffPage::CheckEnableWindow ( int nCurSel )
{
	CWnd*		pWnd;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ	pd3dDevice = pView->GetD3dDevice();

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	if ( pMatrials[nCurSel].IsType( EMCET_BASE ) )
	{
		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pWnd->EnableWindow ( TRUE );

		if ( !strlen(pMatrials[nCurSel].szEffTex) )
		{
			std::string	strSrcTex = pMatrials[nCurSel].szTexture;

			std::string	strName;
			strName = sc::util::GetSpecularName( strSrcTex, "_e" );

			if( !TextureManager::GetInstance().IsPathFile( strName ) )
                strcpy( pMatrials[nCurSel].szEffTex, "" );
		}
	}
	else
	{
		pWnd = GetDlgItem ( IDC_BUTTON_TEXTURE );
		pWnd->EnableWindow ( FALSE );
	}
}

void CEffTexDiffPage::OnBnClickedRadio1x()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_dwIntensity = 0;
	m_pEffChar->SetProperty( &m_Property );
}

void CEffTexDiffPage::OnBnClickedRadio2x()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_dwIntensity = 1;
	m_pEffChar->SetProperty( &m_Property );
}

void CEffTexDiffPage::OnBnClickedRadio4x()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_dwIntensity = 2;
	m_pEffChar->SetProperty( &m_Property );
}

void CEffTexDiffPage::OnBnClickedCheckUpdown()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_UPDOWN );
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= NO_UPDOWN;
	else						m_Property.m_dwFlag &= ~NO_UPDOWN;

	m_pEffChar->SetProperty( &m_Property );
}



void CEffTexDiffPage::OnEnChangeEditAlpha()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nCurSel = m_ComboMatrials.GetCurSel();

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	CString	strTexture;
	CWnd* pWnd = GetDlgItem ( IDC_EDIT_ALPHA );
	pWnd->GetWindowText ( strTexture );
	pMatrials[nCurSel].d3dMaterial.Power = (float) atof ( strTexture.GetString() );
}
