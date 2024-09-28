// PointLightPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"
#include "EffSheetWithTab.h"

#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Single/DxEffectPointLight.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinCharData.h"
#include "EffGroupPage.h"

#include "../Dialog/ColorPickerDlg.h"
#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"

#include "PointLightPage.h"

// CPointLightPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPointLightPage, CPropertyPage)
CPointLightPage::CPointLightPage()
	: CPropertyPage(CPointLightPage::IDD)
{
}

CPointLightPage::~CPointLightPage()
{
}

void CPointLightPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPointLightPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_POWER, OnBnClickedButtonPower)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_SCALE, OnBnClickedButtonScale)
END_MESSAGE_MAP()


// CPointLightPage 메시지 처리기입니다.
void CPointLightPage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (POINTLIGHT_PROPERTY*) pProp;
}

void CPointLightPage::OnBnClickedButtonPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

	//	Note : 이펙트 생성.
	//
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );

	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
	DxEffGroupPlayer::GetInstance().NewEffGroup ( &CEffGroupPage::m_EffPropGroup, m_pProperty, matTrans );
}

void CPointLightPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString	strText;

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();
	m_pProperty->Create ( pView->GetD3dDevice(), FALSE );
}

void CPointLightPage::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CEffectToolView::m_curType = _CUR_NONE;
	m_pSheetTab->ActiveEffGroupPage ();

	//	Note : 속성을 적용.
	//
	OnBnClickedButtonApply();

	//	Note : Tap 적용
	//
	CEffectToolView::m_iSheetTap = 0;
}

void CPointLightPage::OnBnClickedButtonPower()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetMultiSlider DlgSetSlider;

	DlgSetSlider.m_StrTitle = "Set Point Light";

	DlgSetSlider.m_values[0] = m_pProperty->m_fPowerRate1;
	DlgSetSlider.m_values[1] = m_pProperty->m_fPowerRate2;

	DlgSetSlider.m_fStart = m_pProperty->m_fPowerStart;
	DlgSetSlider.m_fMid1 = m_pProperty->m_fPowerMid1;
	DlgSetSlider.m_fMid2 = m_pProperty->m_fPowerMid2;
	DlgSetSlider.m_fEnd = m_pProperty->m_fPowerEnd;

	if ( IDOK == DlgSetSlider.DoModal() )
	{
		m_pProperty->m_fPowerRate1	= DlgSetSlider.m_values[0];
		m_pProperty->m_fPowerRate2	= DlgSetSlider.m_values[1];
	
		m_pProperty->m_fPowerStart	= DlgSetSlider.m_fStart;
		m_pProperty->m_fPowerMid1	= DlgSetSlider.m_fMid1;
		m_pProperty->m_fPowerMid2	= DlgSetSlider.m_fMid2;
		m_pProperty->m_fPowerEnd	= DlgSetSlider.m_fEnd;
	}
}

void CPointLightPage::OnBnClickedButtonColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( m_pProperty->m_cColor.r * 255.0f );
	int ThisG = int ( m_pProperty->m_cColor.g * 255.0f );
	int ThisB = int ( m_pProperty->m_cColor.b * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_pProperty->m_cColor.r = GetRValue(SelectColor)/255.0f;
		m_pProperty->m_cColor.g = GetGValue(SelectColor)/255.0f;
		m_pProperty->m_cColor.b = GetBValue(SelectColor)/255.0f;
	}
}

void CPointLightPage::OnBnClickedButtonScale()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetMultiSlider DlgSetSlider;

	DlgSetSlider.m_StrTitle = "Set Point Light";

	DlgSetSlider.m_values[0] = m_pProperty->m_fScaleRate1;
	DlgSetSlider.m_values[1] = m_pProperty->m_fScaleRate2;

	DlgSetSlider.m_fStart = m_pProperty->m_fScaleStart;
	DlgSetSlider.m_fMid1 = m_pProperty->m_fScaleMid1;
	DlgSetSlider.m_fMid2 = m_pProperty->m_fScaleMid2;
	DlgSetSlider.m_fEnd = m_pProperty->m_fScaleEnd;

	if ( IDOK == DlgSetSlider.DoModal() )
	{
		m_pProperty->m_fScaleRate1	= DlgSetSlider.m_values[0];
		m_pProperty->m_fScaleRate2	= DlgSetSlider.m_values[1];
	
		m_pProperty->m_fScaleStart	= DlgSetSlider.m_fStart;
		m_pProperty->m_fScaleMid1	= DlgSetSlider.m_fMid1;
		m_pProperty->m_fScaleMid2	= DlgSetSlider.m_fMid2;
		m_pProperty->m_fScaleEnd	= DlgSetSlider.m_fEnd;
	}
}
