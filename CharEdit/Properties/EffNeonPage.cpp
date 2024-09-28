// EffNeonPage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "EffNeonPage.h"
#include ".\effneonpage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffNeonPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffNeonPage, CPropertyPage)
CEffNeonPage::CEffNeonPage()
	: CEffCharPage(CEffNeonPage::IDD)
{
}

CEffNeonPage::~CEffNeonPage()
{
}

void CEffNeonPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MATRIAL, m_ComboMatrials);
}


BEGIN_MESSAGE_MAP(CEffNeonPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX, OnBnClickedCheckSrctex)
	ON_CBN_SELCHANGE(IDC_COMBO_MATRIAL, OnCbnSelchangeComboMatrial)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnBnClickedRadioAll)
	ON_BN_CLICKED(IDC_RADIO_SELECT, OnBnClickedRadioSelect)
	ON_BN_CLICKED(IDC_RADIO_ORI, OnBnClickedRadioOri)
	ON_BN_CLICKED(IDC_RADIO_USER, OnBnClickedRadioUser)
	ON_BN_CLICKED(IDC_RADIO_GENERAL, OnBnClickedRadioGeneral)
	ON_BN_CLICKED(IDC_RADIO_BURN, OnBnClickedRadioBurn)
END_MESSAGE_MAP()

// CEffNeonPage 메시지 처리기입니다.
void CEffNeonPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_NEON*)pProp);

	CString		Str;
	CWnd*		pWnd;
	CButton*	pButton;
	int			nIndex;

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( m_Property.m_szTexture );

	//	Check General Burn
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_GENERAL );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_BURN );
	pButton->SetCheck ( FALSE );

	if ( m_Property.m_dwFlag & USE_GENERAL )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_GENERAL );
		pButton->SetCheck ( TRUE );
	}
	else if ( m_Property.m_dwFlag & USE_BURN )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_BURN );
		pButton->SetCheck ( TRUE );
	}

	//	Check All Select
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_ALL );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_SELECT );
	pButton->SetCheck ( FALSE );

	if ( m_Property.m_dwFlag & USE_ALL_TEX )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_ALL );
		pButton->SetCheck ( TRUE );
	}
	else if ( m_Property.m_dwFlag & USE_SELECT_TEX )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_SELECT );
		pButton->SetCheck ( TRUE );
	}

	Check_All_Select ();

	//	Check Original User Texture
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_ORI );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_USER );
	pButton->SetCheck ( FALSE );

	if ( m_Property.m_dwFlag & USE_GLOW_SRCTEX )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_ORI );
		pButton->SetCheck ( TRUE );
	}
	else if ( m_Property.m_dwFlag & USE_GLOW_USER )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_USER );
		pButton->SetCheck ( TRUE );
	}

	Check_Src_User_Tex ();


	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharNeon::TYPEID,
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

		m_Property = *(EFFCHAR_PROPERTY_NEON*)m_pEffChar->GetProperty ();
	}

	DWORD	dwMaterials = m_Property.m_dwMaterials;

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	m_ComboMatrials.ResetContent();
	for ( DWORD i=0; i<dwMaterials; ++i )
	{
		Str.Format ( "%s",pMatrials[i].szTexture );
		nIndex = m_ComboMatrials.AddString ( Str );
		m_ComboMatrials.SetItemData ( nIndex, i );
	}

	int nCurSel = 0;
	m_ComboMatrials.SetCurSel( nCurSel );			// 초기값.. !!

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].IsType( EMCET_BASE ) )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );
}

void CEffNeonPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString	strText;
	CWnd	*pWnd;

	const char* szTexture;

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strText );
	szTexture = strText.GetString();
	strcpy ( m_Property.m_szTexture, szTexture );

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

    EffectApply();
}

void CEffNeonPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffNeonPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();
}

void CEffNeonPage::OnBnClickedButtonColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = (m_Property.m_cColor&0xff0000)>>16;
	int ThisG = (m_Property.m_cColor&0xff00)>>8;
	int ThisB = (m_Property.m_cColor&0xff);

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cColor = 0xff000000;
		m_Property.m_cColor += GetRValue(SelectColor)<<16;
		m_Property.m_cColor += GetGValue(SelectColor)<<8;
		m_Property.m_cColor += GetBValue(SelectColor);

		int nCurSel = m_ComboMatrials.GetCurSel();
		DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();
		for ( DWORD i=0; i<m_Property.m_dwMaterials; ++i )
		{
			pMatrials[i].d3dMaterial.Diffuse.r = (float)GetRValue(SelectColor)/255.f;
			pMatrials[i].d3dMaterial.Diffuse.g = (float)GetGValue(SelectColor)/255.f;
			pMatrials[i].d3dMaterial.Diffuse.b = (float)GetBValue(SelectColor)/255.f;
		}
	}
}

void CEffNeonPage::OnBnClickedButtonTexture()
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

void CEffNeonPage::OnBnClickedCheckSrctex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	int nCurSel = m_ComboMatrials.GetCurSel();

	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pButton->GetCheck() )		pMatrials[nCurSel].SetType( EMCET_BASE );
	else							pMatrials[nCurSel].SetType( EMCET_NULL );
}

void CEffNeonPage::OnCbnSelchangeComboMatrial()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;

	int nCurSel = m_ComboMatrials.GetCurSel();

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].IsType( EMCET_BASE ) )		pButton->SetCheck ( TRUE );
	else									pButton->SetCheck ( FALSE );
}

void CEffNeonPage::OnBnClickedRadioGeneral()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_dwFlag &= ~USE_BURN;

	m_Property.m_dwFlag |= USE_GENERAL;
}

void CEffNeonPage::OnBnClickedRadioBurn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_dwFlag &= ~USE_GENERAL;

	m_Property.m_dwFlag |= USE_BURN;
}

void CEffNeonPage::OnBnClickedRadioAll()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_dwFlag &= ~USE_SELECT_TEX;

	m_Property.m_dwFlag |= USE_ALL_TEX;

	Check_All_Select ();
}

void CEffNeonPage::OnBnClickedRadioSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_dwFlag &= ~USE_ALL_TEX;

	m_Property.m_dwFlag |= USE_SELECT_TEX;
	
	Check_All_Select ();
}

void CEffNeonPage::Check_All_Select ()
{
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	
	if ( m_Property.m_dwFlag & USE_ALL_TEX )
	{
		m_ComboMatrials.EnableWindow ( FALSE );
		pButton->EnableWindow ( FALSE );
	}
	else if ( m_Property.m_dwFlag & USE_SELECT_TEX )
	{
		m_ComboMatrials.EnableWindow ( TRUE );
		pButton->EnableWindow ( TRUE );
	}
}

void CEffNeonPage::OnBnClickedRadioOri()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_dwFlag &= ~USE_GLOW_USER;

	m_Property.m_dwFlag |= USE_GLOW_SRCTEX;
	
	Check_Src_User_Tex ();
}

void CEffNeonPage::OnBnClickedRadioUser()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_dwFlag &= ~USE_GLOW_SRCTEX;

	m_Property.m_dwFlag |= USE_GLOW_USER;
	
	Check_Src_User_Tex ();
}

void CEffNeonPage::Check_Src_User_Tex ()
{
	CWnd*		pWnd;
	pWnd		= GetDlgItem ( IDC_BUTTON_TEXTURE );
	
	if ( m_Property.m_dwFlag & USE_GLOW_SRCTEX )
	{
		pWnd->EnableWindow ( FALSE );
	}
	else if ( m_Property.m_dwFlag & USE_GLOW_USER )
	{
		pWnd->EnableWindow ( TRUE );
	}
}
