// EffWorldBattleFlagPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"

#include "EffWorldBattleFlagPage.h"


// CEffWorldBattleFlagPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffWorldBattleFlagPage, CPropertyPage)

CEffWorldBattleFlagPage::CEffWorldBattleFlagPage()
	: CEffCharPage(CEffWorldBattleFlagPage::IDD)
{

}

CEffWorldBattleFlagPage::~CEffWorldBattleFlagPage()
{
}

void CEffWorldBattleFlagPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MATRIAL, m_ComboMatrials);
}


BEGIN_MESSAGE_MAP(CEffWorldBattleFlagPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_MATRIAL, OnCbnSelchangeComboMatrial)
	ON_BN_CLICKED(IDC_CHECK_SRCTEX, OnBnClickedCheckSrctex)
END_MESSAGE_MAP()


// CEffWorldBattleFlagPage 메시지 처리기입니다.
void CEffWorldBattleFlagPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_WORLD_BATTLE_FLAG*)pProp);

	CString	Str;

	CButton*	pButton;

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharWorldBattleFlag::TYPEID,
			&m_Property,
			NULL,
			pSkinPiece
		);

		if ( !m_pEffChar )
		{
			MessageBox ( _T("이팩트 생성에 실패하였습니다."), _T("ERROR"), MB_OK );
			return;
		}

		pSkinPiece->AddEffList ( m_pEffChar );

		m_Property = *((EFFCHAR_PROPERTY_WORLD_BATTLE_FLAG*)m_pEffChar->GetProperty ());
	}

	DWORD	dwMaterials = m_Property.m_dwMaterials;

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	int nIndex = 0;
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
	if ( pMatrials[nCurSel].IsType(EMCET_TEXTURE_MATRIX) )	pButton->SetCheck ( TRUE );
	else													pButton->SetCheck ( FALSE );
}

void CEffWorldBattleFlagPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

    EffectApply();
}

void CEffWorldBattleFlagPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffWorldBattleFlagPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();
}

void CEffWorldBattleFlagPage::OnBnClickedCheckSrctex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	int nCurSel = m_ComboMatrials.GetCurSel();

	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pButton->GetCheck() )		pMatrials[nCurSel].SetType( EMCET_TEXTURE_MATRIX );
	else							pMatrials[nCurSel].SetType( EMCET_NULL );
}

void CEffWorldBattleFlagPage::OnCbnSelchangeComboMatrial()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;

	int nCurSel = m_ComboMatrials.GetCurSel();

	DXMATERIAL_CHAR_EFF*	pMatrials = m_pEffChar->GetMaterials();

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SRCTEX );
	if ( pMatrials[nCurSel].IsType( EMCET_TEXTURE_MATRIX ) )	pButton->SetCheck ( TRUE );
	else														pButton->SetCheck ( FALSE );
}
 