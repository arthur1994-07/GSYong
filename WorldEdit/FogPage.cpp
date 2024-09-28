// WeatherPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"

#include "ColorPickerDlg.h"
#include "sheetWithTab.h"

#include "../EngineLib/DxLand/DxSetLandMan.h"

#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "FogPage.h"
#include ".\fogpage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFogPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CFogPage, CPropertyPage)
CFogPage::CFogPage()
	: CPropertyPage(CFogPage::IDD),
	m_bDlgInit(FALSE)
{
}

CFogPage::~CFogPage()
{
}

void CFogPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFogPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE, OnBnClickedButtonDiffuse)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE_NIGHT, OnBnClickedButtonDiffuseNight)
	ON_BN_CLICKED(IDC_CHECK_SKY, OnBnClickedCheckSky)
	ON_BN_CLICKED(IDC_CHECK_DAY_NIGHT, OnBnClickedCheckDayNight)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE_DAY, OnBnClickedButtonDiffuseDay)
	ON_BN_CLICKED(IDC_CHECK_SKY_CLOUD, OnBnClickedCheckSkyCloud)
	ON_BN_CLICKED(IDC_CHECK_RANGE_SETTING, OnBnClickedCheckRangeSetting)
	ON_BN_CLICKED(IDC_RADIO_X, OnBnClickedRadioX)
	ON_BN_CLICKED(IDC_RADIO_Y, OnBnClickedRadioY)
	ON_BN_CLICKED(IDC_RADIO_Z, OnBnClickedRadioZ)
	ON_EN_CHANGE(IDC_EDIT_AXIS_VALUE, OnEnChangeEditAxisValue)
	ON_BN_CLICKED(IDC_CHECK_SUN_MOON, &CFogPage::OnBnClickedCheckSunMoon)
END_MESSAGE_MAP()


// CFogPage 메시지 처리기입니다.

BOOL CFogPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CWnd	*pWnd;
	CButton *pButton;
	char	szValue[256];

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//m_sOldProperty	= m_sProperty	= DxFogMan::GetInstance().GetProperty();
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_sProperty = m_sOldProperty = pView->GetSetLandMan()->GetFogProperty();

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_RANGE_SETTING );
	pButton->SetCheck ( m_sProperty.m_bFogStaticRange );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_DAY_NIGHT );
	pButton->SetCheck ( m_sProperty.m_bSkyFogEnable );


	pWnd = GetDlgItem ( IDC_EDIT_START );
	_gcvt ( m_sProperty.m_fFogStart, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_END );
	_gcvt ( m_sProperty.m_fFogEnd, 7, szValue );
	pWnd->SetWindowText ( szValue );


	ReSetOptionRange ( m_sProperty.m_bFogStaticRange );	// 옵션.. !!
	ReSetOption ( m_sProperty.m_bSkyFogEnable );		// 옵션.. !!


	m_sSkyProperty = m_sOldSkyProperty = pView->GetSetLandMan()->GetSkyProperty();		// 하늘쪽이닷.

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SKY );
	pButton->SetCheck ( m_sSkyProperty.m_bSkyEnable );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SKY_CLOUD );
	pButton->SetCheck ( m_sSkyProperty.m_bCloudEnable );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SUN_MOON );
	pButton->SetCheck ( m_sSkyProperty.m_bSunMoon );


	pButton = (CButton*) GetDlgItem ( IDC_RADIO_X );
	pButton->SetCheck ( 0 );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_Y );
	pButton->SetCheck ( 0 );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_Z );
	pButton->SetCheck ( 0 );

	if ( m_sSkyProperty.m_nRadioAxis==0 )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_X );
		pButton->SetCheck ( 1 );
	}
	else if ( m_sSkyProperty.m_nRadioAxis==1 )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_Y );
		pButton->SetCheck ( 1 );
	}
	else if ( m_sSkyProperty.m_nRadioAxis==2 )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_Z );
		pButton->SetCheck ( 1 );
	}

	pWnd = GetDlgItem ( IDC_EDIT_AXIS_VALUE );
	_gcvt ( m_sSkyProperty.m_fAxisValue, 7, szValue );
	pWnd->SetWindowText ( szValue );

	m_bDlgInit = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CFogPage::ReSetOptionRange ( BOOL bUse )
{
	CWnd	*pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_START );
	pWnd->EnableWindow ( bUse );

	pWnd = GetDlgItem ( IDC_EDIT_END );
	pWnd->EnableWindow ( bUse );
}

void CFogPage::ReSetOption ( BOOL bUse )
{
	CWnd	*pWnd;

	pWnd = GetDlgItem ( IDC_BUTTON_DIFFUSE );
	pWnd->EnableWindow ( !bUse );

	pWnd = GetDlgItem ( IDC_BUTTON_DIFFUSE_DAY );
	pWnd->EnableWindow ( bUse );

	pWnd = GetDlgItem ( IDC_BUTTON_DIFFUSE_NIGHT );
	pWnd->EnableWindow ( bUse );
}

void CFogPage::UpdateContrl ()
{
	if ( !m_bDlgInit )	return;

	OnInitDialog();		// 다시 초기화를 시킨다.
}

void CFogPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd *pWnd;

	CString strText;

	pWnd = GetDlgItem ( IDC_EDIT_START );
	pWnd->GetWindowText ( strText );
	m_sProperty.m_fFogStart = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_END );
	pWnd->GetWindowText ( strText );
	m_sProperty.m_fFogEnd = (float)atof ( strText.GetString() );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetSetLandMan()->SetFogProperty ( m_sProperty );
	pView->GetLandMan()->SetFogProperty ( m_sProperty );

	pView->GetSetLandMan()->SetSkyProperty ( m_sSkyProperty );
	pView->GetLandMan()->SetSkyProperty ( m_sSkyProperty );
}

void CFogPage::OnBnClickedCheckRangeSetting()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_RANGE_SETTING );
	if ( pButton->GetCheck() )	m_sProperty.m_bFogStaticRange = TRUE;
	else						m_sProperty.m_bFogStaticRange = FALSE;

	ReSetOptionRange ( m_sProperty.m_bFogStaticRange );		// 옵션.. !!
}

void CFogPage::OnBnClickedCheckDayNight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_DAY_NIGHT );
	if ( pButton->GetCheck() )	m_sProperty.m_bSkyFogEnable = TRUE;
	else						m_sProperty.m_bSkyFogEnable = FALSE;

	ReSetOption ( m_sProperty.m_bSkyFogEnable );		// 옵션.. !!
}

void CFogPage::OnBnClickedButtonDiffuse()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD dwColor = m_sProperty.m_cFogColor;

	COLORREF retColor;
	COLORREF Color = RGB ( ((dwColor&0xff0000)>>16), ((dwColor&0xff00)>>8), dwColor&0xff );
	CColorPickerDlg colorDlg ( Color );

	if ( IDOK == colorDlg.DoModal () )
	{
		int	R,G,B;
		retColor = colorDlg.GetColor ();

		R = GetRValue ( retColor );
		G = GetGValue ( retColor );
		B = GetBValue ( retColor );

		dwColor = 0xff<<24;
		dwColor += R<<16;
		dwColor += G<<8;
		dwColor += B;

		m_sProperty.m_cFogColor = dwColor;

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		pView->GetSetLandMan()->SetFogProperty ( m_sProperty );
	}
}

void CFogPage::OnBnClickedButtonDiffuseDay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD dwColor = m_sProperty.m_cFogColor_Day;

	COLORREF retColor;
	COLORREF Color = RGB ( ((dwColor&0xff0000)>>16), ((dwColor&0xff00)>>8), dwColor&0xff );
	CColorPickerDlg colorDlg ( Color );

	if ( IDOK == colorDlg.DoModal () )
	{
		int	R,G,B;
		retColor = colorDlg.GetColor ();

		R = GetRValue ( retColor );
		G = GetGValue ( retColor );
		B = GetBValue ( retColor );

		dwColor = 0xff<<24;
		dwColor += R<<16;
		dwColor += G<<8;
		dwColor += B;

		m_sProperty.m_cFogColor_Day = dwColor;

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		pView->GetSetLandMan()->SetFogProperty ( m_sProperty );
	}
}

void CFogPage::OnBnClickedButtonDiffuseNight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD dwColor = m_sProperty.m_cFogColor_Night;

	COLORREF retColor;
	COLORREF Color = RGB ( ((dwColor&0xff0000)>>16), ((dwColor&0xff00)>>8), dwColor&0xff );
	CColorPickerDlg colorDlg ( Color );

	if ( IDOK == colorDlg.DoModal () )
	{
		int	R,G,B;
		retColor = colorDlg.GetColor ();

		R = GetRValue ( retColor );
		G = GetGValue ( retColor );
		B = GetBValue ( retColor );

		dwColor = 0xff<<24;
		dwColor += R<<16;
		dwColor += G<<8;
		dwColor += B;

		m_sProperty.m_cFogColor_Night = dwColor;

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		pView->GetSetLandMan()->SetFogProperty ( m_sProperty );
	}
}

BOOL CFogPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	DxFogMan::GetInstance().SetFrameMode_Fog ( TRUE );

	OnInitDialog();								// 다시 초기화 한다.

	return CPropertyPage::OnSetActive();
}

BOOL CFogPage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	DxFogMan::GetInstance().SetFrameMode_Fog ( FALSE );

	return CPropertyPage::OnKillActive();
}

void CFogPage::OnBnClickedCheckSky()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SKY );
	if ( pButton->GetCheck() )	m_sSkyProperty.m_bSkyEnable = TRUE;
	else						m_sSkyProperty.m_bSkyEnable = FALSE;
}

void CFogPage::OnBnClickedCheckSkyCloud()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SKY_CLOUD );
	if ( pButton->GetCheck() )	m_sSkyProperty.m_bCloudEnable = TRUE;
	else						m_sSkyProperty.m_bCloudEnable = FALSE;
}

void CFogPage::OnBnClickedRadioX()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_sSkyProperty.m_nRadioAxis = 0;
}

void CFogPage::OnBnClickedRadioY()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_sSkyProperty.m_nRadioAxis = 1;
}

void CFogPage::OnBnClickedRadioZ()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_sSkyProperty.m_nRadioAxis = 2;
}

void CFogPage::OnEnChangeEditAxisValue()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strText;
	CWnd*	pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_AXIS_VALUE );
	pWnd->GetWindowText ( strText );
	m_sSkyProperty.m_fAxisValue = (float)atof ( strText.GetString() );
}

void CFogPage::OnBnClickedCheckSunMoon()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SUN_MOON );
	if ( pButton->GetCheck() )	m_sSkyProperty.m_bSunMoon = TRUE;
	else						m_sSkyProperty.m_bSunMoon = FALSE;
}
