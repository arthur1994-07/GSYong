// GameSetDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../SigmaCore/DebugSet.h"

#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/DxTools/DxFontDefine.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/DxSound/DxSoundMan.h"
#include "../EngineLib/DxSound/NSSoundDef.h"
#include "../RanLogic/GLUseFeatures.h"
#include "../EngineLib/GUInterface/GameTextControl.h"

#include "../RanLogic/RANPARAM.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "AutoPatchMan.h"
#include "GameSetDlg.h"
#include "GlobalVariable.h"
#include "LauncherText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static	const int	INTERPOLIATION = -25;

//std::string szCharShadow[5] = 
//{
//	"그림자 없음",	//	1단계
//	"그림자 저급",		//
//	"그림자 중급",
//	"그림자 고급1",
//	"그림자 고급2"
//};
//
//std::string szCharDetail[3] = 
//{
//	"저급",
//	"중급",
//	"고급",
//};
//
//std::string sz3DAlgorithm[3] =
//{	
//	"기본",
//	"하드웨어가속 HALF",
//	"하드웨어가속 FULL"
//};
//
//std::string szFogRange[3] = 
//{
//	"가까이",
//	"보통",
//	"멀리"
//};

BOOL CGameSetDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if ( m_sliderBGM.m_hWnd )
	{
		if ( m_sliderBGM.m_hWnd == pMsg->hwnd )
		{	
			if ( pMsg->message == WM_LBUTTONUP ||
				 pMsg->message == WM_KEYUP ||
				 pMsg->message == WM_MOUSEWHEEL )
			{	
				m_BgmVolume = m_sliderBGM.GetPos ();
				m_BgmVolume = (INTERPOLIATION<=m_BgmVolume)?0:m_BgmVolume;
				
			}
		}
	}

	if ( m_sliderSFX.m_hWnd )
	{
		if ( m_sliderSFX.m_hWnd == pMsg->hwnd )
		{	
			if ( pMsg->message == WM_LBUTTONUP ||
				 pMsg->message == WM_KEYUP ||
				 pMsg->message == WM_MOUSEWHEEL )
			{
				m_SfxVolume = m_sliderSFX.GetPos ();
				m_SfxVolume = (INTERPOLIATION<=m_SfxVolume)?0:m_SfxVolume;
			}
		}
	}

	if ( m_sliderMAP.m_hWnd )
	{
		if ( m_sliderMAP.m_hWnd == pMsg->hwnd )
		{	
			if ( pMsg->message == WM_LBUTTONUP ||
				 pMsg->message == WM_KEYUP ||
				 pMsg->message == WM_MOUSEWHEEL )
			{				
				m_MapVolume = m_sliderMAP.GetPos ();
				m_MapVolume = (INTERPOLIATION<=m_MapVolume)?0:m_MapVolume;
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CGameSetDlg::InitSubCtrls()
{
	m_btnOk.SetBitmaps(IDB_BITMAP_OK_ON, IDB_BITMAP_OK_NOR, IDB_BITMAP_OK_DN, IDB_BITMAP_OK_NOR );
	m_btnOk.SizeToContent();

	m_btnCancel.SetBitmaps(IDB_BITMAP_CANCEL_ON, IDB_BITMAP_CANCEL_NOR, IDB_BITMAP_CANCEL_DN, IDB_BITMAP_CANCEL_NOR );
	m_btnCancel.SizeToContent();

	m_btnSound.SetBitmaps(IDB_BITMAP_SOUND_ON, IDB_BITMAP_SOUND_NOR, IDB_BITMAP_SOUND_DN, IDB_BITMAP_SOUND_NOR );
	m_btnSound.SizeToContent();

	m_sliderBGM.SetBitmapID( IDB_BITMAP_SLIDER_BACK, IDB_BITMAP_SLIDER_DRAG );
	m_sliderSFX.SetBitmapID( IDB_BITMAP_SLIDER_BACK, IDB_BITMAP_SLIDER_DRAG );
	m_sliderMAP.SetBitmapID( IDB_BITMAP_SLIDER_BACK, IDB_BITMAP_SLIDER_DRAG );

	m_sliderBGM.SubclassDlgItem( IDC_SLIDER_BGM, this);
	m_sliderSFX.SubclassDlgItem( IDC_SLIDER_SFX, this);
	m_sliderMAP.SubclassDlgItem( IDC_SLIDER_MAP, this);

	m_checkWindowMode.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkWindowMode.DrawTransparent();
	
	m_checkRealReflect.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkRealReflect.DrawTransparent();

	m_checkGlow.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkGlow.DrawTransparent();

	m_checkReflect.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkReflect.DrawTransparent();
	
	m_checkBuff.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkBuff.DrawTransparent();

	m_checkPostProcessing.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkPostProcessing.DrawTransparent();

	m_checkGameCursor.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkGameCursor.DrawTransparent();

	m_checkAllMinimum.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkAllMinimum.DrawTransparent();

	m_checkTextureFont.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkTextureFont.DrawTransparent();

	m_checkFrameLimit.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkFrameLimit.DrawTransparent();

	m_checkTargetEffect.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkTargetEffect.DrawTransparent();

	m_checkClickEffect.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkClickEffect.DrawTransparent();	

	m_checkMineEffect.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkMineEffect.DrawTransparent();

	m_checkNormalMap.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkNormalMap.DrawTransparent();

	m_check3DSound.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_check3DSound.DrawTransparent();

	m_checkBGM.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkBGM.DrawTransparent();

	m_checkSFX.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkSFX.DrawTransparent();

	m_checkMAP.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_checkMAP.DrawTransparent();

	m_checkUsePassiveDN.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF, IDI_CHECK_ON, IDI_CHECK_DIS);
	m_checkUsePassiveDN.DrawTransparent();

//#ifdef CH_PARAM
	CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
	if (pApp->GetServiceProvider() == SP_CHINA)
	{
		m_radioCnDownLoad1.SetIcon(IDI_RADIO_ON, IDI_RADIO_OFF);
		m_radioCnDownLoad1.DrawTransparent();
		m_radioCnDownLoad2.SetIcon(IDI_RADIO_ON, IDI_RADIO_OFF);
		m_radioCnDownLoad2.DrawTransparent();
		m_radioCnDownLoad3.SetIcon(IDI_RADIO_ON, IDI_RADIO_OFF);
		m_radioCnDownLoad3.DrawTransparent();
	}
//#endif	

	m_comboHz.SetComboBitmap(IDB_COMBO_LEFT,IDB_COMBO_RIGHT,IDB_COMBO_CEN);
	m_comboHz.SetComboListBitmap(IDB_LIST_LEFT, IDB_LIST_LEFT,IDB_LIST_TOP,IDB_LIST_TOP);

	m_comboResolution.SetComboBitmap(IDB_COMBO_LEFT,IDB_COMBO_RIGHT,IDB_COMBO_CEN);
	m_comboResolution.SetComboListBitmap(IDB_LIST_LEFT, IDB_LIST_LEFT,IDB_LIST_TOP,IDB_LIST_TOP);

	m_comboDefaultOption.SetComboBitmap(IDB_COMBO_LEFT,IDB_COMBO_RIGHT,IDB_COMBO_CEN);
	m_comboDefaultOption.SetComboListBitmap(IDB_LIST_LEFT, IDB_LIST_LEFT,IDB_LIST_TOP,IDB_LIST_TOP);

	m_comboFogRange.SetComboBitmap(IDB_COMBO_LEFT,IDB_COMBO_RIGHT,IDB_COMBO_CEN);
	m_comboFogRange.SetComboListBitmap(IDB_LIST_LEFT, IDB_LIST_LEFT,IDB_LIST_TOP,IDB_LIST_TOP);

	m_comboShadowChar.SetComboBitmap(IDB_COMBO_LEFT,IDB_COMBO_RIGHT,IDB_COMBO_CEN);
	m_comboShadowChar.SetComboListBitmap(IDB_LIST_LEFT, IDB_LIST_LEFT,IDB_LIST_TOP,IDB_LIST_TOP);

	m_comboSkinDetail.SetComboBitmap(IDB_COMBO_LEFT,IDB_COMBO_RIGHT,IDB_COMBO_CEN);
	m_comboSkinDetail.SetComboListBitmap(IDB_LIST_LEFT, IDB_LIST_LEFT,IDB_LIST_TOP,IDB_LIST_TOP);

	m_combo3DAlgorithm.SetComboBitmap(IDB_COMBO_LEFT,IDB_COMBO_RIGHT,IDB_COMBO_CEN);
	m_combo3DAlgorithm.SetComboListBitmap(IDB_LIST_LEFT, IDB_LIST_LEFT,IDB_LIST_TOP,IDB_LIST_TOP);

	m_stTitle.SubclassDlgItem( IDC_TITLE_STATIC, this );
	m_stTitle.SetFontSize( 10, true );

	m_stDisplay.SubclassDlgItem( IDC_DISPLAY_STATIC, this );
	m_stGraphic.SubclassDlgItem( IDC_GRAPHIC_STATIC, this );
	m_stSound.SubclassDlgItem( IDC_SOUND_STATIC, this );
	m_stDownload.SubclassDlgItem( IDC_DOWNLOAD_STATIC, this );
	m_stDownArea.SubclassDlgItem( IDC_DOWNAREA_STATIC, this );		

	m_stDefaultOption.SubclassDlgItem( IDC_DEFAULT_OPTION_STATIC, this );
	m_stFogRange.SubclassDlgItem( IDC_FOGRANGE_STATIC, this );
	m_stShadowChar.SubclassDlgItem( IDC_SHADOWCHAR_STATIC, this );
	m_stSkinDetail.SubclassDlgItem( IDC_SKINDETAIL_STATIC, this );

	m_stBGM.SubclassDlgItem( IDC_BGM_STATIC, this );
	m_stSFX.SubclassDlgItem( IDC_SFX_STATIC, this );
	m_stMAP.SubclassDlgItem( IDC_MAP_STATIC, this );
	
	return TRUE;
}

BOOL CGameSetDlg::InitCtrls ()
{
	//	해상도 결정
	D3DDeviceInfo* pDeviceInfo = m_pD3DApp->GetCurDeviceInfo ();
	DWORD dwNumModes = pDeviceInfo->dwNumModes;
	D3DModeInfo* pModeInfo = pDeviceInfo->modes;
    
	CString str;
	for ( DWORD i = 0; i < dwNumModes; i++ )
	{
		D3DModeInfo &sModeInfo = pDeviceInfo->modes[i];

		DWORD	bpp = 16;
		D3DFORMAT &d3dformat = sModeInfo.Format;
		if ((d3dformat == D3DFMT_R8G8B8)	||
			(d3dformat == D3DFMT_A8R8G8B8)	||
			(d3dformat == D3DFMT_X8R8G8B8) )
		{
			bpp = 32;
		}

		//	비권장 체크
		if ( sModeInfo.Width < RANPARAM::X_RESOLUTION || sModeInfo.Height < RANPARAM::Y_RESOLUTION )
			str.Format ( "%4d * %4d * %d (%s)", sModeInfo.Width, sModeInfo.Height, bpp, ID2LAUNCHERTEXT( "DISCOURAGES" ) );
		else	
			str.Format ( "%4d * %4d * %d", sModeInfo.Width, sModeInfo.Height, bpp );
		
		m_comboResolution.AddString ( str );
	}

	SetWin_Combo_Init ( this, IDC_COMBO_DEFAULTOPTION, szDefaultOption, 5 );
	SetWin_Combo_Init ( this, IDC_COMBO_FOGRANGE, szFogRange, 4 );
	SetWin_Combo_Init ( this, IDC_COMBO_SHADOWCHAR, szCharShadow, 5 );
	SetWin_Combo_Init ( this, IDC_COMBO_SKINDETAIL, szCharDetail, 3 );
	SetWin_Combo_Init ( this, IDC_COMBO_3DALGORITHM, sz3DAlgorithm, 3 );
	
	m_sliderBGM.SetRange ( -NSSoundDef::CONTROL_DECIBEL, 0 + INTERPOLIATION );
	m_sliderSFX.SetRange ( -NSSoundDef::CONTROL_DECIBEL, 0 + INTERPOLIATION );
	m_sliderMAP.SetRange ( -NSSoundDef::CONTROL_DECIBEL, 0 + INTERPOLIATION );


//#ifdef CH_PARAM
	CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
	if (pApp->GetServiceProvider() == SP_CHINA)
	{
		if ( !m_strAreaName[0].IsEmpty() )	SetDlgItemText( IDC_RADIO1, m_strAreaName[0] );
		if ( !m_strAreaName[1].IsEmpty() )	SetDlgItemText( IDC_RADIO2, m_strAreaName[1] );
		if ( !m_strAreaName[2].IsEmpty() )	SetDlgItemText( IDC_RADIO3, m_strAreaName[2] );
//		SetDlgItemText( IDC_RADIO4, m_strAreaName[3] );
//		SetDlgItemText( IDC_RADIO5, m_strAreaName[4] );
//		SetDlgItemText( IDC_RADIO6, m_strAreaName[5] ); // 중국지역 추가
	}
//#endif

	return TRUE;
}
BOOL CGameSetDlg::UpdateCtrls ()
{
	CString str;

	//	비권장 체크
	if (  RANPARAM::dwScrWidth < RANPARAM::X_RESOLUTION || RANPARAM::dwScrHeight < RANPARAM::Y_RESOLUTION )
	{
		str.Format ( "%4d * %4d * %d (%s)", RANPARAM::dwScrWidth , RANPARAM::dwScrHeight , 
			(RANPARAM::emScrFormat)?32:16, ID2LAUNCHERTEXT( "DISCOURAGES" ) );
	}
	else
	{
		str.Format ( "%4d * %4d * %d", RANPARAM::dwScrWidth , 
		RANPARAM::dwScrHeight , (RANPARAM::emScrFormat)?32:16 );
	}
	

	bool bSEL = SetWin_Combo_SelMfc ( this, IDC_COMBO_RESOLUTION, str );
	if ( !bSEL )
	{
		//	해상도 결정
		D3DDeviceInfo* pDeviceInfo = m_pD3DApp->GetCurDeviceInfo ();
		D3DModeInfo &sModeInfo = pDeviceInfo->modes[pDeviceInfo->dwCurrentMode];

		DWORD	bpp = 16;
		D3DFORMAT &d3dformat = sModeInfo.Format;
		if ((d3dformat == D3DFMT_R8G8B8)	||
			(d3dformat == D3DFMT_A8R8G8B8)	||
			(d3dformat == D3DFMT_X8R8G8B8) )
		{
			bpp = 32;
		}

		//	비권장 체크
		if ( sModeInfo.Width < RANPARAM::X_RESOLUTION || sModeInfo.Height < RANPARAM::Y_RESOLUTION )
			str.Format ( "%4d * %4d * %d (%s)", sModeInfo.Width, sModeInfo.Height, bpp, ID2LAUNCHERTEXT( "DISCOURAGES" ) );
		else	
			str.Format ( "%4d * %4d * %d", sModeInfo.Width, sModeInfo.Height, bpp );

		bSEL = SetWin_Combo_SelMfc ( this, IDC_COMBO_RESOLUTION, str );
		if ( !bSEL )
		{
			SetWin_Combo_Sel ( this, IDC_COMBO_RESOLUTION, 0 );
		}
	}
	
	SetWin_Check ( this, IDC_CHECK_WINDOWMODE, RANPARAM::bScrWindowed );

	//	의도된것임
	OnCbnSelchangeComboResolution ();

	m_BgmVolume = RANPARAM::nSndMusic;
	m_SfxVolume = RANPARAM::nSndSfx;
	m_MapVolume = RANPARAM::nSndMap;
	
	m_sliderBGM.SetPos ( m_BgmVolume );
	m_sliderSFX.SetPos ( m_SfxVolume );
	m_sliderMAP.SetPos ( m_MapVolume );	
	
	
	SetWin_Check ( this, IDC_CHECK_3DSOUND, RANPARAM::b3D_SOUND );
	SetWin_Check ( this, IDC_CHECK_GAMECURSOR, RANPARAM::bGameCursor );
	SetWin_Check ( this, IDC_CHECK_ALL_MINIMUM, RANPARAM::bAllMinimum ); // by luxes.

	SetWin_Combo_Sel ( this, IDC_COMBO_DEFAULTOPTION, szDefaultOption[RANPARAM::dwVideoLevel] );
	SetWin_Combo_Sel ( this, IDC_COMBO_FOGRANGE, szFogRange[RANPARAM::dwFogRange] );
	SetWin_Combo_Sel ( this, IDC_COMBO_SHADOWCHAR, szCharShadow[RANPARAM::dwShadowChar] );
	SetWin_Combo_Sel ( this, IDC_COMBO_SKINDETAIL, szCharDetail[RANPARAM::dwSkinDetail] );
	SetWin_Combo_Sel ( this, IDC_COMBO_3DALGORITHM, sz3DAlgorithm[RANPARAM::dw3DAlgorithm] );

	SetWin_Check ( this, IDC_CHECK_REALREFLECT, RANPARAM::bRealReflect );	
	SetWin_Check ( this, IDC_CHECK_REFRACT, RANPARAM::bRefract );	
	SetWin_Check ( this, IDC_CHECK_GLOW, RANPARAM::bGlow );	
	SetWin_Check ( this, IDC_CHECK_BUFFEFFECT, RANPARAM::bBuff );	
	SetWin_Check ( this, IDC_CHECK_POSTPROCESSING, RANPARAM::bPost );	
	SetWin_Check ( this, IDC_CHECK_FRAME_LIMIT, RANPARAM::bFrameLimit );	
	SetWin_Check ( this, IDC_CHECK_TARGETEFFECT, RENDERPARAM::bTargetEffect );	
	SetWin_Check ( this, IDC_CHECK_MINEEFFECT, RENDERPARAM::bMineEffect );	
	SetWin_Check ( this, IDC_CHECK_CLICKEFFECT, RENDERPARAM::bClickEffect );	
	SetWin_Check ( this, IDC_CHECK_NORMALMAP, RENDERPARAM::bUseNormalMap );	

	SetWin_Check ( this, IDC_CHECK_MAP, !RANPARAM::bSndMuteMap );	
	SetWin_Check ( this, IDC_CHECK_BGM, !RANPARAM::bSndMuteMusic );	
	SetWin_Check ( this, IDC_CHECK_SFX, !RANPARAM::bSndMuteSfx );	

	SetWin_Check ( this, IDC_CHECK_PASSIVE, RANPARAM::bUsePassiveDN );

	// 아래의 COMMENT는 데이터에서 읽어오지않는다;
// 	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 		SetWin_Check(this, IDC_CHECK_TEXTURE_FONT, RANPARAM::strFontType==CGameTextMan::GetInstance().GetCommentText("FONTSYS",0).GetString());
// 	else
	SetWin_Check ( this, IDC_CHECK_TEXTURE_FONT, RANPARAM::strFontType==COMMENT::FONTSYS[0].c_str() );

//#ifdef CH_PARAM 
	CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
	if (pApp->GetServiceProvider() == SP_CHINA)
	{
		SetWin_Check ( this, IDC_RADIO1, m_nDownloadArea == 0 );
		SetWin_Check ( this, IDC_RADIO2, m_nDownloadArea == 1 );
		SetWin_Check ( this, IDC_RADIO3, m_nDownloadArea == 2 );
	}
//#endif

	return TRUE;
}

void CGameSetDlg::InitDlgText ()
{
	SetDlgItemText( IDC_TITLE_STATIC, ID2LAUNCHERTEXT("IDD_GAMESETDLG"));	
	
	SetDlgItemText( IDC_DISPLAY_STATIC,  ID2LAUNCHERTEXT("IDC_DISPLAY_STATIC") );
	SetDlgItemText( IDC_CHECK_WINDOWMODE, ID2LAUNCHERTEXT("IDC_CHECK_WINDOWMODE") );
	
	SetDlgItemText( IDC_DOWNLOAD_STATIC, ID2LAUNCHERTEXT("IDC_DOWNLOAD_STATIC") );		
	SetDlgItemText( IDC_CHECK_PASSIVE, ID2LAUNCHERTEXT("IDC_CHECK_PASSIVE") );
	SetDlgItemText( IDC_DOWNAREA_STATIC, ID2LAUNCHERTEXT("IDC_DOWNAREA_STATIC") );

	SetDlgItemText( IDOK, "" );
	SetDlgItemText( IDCANCEL, "" );
	SetDlgItemText( IDC_GRAPHIC_STATIC, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",0) );
	SetDlgItemText( IDC_DEFAULT_OPTION_STATIC, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",1) );
	SetDlgItemText( IDC_FOGRANGE_STATIC, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",2) );
	SetDlgItemText( IDC_SHADOWCHAR_STATIC, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",3) );
	SetDlgItemText( IDC_SKINDETAIL_STATIC, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",4) );
	SetDlgItemText( IDC_CHECK_SHADOWLAND, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",5) );
	SetDlgItemText( IDC_CHECK_REALREFLECT, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",6) );
	SetDlgItemText( IDC_CHECK_REFRACT, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",7) );
	SetDlgItemText( IDC_CHECK_GLOW, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",8) );
	SetDlgItemText( IDC_CHECK_BUFFEFFECT, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",9) );
	SetDlgItemText( IDC_CHECK_POSTPROCESSING, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",10) );
	SetDlgItemText( IDC_CHECK_GAMECURSOR, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",11) );
	SetDlgItemText( IDC_CHECK_ALL_MINIMUM, ID2LAUNCHERTEXT("IDC_CHECK_ALL_MINIMUM") ); // by luxes.
	SetDlgItemText( IDC_CHECK_TEXTURE_FONT, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",12) );
	SetDlgItemText( IDC_CHECK_FRAME_LIMIT, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",13) );

	SetDlgItemText( IDC_CHECK_TARGETEFFECT, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",14) );
	SetDlgItemText( IDC_CHECK_MINEEFFECT, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",15) );
	SetDlgItemText( IDC_CHECK_CLICKEFFECT, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",16) );
	SetDlgItemText( IDC_CHECK_NORMALMAP, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",17) );
	
	SetDlgItemText( IDC_SOUND_STATIC, ID2LAUNCHERTEXT("IDC_SOUND_OPTION",0) );
	SetDlgItemText( IDC_CHECK_3DSOUND, ID2LAUNCHERTEXT("IDC_SOUND_OPTION",1) );
	SetDlgItemText( IDC_BUTTON_COMPATIBLE, "" );
	SetDlgItemText( IDC_BGM_STATIC, ID2LAUNCHERTEXT("IDC_SOUND_OPTION",3) );
	SetDlgItemText( IDC_MAP_STATIC, ID2LAUNCHERTEXT("IDC_SOUND_OPTION",4) );
	SetDlgItemText( IDC_SFX_STATIC, ID2LAUNCHERTEXT("IDC_SOUND_OPTION",5) );
}

BOOL CGameSetDlg::InverseUpdateCtrls ()
{
	RANPARAM::dwScrWidth = m_dwScrWidth;
	RANPARAM::dwScrHeight = m_dwScrHeight;
	RANPARAM::emScrFormat = m_emScrFormat;
	RANPARAM::uScrRefreshHz = m_uScrRefreshHz;
	RANPARAM::bScrWindowed = GetWin_Check ( this, IDC_CHECK_WINDOWMODE );

	RANPARAM::bGameCursor = GetWin_Check ( this, IDC_CHECK_GAMECURSOR );

	RANPARAM::bAllMinimum = GetWin_Check( this, IDC_CHECK_ALL_MINIMUM ); // by luxes.

	RANPARAM::nSndMap = m_MapVolume;
	RANPARAM::nSndSfx = m_SfxVolume;
	RANPARAM::nSndMusic = m_BgmVolume;


	RANPARAM::dwVideoLevel = m_comboDefaultOption.GetCurSel ();
	RANPARAM::dw3DAlgorithm = m_combo3DAlgorithm.GetCurSel ();
	RANPARAM::dwShadowChar	= m_comboShadowChar.GetCurSel ();
	RANPARAM::bRealReflect	= m_checkRealReflect.GetCheck ();
	RANPARAM::bRefract		= m_checkReflect.GetCheck ();
	RANPARAM::bGlow			= m_checkGlow.GetCheck ();
	RANPARAM::bBuff			= m_checkBuff.GetCheck ();
	RANPARAM::bPost			= m_checkPostProcessing.GetCheck ();
	RANPARAM::bFrameLimit	= m_checkFrameLimit.GetCheck ();
	RENDERPARAM::bTargetEffect	= m_checkTargetEffect.GetCheck ();
	RENDERPARAM::bMineEffect	= m_checkMineEffect.GetCheck ();
	RENDERPARAM::bClickEffect	= m_checkClickEffect.GetCheck ();
	RENDERPARAM::bUseNormalMap	= m_checkNormalMap.GetCheck ();

	RANPARAM::dwSkinDetail	= m_comboSkinDetail.GetCurSel ();
	RANPARAM::dwFogRange	= m_comboFogRange.GetCurSel ();

	RANPARAM::bSndMuteMap	= !m_checkMAP.GetCheck ();
	RANPARAM::bSndMuteMusic	= !m_checkBGM.GetCheck ();
	RANPARAM::bSndMuteSfx	= !m_checkSFX.GetCheck ();

	RANPARAM::b3D_SOUND = m_check3DSound.GetCheck ();
	RANPARAM::bUsePassiveDN = m_checkUsePassiveDN.GetCheck ();

	BOOL bTEX_FONT = GetWin_Check ( this, IDC_CHECK_TEXTURE_FONT );
	// 아래의 COMMENT는 데이터에서 읽어오지않는다;
	//if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	//	RANPARAM::strFontType = bTEX_FONT ? CGameTextMan::GetInstance().GetCommentText("FONTSYS",0).GetString():CGameTextMan::GetInstance().GetCommentText("FONTSYS",1).GetString();
	//else
	RANPARAM::strFontType = bTEX_FONT ? COMMENT::FONTSYS[0].c_str() : COMMENT::FONTSYS[1].c_str();

//#ifdef CH_PARAM
	CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
	if (pApp->GetServiceProvider() == SP_CHINA)
	{
		if ( GetWin_Check( this, IDC_RADIO1 ) ) m_nDownloadArea = 0;
		if ( GetWin_Check( this, IDC_RADIO2 ) ) m_nDownloadArea = 1;
		if ( GetWin_Check( this, IDC_RADIO3 ) ) m_nDownloadArea = 2;

		RANPARAM::nChinaRegion = m_nDownloadArea;

		StringCchCopy( RANPARAM::LoginAddress, STRING_NUM_128, RANPARAM::China_Region[m_nDownloadArea].LoginAddress.GetString() );

		for( int i=0 ; ; ++i )
		{
			if( RANPARAM::China_Region[m_nDownloadArea].HttpAddressTable[i].IsEmpty() ) 
				break;

			StringCchCopy( RANPARAM::HttpAddressTable[i], STRING_NUM_128, RANPARAM::China_Region[m_nDownloadArea].HttpAddressTable[i].GetString() );
		}

		RANPARAM::strNewsURL = RANPARAM::China_Region[m_nDownloadArea].strNewsURL;
		RANPARAM::strImageURL = RANPARAM::China_Region[m_nDownloadArea].strImageURL;
	}
//#endif

	return TRUE;
}