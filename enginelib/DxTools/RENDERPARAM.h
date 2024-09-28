#pragma once

#include "../../InternalCommonLib/dxstdafx.h"
#include "DxFontDefine.h"

enum EMCHARRENDER_TYPE
{
	EMCRT_NON_SOFT = 0,	// 컴퓨터 사양을 보고 SSE 가 지원하면 그쪽으로 하게 한다.
	EMCRT_SOFTWARE = 1,
	EMCRT_NORMAL = 2,
	EMCRT_VERTEX = 3,
	EMCRT_PIXEL = 4,
	EMCRT_SIZE = 5,
};

enum TEXTURE_FILTERING
{
	//TEXTURE_FILTER_BILINEAR,	// intel 내장에서도 속도차이가 없다.
	TEXTURE_FILTER_TRILINEAR,
	TEXTURE_FILTER_ANISOTROPIC_2X,
	TEXTURE_FILTER_ANISOTROPIC_4X,
	TEXTURE_FILTER_ANISOTROPIC_8X,
	TEXTURE_FILTER_ANISOTROPIC_16X,
	TEXTURE_FILTER_SIZE,
};

enum TEXTURE_QUALITY
{
	TEXTURE_QUALITY_LOW,
	TEXTURE_QUALITY_MID,
	TEXTURE_QUALITY_HIGH,
	TEXTURE_QUALITY_SIZE,
};

enum PHYSX_CLOTH_LEVEL
{
	PHYSX_CLOTH_LEVEL_NONE,	// PhysX 작동 안한다.
	PHYSX_CLOTH_LEVEL_MY,	// 내자신만 작동시킨다.
	PHYSX_CLOTH_LEVEL_ALL,	// 모두 렌더링하도록 한다.
	PHYSX_CLOTH_LEVEL_SIZE,
};

enum TnL_MODE
{
	TnL_NONE = 0,
	TnL_FIXED = 1,
	TnL_PS_1_1 = 2,
	TnL_PS_3_0_DF = 3,
	TnL_PS_3_0_NM = 4,
	TnL_PS_3_0_CSM = 5,
};

enum TnL_CHAR_MODE
{
	TnL_CHAR_FIXED = 0,
	TnL_CHAR_FIXED_HIGH = 1,
	TnL_CHAR_PS_1_1 = 2,		// 이곳 이상부터 반사가 가능하다.
	TnL_CHAR_PS_2_0 = 3,		// 여기부터 VisualMaterial 이 가능하다.
	TnL_CHAR_PS_3_0 = 4,
};

namespace RENDERPARAM
{
	//	디스플레이 설정.
	extern EMCHARRENDER_TYPE	emCharRenderTYPE;	// Char Shader Type..!!	- 가장 런쳐에서 건들일 수 있다. 게임 들어가면 못 건들이도록 해야 함.

	// Note : 감마
	extern float	fGamma;
	extern DWORD	dwOverBright;
	extern float	fContrast;

	// Note : Device 성능
	extern BOOL		g_bPixelShader_1_1;
	extern BOOL		g_bPixelShader_1_4;
	extern BOOL		g_bPixelShader_2;
	extern BOOL		g_bPixelShader_3;
	extern BOOL		g_bDefferedRendering;
	extern BOOL		g_bDefferedRendering_HIGH;
	extern BOOL		g_bDefferedRendering_INTZ;
	extern BOOL		g_bDefferedRendering_DF16;
	//extern BOOL		g_bDefferedRendering_RAWZ;
	extern BOOL		g_bSurfaceG32R32F;
	extern BOOL		g_bEnableHW_HDR;
	extern BOOL		g_bEnable_HDR;
	extern BOOL		g_bSeparateAlphaBlend;
	extern TnL_MODE	g_emTnLMode;
	extern BOOL		bEnableEEX;
	extern BOOL		bEnableSSE;

	// 지형 Qulity
	extern TnL_MODE			g_emGroundQulity;
	extern TnL_MODE			g_emGroundQulityTHREAD;

	// 지형 Qulity
	//extern TnL_CHAR_MODE	g_emCharacterQulityHW;	// 가능한 성능
	extern TnL_CHAR_MODE	g_emCharacterQulity;	// 현재 구동중인 성능.

	// 강제 PS_3_0_DF 렌더
	extern BOOL		g_bFORCE_TnL_PS_3_0_DF;

	// Note : 테스트
	extern BOOL		bCALCULATE_BONE;
	extern BOOL		bCALCULATE_SSE;
	extern BOOL		bRENDER_CHAR;
	extern BOOL		bRENDER_WORLD;
	extern BOOL		bRENDER_PIECE;

	extern BOOL		bUseNormalMap;
	extern BOOL		bMineEffect;
	extern BOOL		bTargetEffect;
	extern BOOL		bClickEffect;
    extern BOOL     bVolumeLight;

	extern TEXTURE_FILTERING	g_emTextureFiltering;

	extern BOOL					g_bChangeTexQuality;
	extern TEXTURE_QUALITY		g_emTextureQuality;

	extern PHYSX_CLOTH_LEVEL	g_emPhysXCloth;

	extern DWORD	m_dwResolutionX;
	extern DWORD	m_dwResolutionY; 
	extern DWORD	m_dwGpuVendor; 		// 0 - Etc, 1 - Intel, 2 - Nvidia, 3 - AMD
	extern DWORD	m_dwGpuModel; 
	extern DWORD	m_dwShaderModel; 

	extern BOOL		g_bForceLowHardwareMode;	// 강제 저사양모드 활성화 ON / OFF

	enum EMSERVICE_TYPE
	{
		EMSERVICE_DEFAULT		= 0,
		EMSERVICE_KOREA			= 1,
		EMSERVICE_FEYA			= 2,
		EMSERVICE_MALAYSIA_CN	= 3,
		EMSERVICE_MALAYSIA_EN	= 4,
		EMSERVICE_JAPAN			= 5,
		EMSERVICE_THAILAND		= 6,
		EMSERVICE_CHINA			= 7,
		EMSERVICE_INDONESIA		= 8,
		EMSERVICE_PHILIPPINES	= 9,
		EMSERVICE_VIETNAM		= 10,
		EMSERVICE_GLOBAL		= 11, //! 글로벌 서비스
        EMSERVICE_WORLD_BATTLE  = 12, //! World battle
		EMSERVICE_EU			= 13, //! Games-Masters
		EMSERVICE_US			= 14, //! GameSamba
	};

	extern EMSERVICE_TYPE	emSERVICE_TYPE;
//	extern BOOL				bXML_USE;
	extern language::LANGFLAG emLangSet;

	void CheckSystemInfo( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL LOAD( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice, const char *szRootPath );

	void CheckTnLMode();

	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetTextureFiltering( TEXTURE_FILTERING emTexFilter );

	// 셋팅값을 임시로 저장해 놓고 맵이동시에 적용되도록 한다.
	void SetTextureQuality( TEXTURE_QUALITY emTexQuality );

	// 강제로 옵션을 넣어버린다.
	// Lobby 에서는 Texture Quality 를 최고로 보여줘야되어서 이 함수가 생겨났다.
	void SetTextureQuality_FORCE( TEXTURE_QUALITY emTexQuality );

	// 맵이동 할 때나 적용되도록 한다.
	void ApplyTextureQuality();
}
