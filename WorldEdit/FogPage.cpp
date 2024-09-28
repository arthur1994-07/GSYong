// WeatherPage.cpp : ���� �����Դϴ�.
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

// CFogPage ��ȭ �����Դϴ�.

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


// CFogPage �޽��� ó�����Դϴ�.

BOOL CFogPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CWnd	*pWnd;
	CButton *pButton;
	char	szValue[256];

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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


	ReSetOptionRange ( m_sProperty.m_bFogStaticRange );	// �ɼ�.. !!
	ReSetOption ( m_sProperty.m_bSkyFogEnable );		// �ɼ�.. !!


	m_sSkyProperty = m_sOldSkyProperty = pView->GetSetLandMan()->GetSkyProperty();		// �ϴ����̴�.

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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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

	OnInitDialog();		// �ٽ� �ʱ�ȭ�� ��Ų��.
}

void CFogPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_RANGE_SETTING );
	if ( pButton->GetCheck() )	m_sProperty.m_bFogStaticRange = TRUE;
	else						m_sProperty.m_bFogStaticRange = FALSE;

	ReSetOptionRange ( m_sProperty.m_bFogStaticRange );		// �ɼ�.. !!
}

void CFogPage::OnBnClickedCheckDayNight()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_DAY_NIGHT );
	if ( pButton->GetCheck() )	m_sProperty.m_bSkyFogEnable = TRUE;
	else						m_sProperty.m_bSkyFogEnable = FALSE;

	ReSetOption ( m_sProperty.m_bSkyFogEnable );		// �ɼ�.. !!
}

void CFogPage::OnBnClickedButtonDiffuse()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	DxFogMan::GetInstance().SetFrameMode_Fog ( TRUE );

	OnInitDialog();								// �ٽ� �ʱ�ȭ �Ѵ�.

	return CPropertyPage::OnSetActive();
}

BOOL CFogPage::OnKillActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	DxFogMan::GetInstance().SetFrameMode_Fog ( FALSE );

	return CPropertyPage::OnKillActive();
}

void CFogPage::OnBnClickedCheckSky()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SKY );
	if ( pButton->GetCheck() )	m_sSkyProperty.m_bSkyEnable = TRUE;
	else						m_sSkyProperty.m_bSkyEnable = FALSE;
}

void CFogPage::OnBnClickedCheckSkyCloud()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SKY_CLOUD );
	if ( pButton->GetCheck() )	m_sSkyProperty.m_bCloudEnable = TRUE;
	else						m_sSkyProperty.m_bCloudEnable = FALSE;
}

void CFogPage::OnBnClickedRadioX()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_sSkyProperty.m_nRadioAxis = 0;
}

void CFogPage::OnBnClickedRadioY()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_sSkyProperty.m_nRadioAxis = 1;
}

void CFogPage::OnBnClickedRadioZ()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_sSkyProperty.m_nRadioAxis = 2;
}

void CFogPage::OnEnChangeEditAxisValue()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CPropertyPage::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strText;
	CWnd*	pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_AXIS_VALUE );
	pWnd->GetWindowText ( strText );
	m_sSkyProperty.m_fAxisValue = (float)atof ( strText.GetString() );
}

void CFogPage::OnBnClickedCheckSunMoon()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *pButton;

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SUN_MOON );
	if ( pButton->GetCheck() )	m_sSkyProperty.m_bSunMoon = TRUE;
	else						m_sSkyProperty.m_bSunMoon = FALSE;
}
