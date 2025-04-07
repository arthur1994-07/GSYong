#include "pch.h"
#include "./DxLobyStage.h"
#include "./DxGlobalStage.h"

#include "../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../SigmaCore/Math/Math.h"

#include "../../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../../EngineLib/DxTools/PostProcessing/NSLightShafts.h"
#include "../../EngineLib/DxTools/PostProcessing/NSGlow.h"
#include "../../EngineLib/DxTools/DxEnvironment.h"
#include "../../EngineLib/DxTools/DxSurfaceTex.h"
#include "../../EngineLib/DxTools/DxGlowMan.h"
#include "../../EngineLib/DxTools/DxPostProcess.h"
#include "../../EngineLib/DxTools/DxCubeMap.h"
#include "../../EngineLib/DxTools/DxSkyMan.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/DxTools/DxBackUpRendTarget.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/Common/profile.h"
#include "../../EngineLib/DxTools/DxDynamicVB.h"	// 삭제 해야한다.
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxTools/DxRenderStates.h"
#include "../../EngineLib/Water/NSWaterMain.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../../EngineLib/DxLand/DxLandMan.h"

#include "../../EngineLib/G-Logic/GLPeriod.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxTools/DxFontMan.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/Light/DxLightMan.h"
#include "../../EngineLib/DxTools/Light/NSShadowLight.h"
#include "../../EngineLib/DxTools/CubeMap/NSCubeMapCulling.h"
#include "../../EngineLib/DxTools/CubeMap/NSCubeMapUtil.h"
#include "../../EngineLib/DxTools/Deffered/NSDeffered.h"
#include "../../EngineLib/DxTools/PostProcessing/NSBloomHDR.h"

#include "../../EngineLib/DxSound/BgmSound.h"

#include "../../RanLogic/Msg/ServerMsg.h"
#include "../../RanLogic/RANPARAM.h"

#include "../../EngineLib/GUInterface/InterfaceBaseDefine.h"
//#include "../InterfaceBaseDefine.h"
#include "../Tool/DxParamSet.h"
#include "../GLGaeaClient.h"

#include "../GfxInterfaceBase.h"
#include "../Login/GLLoginClient.h"
#include "../LobyCharacter/GLLobyCharClient.h"
#include "../LobyCharacter/GLLobyCharCreateClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace SEXUAL
{
	enum SEX 
	{
		SEX_MAN   = 1,
		SEX_WOMAN = 0
	};

	SEX GetSex ( EMCHARINDEX emIndex ) 
	{
		if( (WORD)emIndex == GLCI_FIGHTER_M || 
			(WORD)emIndex == GLCI_ARMS_M || 
			(WORD)emIndex == GLCI_ARCHER_M || 
			(WORD)emIndex == GLCI_SPIRIT_M || 
			(WORD)emIndex == GLCI_EXTREME_M ||
			(WORD)emIndex == GLCI_SCIENTIST_M ||
            (WORD)emIndex == GLCI_ASSASSIN_M ||
			(WORD)emIndex == GLCI_TRICKER_M ||
            (WORD)emIndex == GLCI_ETC_M ||
			(WORD)emIndex == GLCI_ACTOR_M )
			return SEX_MAN;
		
		return SEX_WOMAN;
	}
}

DxLobyStage::DxLobyStage(DxGlobalStage* pGlobalStage, GLGaeaClient* pGaeaClient)
    : m_pGlobalStage(pGlobalStage)
    , m_pGaeaClient(pGaeaClient)
	, m_nStartCharNum(0)
    , m_nStartCharLoad(0)
    , m_nChaSNum(0)
    , m_dwCharSelect(MAX_SELECTCHAR)
    , m_bGameJoin(FALSE)
    , m_nCharStart(0)
    , m_fShiftTime(0)
    , m_bShiftButton(FALSE)
    , m_bShiftLBDown(FALSE)
    , m_bShiftRBDown(FALSE)
    , m_bUseInterface(TRUE)
    , m_bRenderSkip(FALSE)
    , m_fRenderSkipTime(0.0f)
    , m_bStyle( false )
    , m_vDir ( D3DXVECTOR3( -1.0f,-10.0f,-0.35f) )
	, m_nStageBG( EMSTAGE_BG_NONE )
	, m_bChangedStage( true )
    , m_strTextureName( "GUI_001.dds")
	, m_fCheckGpuTimeADD(0.f)
	, m_nCheckGpuCountADD(0)
	, m_fElapsedTime(0.f)
	, m_dwCharIDForWorldBattle(0)
	, m_spLandCharSlt(new DxLandMan)
	, m_spLandLogin(new DxLandMan)
{	
}

DxLobyStage::~DxLobyStage(void)
{
}

HRESULT DxLobyStage::ReSizeWindow ( WORD wWidth, WORD wHeight )
{
	InvalidateDeviceObjects();
	RestoreDeviceObjects();

	return S_OK;
}

void DxLobyStage::OnInitLogin ()
{
	m_nStartCharLoad = 0;
	m_nChaSNum = 0;
	//m_bGameJoin = FALSE;
	m_dwCharSelect = MAX_SELECTCHAR;
	m_nCharStart = 0;
	m_bGameJoin = FALSE;
	m_fShiftTime = 0;
	m_bShiftButton = FALSE;
	m_bShiftLBDown = FALSE;
	m_bShiftRBDown = FALSE;
	m_bUseInterface = TRUE;

	for ( int i=0; i<MAX_SERVERCHAR; i++ ) m_CharInfo[i] = SCHARINFO_LOBBY();
	for ( int i=0; i<MAX_SELECTCHAR; i++ ) m_CharSham[i].ResetCharInfo ();

	m_NewCharSham.ResetCharInfo();
}

SCHARINFO_LOBBY* DxLobyStage::GetSelectCharInfo()
{
	if (m_dwCharSelect >= MAX_SELECTCHAR)	
		return NULL;
    else
	    return &(m_CharInfo[m_dwCharSelect+m_nCharStart]);
}

SCHARINFO_LOBBY* DxLobyStage::GetCharInfo(INT nIndex)
{
	if (nIndex >= MAX_SERVERCHAR)
		return NULL;

	if (nIndex >= m_nChaSNum)
		return NULL;

	return &(m_CharInfo[nIndex]);
}

//DxLandMan* DxLobyStage::GetCharNewLand ()
//{
//	switch ( m_sCharNewInfo.m_wSchool )
//	{
//	case 0: m_LandCharSlt_s01.ActiveMap ( m_LandCharSlt_s01 );	return &m_LandCharSlt_s01;
//	case 1:	m_LandCharSlt_s02.ActiveMap ( m_LandCharSlt_s02 );	return &m_LandCharSlt_s02;
//	case 2:	m_LandCharSlt_s03.ActiveMap ( m_LandCharSlt_s03 );	return &m_LandCharSlt_s03;
//
//	default:
//		GASSERT(0&&"새로 생성할 케릭터의 학교정보가 0~2 영역 이내에 있지 않습니다." );
//		break;
//	};
//
//	return NULL;
//}

WORD DxLobyStage::ShiftCharClass ( EMSHIFT emShift, WORD wClassMax )
{
	WORD nRet(wClassMax);

	switch ( emShift )
	{
	case EM_PREV:
		{
			WORD wIndex = (WORD)m_sCharNewInfo.m_emIndex;
			if ( (WORD)m_sCharNewInfo.m_emIndex > 0 )			wIndex--;
			else												wIndex = wClassMax;
			SelCharClass ( wIndex );

			nRet = wIndex;
		}
		break;

	case EM_NEXT:
		{
			WORD wIndex = (WORD)m_sCharNewInfo.m_emIndex;
			if ( (WORD)m_sCharNewInfo.m_emIndex < wClassMax )	wIndex++;
			else												wIndex = 0;
			SelCharClass ( wIndex );

			nRet = wIndex;
		}
		break;
	};

	return nRet;
}

void DxLobyStage::ShiftCharSchool ( EMSHIFT emShift )
{
	switch ( emShift )
	{
	case EM_PREV:
		{
			WORD wIndex = m_sCharNewInfo.m_wSchool;
			if ( wIndex>0 )										wIndex--;
			else												wIndex = (GLCONST_CHAR::wSCHOOLNUM-1);
			SelCharSchool ( wIndex );
		}
		break;

	case EM_NEXT:
		{
			WORD wIndex = m_sCharNewInfo.m_wSchool;
			if ( (WORD)wIndex < (GLCONST_CHAR::wSCHOOLNUM-1) )	wIndex++;
			else												wIndex = 0;
			SelCharSchool ( wIndex );
		}
		break;
	};
}

void DxLobyStage::ShiftCharFace ( EMSHIFT emShift )
{
	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[m_sCharNewInfo.m_emIndex];

	switch ( emShift )
	{
	case EM_PREV:
		{
			WORD wIndex = (WORD)m_sCharNewInfo.m_wFace;
			if ( wIndex > 0 )					wIndex--;
			else								wIndex = (WORD) (sCONST.dwHEADNUM_SELECT-1);
			
			SelCharFace ( wIndex );
		}
		break;

	case EM_NEXT:
		{
			WORD wIndex = (WORD)m_sCharNewInfo.m_wFace;
			if ( wIndex < WORD(sCONST.dwHEADNUM_SELECT-1) )	wIndex++;
			else											wIndex = 0;

			SelCharFace ( wIndex );
		}
		break;
	};
}

void DxLobyStage::ShiftCharHair ( EMSHIFT emShift )
{
	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[m_sCharNewInfo.m_emIndex];

	switch ( emShift )
	{
	case EM_PREV:
		{
			WORD wIndex = (WORD)m_sCharNewInfo.m_wHair;
			if ( wIndex > 0 )					wIndex--;
			else								wIndex = (WORD) (sCONST.dwHAIRNUM_SELECT-1);

			WORD wHairColor = HAIRCOLOR::GetHairColor ( m_sCharNewInfo.m_emIndex, wIndex );
			
			SelCharHair ( wIndex );
			SelCharHairColor ( wHairColor );
		}
		break;

	case EM_NEXT:
		{
			WORD wIndex = (WORD)m_sCharNewInfo.m_wHair;
			if ( wIndex < WORD(sCONST.dwHAIRNUM_SELECT-1) )	wIndex++;
			else											wIndex = 0;

			WORD wHairColor = HAIRCOLOR::GetHairColor ( m_sCharNewInfo.m_emIndex, wIndex );

			SelCharHair ( wIndex );
			SelCharHairColor ( wHairColor );
		}
		break;
	};
}

//void DxLobyStage::ToCameraPos ( int n )
//{
//	ShiftCharClass ( (EMSHIFT)n );
//}

void DxLobyStage::SelCharClass ( WORD wIndex )
{
	//	Character selection.
	m_sCharNewInfo.m_emIndex = (EMCHARINDEX) wIndex;

	m_sCharNewInfo.m_wHair = 0;
	m_sCharNewInfo.m_wFace = 0;
	m_sCharNewInfo.m_wHairColor = HAIRCOLOR::GetHairColor ( m_sCharNewInfo.m_emIndex, 0 );

	// Gender setting
	m_sCharNewInfo.m_wSex = SEXUAL::GetSex ( m_sCharNewInfo.m_emIndex );
}

void DxLobyStage::SelCharSchool ( WORD wIndex )
{
	//	School Selection.
	GASSERT(wIndex<GLCONST_CHAR::wSCHOOLNUM);
	m_sCharNewInfo.m_wSchool = (WORD)wIndex;

	//int nClassMax = GLCI_NUM_ETC;

	////	선택된 케릭터 클레스 리셋.
	//SelCharClass( (WORD)nClassMax);

	//m_sCharNewInfo.m_wHair = 0;
	//m_sCharNewInfo.m_wFace = 0;
}

void DxLobyStage::DelChar ( int nCharID )
{
	//	Note : 기존 캐릭터 정보 수신시.
	int nIndex = -1;
	for ( int i=0; i<m_nChaSNum; i++ )
	{
		if ( m_nChaIDs[i]== nCharID )	nIndex = i;
	}
	if ( nIndex == -1 )		return;

	//	Note : 캐릭터 선택을 취소.
	m_dwCharSelect = MAX_SELECTCHAR;

	//	Note : 삭제된걸 지우고 재정렬.
	for ( int i=nIndex+1; i<m_nChaSNum; i++ )
	{
		m_nChaIDs[i-1] = m_nChaIDs[i];
		m_CharInfo[i-1] = m_CharInfo[i];
	}
	m_nChaSNum--;

	//	Note : 디스플레이 케릭터 형상 갱신.
	for( int i=0; i<MAX_SELECTCHAR; i++ )
	{
		m_CharSham[i].ResetCharInfo ();
		m_CharSham[i].SetSelect ( FALSE );

		InitRenderSkip();
	}

	if( m_nCharStart >= m_nChaSNum )
	{
		ShiftLBDown();
		return;
	}

	for( int i=m_nCharStart, j=0; i<m_nChaSNum; i++, j++ )
	{
		if( j >= MAX_SELECTCHAR ) 
			break;

		m_CharSham[j].SetCharInfo( m_CharInfo[i] );
	}
}

BOOL DxLobyStage::ShiftRightChar()
{
	if( (m_nCharStart+MAX_SELECTCHAR) < m_nChaSNum )
	{
		m_nCharStart += MAX_SELECTCHAR;
		return TRUE;
	}

	return FALSE;
}

BOOL DxLobyStage::ShiftLeftChar()
{
	if( (m_nCharStart-MAX_SELECTCHAR) >= 0 )
	{
		m_nCharStart -= MAX_SELECTCHAR;
		return TRUE;
	}

	return FALSE;
}

BOOL DxLobyStage::ShiftCharIndex(INT nIndex )
{
	if( 0 <= nIndex * MAX_SELECTCHAR && nIndex * MAX_SELECTCHAR < m_nChaSNum )
	{
		if( nIndex * MAX_SELECTCHAR == m_nCharStart )
			return FALSE;

		m_nCharStart = nIndex * MAX_SELECTCHAR;
		m_bShiftButton = TRUE;
		return TRUE;
	}
	return FALSE;
}

void DxLobyStage::ShiftCharInfo()
{
	for(int i = 0; i < MAX_SELECTCHAR; i++ )
	{
		m_CharSham[ i ].ResetCharInfo();
	}

	for ( int i=m_nCharStart, j=0; i<m_nChaSNum; i++, j++ )
	{
		if( j >= MAX_SELECTCHAR ) 
			break;

		m_CharSham[j].SetCharInfo( m_CharInfo[i] );
	}
}

void DxLobyStage::ShiftChar( float fElapsedTime )
{
	DxInputDevice &dxInputDev = DxInputDevice::GetInstance();

	if( !m_bShiftButton )
	{
		if( false == GfxInterfaceBase::GetInstance()->IsMouseHitUI() && 
			false == GfxInterfaceBase::GetInstance()->IsDragging() )
		{
			if( (dxInputDev.GetKeyState( DIK_LEFT )&DXKEY_DOWN) || m_bShiftLBDown )
			{
				if( ShiftLeftChar() )
				{
					m_bShiftButton = TRUE;
				}
			}
			else if( (dxInputDev.GetKeyState( DIK_RIGHT )&DXKEY_DOWN) || m_bShiftRBDown )
			{
				if( ShiftRightChar() )
				{
					m_bShiftButton = TRUE;
				}
			}

			if( m_bShiftButton )
			{
				//	Note : 캐릭터 선택을 취소.
				m_dwCharSelect = MAX_SELECTCHAR;

				for ( int i=0; i<MAX_SELECTCHAR; i++ )
				{
					m_CharSham[i].ResetCharInfo ();
					m_CharSham[i].SetSelect ( FALSE );

					InitRenderSkip();
				}
			}
		}
	}

	if( m_bShiftButton )
	{
		m_fShiftTime += fElapsedTime;

		if( m_fShiftTime > 0.5f )
		{
			ShiftCharInfo();

			m_fShiftTime = 0;
			m_bShiftButton = FALSE;
			m_bShiftLBDown = FALSE;
			m_bShiftRBDown = FALSE;
		}
	}
}

//***********************************************
// Skip animation for about 1 second when initially rendering a character.
//***********************************************
void DxLobyStage::InitRenderSkip()
{
	m_bRenderSkip = TRUE;
	m_fRenderSkipTime = 0.0f;
}

HRESULT DxLobyStage::OneTimeSceneInit ( HWND hWnd, WORD wWidth, WORD wHeight, const char* szAppPath )
{
	GASSERT(hWnd);

	StringCchCopy ( m_szAppPath, MAX_PATH, szAppPath );
	m_hWnd = hWnd;

	m_dwCharSelect = MAX_SELECTCHAR;
	m_nCharStart = 0;

	return S_OK;
}

HRESULT DxLobyStage::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;

	m_pd3dDevice = pd3dDevice;

	//In the lobby, set Texture Quality to the highest.
	// Force the desired settings.
	RENDERPARAM::SetTextureQuality_FORCE( TEXTURE_QUALITY_HIGH );

	hr = m_pd3dDevice->GetDeviceCaps ( &m_d3dCaps );
	if ( FAILED(hr) )	return hr;

	WORD wWidth = 800;
	WORD wHeight = 600;

	//	Note : Get current resolution.
	//
	LPDIRECT3DSURFACEQ pBackBuffer=NULL;
	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

	if ( pBackBuffer )
	{
		pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
		pBackBuffer->Release();

		wWidth = static_cast<WORD> ( m_d3dsdBackBuffer.Width );
		wHeight = static_cast<WORD> ( m_d3dsdBackBuffer.Height );
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	Note : Viewport.
	//
	// D3DXVECTOR3 vFromPt		= D3DXVECTOR3( 0.0f, 70.0f, -70.0f );
	D3DXVECTOR3 vFromPt		= D3DXVECTOR3( -10.0f, -15.0f, -108.475f ); //-1.0f, -10.0f, -100.475f
	D3DXVECTOR3 vLookatPt	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	Note : Initialize character selection background terrain file.
	//
	m_spLandCharSlt->InitDeviceObjects ( m_pd3dDevice );
	m_spLandCharSlt->LoadFile ( "cha_select.wld", m_pd3dDevice, FALSE );

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//m_LandCharSlt_s01.InitDeviceObjects ( m_pd3dDevice );
	//m_LandCharSlt_s01.LoadFile ( "character_slt_s01.wld", m_pd3dDevice, FALSE );

 //   //---------------------------------------------------//
 //   NSLoadingDirect::FrameMoveRender( FALSE );
 //   //---------------------------------------------------//

	//m_LandCharSlt_s02.InitDeviceObjects ( m_pd3dDevice );
	//m_LandCharSlt_s02.LoadFile ( "character_slt_s02.wld", m_pd3dDevice, FALSE );

 //   //---------------------------------------------------//
 //   NSLoadingDirect::FrameMoveRender( FALSE );
 //   //---------------------------------------------------//

	//m_LandCharSlt_s03.InitDeviceObjects ( m_pd3dDevice );
	//m_LandCharSlt_s03.LoadFile ( "character_slt_s03.wld", m_pd3dDevice, FALSE );

 //   //---------------------------------------------------//
 //   NSLoadingDirect::FrameMoveRender( FALSE );
 //   //---------------------------------------------------//

	m_spLandLogin->InitDeviceObjects ( m_pd3dDevice );
	m_spLandLogin->LoadFile ( "log_in.wld", m_pd3dDevice, FALSE );

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// Note : Preloading..
	D3DXVECTOR3 vMax = D3DXVECTOR3(100000,100000,100000);
	D3DXVECTOR3 vMin = D3DXVECTOR3(-100000,-100000,-100000);
	m_spLandCharSlt->LoadBasicPos( m_pd3dDevice, vMax, vMin );
	//m_LandCharSlt_s01.LoadBasicPos( m_pd3dDevice, vMax, vMin );
	//m_LandCharSlt_s02.LoadBasicPos( m_pd3dDevice, vMax, vMin );
	//m_LandCharSlt_s03.LoadBasicPos( m_pd3dDevice, vMax, vMin );
	m_spLandLogin->LoadBasicPos( m_pd3dDevice, vMax, vMin );

	//	NOTE
	//		BGM setting, lobby is fixed
	m_spLandLogin->SetBgmFile( ID2GAMEWORD("LOBY_BGM") );		// Forced setting.
	m_spLandCharSlt->SetBgmFile( ID2GAMEWORD("LOBY_BGM") );	// Forced setting.

	m_spLandLogin->ActiveMap ( m_spLandLogin );

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	NOTE
	//		BGM Settings, lobby is fixed
	//DxBgmSound::GetInstance().SetFile ( ID2GAMEWORD("LOBY_BGM") );
	//DxBgmSound::GetInstance().Play ();

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	Note : Initialize character selection object.
	//
	char szName[MAX_PATH] = {0};
	for ( int i=0; i<MAX_SELECTCHAR; i++ )
	{
		StringCchPrintf ( szName, MAX_PATH, "character[%02d]", i+1 );
		DxLandGateMan* pLandGateMan = m_spLandCharSlt->GetLandGateMan();
		PDXLANDGATE pLandGate = pLandGateMan->FindLandGate ( szName );
		GASSERT(pLandGate&&"character[%02d] Could not find." );
		if ( pLandGate )
		{
            //---------------------------------------------------//
            NSLoadingDirect::FrameMoveRender( FALSE );
            //---------------------------------------------------//

			m_vCharPos[ i ] = (pLandGate->GetMax()+pLandGate->GetMin())/2.0f;
			m_CharSham[i].Create( &m_vCharPos[ i ], m_pd3dDevice, TRUE, i );
		}
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	DxSkinCharDataContainer::GetInstance().LoadData( "chs_kt.chf", pd3dDevice );
	DxSkinCharDataContainer::GetInstance().LoadData( "chs_ac.chf", pd3dDevice );
	DxSkinCharDataContainer::GetInstance().LoadData( "chs_mg.chf", pd3dDevice );

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	StringCchPrintf ( szName, MAX_PATH, "Create_Char" );
	DxLandGateMan* pLandGateMan = m_spLandCharSlt->GetLandGateMan();
	PDXLANDGATE pLandGate = pLandGateMan->FindLandGate ( szName );
	GASSERT(pLandGate&&"Create_Char Could not find." );
	if ( pLandGate )
	{
		D3DXVECTOR3 vPos = (pLandGate->GetMax()+pLandGate->GetMin())/2.0f;
		m_NewCharSham.Create( &vPos, m_pd3dDevice, TRUE, MAX_SELECTCHAR );
	}

	// Since the ZoomOut value changes when loading wld, reset it to the ZoomOut value for Lobby.
	DxViewPort::GetInstance().SetLobyCamera ();
	return S_OK;
}

HRESULT DxLobyStage::RestoreDeviceObjects ()
{
	HRESULT hr;
	LPDIRECT3DSURFACEQ pBackBuffer;
	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

    pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
    pBackBuffer->Release();

	//	Note : Viewport
	//
	DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)m_d3dsdBackBuffer.Width, 
		(float)m_d3dsdBackBuffer.Height, 5.0f, 3000.0f );
	
	hr = DxViewPort::GetInstance().SetViewPort ();
	if ( FAILED(hr) )	return hr;

	//	Note : Initialize character selection background terrain file.
	//
	m_spLandCharSlt->RestoreDeviceObjects ( m_pd3dDevice );
	//m_LandCharSlt_s01.RestoreDeviceObjects ( m_pd3dDevice );
	//m_LandCharSlt_s02.RestoreDeviceObjects ( m_pd3dDevice );
	//m_LandCharSlt_s03.RestoreDeviceObjects ( m_pd3dDevice );

	m_spLandLogin->RestoreDeviceObjects ( m_pd3dDevice );

	//	Note : Initialize character selection object.
	//
	for ( int i=0; i<MAX_SELECTCHAR; i++ )
	{
		m_CharSham[i].RestoreDeviceObjects ( m_pd3dDevice );
	}

	m_NewCharSham.RestoreDeviceObjects( m_pd3dDevice );

	m_setCheckGpuElapsedTime.clear();
	m_fCheckGpuTimeADD = 0.f;
	m_nCheckGpuCountADD = 0;

	// [shhan][2015.04.03] 원래는 FrameMove 쪽에 있었는데 그럴 필요는 없어서 이곳으로 옮겼다.
	//						또한 말도 안되는 Crash 가 발생하여 확인하기 위함도 있다.
	GLPeriod::GetInstance().SetPeriod ( 2003, 180, 1, 0 );
	return S_OK;
}

HRESULT DxLobyStage::InvalidateDeviceObjects ()
{
	//	Note : 캐릭터 선택 배경 지형 파일 초기화.
	//
	m_spLandCharSlt->InvalidateDeviceObjects ();
	//m_LandCharSlt_s01.InvalidateDeviceObjects ();
	//m_LandCharSlt_s02.InvalidateDeviceObjects ();
	//m_LandCharSlt_s03.InvalidateDeviceObjects ();

	m_spLandLogin->InvalidateDeviceObjects ();

	//	Note : 캐릭터 선택 오브젝트 초기화.
	//
	for ( int i=0; i<MAX_SELECTCHAR; i++ )
	{
		m_CharSham[i].InvalidateDeviceObjects ();
	}

	m_NewCharSham.InvalidateDeviceObjects();
    return S_OK;
}

HRESULT DxLobyStage::DeleteDeviceObjects ()
{
	//	Note : 캐릭터 선택 배경 지형 파일 초기화.
	//
	m_spLandCharSlt->DeleteDeviceObjects ();
	//m_LandCharSlt_s01.DeleteDeviceObjects ();
	//m_LandCharSlt_s02.DeleteDeviceObjects ();
	//m_LandCharSlt_s03.DeleteDeviceObjects ();

	m_spLandLogin->DeleteDeviceObjects ();

	DxSkinCharDataContainer::GetInstance().ReleaseData ( "select01.chf" );
	DxSkinCharDataContainer::GetInstance().ReleaseData ( "select02.chf" );

	//DxBgmSound::GetInstance().ForceStop ();

	m_textureRes.Clear();

	// 오토 옵션 확인 다 못했는데 로비화면이 끝났다면, LOW 옵션으로 강제 셋팅한다.
	if ( !RANPARAM::IsAutoOptionSame() )
	{
		RENDERPARAM::g_emTnLMode = TnL_PS_1_1;
		DxShadowMap::GetInstance().SetShadowDetail( LAND_QT_LOW );

		RANPARAM::SetAutoOptionVer();
	}

	return S_OK;
}

HRESULT DxLobyStage::FinalCleanup ()
{
	//	Note : 캐릭터 선택 배경 지형 파일 초기화.
	//
	m_spLandCharSlt->CleanUp ();
 //   m_LandCharSlt_s01.CleanUp ();
	//m_LandCharSlt_s02.CleanUp ();
	//m_LandCharSlt_s03.CleanUp ();

    m_spLandLogin->CleanUp ();

	m_bGameJoin = FALSE;

	//	Note : 안쓰는 텍스쳐들이나 정보들을 들고있기 때문에 정리를 해준다.
	//
	for ( int i=0; i<MAX_SELECTCHAR; i++ )
	{
		m_CharSham[i].DeleteSkin();
	}

	m_NewCharSham.DeleteSkin();

	return S_OK;
}

HRESULT DxLobyStage::FrameMove( float fTime, float fElapsedTime, BOOL bNotRendering )
{
	m_fElapsedTime = fElapsedTime;

	DxInputDevice &dxInputDev = DxInputDevice::GetInstance();

	if( true == m_bChangedStage )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_EnterLoginStage );
		m_bChangedStage = false;
	}

	DWORD MouseLKey = false == GfxInterfaceBase::GetInstance()->IsMouseHitUI() && 
					  false == GfxInterfaceBase::GetInstance()->IsDragging() ? 
						dxInputDev.GetMouseState( DXMOUSE_LEFT ) : 0;

	if( (dxInputDev.GetKeyState( DIK_F12 )&DXKEY_DOWN) )
	{
		m_bUseInterface = !m_bUseInterface;
	}

	NSCubeMapUtil::NSCubeMapBlend::OnFrameMove( fElapsedTime );

	int nCurStageBG = GfxInterfaceBase::GetInstance()->GetStageBG();

	if( m_nStageBG != nCurStageBG )
	{
		// [2012.01.18] shhan
		// 스테이지가 변경되면 시간을 0으로 가도록한다.
		// 이렇게 안 해줄 경우 로그인 화면이 처음 나올 시기에 fElapsedTime 값은 얼마나 큰 값이 나올지는 아무도 모른다.
		{
			fElapsedTime = 0.f;
		}

		switch ( m_nStageBG )
		{
		case EMSTAGE_BG_LOGIN :
			break;

		case EMSTAGE_BG_SELECT_CHAR :
			for ( int i=0; i<MAX_SELECTCHAR; i++ )
			{
				m_CharSham[i].SetSelect ( FALSE );
				//m_CharSham[i].FrameMove ( 0, 0 );
			}
			break;

		case EMSTAGE_BG_CREATE_CHAR :
			{
				m_NewCharSham.SetSelect ( FALSE );
				//m_NewCharSham.FrameMove ( 0, 0 );
			}
			break;
		}

		DxWeatherMan::GetInstance()->ReSetSound ();
		//		DxBgmSound::GetInstance().Stop ();

		switch( nCurStageBG )
		{
		case EMSTAGE_BG_LOGIN :
			{
				m_spLandLogin->ActiveMap ( m_spLandLogin );
				m_spLandLogin->SetNSLandThread();
				DxCamAniMan* pCamAniMan = m_spLandLogin->GetCamAniMan();
				pCamAniMan->ActiveCameraPos ( "login" );
			}
			break;

		case EMSTAGE_BG_SELECT_CHAR :
			{
				//	선택된 케릭터 초기화.
				m_dwCharSelect = MAX_SELECTCHAR;

				m_spLandCharSlt->ActiveMap ( m_spLandCharSlt );
				m_spLandCharSlt->SetNSLandThread();
				DxCamAniMan* pCamAniMan = m_spLandCharSlt->GetCamAniMan();
				pCamAniMan->ActiveCameraPos ( "select_character" );

				InitRenderSkip();

				DxLightMan::GetInstance()->EnableLighting( true );
			}
			break;

		case EMSTAGE_BG_CREATE_CHAR :
			{
				m_spLandCharSlt->ActiveMap ( m_spLandCharSlt );
				m_spLandCharSlt->SetNSLandThread();
				DxCamAniMan* pCamAniMan = m_spLandCharSlt->GetCamAniMan();
				pCamAniMan->ActiveCameraPos ( "create_camera" );

				InitRenderSkip();

				m_sCharNewInfo.RESET();
				SelCharSchool( m_sCharNewInfo.m_wSchool );
				// 캐릭터 성별 Test
				// 카메라 및 조명 셋팅				
				D3DXVECTOR3 vFromPt		= DxViewPort::GetInstance().GetFromPt();
				D3DXVECTOR3 vLookatPt	= DxViewPort::GetInstance().GetLookatPt();
				D3DXVECTOR3 vUpVec		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
				vFromPt.y = 19.0f;

				//DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );
				DxLightMan::GetInstance()->EnableLighting( true );					
				//#endif

			}
			break;
		};

		m_nStageBG = nCurStageBG;
	}

	switch ( nCurStageBG )
	{
	case EMSTAGE_BG_LOGIN :
		{
			//	Note : 뷰포트
			//
			DxCamAniMan* pCamAniMan = m_spLandLogin->GetCamAniMan();
			pCamAniMan->ActiveCameraAni ( "login" );

			DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)m_d3dsdBackBuffer.Width, 
				(float)m_d3dsdBackBuffer.Height, 5.f, DxFogMan::GetInstance().GetFogEnd_RealTime() );

			//GLPeriod::GetInstance().SetPeriod ( 2003, 180, 1, 0 );
			m_spLandLogin->FrameMove ( fTime, fElapsedTime, DxViewPort::GetInstance().GetClipVolume(), bNotRendering );
		}
		break;

	case EMSTAGE_BG_SELECT_CHAR :
		{
			DxCamAniMan* pCamAniMan = m_spLandCharSlt->GetCamAniMan();
			pCamAniMan->ActiveCameraPos ( "select_character" );

			//	Note : 뷰포트
			//

			D3DXVECTOR3 vFromPt		= DxViewPort::GetInstance().GetFromPt();
			D3DXVECTOR3 vLookatPt	= DxViewPort::GetInstance().GetLookatPt();
			D3DXVECTOR3 vUpVec		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );
			DxLightMan::GetInstance()->EnableLighting( true );
			DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)m_d3dsdBackBuffer.Width, 
				(float)m_d3dsdBackBuffer.Height, 5.f, DxFogMan::GetInstance().GetFogEnd_RealTime() );

			//GLPeriod::GetInstance().SetPeriod ( 2003, 180, 1, 0 );

			// 테스트용
			static int g_nFrameMoveCount(0);
			++g_nFrameMoveCount;
			int					_nStartCharNum(m_nStartCharNum);			//	서버의 생성된 케릭 숫자.
			int					_nStartCharLoad(m_nStartCharLoad);			//	클라이언트에 로드된 케릭 숫자. ( 정보 수신된. )
			int					_nChaSNum(m_nChaSNum);					//	생성된 케릭터 숫자. ( 종전 케릭 수 + 새로 생성된 케릭 수 )
			DWORD				_dwCharSelect(m_dwCharSelect);
			int					_nCharStart(m_nCharStart);
			int					_nFrameMoveCount(g_nFrameMoveCount);

			//	Note : 캐릭터 선택 배경 지형 파일.
			//
			m_spLandCharSlt->FrameMove ( fTime, fElapsedTime, DxViewPort::GetInstance().GetClipVolume(), bNotRendering );

			DWORD dwCharSelect = MAX_SELECTCHAR;
			if ( MouseLKey&(DXKEY_UP|DXKEY_DUP) && m_bGameJoin == FALSE )
			{
				D3DXVECTOR3 vFrom, vLook;
				vFrom = DxViewPort::GetInstance().GetFromPt ();
				BOOL bOk = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vLook );

				if ( bOk )
				{
					//for ( int i=0; i<m_nChaSNum; i++ )
					for ( int i=0; i<MAX_SELECTCHAR; i++ )
					{
						if ( !m_CharSham[i].IsValidData() )		continue;

						D3DXVECTOR3 vMax, vMin;
						m_CharSham[i].GetAABB ( vMax, vMin );

						BOOL bCollision = COLLISION::IsCollisionLineToAABB ( vFrom, vLook, vMax, vMin );
						if ( bCollision )
						{
							dwCharSelect = i;

							GLWidgetScript::GetInstance().LuaCallFunc< void >(
								NSWIDGET_SCRIPT::g_strFunc_CharSelect_SetSelecrCharIndex, "-n, -n", dwCharSelect, m_nCharStart );

							m_pGlobalStage->GetGameStage()->SetSelectChar( &m_CharInfo[dwCharSelect+m_nCharStart] );

							break;
						}
					}
				}
			}
			//	Note : 클릭된 케릭이 있을때.
			//
			SelectCharacter( dwCharSelect );

			if ( dwCharSelect != MAX_SELECTCHAR )
			{
				if ( MouseLKey&DXKEY_DUP && !m_bGameJoin )
				{
					//	TODO : 선택된 케릭으로 게임 접속.
					//
					GLLobyCharClient::GetInstance().JoinGame( m_dwCharSelect + m_nCharStart );
				}
			}

			ShiftChar( fElapsedTime ); // 캐릭터 쉬프트


			// ShiftChar 에서 캐릭터가 변경될 수도 있으므로 순서를 바꾼다.
			D3DXVECTOR3 vDir = DxViewPort::GetInstance().GetFromPt() - DxViewPort::GetInstance().GetLookatPt();

			if( m_bRenderSkip )
			{
				m_fRenderSkipTime += fElapsedTime;
				if ( m_fRenderSkipTime > 1.f )
				{
					m_bRenderSkip = FALSE;
				}
			}

			//	Note : 캐릭터 선택 오브젝트.
			//
			if ( !m_bRenderSkip )
			{
				for ( int i=0; i<MAX_SELECTCHAR; i++ )
				{
					m_CharSham[i].SetDirection ( vDir );
					m_CharSham[i].FrameMove ( fTime, fElapsedTime );
				}
			}
		}
		break;

	case EMSTAGE_BG_CREATE_CHAR :
		{
			//if ( m_bStyle )
			{
				DxCamAniMan* pCamAniMan = m_spLandCharSlt->GetCamAniMan();
				pCamAniMan->ActiveCameraAni ( "create_camera" );

				//	Note : 뷰포트
				//

				D3DXVECTOR3 vFromPt		= DxViewPort::GetInstance().GetFromPt();
				D3DXVECTOR3 vLookatPt	= DxViewPort::GetInstance().GetLookatPt();
				D3DXVECTOR3 vUpVec		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
				DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );
				DxLightMan::GetInstance()->EnableLighting( true );
				DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)m_d3dsdBackBuffer.Width, 
					(float)m_d3dsdBackBuffer.Height, 5.f, DxFogMan::GetInstance().GetFogEnd_RealTime() );

				//GLPeriod::GetInstance().SetPeriod ( 2003, 180, 1, 0 );

				//	Note : 캐릭터 선택 배경 지형 파일.
				//
				m_spLandCharSlt->FrameMove ( fTime, fElapsedTime, DxViewPort::GetInstance().GetClipVolume(), bNotRendering );

				m_NewCharSham.SetSelect( FALSE );
				m_NewCharSham.SetDirection( m_vDir );

				m_NewCharSham.m_wFace = m_sCharNewInfo.m_wFace;
				m_NewCharSham.m_wHair = m_sCharNewInfo.m_wHair;
				m_NewCharSham.m_wHairColor = m_sCharNewInfo.m_wHairColor;

				m_NewCharSham.UpdateFaceHair();

				if( m_bRenderSkip )
				{
					m_fRenderSkipTime += fElapsedTime;
					if ( m_fRenderSkipTime > 0.2f )
					{
						m_bRenderSkip = FALSE;
					}
				}

				//	Note : 캐릭터 선택 오브젝트.
				//
				if ( !m_bRenderSkip )
				{
					m_NewCharSham.FrameMove( fTime, fElapsedTime );
				}
			}
			//#endif
		}
		break;
	};

	// Rendering 이 변경되어서 기존 옵션으로 플레이 할 경우 정상적인 플레이가 불가능해져서, 
	// 프로그래머가 원할 때, 옵션 확인 후 알맞는 사양으로 셋팅 할 수 있도록 작업해줌.
	if ( !RANPARAM::IsAutoOptionSame() )
	{
		// 정상적인 fElapsedTime 인지 확인한다.
		// 최소한 10fps 이하는 되야지 않겠나? 안되면 볼 필요도 없다. DeleteDeviceObjects 때 TnL_PS_1_1 로 셋팅된다.
		// 0.f 는 말이 안되는 값이다.
		if ( fElapsedTime < 0.1f && fElapsedTime > 0.f )
		{
			m_fCheckGpuTimeADD += fElapsedTime;
			if ( m_fCheckGpuTimeADD > 1.f )	// 초반 파일 로딩 같은 변동 사양 때문에, 1 초 정도의 시간은 계산에서 제외한다.
			{
				m_setCheckGpuElapsedTime.insert( fElapsedTime );
				++m_nCheckGpuCountADD;

				// 50개의 sample 을 얻어서 중앙에 값을 사용하도록 한다.
				if ( m_nCheckGpuCountADD >= 50 )
				{
					float fCheckGpuElapsedTimeAVG(1.f);
					DWORD dwCount(0);
					std::set<float>::iterator iter = m_setCheckGpuElapsedTime.begin();
					for ( ; iter!=m_setCheckGpuElapsedTime.end(); ++iter, ++dwCount )
					{
						if ( dwCount >= m_setCheckGpuElapsedTime.size()/2 )
						{
							fCheckGpuElapsedTimeAVG = (*iter);
							break;
						}
					}

//#define VISIBLE_MESSAGE_FPS
#ifdef VISIBLE_MESSAGE_FPS
					static TSTRINGSTREAM sstrName;
#endif
					RENDERPARAM::SetTextureFiltering( TEXTURE_FILTER_ANISOTROPIC_8X );

					// ReleaseD로 체크됨.
					//
					// 파동 나왔을 때의 로그인 화면
					//
					// CPU : i5-2500(3.3G)				GPU : Geforce GTX 460		ElapsedTime/fps : 0.00213xx, 469.0fps		TnL_PS_3_0_CSM
					//
					// CPU : Dual-Core E5200(2.5G)		GPU : Geforce 8400 GS		ElapsedTime/fps : 0.0378xxx, 26.3fps		TnL_PS_3_0_CSM
					// CPU : Dual-Core E5200(2.5G)		GPU : Geforce 8400 GS		ElapsedTime/fps : 0.0349xxx, 28.6fps		TnL_PS_3_0_NM
					// CPU : Dual-Core E5200(2.5G)		GPU : Geforce 8400 GS		ElapsedTime/fps : 0.0207xxx, 48.2fps		TnL_PS_3_0_DF
					// CPU : Dual-Core E5200(2.5G)		GPU : Geforce 8400 GS		ElapsedTime/fps : 0.0039xxx, 251.6fps		TnL_PS_1_1
					//
					// CPU : Dual-Core E5200(2.5G)		GPU : Intel Q45/Q43			ElapsedTime/fps : 0.0235xxx, 42fps			TnL_PS_3_0_DF
					// CPU : Dual-Core E5200(2.5G)		GPU : Intel Q45/Q43			ElapsedTime/fps : 0.00605xx, 165.0fps		TnL_PS_1_1
					//
					// CPU : Dual-Core E5200(2.5G)		GPU : Geforce 7600			ElapsedTime/fps : 0.00210xx, 475.9fps		TnL_PS_1_1	( D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS : FALSE )
					//
					// CPU : Dual-Core E8500(3.16G)		GPU : AMD HD 2600			ElapsedTime/fps : 0.0209xxx, 47.fps			TnL_PS_3_0_CSM
					// CPU : Dual-Core E8500(3.16G)		GPU : AMD HD 2600			ElapsedTime/fps : 0.0204xxx, 48.fps			TnL_PS_3_0_NM
					// CPU : Dual-Core E8500(3.16G)		GPU : AMD HD 2600			ElapsedTime/fps : 0.0120xxx, 83.fps			TnL_PS_3_0_DF
					// CPU : Dual-Core E8500(3.16G)		GPU : AMD HD 2600			ElapsedTime/fps : 0.00293xx, 340.fps		TnL_PS_1_1
					//
					// CPU : Core2 Duo E8500(3.16G)		GPU : Intel Q45/Q43			ElapsedTime/fps : 0.0195xxx, 51.1fps		TnL_PS_3_0_DF
					// CPU : Core2 Duo E8500(3.16G)		GPU : Intel Q45/Q43			ElapsedTime/fps : 0.00539xx, 185.0fps		TnL_PS_1_1
					//
					//
					// 극강부 리뉴얼 나왔을 때의 로그인 화면
					//
					// CPU : i5-2500(3.3G)				GPU : Geforce GTX 460		ElapsedTime/fps : 0.00266xx, 383.4fps		TnL_PS_3_0_CSM
					//
					// CPU : Dual-Core E5200(2.5G)		GPU : Intel Q45/Q43			ElapsedTime/fps : 0.0349xxx, 28.6fps		TnL_PS_3_0_DF
					// CPU : Dual-Core E5200(2.5G)		GPU : Intel Q45/Q43			ElapsedTime/fps : 0.0156xx,  64.0fps		TnL_PS_1_1		TnL_CHAR_PS_1_1
					//
					//
					if ( fCheckGpuElapsedTimeAVG > 0.01f && ( RENDERPARAM::g_emTnLMode == TnL_PS_3_0_CSM ||
															RENDERPARAM::g_emTnLMode == TnL_PS_3_0_NM ||
															RENDERPARAM::g_emTnLMode == TnL_PS_3_0_DF ) )		// 100fps 이상은 되어야 한다. ( Test Rendering 과 실제 Rendering 중복이라서 100fps 로 잡음 )
					{
						// 이쪽은 지형만 검사한다.

						// 단계를 낮춘다.
						if ( RENDERPARAM::g_emTnLMode == TnL_PS_3_0_CSM )
						{
							//RENDERPARAM::g_emTnLMode = TnL_PS_3_0_NM;						// DXPARAMSET::GetInstance().SetShadowChar 에서 셋팅됨.
							//DxShadowMap::GetInstance().SetShadowDetail( LAND_QT_HIGH );	// DXPARAMSET::GetInstance().SetShadowChar 에서 셋팅됨.
							DXPARAMSET::GetInstance().SetShadowChar( 3 );
							RENDERPARAM::SetTextureFiltering( TEXTURE_FILTER_ANISOTROPIC_2X );

#ifdef VISIBLE_MESSAGE_FPS
							sstrName << _T("TnL_PS_3_0_CSM : ");
							sstrName << fCheckGpuElapsedTimeAVG;
							sstrName << _T(" / ");
							sstrName << 1.f/fCheckGpuElapsedTimeAVG;
							sstrName << _T("\r\n");
#endif
						}
						else if ( RENDERPARAM::g_emTnLMode == TnL_PS_3_0_NM )
						{
							//RENDERPARAM::g_emTnLMode = TnL_PS_3_0_DF;						// DXPARAMSET::GetInstance().SetShadowChar 에서 셋팅됨.
							//DxShadowMap::GetInstance().SetShadowDetail( LAND_QT_MIDDLE );	// DXPARAMSET::GetInstance().SetShadowChar 에서 셋팅됨.
							DXPARAMSET::GetInstance().SetShadowChar( 2 );
							RENDERPARAM::SetTextureFiltering( TEXTURE_FILTER_TRILINEAR );

#ifdef VISIBLE_MESSAGE_FPS
							sstrName << _T(" TnL_PS_3_0_NM : ");
							sstrName << fCheckGpuElapsedTimeAVG;
							sstrName << _T(" / ");
							sstrName << 1.f/fCheckGpuElapsedTimeAVG;
							sstrName << _T("\r\n");
#endif
						}
						else if ( RENDERPARAM::g_emTnLMode == TnL_PS_3_0_DF )
						{
							//RENDERPARAM::g_emTnLMode = TnL_PS_1_1;						// DXPARAMSET::GetInstance().SetShadowChar 에서 셋팅됨.
							//DxShadowMap::GetInstance().SetShadowDetail( LAND_QT_LOW );	// DXPARAMSET::GetInstance().SetShadowChar 에서 셋팅됨.
							DXPARAMSET::GetInstance().SetShadowChar( 1 );
							RENDERPARAM::SetTextureFiltering( TEXTURE_FILTER_TRILINEAR );

#ifdef VISIBLE_MESSAGE_FPS
							sstrName << _T(" TnL_PS_3_0_DF : ");
							sstrName << fCheckGpuElapsedTimeAVG;
							sstrName << _T(" / ");
							sstrName << 1.f/fCheckGpuElapsedTimeAVG;
							sstrName << _T("\r\n");
#endif
						}
					}
					else
					{
						// 300fps 이상은 되어야 한다. 수치가 미달이라면 옵션을 낮춘다. TnL_CHAR_PS_3_0 는 300 fps
						if ( fCheckGpuElapsedTimeAVG > 0.0033f && RENDERPARAM::g_emCharacterQulity == TnL_CHAR_PS_3_0 )
						{
#ifdef VISIBLE_MESSAGE_FPS
							if ( RENDERPARAM::g_emTnLMode == TnL_PS_3_0_CSM )
							{
								sstrName << _T(" TnL_PS_3_0_CSM : ");
							}
							else if ( RENDERPARAM::g_emTnLMode == TnL_PS_3_0_NM )
							{
								sstrName << _T(" TnL_PS_3_0_NM : ");
							}
							else if ( RENDERPARAM::g_emTnLMode == TnL_PS_3_0_DF )
							{
								sstrName << _T(" TnL_PS_3_0_DF : ");
							}
							else if ( RENDERPARAM::g_emTnLMode == TnL_PS_1_1 )
							{
								sstrName << _T(" TnL_PS_1_1 : ");
							}
							else
							{
								sstrName << _T(" TnL_FIXED : ");
							}
#endif

							if ( RENDERPARAM::g_emCharacterQulity == TnL_CHAR_PS_3_0 )
							{
								//RENDERPARAM::g_emCharacterQulity = TnL_CHAR_PS_2_0;			// DXPARAMSET::GetInstance().SetSkinDetail 에서 셋팅됨.
								//DxShadowMap::GetInstance().SetShadowDetail( LAND_QT_HIGH );	// DXPARAMSET::GetInstance().SetSkinDetail 에서 셋팅됨.
								DXPARAMSET::GetInstance().SetSkinDetail( 1 );

#ifdef VISIBLE_MESSAGE_FPS
								sstrName << _T(" TnL_CHAR_PS_3_0 : ");
								sstrName << fCheckGpuElapsedTimeAVG;
								sstrName << _T(" / ");
								sstrName << 1.f/fCheckGpuElapsedTimeAVG;
								sstrName << _T("\r\n");
#endif
							}
						}
						else
						{
							// 알맞는 셋팅이 되었다고 알려줌.
							RANPARAM::SetAutoOptionVer();
							DXPARAMSET::GetInstance().SetVideoLevel( 0 );

#ifdef VISIBLE_MESSAGE_FPS
							if ( RENDERPARAM::g_emTnLMode == TnL_PS_3_0_CSM )
							{
								sstrName << _T(" TnL_PS_3_0_CSM : ");
							}
							else if ( RENDERPARAM::g_emTnLMode == TnL_PS_3_0_NM )
							{
								sstrName << _T(" TnL_PS_3_0_NM : ");
							}
							else if ( RENDERPARAM::g_emTnLMode == TnL_PS_3_0_DF )
							{
								sstrName << _T(" TnL_PS_3_0_DF : ");
							}
							else if ( RENDERPARAM::g_emTnLMode == TnL_PS_1_1 )
							{
								sstrName << _T(" TnL_PS_1_1 : ");
							}
							else
							{
								sstrName << _T(" TnL_FIXED : ");
							}

							if ( RENDERPARAM::g_emCharacterQulity == TnL_CHAR_PS_3_0 )
							{
								sstrName << _T(" TnL_CHAR_PS_3_0 : ");
							}
							else if ( RENDERPARAM::g_emCharacterQulity == TnL_CHAR_PS_2_0 )
							{
								sstrName << _T(" TnL_CHAR_PS_2_0 : ");
							}
							else
							{
								sstrName << _T(" TnL_CHAR_PS_1_1 : ");
							}

							sstrName << fCheckGpuElapsedTimeAVG;
							sstrName << _T(" / ");
							sstrName << 1.f/fCheckGpuElapsedTimeAVG;
							AfxMessageBox( sstrName.str().c_str() );
#endif
						}
					}

					m_setCheckGpuElapsedTime.clear();
					m_nCheckGpuCountADD = 0;
				}
			}
		}
	}

	return S_OK;
}

void DxLobyStage::SelectCharacter( DWORD dwCharSelect )
{
	if ( dwCharSelect != MAX_SELECTCHAR )
	{
		//	Note : 케릭터 선택 효과 표시.
		//
		if( m_dwCharSelect != dwCharSelect )
		{
			m_dwCharSelect = dwCharSelect;

			for ( int i=0; i<MAX_SELECTCHAR; i++ )	
				m_CharSham[i].SetSelect ( FALSE, TRUE );

			m_CharSham[m_dwCharSelect].SetSelect ( TRUE );
		}
	}
}

void DxLobyStage::UpdateSelSkinChar ( DxSkinChar* pSkinObj, EMCHARINDEX emIndex, WORD wFace, WORD wHair, WORD wHairColor )
{
	if ( !pSkinObj )	return;

	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	if ( sCONST.dwHEADNUM > wFace )
	{
		std::string strHEAD_CPS = sCONST.strHEAD_CPS[wFace];
	
		PDXCHARPART pCharPart = NULL;
		pCharPart = pSkinObj->GetPiece(PIECE_HEAD);		//	현제 장착 스킨.

		if ( !pCharPart || !pCharPart->GetFileName() || strcmp(strHEAD_CPS.c_str(),pCharPart->GetFileName()) )
		{
			pSkinObj->SetPiece( strHEAD_CPS.c_str(), m_pd3dDevice, 0, 0, EMSCD_ZERO );
		}
	}

	if ( sCONST.dwHAIRNUM > wHair )
	{
		std::string strHAIR_CPS = sCONST.strHAIR_CPS[wHair];
	
		PDXCHARPART pCharPart = NULL;
		pCharPart = pSkinObj->GetPiece(PIECE_HAIR);		//	현제 장착 스킨.

		if ( !pCharPart || !pCharPart->GetFileName() || strcmp(strHAIR_CPS.c_str(),pCharPart->GetFileName()) )
		{
			pSkinObj->SetPiece( strHAIR_CPS.c_str(), m_pd3dDevice, 0, 0, EMSCD_ZERO );
		}
	}

	// 헤어컬러 적용
	pSkinObj->SetHairColor( wHairColor );
}

HRESULT DxLobyStage::DxEnvironmentRender( DxLandMan* pWLD )
{
	DxEnvironment::GetInstance().ResetReflectionCV();
	if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && pWLD && pWLD->GetUsedMaterialSystem() )
	{
		DxEnvironment::GetInstance().RenderReflectionMaterial( m_pd3dDevice, pWLD );
	}
	else
	{
		PROFILE_BEGIN2("Environment Render");
		DxEnvironment::GetInstance().Render( m_pd3dDevice, NULL, pWLD );
		PROFILE_END2("Environment Render");
	}

	// Note : 그림자 Clear
	PROFILE_BEGIN2("DxShadowMap::Render");
	DxShadowMap::GetInstance().ClearShadow( m_pd3dDevice, pWLD->GetUsedMaterialSystem() );
	PROFILE_END2("DxShadowMap::Render");

	// 지형 그림자를 생성하도록 한다. 
	// PS_3_0 이상일 경우만 동작한다.
	if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
	{
		// 그림자 렌더전에 NSShadowLight::FrameMove 를 호출해줘야한다.
		NSShadowLight::FrameMove( m_fElapsedTime );
	}

	DxShadowMap::GetInstance().RenderShadowForLobbyStage( pWLD->GetUsedMaterialSystem() );
	pWLD->RenderShadow( m_pd3dDevice );

	return S_OK;
}

HRESULT DxLobyStage::MaterialSystemStretchRect( DxLandMan* pWLD )
{
	//if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && pWLD && pWLD->GetUsedMaterialSystem() )
	//{
	//	// Note : 
	//	D3DSURFACE_DESC	sDesc;
	//	LPDIRECT3DSURFACEQ pBackBuffer;
	//	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	//	pBackBuffer->GetDesc ( &sDesc );
	//	pBackBuffer->Release();

	//	m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorBuffer );
	//}

	return S_OK;
}

void DxLobyStage::DxRenderDefferedOpaque( DxLandMan* pWLD )
{
	//////////////////////////////////////////////////////////////////////////
	//					RenderDefferedOpaqueForPSF
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	LPDIRECT3DSURFACEQ	pColorBuffer(NULL);
	if ( RENDERPARAM::g_bEnable_HDR )
	{
		pColorBuffer = DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR;
	}
	else
	{
		pColorBuffer = DxSurfaceTex::GetInstance().m_pColorBuffer;
	}

	if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_DF )
	{
		NSDeffered::RenderDefferedForSM30DF
		(
			m_pd3dDevice,
			DxFogMan::GetInstance().GetFogColor(),
			pWLD,
			DxSurfaceTex::GetInstance().GetWidth(),
			DxSurfaceTex::GetInstance().GetHeight(),
			pColorBuffer,
			DxSurfaceTex::GetInstance().m_pColorSuf_1st,
			DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
			DxSurfaceTex::GetInstance().m_pColorSuf_4th,
			DxSurfaceTex::GetInstance().m_pColorTex_1st,
			DxSurfaceTex::GetInstance().m_pColorTex_2nd, 
			DxSurfaceTex::GetInstance().m_pColorTex_4th, 
			FALSE,
			0.f,
			matWorld,
			DxViewPort::GetInstance().GetMatView(),
			DxViewPort::GetInstance().GetMatProj(),
			DxViewPort::GetInstance().GetClipVolume(),
			FALSE,
			TRUE,
			TRUE,
			TRUE
		);
	}
	else
	{
		NSDeffered::RenderDefferedForSM30NM
		(
			m_pd3dDevice,
			DxFogMan::GetInstance().GetFogColor(),
			pWLD,
			DxSurfaceTex::GetInstance().GetWidth(),
			DxSurfaceTex::GetInstance().GetHeight(),
			pColorBuffer,
			DxSurfaceTex::GetInstance().m_pColorSuf_1st,
			DxSurfaceTex::GetInstance().m_pColorSuf_4th,
			DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
			DxSurfaceTex::GetInstance().m_pColorSuf_3rd,	// Color는 unsign64 를 쓰겠다.
			DxSurfaceTex::GetInstance().m_pColorSuf_LBuffer,
			DxSurfaceTex::GetInstance().m_pSufFourCC_INTZ,
			DxSurfaceTex::GetInstance().m_pColorTex_1st,
			DxSurfaceTex::GetInstance().m_pColorTex_4th,
			DxSurfaceTex::GetInstance().m_pColorTex_2nd,
			DxSurfaceTex::GetInstance().m_pColorTex_3rd, 	// Color는 unsign64 를 쓰겠다.
			DxSurfaceTex::GetInstance().m_pColorTex_LBuffer,
			DxSurfaceTex::GetInstance().m_pTexFourCC_INTZ, 
			matWorld,
			DxViewPort::GetInstance().GetMatView(),
			DxViewPort::GetInstance().GetMatProj(),
			DxViewPort::GetInstance().GetClipVolume(),
			FALSE
		);
	}

	// 지형 물 렌더링
	NSWaterMain::Render( m_pd3dDevice );
}

void DxLobyStage::DxRenderDefferedSoftAlpha( DxLandMan* pWLD )
{
	LPDIRECT3DSURFACEQ	pColorBuffer(NULL);
	if ( RENDERPARAM::g_bEnable_HDR )
	{
		pColorBuffer = DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR;
	}
	else
	{
		pColorBuffer = DxSurfaceTex::GetInstance().m_pColorBuffer;
	}

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	D3DCOLOR colorClear = D3DCOLOR_XRGB(0,0,0);
	if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_DF )
	{
		NSDeffered::RenderDefferedForSM30DF
		(
			m_pd3dDevice,
			colorClear,
			pWLD,
			DxSurfaceTex::GetInstance().GetWidth(),
			DxSurfaceTex::GetInstance().GetHeight(),
			pColorBuffer,
			DxSurfaceTex::GetInstance().m_pColorSuf_1st,
			DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
			DxSurfaceTex::GetInstance().m_pColorSuf_4th,
			DxSurfaceTex::GetInstance().m_pColorTex_1st,
			DxSurfaceTex::GetInstance().m_pColorTex_2nd, 
			DxSurfaceTex::GetInstance().m_pColorTex_4th, 
			FALSE,
			0.f,
			matWorld,
			DxViewPort::GetInstance().GetMatView(),
			DxViewPort::GetInstance().GetMatProj(),
			DxViewPort::GetInstance().GetClipVolume(),
			TRUE,
			TRUE,
			TRUE,
			FALSE
		);
	}
	else
	{
		NSDeffered::RenderDefferedForSM30NM
		(
			m_pd3dDevice,
			colorClear,
			pWLD,
			DxSurfaceTex::GetInstance().GetWidth(),
			DxSurfaceTex::GetInstance().GetHeight(),
			pColorBuffer,
			DxSurfaceTex::GetInstance().m_pColorSuf_1st,
			DxSurfaceTex::GetInstance().m_pColorSuf_4th,
			DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
			DxSurfaceTex::GetInstance().m_pColorSuf_3rd,	// Color는 unsign64 를 쓰겠다.
			DxSurfaceTex::GetInstance().m_pColorSuf_LBuffer,
			DxSurfaceTex::GetInstance().m_pSufFourCC_INTZ,
			DxSurfaceTex::GetInstance().m_pColorTex_1st,
			DxSurfaceTex::GetInstance().m_pColorTex_4th,
			DxSurfaceTex::GetInstance().m_pColorTex_2nd,
			DxSurfaceTex::GetInstance().m_pColorTex_3rd, 	// Color는 unsign64 를 쓰겠다.
			DxSurfaceTex::GetInstance().m_pColorTex_LBuffer,
			DxSurfaceTex::GetInstance().m_pTexFourCC_INTZ, 
			matWorld,
			DxViewPort::GetInstance().GetMatView(),
			DxViewPort::GetInstance().GetMatProj(),
			DxViewPort::GetInstance().GetClipVolume(),
			TRUE
		);
	}
}

HRESULT DxLobyStage::Render ()
{
	if ( DxFogMan::GetInstance().GetFogRange() != EMFR_OFF )
	{
		RenderWorldCharEffect();
	}

	return S_OK;
}

void DxLobyStage::RenderWorldCharEffect()
{
	D3DXVECTOR3 vDiffuse(1.f,1.f,1.f);
	D3DXVECTOR3 vPointLight(0.f,0.f,0.f);

	CLIPVOLUME &CV = DxViewPort::GetInstance().GetClipVolume ();

	if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX )	
	{
		m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, TRUE );
	}

	// [2013.07.01] Forces Fog to be turned off.
	//				Currently, this is the only way to do it on the map. The layout is too far away. Let's revert it when a normal map is created.
	DxFogMan::GetInstance().RenderFogSB ( m_pd3dDevice, TRUE );

	DxLandMan* pWLD( NULL );

	// g_emGroundQulity BackUp
	TnL_MODE emTnL_BackUp = RENDERPARAM::g_emGroundQulity;

	//There is a strange issue due to CSM issues, so if the option is high, lower it by one level to render.
	if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
	{
		RENDERPARAM::g_emGroundQulity = TnL_PS_3_0_DF;
	}

	//////////////////////////////////////////////////////////////////////////
	//							shadow

	// Check speed
	//Since the ZBuffer is completely cleared after drawing, there is no problem as long as you draw everything without missing any areas.
	if ( !RANPARAM::IsAutoOptionSame() && RENDERPARAM::g_emTnLMode >= TnL_PS_3_0_DF && m_spLandLogin->GetUsedMaterialSystem() )
	{
		DxEnvironmentRender( m_spLandLogin.get() );

		// Lower the level.
		if ( RENDERPARAM::g_emTnLMode == TnL_PS_3_0_CSM )
		{
			RENDERPARAM::g_emGroundQulity = TnL_PS_3_0_CSM;
		}
		else if ( RENDERPARAM::g_emTnLMode == TnL_PS_3_0_NM )
		{
			RENDERPARAM::g_emGroundQulity = TnL_PS_3_0_NM;
		}
		else if ( RENDERPARAM::g_emTnLMode == TnL_PS_3_0_DF )
		{
			RENDERPARAM::g_emGroundQulity = TnL_PS_3_0_DF;
		}

		// Test Rendering
		// Lets you start rendering with your desired options.
		// On low specs, there will be a lot of speed degradation when rendering this part..
		{
			DxCamAniMan* pCamAniMan = m_spLandLogin->GetCamAniMan();
			pCamAniMan->ActiveCameraPos ( "login" );

			// Let's do a render for speed check.
			DxRenderDefferedOpaque( m_spLandLogin.get() );
			DxRenderDefferedSoftAlpha( m_spLandLogin.get() );
			m_spLandLogin->RenderEff_Deffered( m_pd3dDevice, DxViewPort::GetInstance().GetClipVolume() );

			//			m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorBuffer );
			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, 0L, 1.0f, 0L );
		}

		// To check the speed, the basic rendering is set to version 1_1.
		// The actual render is rendered with low options..
		// Checking speed.
		RENDERPARAM::g_emGroundQulity = TnL_PS_1_1;
	}

	//	Note : Character selection background terrain file.
	//

	switch( m_nStageBG )
	{
	case EMSTAGE_BG_LOGIN :
		{
			pWLD = m_spLandLogin.get();

			DxEnvironmentRender( pWLD );

			if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && GetUsedMaterialSystem() )
			{
				DxRenderDefferedOpaque( pWLD );

				// Note : 
				OPTMManager::GetInstance().Render_HardAlpha();

				DxRenderDefferedSoftAlpha( pWLD );
				pWLD->RenderEff_Deffered( m_pd3dDevice, DxViewPort::GetInstance().GetClipVolume() );
			}
			else
			{
				m_spLandLogin->Render ( m_pd3dDevice, CV );

				// Note : 
				OPTMManager::GetInstance().Render_HardAlpha();

				m_spLandLogin->Render_LOW_SoftAlpha( m_pd3dDevice, CV );
				m_spLandLogin->Render_LOW_SingleEff( m_pd3dDevice, CV );
			}

			//MaterialSystemStretchRect( pWLD );
			//m_spLandLogin->Render_2Pass( m_pd3dDevice, CV, TRUE );
			//m_spLandLogin->RenderPickAlpha( m_pd3dDevice );
			DxWeatherMan::GetInstance()->Render ( m_pd3dDevice );
		}
		break;

	case EMSTAGE_BG_SELECT_CHAR :
		{
			pWLD = m_spLandCharSlt.get();
			DxCamAniMan* pCamAniMan = m_spLandCharSlt->GetCamAniMan();
			pCamAniMan->ActiveCameraPos ( "select_character" );

			DxEnvironmentRender( pWLD );

			if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && GetUsedMaterialSystem() )
			{
				DxRenderDefferedOpaque( pWLD );
			}
			else
			{
				m_spLandCharSlt->Render ( m_pd3dDevice, CV );
			}

			//	Note : Character selection object.
			//
			if( !m_bRenderSkip )
			{
				for ( int i=0; i<MAX_SELECTCHAR; i++ )
				{
					pWLD->CalculateActorWorldLight_Thread( vDiffuse, vPointLight, m_vCharPos[ i ] );
					m_CharSham[i].Render ( m_pd3dDevice, CV, FALSE, vDiffuse, vPointLight );

					int nTempIdx = m_nCharStart + i;
					if ( nTempIdx < 0 || nTempIdx >= MAX_SERVERCHAR )
						continue;

					// Note : Character Lock Image Render.
					if ( m_CharInfo[ nTempIdx ].IsLock() )
					{
						//!! SF_TODO
						//m_pOutInterface->ISetLockImagePos( m_vCharPos[ i ], i );
						//m_pOutInterface->ISetVisibleLockImage( true, i );
					}
				}
			}

			// Note : 
			OPTMManager::GetInstance().Render_HardAlpha();

			if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && GetUsedMaterialSystem() )
			{
				DxRenderDefferedSoftAlpha( pWLD );
				pWLD->RenderEff_Deffered( m_pd3dDevice, DxViewPort::GetInstance().GetClipVolume() );
			}
			else
			{
				m_spLandCharSlt->Render_LOW_SoftAlpha( m_pd3dDevice, CV );
				m_spLandCharSlt->Render_LOW_SingleEff( m_pd3dDevice, CV );
			}			

			//MaterialSystemStretchRect( pWLD );

			//m_spLandCharSlt->Render_2Pass( m_pd3dDevice, CV, TRUE );
			//m_spLandCharSlt->Render_EFF ( m_pd3dDevice, CV );
			//m_spLandCharSlt->RenderPickAlpha( m_pd3dDevice );

			DxWeatherMan::GetInstance()->Render ( m_pd3dDevice );
		}
		break;

	case EMSTAGE_BG_CREATE_CHAR :
		{
			//m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET,	D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

			//if ( m_bStyle )
			{
				//m_pd3dDevice->SetTexture ( 0, m_textureRes.GetTexture() );
				//m_pd3dDevice->SetFVF( TEXTUREVERTEXFVF );
				//m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_Vertices, sizeof(TEXTUREVERTEX) );

				//m_pd3dDevice->SetTexture ( 0, NULL );

				pWLD = m_spLandCharSlt.get();
				DxCamAniMan* pCamAniMan = m_spLandCharSlt->GetCamAniMan();
				pCamAniMan->ActiveCameraPos ( "create_camera" );

				DxEnvironmentRender( pWLD );

				//m_spLandCharSlt->Render ( m_pd3dDevice, CV );

				if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && GetUsedMaterialSystem() )
				{
					DxRenderDefferedOpaque( pWLD );
				}
				else
				{
					m_spLandCharSlt->Render ( m_pd3dDevice, CV );
				}

				if( !m_bRenderSkip )
				{
					pWLD->CalculateActorWorldLight_Thread( vDiffuse, vPointLight, m_NewCharSham.GetPosition() );
					m_NewCharSham.Render ( m_pd3dDevice, CV, FALSE, vDiffuse, vPointLight );
				}

				// Note : 
				OPTMManager::GetInstance().Render_HardAlpha();

				if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && GetUsedMaterialSystem() )
				{
					DxRenderDefferedSoftAlpha( pWLD );
					pWLD->RenderEff_Deffered( m_pd3dDevice, DxViewPort::GetInstance().GetClipVolume() );
				}
				else
				{
					m_spLandCharSlt->Render_LOW_SoftAlpha( m_pd3dDevice, CV );
					m_spLandCharSlt->Render_LOW_SingleEff( m_pd3dDevice, CV );
				}	

				//MaterialSystemStretchRect( pWLD );

				//m_spLandCharSlt->Render_2Pass( m_pd3dDevice, CV, TRUE );
				//m_spLandCharSlt->Render_EFF ( m_pd3dDevice, CV );
				//m_spLandCharSlt->RenderPickAlpha( m_pd3dDevice );

				DxWeatherMan::GetInstance()->Render ( m_pd3dDevice );
			}
			//#endif
		}
		break;

	};

	//////////////////////////////////////////////////////////////////////////
	// [2014.08.27][shhan] Lobby does not show Bloom.
	//////////////////////////////////////////////////////////////////////////
	//			HDR - Bloom 적용.
	if ( RENDERPARAM::g_bEnable_HDR )
	{
		m_pd3dDevice->StretchRect( DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR, NULL, DxSurfaceTex::GetInstance().m_pColorBuffer, NULL, D3DTEXF_POINT );
		m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorBuffer );
	}
	//NSBloomHDR::OnRender( m_pd3dDevice );
	//m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorBuffer );




	RENDERPARAM::g_emGroundQulity = emTnL_BackUp;

	if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX )		m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );

	PROFILE_BEGIN("DxEffGroupPlayer::Render");
	DxEffGroupPlayer::GetInstance().Render ( m_pd3dDevice );
	PROFILE_END("DxEffGroupPlayer::Render");

	// Note : Optimized Eff
	OPTMManager::GetInstance().Render();


	// Note : LightShafts Do the work.
	{
		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( m_pd3dDevice );

		if ( pWLD )
		{
			pWLD->Render_LightShafts(m_pd3dDevice);
		}
		NSLightShafts::OnRender(m_pd3dDevice);
	}

	//	Note : Cube texture rendering..!!
	//DxCubeMap::GetInstance().Render ( m_pd3dDevice );
	NSCubeMapUtil::NSCubeMapBlend::OnRender( m_pd3dDevice, NSCubeMapCulling::GetCubeMapTexture( DxViewPort::GetInstance().GetLookatPt() ) );

	//	Note :Used for glow treatment.
	//
	DxGlowMan::GetInstance().RenderTex ( m_pd3dDevice );
	DxGlowMan::GetInstance().Render ( m_pd3dDevice );
	DxGlowMan::GetInstance().RenderBURN( m_pd3dDevice );


	// StarGlow Rendering
	if ( DxSurfaceTex::GetInstance().m_pGlowSuf_SRC )
	{
		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( m_pd3dDevice );

		m_pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pGlowSuf_SRC );
		m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

		// StarGlow 렌더링
		m_pGaeaClient->RenderStarGlow();

		// Note : Glow 작업을 한다.
		NSGlow::OnRender( m_pd3dDevice, DxSurfaceTex::GetInstance().m_pColorBuffer );
	}

	if ( pWLD )
	{
		//Copy what you have drawn so far for refraction..
		PROFILE_BEGIN("DxEnvironment::UserToRefraction()");
		DxEnvironment::GetInstance().UserToRefraction ( m_pd3dDevice );
		PROFILE_END("DxEnvironment::UserToRefraction()");
	}

	//	Note :After saving what needs to be waved, spray it last.
	//

	PROFILE_BEGIN("DxEnvironment::RenderWave()");
	DxEnvironment::GetInstance().RenderWave ( m_pd3dDevice, DxEffectMan::GetInstance().GetBumpTexture() );
	PROFILE_END("DxEnvironment::RenderWave()");

	// Note : 후처리
	//DxPostProcess::GetInstance().Render( m_pd3dDevice );
}

void DxLobyStage::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	CString str;

	switch ( nmg->nType )
	{
	case NET_MSG_LOGIN_FB :
	case GS_NET_MSG_WB_LOGIN_FB : // GS의 경우 월드배틀 접속 여부 확인
		{
			GLLoginClient::GetInstance().MsgProcess( nmg );
		}
		break;

	case NET_MSG_CHA_DEL_FB_OK :
		{
			NET_CHA_DEL_FB* pCharDelFb = (NET_CHA_DEL_FB*) nmg;
			GLLoginClient::GetInstance().SetCharRemain( pCharDelFb->wChaRemain );
		}
		// break 빠진건 의도적인거임;
	case NET_MSG_LOBBY_CHAR_JOIN_FB : 
	case NET_MSG_LOBBY_CHARJOIN_2NDPASS :
	case NET_MSG_CHA_DEL_FB_CLUB:
	case NET_MSG_CHA_DEL_FB_ERROR:
	case NET_MSG_LOBBY_CHINA_ERROR:
		{
			m_bGameJoin = FALSE;

			GLLobyCharClient::GetInstance().MsgProcess( nmg );

			// 에러일 경우 재접속 방지
			if( NET_MSG_LOBBY_CHAR_JOIN_FB == nmg->nType )
			{
				GLMSG::SNETLOBBY_CHARJOIN_FB* pNetMsg = (GLMSG::SNETLOBBY_CHARJOIN_FB *) nmg;
				if( pNetMsg->emCharJoinFB == EMCJOIN_FB_ERROR )
				{
					m_bGameJoin = TRUE;
					// 로그인 프로세스 확인을 위한 로그
					if( m_pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. JOIN ERROR CODE 0"));
				}
			}
		}
		break;

	case NET_MSG_PASSCHECK_FB:
	case JAPAN_NET_MSG_UUID:
		{
			GLLobyCharClient::GetInstance().MsgProcess( nmg );
		}
		break;

	case NET_MSG_CHA_NEW_FB :
		{
			GLLobyCharCreateClient::GetInstance().MsgProcess( nmg );
		}
		break;

	case NET_MSG_CHA_BAINFO_AC:
		{
			// 로그인 프로세스 확인을 위한 로그
			if( m_pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN PROCESS. Answer Basic Char Info End"));

			NET_CHA_BBA_INFO_AC* pPacket = (NET_CHA_BBA_INFO_AC*) nmg;

			m_nStartCharNum = m_nChaSNum = pPacket->m_ChaServerTotalNum;

            std::vector<DWORD> vecChaDbNum;
            for ( int i=0; i<m_nChaSNum; i++ )
            {
                m_nChaIDs[i] = pPacket->m_ChaDbNum[i];
                vecChaDbNum.push_back(m_nChaIDs[i]);				
            }
            m_pGlobalStage->GetNetClient()->SndChaBasicInfoPack(vecChaDbNum);
			// 로그인 프로세스 확인을 위한 로그
			if( m_pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN PROCESS. Request Detail Char Info"));


			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CharSelect_InitList );

			// 유저들의 System 정보를 수집한다.
			if ( m_pGaeaClient )
			{
				GLMSG::SNET_USER_SYSTEM_INFO_CA NetMsg
					(
					sc::getProcessorNumber(), 
					sc::getCpuSpeed(), 
					sc::getMemoryPhysicalTotal(), 
					RENDERPARAM::m_dwResolutionX,
					RENDERPARAM::m_dwResolutionY,
					RENDERPARAM::m_dwGpuVendor,
					RENDERPARAM::m_dwGpuModel,
					RENDERPARAM::m_dwShaderModel, 
					static_cast< int >( RANPARAM::emNation )
					); 

				m_pGaeaClient->NETSENDTOAGENT( &NetMsg );
			}

			// 로비 스테이지에서 메세지박스는 오직 하나만 활성화 될 수 있다;
			// 만약 더필요하다면 아래 콜되는 함수내용 수정이 필요함;
			GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_LobbyWaitMsgBoxClose);
		}
		break;

	case NET_MSG_LOBBY_CHARINFO_AC:
		MsgLobbyCharInfoAC(nmg);
		break;

	case NET_MSG_LOBBY_CHARINFO_AC_END:
		MsgLobbyCharInfoACEnd(nmg);
		break;

	case NET_MSG_DEFAULT_GAME_OPTION:
		{
			NET_DEFAULT_GAME_OPTION* pPacket = ( NET_DEFAULT_GAME_OPTION* ) nmg;
			GLCONST_CHAR::bPARTY_2OTHERSCHOOL = pPacket->bPARTY_2OTHERSCHOOL;
			GLCONST_CHAR::bCLUB_2OTHERSCHOOL = pPacket->bCLUB_2OTHERSCHOOL;
			GLCONST_CHAR::fITEM_DROP_SCALE = pPacket->fITEM_DROP_SCALE;
			GLCONST_CHAR::fMONEY_DROP_SCALE = pPacket->fMONEY_DROP_SCALE;
			GLCONST_CHAR::fITEM_DROP_LOWER_SCALE = pPacket->fITEM_DROP_LOWER_SCALE;
			GLCONST_CHAR::dwCRITICAL_MAX = pPacket->dwCRITICAL_MAX;
			GLCONST_CHAR::fNONPK_ITEM_DROP = pPacket->fNONPK_ITEM_DROP;
			GLCONST_CHAR::fPK_ITEM_DROP = pPacket->fPK_ITEM_DROP;
		}
		break;
	default:
		CDebugSet::ToListView ( "DxLobyStage::MsgProcess unknown message type %d", nmg->nType );
		break;
	};
}

void  DxLobyStage::SetStyleStep( bool bStyle )
{
	m_bStyle = bStyle;
	InitRenderSkip();

	if ( m_bStyle )
	{
		SCHARINFO_LOBBY sCharInfo;
		sCharInfo.m_emClass = CharIndexToClass( m_sCharNewInfo.m_emIndex ); // 직업
	    sCharInfo.m_wSchool = m_sCharNewInfo.m_wSchool; // 학원.
	    sCharInfo.m_wHair = m_sCharNewInfo.m_wHair; // 헤어스타일
		sCharInfo.m_wFace = m_sCharNewInfo.m_wFace;
		sCharInfo.m_wHairColor = m_sCharNewInfo.m_wHairColor;

		SCHARDATA2& sCharData = GLCONST_CHAR::GET_CHAR_DATA2(m_sCharNewInfo.m_wSchool,m_sCharNewInfo.m_emIndex);

		sCharInfo.m_PutOnItems[SLOT_UPPER].Assign( sCharData.m_PutOnItems[SLOT_UPPER] );
		sCharInfo.m_PutOnItems[SLOT_LOWER].Assign( sCharData.m_PutOnItems[SLOT_LOWER] );		

		m_NewCharSham.SetCharInfo( sCharInfo );
		m_vDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f);

		//m_vDir = D3DXVECTOR3( -1.0f,-1.0f,-0.35f);
		
	}
	else
	{
		m_NewCharSham.ResetCharInfo();
	}
}

void  DxLobyStage::RotateChar( bool bLeft, float fDegree )
{
	/*if ( bLeft )
	{
		m_vDir.x += 0.05f;
		m_vDir.z -= 0.05f;		

		if ( m_vDir.x > 0.5f ) m_vDir.x = 0.5f;
		if ( m_vDir.z < 0.5f ) m_vDir.z = 0.5f;
	}
	else
	{
		m_vDir.x -= 0.05f;
		m_vDir.z += 0.05f;

		if ( m_vDir.x < -1.5f ) m_vDir.x = -1.5f;
		if ( m_vDir.z > 1.5f ) m_vDir.z = 1.5f;
	}*/

	if( false == bLeft )
		fDegree *= -1.0f;

	// Rotate around the y-axis.
	m_vDir = scmath::RotateVectorFromAxis( m_vDir, fDegree, D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
}

void DxLobyStage::MsgLobbyCharInfoAC( NET_MSG_GENERIC* nmg )
{
	// 로그인 프로세스 확인을 위한 로그
	if( m_pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN PROCESS. Answer Detail Char Info"));

    // NET_MSG_LOBBY_CHARINFO_AC
    // GLMSG::NET_LOBBY_CHARINFO_AC* pPacket = (GLMSG::NET_LOBBY_CHARINFO_AC*) nmg;
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;

    GLMSG::NET_LOBBY_CHARINFO_AC MsgData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), MsgData))
    {
        sc::writeLogError("DxLobyStage::MsgLobbyCharInfoAC");
        return;
    }

    BOOL bInserted = FALSE;

    // 기존 캐릭터 정보 수신시.
    for (int i=0; i<m_nChaSNum; ++i)
    {
        if (m_nChaIDs[i] == MsgData.Data.m_dwCharID)
        {
            bInserted = TRUE;
            m_CharInfo[i] = MsgData.Data;

            if (i < MAX_SELECTCHAR)
                m_CharSham[i].SetCharInfo(m_CharInfo[i]);

            ++m_nStartCharLoad;
			
			// 전장서버 접속 캐릭터 아이디 셋팅
			m_dwCharIDForWorldBattle = MsgData.Data.m_dwCharID;

            break;
        }
    }

    //	Note : 새로이 생성된 캐릭터 정보 수신시.
    if ( !bInserted )
    {
        if ( m_nChaSNum < MAX_SERVERCHAR )
        {
            m_nChaIDs[m_nChaSNum] = MsgData.Data.m_dwCharID;
            m_CharInfo[m_nChaSNum] = MsgData.Data;
            m_CharSham[m_nChaSNum%MAX_SELECTCHAR].SetCharInfo( m_CharInfo[m_nChaSNum] );

            ++m_nChaSNum;

			// 전장서버 접속 캐릭터 아이디 셋팅
			m_dwCharIDForWorldBattle = MsgData.Data.m_dwCharID;

            ShiftRBDown();
        }
    }

	// 캐릭터 리스트 초기화
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CharSelect_InitList );
}

// 캐릭터 정보 전송
// 자동 로그인 기능을 위해서 추가
void DxLobyStage::MsgLobbyCharInfoACEnd( NET_MSG_GENERIC* nmg )
{
	// 로그인 프로세스 확인을 위한 로그
	if( m_pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN PROCESS. Answer Detail Char Info End"));

	if( m_pGlobalStage->GetNetClient()->IsWorldBattleCharSelectEnd() )
	{
		// 로그인 프로세스 확인을 위한 로그
		if( m_pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN PROCESS. Answer Detail Char Info End ( Check Battle Server )"));

		// 통합전장서버에는 하나의 캐릭밖에 없어야한다. 따라서 무조건 인덱스 0이다.
		// 나중에 여러 캐릭이 있는경우 index를 사용해야함.
		SCHARINFO_LOBBY* pCharInfo = m_pGlobalStage->GetLobyStage()->GetCharInfo( 0 );
		if( NULL == pCharInfo )
			return;

		m_pGlobalStage->GetGameStage()->SetSelectChar( pCharInfo );
	
		if( NULL == pCharInfo || true == pCharInfo->IsLock() )
		{
			m_pGlobalStage->GetLobyStage()->m_bGameJoin = FALSE;

			// 로그인 프로세스 확인을 위한 로그
			if( m_pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. Cha Info error"));
			return;
		}
		else
		{
			m_pGlobalStage->GetLobyStage()->m_bGameJoin = TRUE;
			m_pGlobalStage->GetNetClient()->SetWorldBattleCharSelectEnd(FALSE);
			m_pGlobalStage->GetNetClient()->SndGameJoin(m_dwCharIDForWorldBattle, NULL , FALSE);	
		}
	}
}


BOOL DxLobyStage::GetUsedMaterialSystem()
{
	int nCurStageBG = GfxInterfaceBase::GetInstance()->GetStageBG();

	//	Note : 캐릭터 선택 배경 지형 파일.
	//
	switch( nCurStageBG )
	{
	case EMSTAGE_BG_LOGIN :
		{
			return m_spLandLogin->GetUsedMaterialSystem();
		}
		break;

	case EMSTAGE_BG_SELECT_CHAR :
	case EMSTAGE_BG_CREATE_CHAR :
		{
			return m_spLandCharSlt->GetUsedMaterialSystem();
		}
		break;

	default :
		return FALSE;
	};
}

void DxLobyStage::GetNewCharShanScreenRect(RECT& rc)
{
	D3DXVECTOR3 vMin, vMax, vScreenMin, vScreenMax;
	m_NewCharSham.GetAABB(vMin, vMax);
	vScreenMin = DxViewPort::GetInstance().ComputeVec3Project(&vMin, NULL);
	vScreenMax = DxViewPort::GetInstance().ComputeVec3Project(&vMax, NULL);
	rc.left = vScreenMin.x;
	rc.top = vScreenMin.y;
	rc.right = vScreenMax.x;
	rc.bottom = vScreenMax.y;
}