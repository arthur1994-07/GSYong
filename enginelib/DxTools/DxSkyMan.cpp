#include "pch.h"

#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../G-Logic/GLPeriod.h"
#include "../GlobalParam.h"
#include "./TextureManager.h"
#include "DxGlowMan.h"
#include "DxSurfaceTex.h"
#include "DxWeatherMan.h"
#include "DxViewPort.h"
#include "DxSkyCloud.h"
#include "DxSkyMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void	SKY_PROPERTY::SaveSet ( sc::SerialFile &SFile )
{
	SFile << VERSION;
	SFile << (DWORD)sizeof(SKY_PROPERTY);

	SFile << m_bSunMoon;
	SFile << m_bSkyEnable;
	SFile << m_bCloudEnable;
	SFile << m_fAxisValue;
	SFile << m_nRadioAxis;
}

void	SKY_PROPERTY::LoadSet ( sc::SerialFile &SFile )
{
	DWORD	dwVer;
	DWORD	dwBuffSize;

	SFile >> dwVer;
	SFile >> dwBuffSize;

	if ( dwVer == VERSION )	//	해와 달을 체크박스로 뺌
	{
		SFile >> m_bSunMoon;
		SFile >> m_bSkyEnable;
		SFile >> m_bCloudEnable;
		SFile >> m_fAxisValue;
		SFile >> m_nRadioAxis;
	}
	else if ( dwVer == 0x0103 )	//	데이터 포맷 변경 진행
	{
		SFile >> m_bSkyEnable;
		SFile >> m_bCloudEnable;
		SFile >> m_fAxisValue;
		SFile >> m_nRadioAxis;

		m_bSunMoon = m_bSkyEnable;
	}
	else if ( dwVer == 0x0102 )
	{
		SFile >> m_bSkyEnable;
		SFile >> m_bCloudEnable;
		SFile >> m_nRadioAxis;
		SFile >> m_fAxisValue;

		m_bSunMoon = m_bSkyEnable;
	}
	else if ( dwVer == 0x0101 )
	{
		SFile >> m_bSkyEnable;
		SFile >> m_bCloudEnable;

		m_nRadioAxis = 0;
		m_fAxisValue = 0.f;
		m_bSunMoon = m_bSkyEnable;
	}
	else if ( dwVer == 0x0100 )
	{
		SFile >> m_bSkyEnable;

		if ( m_bSkyEnable )	m_bCloudEnable = TRUE;	// 이전버젼에 켜져 있으면 켠다.

		m_nRadioAxis = 0;
		m_fAxisValue = 0.f;
		m_bSunMoon = m_bSkyEnable;
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
	}
}

DxSkyMan& DxSkyMan::GetInstance()
{
	static DxSkyMan Instance;
	return Instance;

}

D3DXMATRIX	DxSkyMan::m_matWorld = D3DXMATRIX(1.f,0.f,0.f,0.f,
											  0.f,1.f,0.f,0.f,
											  0.f,0.f,1.f,0.f,
											  0.f,0.f,0.f,1.f);

LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pSavedSunSB			= NULL;
LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pDrawSunSB			= NULL;		// 태양
LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pSavedFlareSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pDrawFlareSB		= NULL;		// 렌즈 플레어
LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pSavedNightSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pDrawNightSB		= NULL;		// 별
LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pSavedMoonSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pDrawMoonSB			= NULL;		// 달
LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pSavedUStarSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pDrawUStarSB		= NULL;		// 유성
LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pSavedThunderSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pDrawThunderSB		= NULL;		// 천둥

LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pSavedCloudSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pDrawCloudSB		= NULL;		// 구름
LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pSavedCloudShadowSB	= NULL;
LPDIRECT3DSTATEBLOCK9		DxSkyMan::m_pDrawCloudShadowSB	= NULL;		// 구름, 번개에 사용

DxSkyMan::DxSkyMan() :
	m_dwModeWidth(800),
	m_dwModeHeight(600),
	m_fAlpha_Day(1.f),
	m_fAlpha_Night(1.f),
	m_bOptionEnable(TRUE),
	m_vSunPos(0.f,0.f,0.f),
	m_fSunScale(72000.f),
	m_fMoonScale(23000.f),
	m_fStarScale(3000.f),
	m_fStarTwinkleTime(0.f),
	m_fUStarScale(20.f),			// 유성
	m_fUStarRate(10.f),				// 유성
	m_vUStarDirect(0.f,0.f,0.f),	// 유성
	m_fUStarSpeed(2.f),				// 유성
	m_fUStarTime(0.f),				// 유성
	m_fUStarTimeEnd(3.f),			// 유성
	m_fUStarTimeRest(10.f),			// 유성
	m_nLightningNUM(0),
	m_bLightingNow(FALSE),
	m_bLightingUSE(FALSE),
	m_fLightingTime(0.f),
	m_fLightingEndTime(1.f),
	m_vLightingPos(0.f,0.f,0.f),
	m_cLightingColor(0x00ffffff),
	m_vThunderPos(0.f,0.f,0.f),
	m_vThunderUV(0.f,0.f),
	m_cThunderColor(0x00ffffff),
	m_fThunderScale(1600.f),
	m_fThunderTime(0.f),
	m_fThunderPlayTime(0.f),
	m_fThunderEndTime(0.f),
	m_emMoonCycle(EMMC_L_ZERO),
	m_dwPrevHour(0)
{
	for ( DWORD i=0; i<USTAR_NUM; ++i )
	{
		m_vUStarPos[i] = D3DXVECTOR3(0.f,0.f,0.f);		// 유성
	}

	StringCchCopy( m_szSunTex,			MAX_PATH, "Sun.dds" );
	StringCchCopy( m_szFlareTex,		MAX_PATH, "Flare.dds" );

	StringCchCopy( m_szMoonTex,			MAX_PATH, "moon.dds" );

	StringCchCopy( m_szStarTex,			MAX_PATH, "Star.tga" );
	StringCchCopy( m_szUStarTex,		MAX_PATH, "UStar.tga" );

	StringCchCopy( m_szLightingTex,		MAX_PATH, "Lightning.dds" );
	StringCchCopy( m_szThunderTex,		MAX_PATH, "Cloud_Light.dds" );
	StringCchCopy( m_szThunderTex_C,	MAX_PATH, "Thunder_Circle.dds" );
	StringCchCopy( m_szThundersTex,		MAX_PATH, "Thunders.dds" );
}

DxSkyMan::~DxSkyMan()
{
}

void DxSkyMan::SetProperty ( SKY_PROPERTY& Property )
{
	m_Property = Property;
}

HRESULT DxSkyMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sSkyBaseMan.OnCreateDevice( pd3dDevice );
	m_sSkyCloudMan.OnCreateDevice( pd3dDevice );

    m_textureResSun = TextureManager::GetInstance().LoadTexture(
        m_szSunTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_SKYBOX,
		false,
		FALSE );

    m_textureResFlare = TextureManager::GetInstance().LoadTexture(
        m_szFlareTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_SKYBOX,
		false,
		FALSE );

    m_textureResMoon = TextureManager::GetInstance().LoadTexture(
        m_szMoonTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_SKYBOX,
		false,
		FALSE );

    m_textureResStar = TextureManager::GetInstance().LoadTexture(
        m_szStarTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_SKYBOX,
		false,
		FALSE );

    m_textureResUStar = TextureManager::GetInstance().LoadTexture(
        m_szUStarTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_SKYBOX,
		false,
		FALSE );

    m_textureResLighting = TextureManager::GetInstance().LoadTexture(
        m_szLightingTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_SKYBOX,
		false,
		FALSE );

    m_textureResThunder = TextureManager::GetInstance().LoadTexture(
        m_szThunderTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_SKYBOX,
		false,
		FALSE );

    m_textureResThunderC = TextureManager::GetInstance().LoadTexture(
        m_szThunderTex_C,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_SKYBOX,
		false,
		FALSE );

    m_textureResThunders = TextureManager::GetInstance().LoadTexture(
        m_szThundersTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_SKYBOX,
		false,
		FALSE );

	return S_OK;
}

HRESULT DxSkyMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	m_sSkyBaseMan.OnResetDevice( pd3dDevice );
	m_sSkyCloudMan.OnResetDevice( pd3dDevice );

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps( &d3dCaps );	

	LPDIRECT3DSURFACEQ	pBackBuffer;
	D3DSURFACE_DESC		d3dsdBackBuffer;
	pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
    pBackBuffer->GetDesc( &d3dsdBackBuffer );
	pBackBuffer->Release ();

	m_dwModeWidth	= d3dsdBackBuffer.Width;			// 너비
	m_dwModeHeight	= d3dsdBackBuffer.Height;			// 높이

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG2 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSunSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSunSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_ZENABLE,				D3DZB_FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedFlareSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawFlareSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_WRAP );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_WRAP );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedNightSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawNightSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

	//	// Enable alpha testing (skips pixels with less than a certain alpha.)
	//	if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
	//	{
	//		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	//		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x01 );
	//		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	//	}

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,		D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_SELECTARG2 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP,		D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedMoonSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawMoonSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,			D3DCULL_NONE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedUStarSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawUStarSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,		D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedThunderSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawThunderSB );
	}


	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_WRAP );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_WRAP );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedCloudSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawCloudSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_WRAP );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_WRAP );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG2 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedCloudShadowSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawCloudShadowSB );
	}

	if ( m_nRadioAxis == 0 )		D3DXMatrixRotationX ( &m_matWorld, m_fAxisValue/180.f );
	else if ( m_nRadioAxis == 1 )	D3DXMatrixRotationY ( &m_matWorld, m_fAxisValue/180.f );
	else if ( m_nRadioAxis == 2 )	D3DXMatrixRotationZ ( &m_matWorld, m_fAxisValue/180.f );

	m_sVertices[0].vTex = D3DXVECTOR2 ( 0.f, 0.f );
	m_sVertices[1].vTex = D3DXVECTOR2 ( 1.f, 0.f );
	m_sVertices[2].vTex = D3DXVECTOR2 ( 0.f, 1.f );
	m_sVertices[3].vTex = D3DXVECTOR2 ( 1.f, 1.f );

	m_sVerticesTex2[0].vTex01 = D3DXVECTOR2 ( 0.f, 0.f );
	m_sVerticesTex2[1].vTex01 = D3DXVECTOR2 ( 1.f, 0.f );
	m_sVerticesTex2[2].vTex01 = D3DXVECTOR2 ( 0.f, 1.f );
	m_sVerticesTex2[3].vTex01 = D3DXVECTOR2 ( 1.f, 1.f );

	int		nRand;
	float	fPosX;
	float	fPosZ;
	
	for ( DWORD i=0; i<STAR_NUM; ++i )
	{
		fPosX = sc::Random::RANDOM_POS();
		fPosZ = sc::Random::RANDOM_POS();

		if ( fPosX < 0.5f && fPosZ < 0.5f )
		{
			nRand = rand();
			nRand = nRand % 2;
			if (nRand)
                fPosX = fPosX + 0.5f;
			else
                fPosZ = fPosZ + 0.5f;
		}

		nRand = rand();
		nRand = nRand % 2;
		if ( nRand )	fPosX = -fPosX;

		nRand = rand();
		nRand = nRand % 2;
		if ( nRand )	fPosZ = -fPosZ;

		fPosZ = sc::Random::RANDOM_NUM();
		m_sStarObject[i].vPos			= D3DXVECTOR3 ( fPosX, ((sc::Random::RANDOM_POS()*8.f)+2.f)/10.f, fPosZ );
		m_sStarObject[i].fTwinkle		= sc::Random::RANDOM_POS()*2.f;
		m_sStarObject[i].cColor			= 0xffffffff;
	}

	SAFE_RELEASE ( m_pStarVB );
	hr = pd3dDevice->CreateVertexBuffer ( 4*STAR_NUM*sizeof(VERTEXCOLORTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
									VERTEXCOLORTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pStarVB, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile(std::string("DxShadowMap::RestoreDeviceObjects() -- CreateVertexBuffer( m_pStarVB ) -- Failed"));
		return E_FAIL;
	}

	VERTEXCOLORTEX*	pVertices;
	hr = m_pStarVB->Lock ( 0, 0, (VOID**)&pVertices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile(std::string("DxShadowMap::RestoreDeviceObjects() -- m_pStarVB->Lock() -- Failed"));
		return E_FAIL;
	}

	for (DWORD i=0; i<STAR_NUM; ++i )
	{
		pVertices[(i*4)+0].dwColor = m_sStarObject[i].cColor;
		pVertices[(i*4)+1].dwColor = m_sStarObject[i].cColor;
		pVertices[(i*4)+2].dwColor = m_sStarObject[i].cColor;
		pVertices[(i*4)+3].dwColor = m_sStarObject[i].cColor;

		pVertices[(i*4)+0].vTex	= D3DXVECTOR2 ( 0.f, 0.f );
		pVertices[(i*4)+1].vTex = D3DXVECTOR2 ( 1.f, 0.f );
		pVertices[(i*4)+2].vTex = D3DXVECTOR2 ( 0.f, 1.f );
		pVertices[(i*4)+3].vTex = D3DXVECTOR2 ( 1.f, 1.f );
	}
	m_pStarVB->Unlock ();

	SAFE_RELEASE ( m_pStarIB );
	hr = pd3dDevice->CreateIndexBuffer (	3*2*STAR_NUM*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pStarIB, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile(std::string("DxShadowMap::RestoreDeviceObjects() -- CreateIndexBuffer( m_pStarIB ) -- Failed"));
		return E_FAIL;
	}

	WORD*	pIndices;
	hr = m_pStarIB->Lock ( 0, 0, (VOID**)&pIndices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile(std::string("DxShadowMap::RestoreDeviceObjects() -- m_pStarIB->Lock() -- Failed"));
		return E_FAIL;
	}

	for(DWORD i=0; i<STAR_NUM; ++i )
	{
		*pIndices++ = (WORD)(i*4+0);
		*pIndices++ = (WORD)(i*4+1);
		*pIndices++ = (WORD)(i*4+2);
		*pIndices++ = (WORD)(i*4+1);
		*pIndices++ = (WORD)(i*4+3);
		*pIndices++ = (WORD)(i*4+2);
	}
	m_pStarIB->Unlock ();

	m_sUStar[0].vTex = D3DXVECTOR2 ( 0.f, 1.f );
	m_sUStar[1].vTex = D3DXVECTOR2 ( 1.f, 1.f );
	m_sUStar[2].vTex = D3DXVECTOR2 ( 0.f, 0.f );
	m_sUStar[3].vTex = D3DXVECTOR2 ( 1.f, 0.f );

	m_vUStarDirect = D3DXVECTOR3(sc::Random::RANDOM_NUM(), -0.3f-(sc::Random::RANDOM_POS()*4.f), sc::Random::RANDOM_NUM());
	m_fUStarSpeed = 10000.f;
	m_fUStarTime = 0.f;

	m_vLightingPos	= D3DXVECTOR3((sc::Random::RANDOM_POS()+1.f)*2000.f, 900.f, (sc::Random::RANDOM_POS()+1.f)*2000.f);

	m_vThunderPos		= m_vLightingPos;
	m_vThunderUV		= D3DXVECTOR2 ( 0.f, 0.f );

	return S_OK;
}

HRESULT DxSkyMan::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sSkyBaseMan.OnLostDevice();
	m_sSkyCloudMan.OnLostDevice();

	SAFE_RELEASE( m_pSavedSunSB );
	SAFE_RELEASE( m_pDrawSunSB );
	SAFE_RELEASE( m_pSavedFlareSB );
	SAFE_RELEASE( m_pDrawFlareSB );
	SAFE_RELEASE( m_pSavedNightSB );
	SAFE_RELEASE( m_pDrawNightSB );
	SAFE_RELEASE( m_pSavedMoonSB );
	SAFE_RELEASE( m_pDrawMoonSB );
	SAFE_RELEASE( m_pSavedUStarSB );
	SAFE_RELEASE( m_pDrawUStarSB );
	SAFE_RELEASE( m_pSavedThunderSB );
	SAFE_RELEASE( m_pDrawThunderSB );

	SAFE_RELEASE( m_pSavedCloudSB );
	SAFE_RELEASE( m_pDrawCloudSB );
	SAFE_RELEASE( m_pSavedCloudShadowSB );
	SAFE_RELEASE( m_pDrawCloudShadowSB );


	SAFE_RELEASE ( m_pStarVB );
	SAFE_RELEASE ( m_pStarIB );

	return S_OK;
}

HRESULT DxSkyMan::DeleteDeviceObjects ()
{
	m_sSkyBaseMan.OnDestroyDevice();
	m_sSkyCloudMan.OnDestroyDevice();

	m_textureResSun.Clear();
	m_textureResFlare.Clear();
	m_textureResMoon.Clear();
	m_textureResStar.Clear();
	m_textureResUStar.Clear();
	m_textureResLighting.Clear();
	m_textureResThunder.Clear();
	m_textureResThunderC.Clear();
	m_textureResThunders.Clear();

	return S_OK;
}

HRESULT DxSkyMan::FrameMove ( float fTime, float fElapsedTime )
{
	if ( !m_bOptionEnable || (!m_bSunMoon&&!m_bSkyEnable&&!m_bCloudEnable) )
        return S_OK;

    if( !GLOBAL_PARAM::IsValue( GP_SKYBOX ) )
		return S_OK;

	if ( m_nRadioAxis == 0 )		D3DXMatrixRotationX ( &m_matWorld, m_fAxisValue*D3DX_PI/180.f );
	else if ( m_nRadioAxis == 1 )	D3DXMatrixRotationY ( &m_matWorld, m_fAxisValue*D3DX_PI/180.f );
	else if ( m_nRadioAxis == 2 )	D3DXMatrixRotationZ ( &m_matWorld, m_fAxisValue*D3DX_PI/180.f );

	m_sSkyBaseMan.FrameMove( fElapsedTime );
	m_sSkyCloudMan.FrameMove( fElapsedTime );

	//	여기도 낮과 밤을 나눠서 해야겠네.
	
	m_fElapsedTime = fElapsedTime;

	D3DXVECTOR3& vLookatPt	= DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3& vFromPt	= DxViewPort::GetInstance().GetFromPt();

	m_vSkyCenter = D3DXVECTOR3 ( vFromPt.x, vLookatPt.y-8.f, vFromPt.z );

	float fRainRate	= DxWeatherMan::GetInstance()->GetRain()->GetApplyRate();
	float fSnowRate	= DxWeatherMan::GetInstance()->GetSnow()->GetApplyRate();
	m_fWeatherRate	= (fRainRate>fSnowRate) ? fRainRate : fSnowRate;

	if ( m_bSunMoon )
	{
		float	fAlpha;
		float	fBaseFact = GLPeriod::GetInstance().GetBaseFact();
		{
			if ( fBaseFact <= 0.5f )	m_fAlpha_Day = 0.f;
			else
			{
				fAlpha			= fBaseFact - 0.5f;
				fAlpha			= fAlpha*2.f;
				m_fAlpha_Day	= fAlpha*(1.f-m_fWeatherRate);
			}
		}
		{
			if ( fBaseFact >= 0.5f )		m_fAlpha_Night = 0.f;
			else
			{
				fAlpha			= 0.5f - fBaseFact;
				fAlpha			= fAlpha*2.f;
				m_fAlpha_Night	= fAlpha*(1.f-m_fWeatherRate);
			}
		}
	}

	if ( m_bSkyEnable )
	{
		if ( m_fStarTwinkleTime > 0.03f )	// 일정하게 바뀐다.
		{
			for ( DWORD i=0; i<STAR_NUM-1; ++i )				
			{
				m_sStarObject[i].fTwinkle	= m_sStarObject[i+1].fTwinkle;
			}

			m_sStarObject[STAR_NUM-1].fTwinkle	= 0.8f + (sc::Random::RANDOM_POS()*0.4f);

			m_fStarTwinkleTime = 0.f;
		}
		else								// 쓰레기 ( 속도를 유지하기 위함 )
		{
			float fTemp;
			for ( DWORD i=0; i<STAR_NUM-1; ++i )
			{
				fTemp = 0.f;
			}

			fTemp	= 0.8f + (sc::Random::RANDOM_POS()*0.4f);
		}

		m_fStarTwinkleTime += fElapsedTime;

		//m_fUStarTime += fElapsedTime;					// 유성 쪽 초기화
		//if ( m_fUStarTime > m_fUStarTimeRest )
		//{
		//	D3DXVECTOR3 vDirect	= DxViewPort::GetInstance().GetLookDir_Y0();

		//	m_fUStarTime = 0.f;
		//	for ( DWORD i=0; i<USTAR_NUM; ++i )
		//	{
		//		m_vUStarPos[i] = D3DXVECTOR3 ( (vDirect.x*3.f)+sc::Random::RANDOM_NUM(), sc::Random::RANDOM_POS(), (vDirect.z*3.f)+sc::Random::RANDOM_NUM() );
		//		m_vUStarPos[i].y += 1.f;
		//		m_vUStarPos[i]	*= 2000.f;
		//	}
		//	m_vUStarDirect = D3DXVECTOR3 ( (sc::Random::RANDOM_NUM()*2.f), -sc::Random::RANDOM_POS(), (sc::Random::RANDOM_NUM()*2.f) );
		//	m_fUStarSpeed = sc::Random::RANDOM_POS()*5000.f;
		//}
		//else if ( m_fUStarTime < m_fUStarTimeEnd )		// 움직일때닷.
		//{
		//	for ( DWORD i=0; i<USTAR_NUM; ++i )
		//	{
		//		m_vUStarPos[i] += m_vUStarDirect*m_fUStarSpeed*fElapsedTime;	// 계속 움직여라. 짜슥. ㅋㅋ
		//	}
		//}
	}

	//	Note : 번개가 칠 경우만 계산을 한다.
	//
	if ( m_bLightingUSE )
	{
		m_fLightingTime += fElapsedTime;
		if ( m_fLightingTime > m_fLightingEndTime )
		{
			m_fLightingTime = 0.f;
			m_bLightingUSE	= FALSE;
		}
	}

	m_fThunderTime += fElapsedTime;
	if ( m_fThunderTime > m_fThunderEndTime )
	{
		D3DXVECTOR3	vDirect = DxViewPort::GetInstance().GetLookDir_Y0();

		m_vThunderPos	= D3DXVECTOR3 ( (sc::Random::RANDOM_NUM()+(vDirect.x*2.f))*1600.f, vLookatPt.y+600.f, (sc::Random::RANDOM_NUM()+(vDirect.z*2.f))*1600.f );
		m_vThunderUV	= D3DXVECTOR2 ( sc::Random::RANDOM_POS()*0.5f, sc::Random::RANDOM_POS()*0.5f );

		m_fThunderTime		= 0.f;
		m_fThunderPlayTime	= 1.f + (sc::Random::RANDOM_POS()*2.f);
		m_fThunderEndTime	= m_fThunderPlayTime + (5.f + (sc::Random::RANDOM_POS()*10.f));

		m_sLighting_1.emTex = (LIGHTINGTEXUV) (rand()%EMLT_MAX);
		m_sLighting_2.emTex = (LIGHTINGTEXUV) (rand()%EMLT_MAX);
		m_sLighting_3.emTex = (LIGHTINGTEXUV) (rand()%EMLT_MAX);

		if ( m_sLighting_1.emTex )
		{
			m_sLighting_1.vPos = m_vThunderPos + D3DXVECTOR3 ( sc::Random::RANDOM_NUM()*800.f, -sc::Random::RANDOM_POS()*200.f, sc::Random::RANDOM_NUM()*800.f );
			m_sLighting_1.m_iSprite = 0;
			m_sLighting_1.fTime = sc::Random::RANDOM_POS()*m_fThunderPlayTime;
		}
		if ( m_sLighting_2.emTex )
		{
			m_sLighting_2.vPos = m_vThunderPos + D3DXVECTOR3 ( sc::Random::RANDOM_NUM()*800.f, -sc::Random::RANDOM_POS()*200.f, sc::Random::RANDOM_NUM()*800.f );
			m_sLighting_2.m_iSprite = 0;
			m_sLighting_2.fTime = sc::Random::RANDOM_POS()*m_fThunderPlayTime;
		}
		if ( m_sLighting_3.emTex )
		{
			m_sLighting_3.vPos = m_vThunderPos + D3DXVECTOR3 ( sc::Random::RANDOM_NUM()*800.f, -sc::Random::RANDOM_POS()*200.f, sc::Random::RANDOM_NUM()*800.f );
			m_sLighting_3.m_iSprite = 0;
			m_sLighting_3.fTime = sc::Random::RANDOM_POS()*m_fThunderPlayTime;
		}
	}

	if ( m_sLighting_1.emTex )
	{
		m_sLighting_1.m_iSprite = (int)((m_fThunderTime-m_sLighting_1.fTime)/0.03f);	// 33 fps
	}
	if ( m_sLighting_2.emTex )
	{
		m_sLighting_2.m_iSprite = (int)((m_fThunderTime-m_sLighting_2.fTime)/0.03f);	// 33 fps
	}
	if ( m_sLighting_3.emTex )
	{
		m_sLighting_3.m_iSprite = (int)((m_fThunderTime-m_sLighting_3.fTime)/0.03f);	// 33 fps
	}

	return S_OK;
}

HRESULT DxSkyMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice, float fFOV, float fAspect, BOOL bReflect )
{
	if ( !m_bOptionEnable || (!m_bSunMoon&&!m_bSkyEnable&&!m_bCloudEnable) )
        return S_OK;

    if( !GLOBAL_PARAM::IsValue( GP_SKYBOX ) )
		return S_OK;

	D3DMATERIALQ	cMaterial;

	memset(&cMaterial, 0, sizeof(D3DMATERIALQ));
	cMaterial.Diffuse.r = 1.0f;
	cMaterial.Diffuse.g = 1.0f;
	cMaterial.Diffuse.b = 1.0f;
	cMaterial.Ambient = cMaterial.Specular = cMaterial.Diffuse;

	pd3dDevice->SetMaterial ( &cMaterial );

    DxViewPort::GetInstance().BackUpFarPlane();
	DxViewPort::GetInstance().SetProjection_FarPlane ( 1000000.f, fFOV, fAspect );	// Far 클리핑을 멀리 보낸다.

	if ( m_bSkyEnable )
	{
		m_sSkyBaseMan.Render( pd3dDevice, bReflect, m_matWorld );
	}

	// 낮밤 구분이 없다면 낮이기때문에 건너뛴다;
	if ( m_bSunMoon && DxLightMan::GetInstance()->IsNightAndDay() )
	{
		//SunRender ( pd3dDevice );			// 해 렌더링

		MoonRender ( pd3dDevice );			// 달 렌더링
	}

	if ( m_bSkyEnable )
	{
		StarRender ( pd3dDevice );			// 별 렌더링

		//if ( m_fUStarTime < m_fUStarTimeEnd )
		//{
		//	UStarRender ( pd3dDevice );		// 유성 렌더링		// 우주진(宇宙塵)이 지구의 대기권에 들어와 공기의 압축과 마찰로 빛을 내는 것.
		//}
	}
	
	if ( m_bCloudEnable )
	{
		m_sSkyCloudMan.Render( pd3dDevice, m_matWorld );

		//float fRainRate	= DxWeatherMan::GetInstance()->GetRain()->GetApplyRate();

		//if ( fRainRate > 0.f )				// 비가 올꺼 같으면 동작을 시킨다.
		//{
		//	if ( m_fThunderTime < m_fThunderPlayTime )
		//	{
		//		ThunderRender ( pd3dDevice, m_vThunderPos, 2000.f, 0.2f, 0.2f );	// 구름에 천둥이 치는 모습 구현
		//	}

		//	LightingCheckReset ();					// 번개 체크
		//	if ( m_bLightingUSE )
		//		LightingRender ( pd3dDevice );		// 번개 렌더링
		//}
	}

    DxViewPort::GetInstance().RollBackFarPlane( pd3dDevice );

	m_bLightingNow = FALSE;	//	받아오는 번개 초기화

	return S_OK;
}

void DxSkyMan::Render_AFTER( LPDIRECT3DDEVICEQ pd3dDevice, float fFOV, float fAspect )
{
	if ( !m_bOptionEnable || (!m_bSunMoon&&!m_bSkyEnable&&!m_bCloudEnable) )
        return;

    if( !GLOBAL_PARAM::IsValue( GP_SKYBOX ) )
		return;

	D3DMATERIALQ	cMaterial;

	memset(&cMaterial, 0, sizeof(D3DMATERIALQ));
	cMaterial.Diffuse.r = 1.0f;
	cMaterial.Diffuse.g = 1.0f;
	cMaterial.Diffuse.b = 1.0f;
	cMaterial.Ambient = cMaterial.Specular = cMaterial.Diffuse;

	pd3dDevice->SetMaterial ( &cMaterial );

    DxViewPort::GetInstance().BackUpFarPlane();
	DxViewPort::GetInstance().SetProjection_FarPlane ( 1000000.f, fFOV, fAspect );	// Far 클리핑을 멀리 보낸다.

	if ( m_bSunMoon )	FlareRender ( pd3dDevice, m_vSunPos );

    DxViewPort::GetInstance().RollBackFarPlane( pd3dDevice );
}

HRESULT DxSkyMan::Render_Reflect ( LPDIRECT3DDEVICEQ pd3dDevice, float fFOV, float fAspect )
{
	if ( !m_bOptionEnable || (!m_bSunMoon&&!m_bSkyEnable&&!m_bCloudEnable) )
        return S_OK;

    if( !GLOBAL_PARAM::IsValue( GP_SKYBOX ) )
		return S_OK;

	D3DMATERIALQ	cMaterial;

	memset(&cMaterial, 0, sizeof(D3DMATERIALQ));
	cMaterial.Diffuse.r = 1.0f;
	cMaterial.Diffuse.g = 1.0f;
	cMaterial.Diffuse.b = 1.0f;
	cMaterial.Ambient = cMaterial.Specular = cMaterial.Diffuse;

	pd3dDevice->SetMaterial ( &cMaterial );

    DxViewPort::GetInstance().BackUpFarPlane();
	DxViewPort::GetInstance().SetProjection_FarPlane ( 1000000.f, fFOV, fAspect );	// Far 클리핑을 멀리 보낸다.

	if ( m_bSkyEnable )
	{
		m_sSkyBaseMan.RenderReflect( pd3dDevice, m_matWorld );
	}
	
	if ( m_bCloudEnable )
	{
		m_sSkyCloudMan.Render( pd3dDevice, m_matWorld );
	}

    DxViewPort::GetInstance().RollBackFarPlane( pd3dDevice );

	return S_OK;
}

HRESULT DxSkyMan::Render_ReflectMaterial ( LPDIRECT3DDEVICEQ pd3dDevice, float fFOV, float fAspect )
{
	if ( !m_bOptionEnable || (!m_bSunMoon&&!m_bSkyEnable&&!m_bCloudEnable) )
		return S_OK;

	if( !GLOBAL_PARAM::IsValue( GP_SKYBOX ) )
		return S_OK;

	D3DMATERIALQ	cMaterial;

	memset(&cMaterial, 0, sizeof(D3DMATERIALQ));
	cMaterial.Diffuse.r = 1.0f;
	cMaterial.Diffuse.g = 1.0f;
	cMaterial.Diffuse.b = 1.0f;
	cMaterial.Ambient = cMaterial.Specular = cMaterial.Diffuse;

	pd3dDevice->SetMaterial ( &cMaterial );

	DxViewPort::GetInstance().BackUpFarPlane();
	DxViewPort::GetInstance().SetProjection_FarPlane ( 1000000.f, fFOV, fAspect );	// Far 클리핑을 멀리 보낸다.

	if ( m_bSkyEnable )
	{
		m_sSkyBaseMan.RenderReflect( pd3dDevice, m_matWorld );
	}

	if ( m_bCloudEnable )
	{
		m_sSkyCloudMan.RenderReflectMaterial( pd3dDevice, m_matWorld );
	}

	DxViewPort::GetInstance().RollBackFarPlane( pd3dDevice );

	return S_OK;
}

void DxSkyMan::RenderGlOW( LPDIRECT3DDEVICEQ pd3dDevice, float fFOV, float fAspect )
{
    if( !GLOBAL_PARAM::IsValue( GP_SKYBOX ) )
		return;

	if ( !DxSurfaceTex::GetInstance().m_pGlowSuf_SRC )
		return;

	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pGlowSuf_SRC );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	DxGlowMan::GetInstance().SetGlowON ();

	Render( pd3dDevice, fFOV, fAspect );

	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );
}