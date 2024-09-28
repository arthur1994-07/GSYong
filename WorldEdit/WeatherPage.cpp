// WeatherPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"

#include "ColorPickerDlg.h"
#include "sheetWithTab.h"

#include "../EngineLib/G-Logic/GLPeriod.h"

#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "WeatherPage.h"
#include ".\weatherpage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWeatherPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWeatherPage, CPropertyPage)
CWeatherPage::CWeatherPage()
	: CPropertyPage(CWeatherPage::IDD),
	m_bDlgInit(FALSE)
{
}

CWeatherPage::~CWeatherPage()
{
}

void CWeatherPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWeatherPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_WINDDIR, OnBnClickedButtonWinddir)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_DIFF_F, OnBnClickedButtonDiffF)
	ON_BN_CLICKED(IDC_CHECK_RAIN, OnBnClickedCheckRain)
	ON_BN_CLICKED(IDC_CHECK_SNOW, OnBnClickedCheckSnow)
	ON_BN_CLICKED(IDC_CHECK_LEAVES, OnBnClickedCheckLeaves)
	ON_BN_CLICKED(IDC_CHECK_RAIN_DOING, OnBnClickedCheckRainDoing)
	ON_BN_CLICKED(IDC_CHECK_SNOW_DOING, OnBnClickedCheckSnowDoing)
	ON_BN_CLICKED(IDC_CHECK_LEAVES_DOING, OnBnClickedCheckLeavesDoing)
	ON_BN_CLICKED(IDC_CHECK_ACTIVE, OnBnClickedCheckActive)
END_MESSAGE_MAP()


// CWeatherPage 메시지 처리기입니다.

BOOL CWeatherPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CButton *pButton;

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_sOldRainProp	= m_sRainProp	= DxWeatherMan::GetInstance()->GetRain()->GetProperty();	
	m_sOldSnowProp	= m_sSnowProp	= DxWeatherMan::GetInstance()->GetSnow()->GetProperty();
	m_sOldCloudProp	= m_sCloudProp	= DxWeatherMan::GetInstance()->GetCloud()->GetProperty();
	m_sOldProperty	= m_sProperty	= DxWeatherMan::GetInstance()->GetProperty();


	pButton = (CButton*)GetDlgItem ( IDC_CHECK_ACTIVE );
	if ( m_sProperty.m_dwFlag & ISACTIVE )		pButton->SetCheck (TRUE);
	else										pButton->SetCheck (FALSE);

	//	Note : 비또는 눈 확인
	//
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_RAIN );
	if ( m_sProperty.m_dwFlag & ISRAIN )		pButton->SetCheck (TRUE);
	else										pButton->SetCheck (FALSE);

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SNOW );
	if ( m_sProperty.m_dwFlag & ISSNOW )		pButton->SetCheck (TRUE);
	else										pButton->SetCheck (FALSE);

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_LEAVES );
	if ( m_sProperty.m_dwFlag & ISLEAVES )		pButton->SetCheck (TRUE);
	else										pButton->SetCheck (FALSE);

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_RAIN_DOING );
	if ( m_sProperty.m_dwFlag & ISRAINDOING )	pButton->SetCheck (TRUE);
	else										pButton->SetCheck (FALSE);

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SNOW_DOING );
	if ( m_sProperty.m_dwFlag & ISSNOWDOING )	pButton->SetCheck (TRUE);
	else										pButton->SetCheck (FALSE);

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_LEAVES_DOING );
	if ( m_sProperty.m_dwFlag & ISLEAVESDOING )	pButton->SetCheck (TRUE);
	else										pButton->SetCheck (FALSE);

	//	Note : 활성화, 비활성화 셋팅
	//
	ReSetCheckBox ();


	char szValue[256];

	D3DXVECTOR3& vPos = *DxWeatherMan::GetInstance()->GetWindDirec();

	_gcvt ( vPos.x, 7, szValue );
	GetDlgItem(IDC_EDIT_DIRX)->SetWindowText(szValue);
	_gcvt ( vPos.y, 7, szValue );
	GetDlgItem(IDC_EDIT_DIRY)->SetWindowText(szValue);
	_gcvt ( vPos.z, 7, szValue );
	GetDlgItem(IDC_EDIT_DIRZ)->SetWindowText(szValue);

	_gcvt ( DxWeatherMan::GetInstance()->GetWindPower(), 7, szValue );
	GetDlgItem(IDC_EDIT_WIND_FORCE)->SetWindowText(szValue);



	_gcvt ( m_sRainProp.m_dwFaces, 7, szValue );
	GetDlgItem(IDC_EDIT_RAINNUM)->SetWindowText(szValue);

	_gcvt ( m_sRainProp.m_iRadius, 7, szValue );
	GetDlgItem(IDC_EDIT_RADIUS)->SetWindowText(szValue);

	_gcvt ( m_sRainProp.m_iHeight, 7, szValue );
	GetDlgItem(IDC_EDIT_HEIGHT)->SetWindowText(szValue);

	_gcvt ( m_sRainProp.m_fLength, 7, szValue );
	GetDlgItem(IDC_EDIT_LENGTH)->SetWindowText(szValue);

	_gcvt ( m_sRainProp.m_fSpeed, 7, szValue );
	GetDlgItem(IDC_EDIT_SPEED)->SetWindowText(szValue);

	D3DCOLOR d3dColor = m_sRainProp.m_vColor;

	sprintf ( szValue, "%d", int ((d3dColor&0xff0000)>>16) );
	GetDlgItem(IDC_EDIT_DR)->SetWindowText(szValue);
	sprintf ( szValue, "%d", int ((d3dColor&0xff00)>>8) );
	GetDlgItem(IDC_EDIT_DG)->SetWindowText(szValue);
	sprintf ( szValue, "%d", int (d3dColor&0xff) );
	GetDlgItem(IDC_EDIT_DB)->SetWindowText(szValue);
	
	m_bDlgInit = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CWeatherPage::UpdateContrl ()
{
	if ( !m_bDlgInit )	return;

	OnInitDialog();		// 다시 초기화를 시킨다.
}

void CWeatherPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd *pWnd;

	CString strText;

	pWnd = GetDlgItem ( IDC_EDIT_WIND_FORCE );
	pWnd->GetWindowText ( strText );
	m_sProperty.m_fWindPower = (float)atof (strText.GetString());

	pWnd = GetDlgItem ( IDC_EDIT_RAINNUM );
	pWnd->GetWindowText ( strText );
	m_sRainProp.m_dwFaces = atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_RADIUS );
	pWnd->GetWindowText ( strText );
	m_sRainProp.m_iRadius = atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_HEIGHT );
	pWnd->GetWindowText ( strText );
	m_sRainProp.m_iHeight = atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_LENGTH );
	pWnd->GetWindowText ( strText );
	m_sRainProp.m_fLength = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SPEED );
	pWnd->GetWindowText ( strText );
	m_sRainProp.m_fSpeed = (float)atof ( strText.GetString() );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	DxWeatherMan::GetInstance()->GetRain()->SetProperty ( m_sRainProp );
	DxWeatherMan::GetInstance()->GetSnow()->SetProperty ( m_sSnowProp );
	DxWeatherMan::GetInstance()->GetCloud()->SetProperty ( m_sCloudProp );
	DxWeatherMan::GetInstance()->SetProperty ( m_sProperty );
	DxWeatherMan::GetInstance()->GetRain()->ResetMesh ( pView->GetD3DDevice() );
}

void CWeatherPage::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_sRainProp		= m_sOldRainProp;
	m_sSnowProp		= m_sOldSnowProp;
	m_sCloudProp	= m_sOldCloudProp;
	m_sProperty		= m_sOldProperty;

}

void CWeatherPage::OnBnClickedButtonWinddir()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	m_pSheetTab->ActiveDirectionPage ( (D3DXVECTOR3*) DxWeatherMan::GetInstance()->GetWindDirec(), WEATHERPAGEINDEX );
}

BOOL CWeatherPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	D3DXVECTOR3& vPos = *DxWeatherMan::GetInstance()->GetWindDirec();

	char szValue[256];
	_gcvt ( vPos.x, 7, szValue );
	GetDlgItem(IDC_EDIT_DIRX)->SetWindowText(szValue);
	_gcvt ( vPos.y, 7, szValue );
	GetDlgItem(IDC_EDIT_DIRY)->SetWindowText(szValue);
	_gcvt ( vPos.z, 7, szValue );
	GetDlgItem(IDC_EDIT_DIRZ)->SetWindowText(szValue);

	OnInitDialog();								// 다시 초기화 한다.

	return CPropertyPage::OnSetActive();
}

void CWeatherPage::OnBnClickedButtonDiffF()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	D3DCOLOR &d3dColor = m_sRainProp.m_vColor;
	COLORREF retColor;
	COLORREF Color = RGB ( ((d3dColor&0xff0000)>>16), ((d3dColor&0xff00)>>8), (d3dColor&0xff) );
	CColorPickerDlg colorDlg ( Color );

	int	R,G,B;
	R = (d3dColor&0xff0000)>>16;
	G = (d3dColor&0xff00)>>8;
	B = d3dColor&0xff;

	if ( IDOK == colorDlg.DoModal () )
	{
		DWORD dwAlpha = ((d3dColor&0xff000000)>>24);

		retColor = colorDlg.GetColor ();

		R = GetRValue ( retColor );
		G = GetGValue ( retColor );
		B = GetBValue ( retColor );
		m_sRainProp.m_vColor = D3DCOLOR_ARGB ( dwAlpha, R, G, B );
	}
	
	char szValue[128];
	sprintf ( szValue, "%d", R );
	GetDlgItem(IDC_EDIT_DR)->SetWindowText(szValue);
	sprintf ( szValue, "%d", G );
	GetDlgItem(IDC_EDIT_DG)->SetWindowText(szValue);
	sprintf ( szValue, "%d", B );
	GetDlgItem(IDC_EDIT_DB)->SetWindowText(szValue);


	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	DxWeatherMan::GetInstance()->GetRain()->ResetMesh ( pView->GetD3DDevice() );
}

void CWeatherPage::OnBnClickedCheckRain()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_RAIN );
	if ( pButton->GetCheck() )		m_sProperty.m_dwFlag |= ISRAIN;
	else							m_sProperty.m_dwFlag &= ~ISRAIN;

	//	Note : 활성화 비활성화 셋팅
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_RAIN_DOING );
	if ( m_sProperty.m_dwFlag & ISRAIN )	pButton->EnableWindow ( TRUE );
	else									pButton->EnableWindow ( FALSE );
}

void CWeatherPage::OnBnClickedCheckSnow()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SNOW );
	if ( pButton->GetCheck() )		m_sProperty.m_dwFlag |= ISSNOW;
	else							m_sProperty.m_dwFlag &= ~ISSNOW;

	//	Note : 활성화 비활성화 셋팅
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SNOW_DOING );
	if ( m_sProperty.m_dwFlag & ISSNOW )	pButton->EnableWindow ( TRUE );
	else									pButton->EnableWindow ( FALSE );
}

void CWeatherPage::OnBnClickedCheckLeaves()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_LEAVES );
	if ( pButton->GetCheck() )		m_sProperty.m_dwFlag |= ISLEAVES;
	else							m_sProperty.m_dwFlag &= ~ISLEAVES;

	//	Note : 활성화 비활성화 셋팅
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_LEAVES_DOING );
	if ( m_sProperty.m_dwFlag & ISLEAVES )	pButton->EnableWindow ( TRUE );
	else									pButton->EnableWindow ( FALSE );
}

void CWeatherPage::OnBnClickedCheckRainDoing()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_RAIN_DOING );

	DWORD	dwWeather = GLPeriod::GetInstance().GetWeather();

	if ( pButton->GetCheck() )
	{
		m_sProperty.m_dwFlag |= ISRAINDOING;
		dwWeather |= FGW_RAIN;
	}
	else
	{
		m_sProperty.m_dwFlag &= ~ISRAINDOING;
		dwWeather &= ~FGW_RAIN;
	}

	GLPeriod::GetInstance().SetWeather( dwWeather );
}

void CWeatherPage::OnBnClickedCheckSnowDoing()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SNOW_DOING );

	DWORD	dwWeather = GLPeriod::GetInstance().GetWeather();

	if ( pButton->GetCheck() )	
	{
		m_sProperty.m_dwFlag |= ISSNOWDOING;
		dwWeather |= FGW_SNOW;
	}
	else
	{
		m_sProperty.m_dwFlag &= ~ISSNOWDOING;
		dwWeather &= ~FGW_SNOW;
	}

	GLPeriod::GetInstance().SetWeather( dwWeather );
}

void CWeatherPage::OnBnClickedCheckLeavesDoing()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_LEAVES_DOING );

	DWORD	dwWeather = GLPeriod::GetInstance().GetWeather();

	if ( pButton->GetCheck() )	
	{
		m_sProperty.m_dwFlag |= ISLEAVESDOING;
		dwWeather |= FGW_LEAVES;
	}
	else		
	{
		m_sProperty.m_dwFlag &= ~ISLEAVESDOING;
		dwWeather &= ~FGW_LEAVES;
	}

	GLPeriod::GetInstance().SetWeather( dwWeather );
}

void CWeatherPage::OnBnClickedCheckActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_ACTIVE );
	if ( pButton->GetCheck() )		m_sProperty.m_dwFlag |= ISACTIVE;
	else							m_sProperty.m_dwFlag &= ~ISACTIVE;

	ReSetCheckBox ();
}

void CWeatherPage::ReSetCheckBox ()
{
	CButton*	pButton;

	//	Note : 활성화 비활성화 셋팅
	//
	((CButton*)GetDlgItem ( IDC_CHECK_RAIN ))->EnableWindow ( TRUE );
	((CButton*)GetDlgItem ( IDC_CHECK_SNOW ))->EnableWindow ( TRUE );
	((CButton*)GetDlgItem ( IDC_CHECK_LEAVES ))->EnableWindow ( TRUE );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_RAIN_DOING );
	if ( m_sProperty.m_dwFlag & ISRAIN )	pButton->EnableWindow ( TRUE );
	else									pButton->EnableWindow ( FALSE );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SNOW_DOING );
	if ( m_sProperty.m_dwFlag & ISSNOW )	pButton->EnableWindow ( TRUE );
	else									pButton->EnableWindow ( FALSE );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_LEAVES_DOING );
	if ( m_sProperty.m_dwFlag & ISLEAVES )	pButton->EnableWindow ( TRUE );
	else									pButton->EnableWindow ( FALSE );

	//	Note : 활성화 비 활성화
	//
	if ( !(m_sProperty.m_dwFlag&ISACTIVE) )
	{
		((CButton*)GetDlgItem ( IDC_CHECK_RAIN ))->EnableWindow ( FALSE );
		((CButton*)GetDlgItem ( IDC_CHECK_SNOW ))->EnableWindow ( FALSE );
		((CButton*)GetDlgItem ( IDC_CHECK_LEAVES ))->EnableWindow ( FALSE );
		((CButton*)GetDlgItem ( IDC_CHECK_RAIN_DOING ))->EnableWindow ( FALSE );
		((CButton*)GetDlgItem ( IDC_CHECK_SNOW_DOING ))->EnableWindow ( FALSE );
		((CButton*)GetDlgItem ( IDC_CHECK_LEAVES_DOING ))->EnableWindow ( FALSE );
	}
}
