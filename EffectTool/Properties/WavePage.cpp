// WavePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"

#include "../EffectToolDoc.h"
#include "../EffectToolView.h"
#include "EffSheetWithTab.h"

#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "EffGroupPage.h"

#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinCharData.h"

#include "../../EngineLib/DxEffect/Single/DxEffectWave.h"

#include "../Dialog/ColorPickerDlg.h"
#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"
#include "Wavepage.h"


// CWavePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWavePage, CPropertyPage)
CWavePage::CWavePage()
	: CPropertyPage(CWavePage::IDD)
{
}

CWavePage::~CWavePage()
{
}

void CWavePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWavePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_SCALE, OnBnClickedButtonScale)
	ON_BN_CLICKED(IDC_BUTTON_ALPHA, OnBnClickedButtonAlpha)
	ON_BN_CLICKED(IDC_BUTTON_WAVE, OnBnClickedButtonWave)
	ON_BN_CLICKED(IDC_RADIO_BILLBOARD, OnBnClickedRadioBillboard)
	ON_BN_CLICKED(IDC_RADIO_GROUND, OnBnClickedRadioGround)
	ON_BN_CLICKED(IDC_RADIO_RANDOM, OnBnClickedRadioRandom)
	ON_BN_CLICKED(IDC_RADIO_WAVE, OnBnClickedRadioWave)
	ON_BN_CLICKED(IDC_RADIO_EMISSION, OnBnClickedRadioEmission)
	ON_BN_CLICKED(IDC_RADIO_COLLECT, OnBnClickedRadioCollect)
END_MESSAGE_MAP()


// CWavePage 메시지 처리기입니다.
void CWavePage::SetProperty ( EFF_PROPERTY* pProp )
{
	m_pProperty = (WAVE_PROPERTY*) pProp;

	char		szValue[256];
	CWnd*		pWnd;
	CButton*	pButton;

	pWnd = GetDlgItem ( IDC_EDIT_FLAREPOS );
	_gcvt ( m_pProperty->m_fFlarePos, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SPEED );
	_gcvt ( m_pProperty->m_fSpeed, 7, szValue );
	pWnd->SetWindowText ( szValue );

	//	Note : 라디오 버튼
	//
	if ( m_pProperty->m_dwFlag & USEBILLBOARD )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_BILLBOARD );
		pButton->SetCheck ( 1 );

		pButton = (CButton*) GetDlgItem ( IDC_RADIO_GROUND );
		pButton->SetCheck ( 0 );

	}
	else if ( m_pProperty->m_dwFlag & USEGROUND )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_BILLBOARD );
		pButton->SetCheck ( 0 );

		pButton = (CButton*) GetDlgItem ( IDC_RADIO_GROUND );
		pButton->SetCheck ( 1 );
	}

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_RANDOM );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_WAVE );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_EMISSION );
	pButton->SetCheck ( FALSE );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_COLLECT );
	pButton->SetCheck ( FALSE );

	if ( m_pProperty->m_emDrawMode==EM_RANDOM )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_RANDOM );
		pButton->SetCheck ( TRUE );
	}
	else if ( m_pProperty->m_emDrawMode==EM_WAVE )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_WAVE );
		pButton->SetCheck ( TRUE );
	}
	else if ( m_pProperty->m_emDrawMode==EM_EMISSION )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_EMISSION );
		pButton->SetCheck ( TRUE );
	}
	else if ( m_pProperty->m_emDrawMode==EM_COLLECT )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_COLLECT );
		pButton->SetCheck ( TRUE );
	}
}

void CWavePage::OnBnClickedButtonPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

	//	Note : 이펙트 생성.
	//
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );

	DxEffGroupPlayer::GetInstance().SetTime ( 0.0f );
	DxEffGroupPlayer::GetInstance().NewEffGroup( &CEffGroupPage::m_EffPropGroup, m_pProperty, matTrans );
}

void CWavePage::OnBnClickedButtonApply()
{
	CString	strText;
	CWnd*	pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_FLAREPOS );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fFlarePos = (float) atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SPEED );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fSpeed = (float) atof ( strText.GetString() );

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();
	m_pProperty->Create ( pView->GetD3dDevice(), FALSE );
}

void CWavePage::OnBnClickedButtonOk()
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

void CWavePage::OnBnClickedButtonScale()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetMultiSlider DlgSetSlider;

	DlgSetSlider.m_StrTitle = "Set Scale";
	DlgSetSlider.m_StrTemp4 = "스케일";
	DlgSetSlider.m_StrWidth = "가로 :";
	DlgSetSlider.m_StrTemp = "세로 :";

	DlgSetSlider.m_dwFlag |= USEWIDTH;

	DlgSetSlider.m_values[0] = m_pProperty->m_fSizeRate1;
	DlgSetSlider.m_values[1] = m_pProperty->m_fSizeRate2;

	DlgSetSlider.m_fStart = m_pProperty->m_fSizeStart;
	DlgSetSlider.m_fMid1 = m_pProperty->m_fSizeMid1;
	DlgSetSlider.m_fMid2 = m_pProperty->m_fSizeMid2;
	DlgSetSlider.m_fEnd = m_pProperty->m_fSizeEnd;

//	DlgSetSlider.m_fWidth = m_pProperty->m_fWidthRate;
//	DlgSetSlider.m_fTemp = m_pProperty->m_fHeightRate;
	
	if ( IDOK == DlgSetSlider.DoModal() )
	{
		m_pProperty->m_fSizeRate1	= DlgSetSlider.m_values[0];
		m_pProperty->m_fSizeRate2	= DlgSetSlider.m_values[1];
	
		m_pProperty->m_fSizeStart	= DlgSetSlider.m_fStart;
		m_pProperty->m_fSizeMid1	= DlgSetSlider.m_fMid1;
		m_pProperty->m_fSizeMid2	= DlgSetSlider.m_fMid2;
		m_pProperty->m_fSizeEnd		= DlgSetSlider.m_fEnd;

//		m_pProperty->m_fWidthRate	= DlgSetSlider.m_fWidth;
//		m_pProperty->m_fHeightRate	= DlgSetSlider.m_fTemp;
	}
}

void CWavePage::OnBnClickedButtonAlpha()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetMultiSlider DlgSetSlider;

	DlgSetSlider.m_StrTitle = "Set Alpha";
	DlgSetSlider.m_StrTemp4 = "알파값";

	DlgSetSlider.m_values[0] = m_pProperty->m_fAlphaRate1;
	DlgSetSlider.m_values[1] = m_pProperty->m_fAlphaRate2;

	DlgSetSlider.m_fStart = m_pProperty->m_fAlphaStart;
	DlgSetSlider.m_fMid1 = m_pProperty->m_fAlphaMid1;
	DlgSetSlider.m_fMid2 = m_pProperty->m_fAlphaMid2;
	DlgSetSlider.m_fEnd = m_pProperty->m_fAlphaEnd;
	
	if ( IDOK == DlgSetSlider.DoModal() )
	{
		m_pProperty->m_fAlphaRate1 = DlgSetSlider.m_values[0];
		m_pProperty->m_fAlphaRate2 = DlgSetSlider.m_values[1];
	
		m_pProperty->m_fAlphaStart = DlgSetSlider.m_fStart;
		m_pProperty->m_fAlphaMid1 = DlgSetSlider.m_fMid1;
		m_pProperty->m_fAlphaMid2 = DlgSetSlider.m_fMid2;
		m_pProperty->m_fAlphaEnd = DlgSetSlider.m_fEnd;
	}
}

void CWavePage::OnBnClickedButtonWave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetMultiSlider DlgSetSlider;

	DlgSetSlider.m_StrTitle = "Set Scale";
	DlgSetSlider.m_StrTemp4 = "스케일";
	DlgSetSlider.m_StrWidth = "가로 :";
	DlgSetSlider.m_StrTemp = "세로 :";

	DlgSetSlider.m_dwFlag |= USEWIDTH;

	DlgSetSlider.m_values[0] = m_pProperty->m_fBumpWaveRate1;
	DlgSetSlider.m_values[1] = m_pProperty->m_fBumpWaveRate2;

	DlgSetSlider.m_fStart = m_pProperty->m_fBumpWaveStart;
	DlgSetSlider.m_fMid1 = m_pProperty->m_fBumpWaveMid1;
	DlgSetSlider.m_fMid2 = m_pProperty->m_fBumpWaveMid2;
	DlgSetSlider.m_fEnd = m_pProperty->m_fBumpWaveEnd;

//	DlgSetSlider.m_fWidth = m_pProperty->m_fWidthRate;
//	DlgSetSlider.m_fTemp = m_pProperty->m_fHeightRate;
	
	if ( IDOK == DlgSetSlider.DoModal() )
	{
		m_pProperty->m_fBumpWaveRate1	= DlgSetSlider.m_values[0];
		m_pProperty->m_fBumpWaveRate2	= DlgSetSlider.m_values[1];
	
		m_pProperty->m_fBumpWaveStart	= DlgSetSlider.m_fStart;
		m_pProperty->m_fBumpWaveMid1	= DlgSetSlider.m_fMid1;
		m_pProperty->m_fBumpWaveMid2	= DlgSetSlider.m_fMid2;
		m_pProperty->m_fBumpWaveEnd		= DlgSetSlider.m_fEnd;

//		m_pProperty->m_fWidthRate	= DlgSetSlider.m_fWidth;
//		m_pProperty->m_fHeightRate	= DlgSetSlider.m_fTemp;
	}
}

void CWavePage::OnBnClickedRadioBillboard()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag |= USEBILLBOARD;
	m_pProperty->m_dwFlag &= ~USEGROUND;
}

void CWavePage::OnBnClickedRadioGround()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_dwFlag |= USEGROUND;
	m_pProperty->m_dwFlag &= ~USEBILLBOARD;
}

void CWavePage::OnBnClickedRadioRandom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_emDrawMode = EM_RANDOM;
}

void CWavePage::OnBnClickedRadioWave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_emDrawMode = EM_WAVE;
}

void CWavePage::OnBnClickedRadioEmission()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_emDrawMode = EM_EMISSION;
}

void CWavePage::OnBnClickedRadioCollect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pProperty->m_emDrawMode = EM_COLLECT;
}
