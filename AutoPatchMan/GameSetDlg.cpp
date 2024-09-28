// GameSetDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../SigmaCore/DebugSet.h"

#include "../EngineLib/DxSound/DxSoundMan.h"
#include "../EngineLib/DxCommon/dsutil.h"

#include "../RanLogic/GLUseFeatures.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "AutoPatchMan.h"
#include "GlobalVariable.h"
#include "LauncherText.h"
#include "GameSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGameSetDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGameSetDlg, CDialog)
CGameSetDlg::CGameSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGameSetDlg::IDD, pParent)
	, m_nDownloadArea(0)
	, m_pMemDC(NULL)
	, m_pOldBitmap(NULL)
	, m_pBitmapMem(NULL)
	, m_bLBDown( FALSE )
{
    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
    m_pD3DApp = new CD3DApplication(pApp->GetServiceProvider());
	InitString();
}

CGameSetDlg::~CGameSetDlg()
{
    delete m_pD3DApp;
    m_pD3DApp = NULL;
}

void CGameSetDlg::InitString()
{
	CString strMsg;
	
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 31 );
	szCharShadow[0] = strMsg.GetString();//ID2LAUNCHERTEXT("IDS_MESSAGE", 31 );//strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 32 ) ;
	szCharShadow[1] = strMsg.GetString();//ID2LAUNCHERTEXT("IDS_MESSAGE", 32 );
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 33 ) ;
	szCharShadow[2] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 34 ) ;
	szCharShadow[3] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 35 ) ;
	szCharShadow[4] = strMsg.GetString();

	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 36 ) ;
	szCharDetail[0] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 37 ) ;
	szCharDetail[1] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 38 ) ;
	szCharDetail[2] = strMsg.GetString();

	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 39 ) ;
	sz3DAlgorithm[0] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 40 ) ;
	sz3DAlgorithm[1] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 41 ) ;
	sz3DAlgorithm[2] = strMsg.GetString();

	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 81 ) ;
	szFogRange[0] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 42 ) ;
	szFogRange[1] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 43 ) ;
	szFogRange[2] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 44 ) ;
	szFogRange[3] = strMsg.GetString();

	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 73 ) ;
	szDefaultOption[0] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 36 ) ;
	szDefaultOption[1] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 37 ) ;
	szDefaultOption[2] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 38 ) ;
	szDefaultOption[3] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 80 ) ;
	szDefaultOption[4] = strMsg.GetString();


}

void CGameSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BUTTON_COMPATIBLE, m_btnSound);
	DDX_Control(pDX, IDC_CHECK_WINDOWMODE, m_checkWindowMode);
	DDX_Control(pDX, IDC_CHECK_REALREFLECT, m_checkRealReflect);
	DDX_Control(pDX, IDC_CHECK_REFRACT, m_checkReflect);	
	DDX_Control(pDX, IDC_CHECK_GLOW, m_checkGlow);	
	DDX_Control(pDX, IDC_CHECK_BUFFEFFECT, m_checkBuff);
	DDX_Control(pDX, IDC_CHECK_POSTPROCESSING, m_checkPostProcessing);
	DDX_Control(pDX, IDC_CHECK_GAMECURSOR, m_checkGameCursor);
	DDX_Control(pDX, IDC_CHECK_ALL_MINIMUM, m_checkAllMinimum); // by luxes.
	DDX_Control(pDX, IDC_CHECK_TEXTURE_FONT, m_checkTextureFont);
	DDX_Control(pDX, IDC_CHECK_FRAME_LIMIT, m_checkFrameLimit);
	DDX_Control(pDX, IDC_CHECK_TARGETEFFECT, m_checkTargetEffect);
	DDX_Control(pDX, IDC_CHECK_CLICKEFFECT, m_checkClickEffect);
	DDX_Control(pDX, IDC_CHECK_MINEEFFECT, m_checkMineEffect);
	DDX_Control(pDX, IDC_CHECK_NORMALMAP, m_checkNormalMap);	
	DDX_Control(pDX, IDC_CHECK_3DSOUND, m_check3DSound);
	DDX_Control(pDX, IDC_CHECK_BGM, m_checkBGM);
	DDX_Control(pDX, IDC_CHECK_SFX, m_checkSFX);
	DDX_Control(pDX, IDC_CHECK_MAP, m_checkMAP);		
	DDX_Control(pDX, IDC_CHECK_PASSIVE, m_checkUsePassiveDN);		
	DDX_Control(pDX, IDC_COMBO_HZ, m_comboHz );
	DDX_Control(pDX, IDC_COMBO_RESOLUTION, m_comboResolution );
	DDX_Control(pDX, IDC_COMBO_DEFAULTOPTION, m_comboDefaultOption );
	DDX_Control(pDX, IDC_COMBO_FOGRANGE, m_comboFogRange );
	DDX_Control(pDX, IDC_COMBO_SHADOWCHAR, m_comboShadowChar );
	DDX_Control(pDX, IDC_COMBO_SKINDETAIL, m_comboSkinDetail );
	DDX_Control(pDX, IDC_COMBO_3DALGORITHM, m_combo3DAlgorithm );

//#ifdef CH_PARAM
	CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
	if (pApp->GetServiceProvider() == SP_CHINA)
	{
		DDX_Control(pDX, IDC_RADIO1, m_radioCnDownLoad1 );
		DDX_Control(pDX, IDC_RADIO2, m_radioCnDownLoad2 );
		DDX_Control(pDX, IDC_RADIO3, m_radioCnDownLoad3 );
	}

//#endif
}


BEGIN_MESSAGE_MAP(CGameSetDlg, CDialog)	
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_COMPATIBLE, OnBnClickedButtonCompatible)
	ON_CBN_SELCHANGE(IDC_COMBO_RESOLUTION, OnCbnSelchangeComboResolution)
	ON_CBN_SELCHANGE(IDC_COMBO_HZ, OnCbnSelchangeComboHz)
	ON_CBN_SELCHANGE(IDC_COMBO_DEFAULTOPTION, OnCbnSelchangeComboDefaultoption)
	ON_BN_CLICKED(IDC_CHECK_SHADOWLAND, OnBnClickedCheckButton)
	ON_BN_CLICKED(IDC_CHECK_REALREFLECT, OnBnClickedCheckButton)
	ON_BN_CLICKED(IDC_CHECK_REFRACT, OnBnClickedCheckButton)
	ON_BN_CLICKED(IDC_CHECK_GLOW, OnBnClickedCheckButton)
	ON_BN_CLICKED(IDC_CHECK_POSTPROCESSING, OnBnClickedCheckButton)
	ON_BN_CLICKED(IDC_CHECK_BUFFEFFECT, OnBnClickedCheckButton)
	ON_BN_CLICKED(IDC_CHECK_TARGETEFFECT, OnBnClickedCheckButton)
	ON_BN_CLICKED(IDC_CHECK_CLICKEFFECT, OnBnClickedCheckButton)
	ON_BN_CLICKED(IDC_CHECK_MINEEFFECT, OnBnClickedCheckButton)
	ON_BN_CLICKED(IDC_CHECK_NORMALMAP, OnBnClickedCheckButton)	
	ON_CBN_SELCHANGE(IDC_COMBO_FOGRANGE, OnCbnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_SHADOWCHAR, OnCbnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_SKINDETAIL, OnCbnSelchangeCombo)
	ON_WM_PAINT()
	ON_MESSAGE( WM_TRANSPARENT_BK, OnTransaprentBk)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedRadio3)
END_MESSAGE_MAP()


// CGameSetDlg 메시지 처리기입니다.
BOOL CGameSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.	
	m_pD3DApp->BuildDeviceListSet ();

	D3DAdapterInfo *pAdapter = m_pD3DApp->GetCurAdapterInfo();
	
	BOOL bWHQL = ( pAdapter->d3dAdapterIdentifier.WHQLLevel != 0 );

	CString strWHQLCommant;
	if ( bWHQL )	strWHQLCommant = ID2LAUNCHERTEXT("IDS_WHQL_CVD", 0 );
	else			strWHQLCommant = ID2LAUNCHERTEXT("IDS_WHQL_UCVD", 0 );


//	CWnd *pWnd = GetDlgItem(IDC_STATIC_WHQL);
//	pWnd->SetWindowText ( strWHQLCommant );
	
	InitSubCtrls();
	InitCtrls ();
	UpdateCtrls ();
	InitDlgText ();

//#ifdef CH_PARAM
	CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
	if (pApp->GetServiceProvider() == SP_CHINA)
	{
		// Note : 중국 런쳐의 경우 창 모드 지원 체크 박스를 숨긴다.
		::ShowWindow( m_checkWindowMode.GetSafeHwnd(), SW_HIDE );
	}
//#endif
	else
	{
//#ifndef	CH_PARAM	// 중국지역 추가
		CStatic* pStatic = (CStatic*) GetDlgItem(IDC_DOWNAREA_STATIC);	
		pStatic->ShowWindow(SW_HIDE);
		CButton* pButton = (CButton*) GetDlgItem(IDC_RADIO1);	
		pButton->ShowWindow(SW_HIDE);
		pButton = (CButton*) GetDlgItem(IDC_RADIO2);	
		pButton->ShowWindow(SW_HIDE);
		pButton = (CButton*) GetDlgItem(IDC_RADIO3);	
		pButton->ShowWindow(SW_HIDE);
	}
//#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CGameSetDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InverseUpdateCtrls ();
    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
	RANPARAM::SAVE(pApp->GetServiceProvider());
	RANPARAM::SAVE_PARAM( pApp->GetServiceProvider(), TRUE );
	OnOK();
}

void CGameSetDlg::OnBnClickedButtonCompatible()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bResult = DxSoundMan::GetInstance().OneTimeSceneInit ( m_hWnd, "Nothing" );
	if ( !bResult )
	{
		CString strMsg;
		strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 30 ) ;
		MessageBox ( strMsg.GetString(), "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return ;
	}

	DSCAPS DSCaps;
	LPDIRECTSOUND8 pDirectSound = DxSoundMan::GetInstance().GetSoundManager()->GetDirectSound ();
	if ( !pDirectSound )
	{		
		DxSoundMan::GetInstance().FinalCleanup ();
		CString strMsg;
		strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 30 ) ;
		MessageBox ( strMsg.GetString(), "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return;
	}

	SecureZeroMemory ( &DSCaps, sizeof ( DSCAPS ) );
	DSCaps.dwSize = sizeof ( DSCAPS );
	pDirectSound->GetCaps ( &DSCaps );

	BOOL bCompatible = TRUE;
	if ( (!( DSCaps.dwFlags & DSCAPS_PRIMARY8BIT )) ||
		 (!( DSCaps.dwFlags & DSCAPS_SECONDARY8BIT)) )
	{
		bCompatible = FALSE;
	}

	if (( DSCaps.dwFreeHwMixingAllBuffers <= 0 )		||
		( DSCaps.dwFreeHwMixingStaticBuffers <= 0 )		||
		( DSCaps.dwFreeHwMixingStreamingBuffers <= 0 )	||
		( DSCaps.dwMaxHw3DAllBuffers <= 0 )				||
		( DSCaps.dwMaxHw3DStaticBuffers <= 0 )			||
		( DSCaps.dwMaxHw3DStreamingBuffers <= 0 )		||
		( DSCaps.dwFreeHw3DAllBuffers <= 0 )			||
		( DSCaps.dwFreeHw3DStaticBuffers <= 0 )			||
		( DSCaps.dwFreeHw3DStreamingBuffers <= 0 ))
	{
		bCompatible = FALSE;
	}

	if ( bCompatible )
	{
		m_combo3DAlgorithm.SetCurSel ( DxSoundMan::HRTF_LIGHT );
	}
	else
	{
		m_combo3DAlgorithm.SetCurSel ( DxSoundMan::DEFAULT );
	}

	DxSoundMan::GetInstance().FinalCleanup ();
}

void CGameSetDlg::OnCbnSelchangeComboResolution()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_comboHz.ResetContent();

	D3DDeviceInfo* pDeviceInfo = m_pD3DApp->GetCurDeviceInfo ();
	DWORD dwNumModes = pDeviceInfo->dwNumModes;	
	D3DModeInfo* pModeInfo = pDeviceInfo->modes;

	if ( !dwNumModes ) return ;

	BOOL	bCompatible = FALSE;	
	BOOL	bFirstHz = TRUE;
	UINT	uFirstHz;

	DWORD	CurSel = m_comboResolution.GetCurSel ();
	if ( CurSel==LB_ERR )	return;

	sc::CMList<UINT> &listRefreshRate = pModeInfo[CurSel].RefreshRate;


	sc::CMList<UINT> listTemp;

	listRefreshRate.SetHead();
	while( !listRefreshRate.IsEnd() )
	{
		UINT uHz1, uHz2;
		listRefreshRate.GetCurrent( uHz1 );

		listTemp.GetTail( uHz2 );
		if( uHz1 != uHz2 )
		{
			listTemp.AddTail( uHz1 );
		}

		listRefreshRate.GoNext();
	}

	CString str;
	listTemp.SetHead ();
	while ( !listTemp.IsEnd () )
	{
		UINT uHz;
		listTemp.GetCurrent ( uHz );

		if( RANPARAM::uScrRefreshHz == uHz ) //	헤르츠가 현재 설정으로 지원할수 있는지 확인
		{
			bCompatible = TRUE;
		}

		if( bFirstHz ) // 최소 헤르츠를 기록
		{
			uFirstHz = uHz;
			bFirstHz = FALSE;
		}
	
		str.Format ( "%3d", uHz );
		int nIndex = m_comboHz.AddString ( str );
		m_comboHz.SetItemData ( nIndex, (DWORD_PTR) uHz );

		listTemp.GoNext ();
	}

	UINT uScrRefreshHz;
	if ( bCompatible )
	{
		uScrRefreshHz = RANPARAM::uScrRefreshHz;		
	}
	else
	{
		uScrRefreshHz = uFirstHz;
	}

	str.Format ( "%3d", uScrRefreshHz );
	SetWin_Combo_SelMfc ( this, IDC_COMBO_HZ, str );

	//	의도된 것임
	OnCbnSelchangeComboHz ();


	//	현재 상태 저장
	DWORD	bpp = 16;
	D3DFORMAT &d3dformat = pModeInfo[CurSel].Format;
	if ((d3dformat == D3DFMT_R8G8B8)	||
		(d3dformat == D3DFMT_A8R8G8B8)	||
		(d3dformat == D3DFMT_X8R8G8B8) )
	{
		bpp = 32;
	}

	m_dwScrWidth = pModeInfo[CurSel].Width;
	m_dwScrHeight = pModeInfo[CurSel].Height;
	m_emScrFormat = (bpp==16)?EMSCREEN_F16:EMSCREEN_F32;	

	if( m_dwScrWidth < 1024 || m_dwScrHeight < 768 )
	{
		MessageBox ( ID2LAUNCHERTEXT( "DISCOURAGES_MESSAGE" ),  "Warning", MB_OK );
	}
}

void CGameSetDlg::OnCbnSelchangeComboHz()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_comboHz.GetCurSel ();
	m_uScrRefreshHz = (UINT)(m_comboHz.GetItemData ( nIndex ));
}
void CGameSetDlg::OnCbnSelchangeComboDefaultoption()
{
	int nIndex = m_comboDefaultOption.GetCurSel ();

	if( nIndex == 1)
	{
		SetLowLevel();
	}
	else if( nIndex == 2)
	{
		SetMediumLevel();
	}
	else if( nIndex == 3)
	{
		SetHighLevel();
	}
	else if( nIndex == 4)
	{
		SetVeryHighLevel();
	}

	m_comboDefaultOption.SetCurSel(nIndex);

}

void CGameSetDlg::OnBnClickedCheckButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetWin_Combo_Sel ( this, IDC_COMBO_DEFAULTOPTION, szDefaultOption[0] );
}

void CGameSetDlg::OnCbnSelchangeCombo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetWin_Combo_Sel ( this, IDC_COMBO_DEFAULTOPTION, szDefaultOption[0] );
}

void CGameSetDlg::OnCnbSelChangeComboNation()
{
}

void CGameSetDlg::SetVeryHighLevel()
{
	SetWin_Combo_Sel ( this, IDC_COMBO_SHADOWCHAR, szCharShadow[4] );
	SetWin_Combo_Sel ( this, IDC_COMBO_SKINDETAIL, szCharDetail[2] );
	SetWin_Combo_Sel ( this, IDC_COMBO_FOGRANGE, szFogRange[3] );	

	SetWin_Check ( this, IDC_CHECK_SHADOWLAND, true );
	SetWin_Check ( this, IDC_CHECK_REALREFLECT, true );	
	SetWin_Check ( this, IDC_CHECK_REFRACT, true );	
	SetWin_Check ( this, IDC_CHECK_GLOW, true );	
	SetWin_Check ( this, IDC_CHECK_BUFFEFFECT, true );	
	SetWin_Check ( this, IDC_CHECK_POSTPROCESSING, true );	
	SetWin_Check ( this, IDC_CHECK_TARGETEFFECT, true );
	SetWin_Check ( this, IDC_CHECK_CLICKEFFECT, true );
	SetWin_Check ( this, IDC_CHECK_MINEEFFECT, true );
	SetWin_Check ( this, IDC_CHECK_NORMALMAP, true );
}

void CGameSetDlg::SetHighLevel()
{
	SetWin_Combo_Sel ( this, IDC_COMBO_SHADOWCHAR, szCharShadow[3] );
	SetWin_Combo_Sel ( this, IDC_COMBO_SKINDETAIL, szCharDetail[1] );
	SetWin_Combo_Sel ( this, IDC_COMBO_FOGRANGE, szFogRange[3] );	
	
	SetWin_Check ( this, IDC_CHECK_SHADOWLAND, true );
	SetWin_Check ( this, IDC_CHECK_REALREFLECT, true );	
	SetWin_Check ( this, IDC_CHECK_REFRACT, true );	
	SetWin_Check ( this, IDC_CHECK_GLOW, true );	
	SetWin_Check ( this, IDC_CHECK_BUFFEFFECT, true );	
	SetWin_Check ( this, IDC_CHECK_POSTPROCESSING, true );	
	SetWin_Check ( this, IDC_CHECK_TARGETEFFECT, true );
	SetWin_Check ( this, IDC_CHECK_CLICKEFFECT, true );
	SetWin_Check ( this, IDC_CHECK_MINEEFFECT, true );
	SetWin_Check ( this, IDC_CHECK_NORMALMAP, true );
}

void CGameSetDlg::SetMediumLevel()
{
	SetWin_Combo_Sel ( this, IDC_COMBO_SHADOWCHAR, szCharShadow[2] );
	SetWin_Combo_Sel ( this, IDC_COMBO_SKINDETAIL, szCharDetail[1] );
	SetWin_Combo_Sel ( this, IDC_COMBO_FOGRANGE, szFogRange[2] );	
	
	SetWin_Check ( this, IDC_CHECK_SHADOWLAND, true );
	SetWin_Check ( this, IDC_CHECK_REALREFLECT,false );	
	SetWin_Check ( this, IDC_CHECK_REFRACT, false );	
	SetWin_Check ( this, IDC_CHECK_GLOW, false );	
	SetWin_Check ( this, IDC_CHECK_BUFFEFFECT, true );	
	SetWin_Check ( this, IDC_CHECK_POSTPROCESSING, false );	
	SetWin_Check ( this, IDC_CHECK_TARGETEFFECT, true );
	SetWin_Check ( this, IDC_CHECK_CLICKEFFECT, true );
	SetWin_Check ( this, IDC_CHECK_MINEEFFECT, false );
	SetWin_Check ( this, IDC_CHECK_NORMALMAP, false );
}

void CGameSetDlg::SetLowLevel()
{
	SetWin_Combo_Sel ( this, IDC_COMBO_SHADOWCHAR, szCharShadow[1] );
	SetWin_Combo_Sel ( this, IDC_COMBO_SKINDETAIL, szCharDetail[0] );
	SetWin_Combo_Sel ( this, IDC_COMBO_FOGRANGE, szFogRange[1] );	
	
	SetWin_Check ( this, IDC_CHECK_SHADOWLAND, false );
	SetWin_Check ( this, IDC_CHECK_REALREFLECT, false );	
	SetWin_Check ( this, IDC_CHECK_REFRACT, false );	
	SetWin_Check ( this, IDC_CHECK_GLOW, false );	
	SetWin_Check ( this, IDC_CHECK_BUFFEFFECT, false );	
	SetWin_Check ( this, IDC_CHECK_POSTPROCESSING, false );		
	SetWin_Check ( this, IDC_CHECK_TARGETEFFECT, false );
	SetWin_Check ( this, IDC_CHECK_CLICKEFFECT, false );
	SetWin_Check ( this, IDC_CHECK_MINEEFFECT, false );
	SetWin_Check ( this, IDC_CHECK_NORMALMAP, false );
}

void CGameSetDlg::OnPaint()
{
	CPaintDC dc(this);
	CRect rtClient;
	GetClientRect(&rtClient);

	if ( m_pMemDC )
	{
		dc.BitBlt( 0,0,rtClient.Width(), rtClient.Height(), m_pMemDC, 0,0, SRCCOPY);
	}
}


LRESULT CGameSetDlg::OnTransaprentBk( WPARAM wParam, LPARAM lParam)
{
	HDC hdc = ( HDC)wParam;
	HWND hwnd = ( HWND)lParam;
    CTransparentHelper::OnTranparentControl( m_pMemDC->GetSafeHdc(), (WPARAM)hdc, (LPARAM)hwnd);
	return TRUE;
}

void CGameSetDlg::BuildBkDC()
{
	CBitmap bmpbk;
	bmpbk.LoadBitmap(  IDB_BITMAP_OPTIONBACK );
	BITMAP bmpInfo;
	bmpbk.GetBitmap( &bmpInfo);

	CDC* pWndDC = GetDC();
	CDC memdc;
	memdc.CreateCompatibleDC( pWndDC);
	CBitmap* pOld = memdc.SelectObject( &bmpbk);


	CRect rtClient;
	GetClientRect(&rtClient);

	if ( m_pMemDC )
	{
		m_pMemDC->SelectObject( m_pOldBitmap);
		delete m_pBitmapMem;
		m_pBitmapMem = NULL;
		delete m_pMemDC;
		m_pMemDC = NULL;
	}
	
	if ( m_pMemDC == NULL )
	{
		m_pMemDC = new CDC;
		m_pMemDC->CreateCompatibleDC( pWndDC);

		m_pBitmapMem = new CBitmap;
		m_pBitmapMem->CreateCompatibleBitmap( pWndDC, rtClient.Width(), rtClient.Height());
		m_pOldBitmap = (CBitmap*)m_pMemDC->SelectObject( m_pBitmapMem);
	}

	m_pMemDC->StretchBlt( 0,0, rtClient.Width(), rtClient.Height(), &memdc, 0,0,bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY);
    memdc.SelectObject( pOld);
	ReleaseDC( pWndDC);

	//when the parent dialog's background is rebuild, notify the child which has an transparent tag.
	CTransparentHelper::NotifyTransparentChild( GetSafeHwnd());
}
void CGameSetDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	SAFE_DELETE ( m_pMemDC );
	SAFE_DELETE ( m_pBitmapMem );
	
	CDialog::PostNcDestroy();
}

void CGameSetDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	BuildBkDC();
	Invalidate(FALSE);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CGameSetDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_MouseClickPos = point;
	m_bLBDown = TRUE;
	SetCapture();

	CDialog::OnLButtonDown(nFlags, point);
}

void CGameSetDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if ( m_bLBDown )
	{
		ReleaseCapture ();
		m_bLBDown = FALSE;
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CGameSetDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.


	if ( !m_bLBDown ) return;

	RECT rect;
	GetWindowRect ( &rect );

	CPoint MovePos;
	MovePos.x = m_MouseClickPos.x - point.x;
	MovePos.y = m_MouseClickPos.y - point.y;
	
	rect.left -= MovePos.x;
	rect.top -= MovePos.y;
	rect.right -= MovePos.x;
	rect.bottom -= MovePos.y;

	MoveWindow ( &rect );

	CDialog::OnMouseMove(nFlags, point);
}

void CGameSetDlg::OnBnClickedRadio1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCheck = m_radioCnDownLoad1.GetCheck();
	if ( nCheck ) 
	{
		m_radioCnDownLoad1.SetCheck( 1 );
		m_radioCnDownLoad2.SetCheck( 0 );
		m_radioCnDownLoad3.SetCheck( 0 );
	}
	else
	{
		m_radioCnDownLoad1.SetCheck( 1 );
	}
}

void CGameSetDlg::OnBnClickedRadio2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCheck = m_radioCnDownLoad2.GetCheck();
	if ( nCheck ) 
	{
		m_radioCnDownLoad1.SetCheck( 0 );
		m_radioCnDownLoad2.SetCheck( 1 );
		m_radioCnDownLoad3.SetCheck( 0 );
	}
	else
	{
		m_radioCnDownLoad2.SetCheck( 1 );
	}
}

void CGameSetDlg::OnBnClickedRadio3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCheck = m_radioCnDownLoad3.GetCheck();
	if ( nCheck ) 
	{
		m_radioCnDownLoad1.SetCheck( 0 );
		m_radioCnDownLoad2.SetCheck( 0 );
		m_radioCnDownLoad3.SetCheck( 1 );
	}
	else
	{
		m_radioCnDownLoad3.SetCheck( 1 );
	}
}
