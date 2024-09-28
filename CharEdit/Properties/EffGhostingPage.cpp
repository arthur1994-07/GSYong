// EffReflectionPage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"
#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../enginelib/GUInterface/GameTextControl.h"

#include "EffGhostingPage.h"
#include ".\effghostingpage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffGhostingPage 대화 상자입니다.
IMPLEMENT_DYNAMIC(CEffGhostingPage, CPropertyPage)
CEffGhostingPage::CEffGhostingPage()
	: CEffCharPage(CEffGhostingPage::IDD)
{
}

CEffGhostingPage::~CEffGhostingPage()
{
}

void CEffGhostingPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_M_TYPE, m_comboMType);
	DDX_Control(pDX, IDC_COMBO_S_TYPE, m_comboSType);
}


BEGIN_MESSAGE_MAP(CEffGhostingPage, CPropertyPage)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
	ON_CBN_SELCHANGE(IDC_COMBO_M_TYPE, OnCbnSelchangeComboMType)
	ON_CBN_SELCHANGE(IDC_COMBO_S_TYPE, OnCbnSelchangeComboSType)
END_MESSAGE_MAP()


// CEffGhostingPage 메시지 처리기입니다.

BOOL CEffGhostingPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CEffGhostingPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_GHOSTING*)pProp);

	char	szValue[256];
	CWnd	*pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_NUM );
	_gcvt ( m_Property.m_dwGhostingNum, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_UNIT );
	_gcvt ( m_Property.m_dwUnit, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_ALPHA );
	_gcvt ( m_Property.m_dwAlphaMax, 7, szValue );
	pWnd->SetWindowText ( szValue );

	m_comboMType.ResetContent ();

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strANI_MAINTYPE[AN_TYPE_SIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("ANI_MAINTYPE", strANI_MAINTYPE, CGameTextMan::EM_COMMENT_TEXT, AN_TYPE_SIZE);
		SetWin_Combo_Init ( this, IDC_COMBO_M_TYPE, strANI_MAINTYPE, AN_TYPE_SIZE );
		SetWin_Combo_Sel ( this, IDC_COMBO_M_TYPE, m_Property.m_AniMType );

		std::string strANI_SUBTYPE_00[AN_SUB_00_SIZE];
		for ( INT i = 0; i < AN_SUB_00_SIZE; ++i )
			strANI_SUBTYPE_00[i] = CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE_00", i);

		m_comboSType.ResetContent ();
		SetWin_Combo_Init ( this, IDC_COMBO_S_TYPE, strANI_SUBTYPE_00, AN_SUB_00_SIZE );
		SetWin_Combo_Sel ( this, IDC_COMBO_S_TYPE, m_Property.m_AniSType );
	}
	else
	{
		SetWin_Combo_Init ( this, IDC_COMBO_M_TYPE, COMMENT::ANI_MAINTYPE, AN_TYPE_SIZE );
		SetWin_Combo_Sel ( this, IDC_COMBO_M_TYPE, m_Property.m_AniMType );

		m_comboSType.ResetContent ();
		SetWin_Combo_Init ( this, IDC_COMBO_S_TYPE, COMMENT::ANI_SUBTYPE_00, AN_SUB_00_SIZE );
		SetWin_Combo_Sel ( this, IDC_COMBO_S_TYPE, m_Property.m_AniSType );
	}

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharGhosting::TYPEID,
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
	}
}

void CEffGhostingPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString	strText;
	CWnd *pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_NUM );
	pWnd->GetWindowText ( strText );
	m_Property.m_dwGhostingNum = atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_UNIT );
	pWnd->GetWindowText ( strText );
	m_Property.m_dwUnit = atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ALPHA );
	pWnd->GetWindowText ( strText );
	m_Property.m_dwAlphaMax = atoi ( strText.GetString() );

	int nIndex = m_comboMType.GetCurSel();
	m_Property.m_AniMType = (EMANI_MAINTYPE) m_comboMType.GetItemData(nIndex);

	nIndex = m_comboSType.GetCurSel();
	m_Property.m_AniSType = (EMANI_SUBTYPE) m_comboSType.GetItemData(nIndex);

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

    EffectApply();
}

void CEffGhostingPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

	ReturnPage();
}

void CEffGhostingPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();
}

void CEffGhostingPage::OnBnClickedButtonColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	D3DCOLOR &d3dColor = m_Property.m_dwColor;
	COLORREF retColor;
	COLORREF Color = RGB ( ((d3dColor&0xff0000)>>16), ((d3dColor&0xff00)>>8), (d3dColor&0xff) );
	CColorPickerDlg colorDlg ( Color );

	int	R,G,B;
//	R = (d3dColor&0xff0000)>>16;
//	G = (d3dColor&0xff00)>>8;
//	B = d3dColor&0xff;

	if ( IDOK == colorDlg.DoModal () )
	{
		retColor = colorDlg.GetColor ();

		R = GetRValue ( retColor );
		G = GetGValue ( retColor );
		B = GetBValue ( retColor );
		m_Property.m_dwColor = D3DCOLOR_ARGB ( 0x00, R, G, B );
	}
}

void CEffGhostingPage::OnCbnSelchangeComboMType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CEffGhostingPage::OnCbnSelchangeComboSType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
