// EffShockPage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"
#include "../Dialog/SetVar.h"
#include "../MfcExLib/ExLibs/MultiSlider.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "EffShockPage.h"
#include ".\effshockpage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffShockPage 대화 상자입니다.
IMPLEMENT_DYNAMIC(CEffShockPage, CPropertyPage)
CEffShockPage::CEffShockPage()
	: CEffCharPage(CEffShockPage::IDD)
{
}

CEffShockPage::~CEffShockPage()
{
}

void CEffShockPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffShockPage, CPropertyPage)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
END_MESSAGE_MAP()


// CEffShockPage 메시지 처리기입니다.

BOOL CEffShockPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CEffShockPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_SHOCK*)pProp);

	CWnd*	pWnd;
	char	szValue[256];

	pWnd = GetDlgItem(IDC_EDIT_TIME);
	_gcvt ( m_Property.m_fFullTime, 7, szValue );
	pWnd->SetWindowText ( szValue );
}

void CEffShockPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString	strText;
	CWnd	*pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_TIME );
	pWnd->GetWindowText ( strText );
	m_Property.m_fFullTime = (float)atof ( strText.GetString() );

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharShock::TYPEID,
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
	else
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}

    EffectApply();
}

void CEffShockPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffShockPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();
}

void CEffShockPage::OnBnClickedButtonColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( m_Property.m_cColor.r * 255.0f );
	int ThisG = int ( m_Property.m_cColor.g * 255.0f );
	int ThisB = int ( m_Property.m_cColor.b * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_Property.m_cColor.a = 1.f;
		m_Property.m_cColor.r = GetRValue(SelectColor)/255.0f;
		m_Property.m_cColor.g = GetGValue(SelectColor)/255.0f;
		m_Property.m_cColor.b = GetBValue(SelectColor)/255.0f;
	}
}
