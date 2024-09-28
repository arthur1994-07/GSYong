// WavePage.cpp : ���� �����Դϴ�.
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


// CWavePage ��ȭ �����Դϴ�.

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


// CWavePage �޽��� ó�����Դϴ�.
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

	//	Note : ���� ��ư
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonApply();

	//	Note : ����Ʈ ����.
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

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();

	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();
	m_pProperty->Create ( pView->GetD3dDevice(), FALSE );
}

void CWavePage::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CEffectToolView::m_curType = _CUR_NONE;
	m_pSheetTab->ActiveEffGroupPage ();

	//	Note : �Ӽ��� ����.
	//
	OnBnClickedButtonApply();

	//	Note : Tap ����
	//
	CEffectToolView::m_iSheetTap = 0;
}

void CWavePage::OnBnClickedButtonScale()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CSetMultiSlider DlgSetSlider;

	DlgSetSlider.m_StrTitle = "Set Scale";
	DlgSetSlider.m_StrTemp4 = "������";
	DlgSetSlider.m_StrWidth = "���� :";
	DlgSetSlider.m_StrTemp = "���� :";

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CSetMultiSlider DlgSetSlider;

	DlgSetSlider.m_StrTitle = "Set Alpha";
	DlgSetSlider.m_StrTemp4 = "���İ�";

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CSetMultiSlider DlgSetSlider;

	DlgSetSlider.m_StrTitle = "Set Scale";
	DlgSetSlider.m_StrTemp4 = "������";
	DlgSetSlider.m_StrWidth = "���� :";
	DlgSetSlider.m_StrTemp = "���� :";

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pProperty->m_dwFlag |= USEBILLBOARD;
	m_pProperty->m_dwFlag &= ~USEGROUND;
}

void CWavePage::OnBnClickedRadioGround()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pProperty->m_dwFlag |= USEGROUND;
	m_pProperty->m_dwFlag &= ~USEBILLBOARD;
}

void CWavePage::OnBnClickedRadioRandom()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pProperty->m_emDrawMode = EM_RANDOM;
}

void CWavePage::OnBnClickedRadioWave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pProperty->m_emDrawMode = EM_WAVE;
}

void CWavePage::OnBnClickedRadioEmission()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pProperty->m_emDrawMode = EM_EMISSION;
}

void CWavePage::OnBnClickedRadioCollect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pProperty->m_emDrawMode = EM_COLLECT;
}
