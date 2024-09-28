#include "pch.h"
#include "../../RanLogicClient/RanParamClient.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../../EngineLib/DxSound/DxSoundMan.h"
#include "../../EngineLib/DxSound/BgmSound.h"
#include "../../EngineLib/DxTools/DxSurfaceTex.h"
#include "../../EngineLib/DxTools/DxGlowMan.h"
#include "../../EngineLib/DxTools/DxFogMan.h"
#include "../../EngineLib/DxTools/DxPostProcess.h"

#include "./DxParamSet.h"

#include "../GfxInterfaceBase.h"
#include "../GLGaeaClient.h"
#include "../Stage/DxGlobalStage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern BOOL				g_bFRAME_LIMIT;

namespace DXPARAMSET
{
	STESTSET& GetInstance()
	{
		static STESTSET Instance;
		return Instance;
	}

	void INIT ()
	{
		//	그래픽 옵션 조정.
		//
		DxShadowMap::GetInstance().SetShadowDetail ( (ENUM_LAND_QUALITY) RANPARAM::dwShadowChar );

		//if ( RANPARAM::bShadowLand )	DxEffectMan::GetInstance().SetSSDetail ( SS_ONETEX );
		//else							DxEffectMan::GetInstance().SetSSDetail ( SS_DISABLE );

		DxFogMan::GetInstance().SetFogRange ( (FOGRANGE) RANPARAM::dwFogRange );

		DxEffectMan::GetInstance().SetRealReflect ( RANPARAM::bRealReflect );
		DxEffectMan::GetInstance().SetSkinDetail ( (EMSKINDETAIL) RANPARAM::dwSkinDetail );
		DxSurfaceTex::GetInstance().SetOptionEnable ( RANPARAM::bRefract );
		DxGlowMan::GetInstance().SetOptionEnable ( RANPARAM::bGlow );
		DxPostProcess::GetInstance().SetOptionEnable( RANPARAM::bPost );
		
		g_bFRAME_LIMIT = RANPARAM::bFrameLimit;
		

		//	사운드 설정.
		//		
		DxSoundMan::GetInstance().SetStereo ( !RANPARAM::b3D_SOUND );
		DxSoundMan::GetInstance().SetMapVolume ( RANPARAM::nSndMap );
		DxSoundMan::GetInstance().SetSfxVolume ( RANPARAM::nSndSfx );
		DxBgmSound::GetInstance().SetVolume ( RANPARAM::nSndMusic );
		DxSoundMan::GetInstance().Set3DAlgorithm ( RANPARAM::dw3DAlgorithm );
		DxSoundMan::GetInstance().SetBufferSize ( RANPARAM::dwSndBufferSize );

		DxSoundMan::GetInstance().SetMapMute ( RANPARAM::bSndMuteMap );
		DxSoundMan::GetInstance().SetSfxMute ( RANPARAM::bSndMuteSfx );
		DxBgmSound::GetInstance().SetMute ( RANPARAM::bSndMuteMusic );

		LOADDATA();
	}

	void LOADDATA ()
	{
		GetInstance().m_bDIS_CONFT = RANPARAM::bDIS_CONFT;
		GetInstance().m_bDIS_TRADE = RANPARAM::bDIS_TRADE;
		GetInstance().m_bDIS_PARTY = RANPARAM::bDIS_PARTY;
//		GetInstance().m_bDIS_FRIEND = RANPARAM::bDIS_FRIEND;		

		GetInstance().m_bSHOW_SIMPLEHP   = RANPARAM::bSHOW_SIMPLEHP;
		GetInstance().m_bSIMPLE_SUM_NAME = RANPARAM::bSIMPLE_SUM_NAME;
        GetInstance().m_bAUTO_CROW_HP    = RANPARAM::bAUTO_CROW_HP;

		GetInstance().m_dwScrWidth = RANPARAM::dwScrWidth;
		GetInstance().m_dwScrHeight = RANPARAM::dwScrHeight;
		GetInstance().m_emScrFormat = RANPARAM::emScrFormat;

		GetInstance().m_uScrRefreshHz = RANPARAM::uScrRefreshHz;
		GetInstance().m_bScrWindowed = RANPARAM::bScrWindowed;

		GetInstance().m_dwVideoLevel = RANPARAM::dwVideoLevel;		
		GetInstance().m_dwShadowChar = RANPARAM::dwShadowChar;
		GetInstance().m_bBuff = RANPARAM::bBuff;
		//GetInstance().m_bShadowLand = RANPARAM::bShadowLand;
		GetInstance().m_bRealReflect = RANPARAM::bRealReflect;
		GetInstance().m_dwSkinDetail = RANPARAM::dwSkinDetail;
		GetInstance().m_bGlow = RANPARAM::bGlow;
		GetInstance().m_bPost = RANPARAM::bPost;
		GetInstance().m_bRefract = RANPARAM::bRefract;
		GetInstance().m_dwFogRange = RANPARAM::dwFogRange;
		GetInstance().m_dwTexFilter = static_cast<DWORD>( RENDERPARAM::g_emTextureFiltering );
		GetInstance().m_dwTexQuality = static_cast<DWORD>( RENDERPARAM::g_emTextureQuality );
		GetInstance().m_dwPhysXCloth = static_cast<DWORD>( RENDERPARAM::g_emPhysXCloth );
		GetInstance().m_bFrameLimit = RANPARAM::bFrameLimit;
		GetInstance().m_bTargetEffect = RENDERPARAM::bTargetEffect;
		GetInstance().m_bMineEffect = RENDERPARAM::bMineEffect;
		GetInstance().m_bClickEffect = RENDERPARAM::bClickEffect;
		GetInstance().m_bUseNormalMap = RENDERPARAM::bUseNormalMap;
        GetInstance().m_bVolumeLight = RENDERPARAM::bVolumeLight;
		GetInstance().m_bPossibleNormalMap = RANPARAM::bPossibleNormalMap;


//		RENDERPARAM::bUseNormalMap = RANPARAM::bUseNormalMap;
//		RENDERPARAM::bMineEffect   = RENDERPARAM::bMineEffect;
//		RENDERPARAM::bTargetEffect = RENDERPARAM::bTargetEffect;
//		RENDERPARAM::bClickEffect  = RENDERPARAM::bClickEffect;
		
//		RENDERPARAM::bXML_USE      = RANPARAM::bXML_USE;
//		RENDERPARAM::emLangSet     = RANPARAM::emLangSet;		

	}

	void STESTSET::APPLY ( CD3DApplication *pD3dApp )
	{	
		// 창모드가 아닐경우만 동작되도록 한다.
		if ( !RANPARAM::bScrWindowed )
		{
			//	스크린 설정 적용.
			DWORD dwNewMode = pD3dApp->FindDeviceMode ( m_dwScrWidth, m_dwScrHeight, m_emScrFormat );
			DWORD dwOldMode = pD3dApp->GetCurrentMode();
			if ( dwNewMode != dwOldMode )
			{
				pD3dApp->ChangeDeviceMode ( dwNewMode, m_uScrRefreshHz, m_bScrWindowed );
			}
		}

		//	그래픽 설정.
		//
		DxShadowMap::GetInstance().SetShadowDetail ( (ENUM_LAND_QUALITY) m_dwShadowChar );

		//if ( m_bShadowLand )	DxEffectMan::GetInstance().SetSSDetail ( SS_ONETEX );
		//else					DxEffectMan::GetInstance().SetSSDetail ( SS_DISABLE );

		DxEffectMan::GetInstance().SetRealReflect ( m_bRealReflect );
		DxSurfaceTex::GetInstance().SetOptionEnable ( m_bRefract );
		DxGlowMan::GetInstance().SetOptionEnable ( m_bGlow );
		DxPostProcess::GetInstance().SetOptionEnable( m_bPost );
		DxEffectMan::GetInstance().SetSkinDetail ( (EMSKINDETAIL) m_dwSkinDetail );
		
		DxFogMan::GetInstance().SetFogRange ( (FOGRANGE) m_dwFogRange );
		
		RENDERPARAM::SetTextureFiltering( static_cast<TEXTURE_FILTERING>( m_dwTexFilter ) );

		RENDERPARAM::SetTextureQuality( static_cast<TEXTURE_QUALITY>( m_dwTexQuality ) );

		RENDERPARAM::g_emPhysXCloth = static_cast<PHYSX_CLOTH_LEVEL>( m_dwPhysXCloth );

		g_bFRAME_LIMIT = m_bFrameLimit;
		
	}

	void STESTSET::VideoOK(CD3DApplication *pD3dApp, const SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider)
	{
		RANPARAM::dwScrWidth = m_dwScrWidth;
		RANPARAM::dwScrHeight = m_dwScrHeight;
		RANPARAM::emScrFormat = m_emScrFormat;
		RANPARAM::uScrRefreshHz = m_uScrRefreshHz;
		RANPARAM::bScrWindowed = m_bScrWindowed;

		RANPARAM::dwVideoLevel = m_dwVideoLevel;
		RANPARAM::dwShadowChar = m_dwShadowChar;
		RANPARAM::bBuff = m_bBuff;
		//RANPARAM::bShadowLand = m_bShadowLand;
		RANPARAM::bRealReflect = m_bRealReflect;
		RANPARAM::dwSkinDetail = m_dwSkinDetail;
		RANPARAM::bGlow = m_bGlow;
		RANPARAM::bPost = m_bPost;
		RANPARAM::bRefract = m_bRefract;
		RANPARAM::bFrameLimit = m_bFrameLimit;
		RENDERPARAM::bTargetEffect = m_bTargetEffect;
		RENDERPARAM::bMineEffect = m_bMineEffect;
		RENDERPARAM::bClickEffect = m_bClickEffect;
		RENDERPARAM::bUseNormalMap = m_bUseNormalMap;
        RENDERPARAM::bVolumeLight = m_bVolumeLight;
		RENDERPARAM::SetTextureFiltering( static_cast<TEXTURE_FILTERING>( m_dwTexFilter ) );
		RENDERPARAM::SetTextureQuality( static_cast<TEXTURE_QUALITY>( m_dwTexQuality ) );
		RENDERPARAM::g_emPhysXCloth = static_cast<PHYSX_CLOTH_LEVEL>( m_dwPhysXCloth );

		RANPARAM::dwFogRange = m_dwFogRange;
		

		RANPARAM::SAVE(ServiceProvider);
		RANPARAM::SAVE_GAMEOPT(pPath);

		APPLY ( pD3dApp );
	}

	void STESTSET::GameOK(const SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider)
	{
		RANPARAM::bDIS_CONFT = m_bDIS_CONFT;
		RANPARAM::bDIS_TRADE = m_bDIS_TRADE;
		RANPARAM::bDIS_PARTY = m_bDIS_PARTY;
		//		RANPARAM::bDIS_FRIEND = m_bDIS_FRIEND;		

		RANPARAM::bSHOW_SIMPLEHP   = m_bSHOW_SIMPLEHP;
        RANPARAM::bSIMPLE_SUM_NAME = m_bSIMPLE_SUM_NAME;
        RANPARAM::bAUTO_CROW_HP    = m_bAUTO_CROW_HP;

		RANPARAM::SAVE(ServiceProvider);
		RANPARAM::SAVE_GAMEOPT(pPath);
	}

	void STESTSET::AudioOK(const SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider)
	{
		RANPARAM::SAVE(ServiceProvider);
		RANPARAM::SAVE_GAMEOPT(pPath);
	}

	void STESTSET::CANCEL ( CD3DApplication *pD3dApp )
	{
		m_bDIS_CONFT = RANPARAM::bDIS_CONFT;
		m_bDIS_TRADE = RANPARAM::bDIS_TRADE;
		m_bDIS_PARTY = RANPARAM::bDIS_PARTY;
//		m_bDIS_FRIEND = RANPARAM::bDIS_FRIEND;


		m_bSHOW_SIMPLEHP   = RANPARAM::bSHOW_SIMPLEHP;
        m_bSIMPLE_SUM_NAME = RANPARAM::bSIMPLE_SUM_NAME;	
        m_bAUTO_CROW_HP    = RANPARAM::bAUTO_CROW_HP;

		m_dwScrWidth = RANPARAM::dwScrWidth;
		m_dwScrHeight = RANPARAM::dwScrHeight;
		m_emScrFormat = RANPARAM::emScrFormat;
		m_uScrRefreshHz = RANPARAM::uScrRefreshHz;
		m_bScrWindowed = RANPARAM::bScrWindowed;

		m_dwVideoLevel = RANPARAM::dwVideoLevel;				
		m_dwShadowChar = RANPARAM::dwShadowChar;
		m_bBuff = RANPARAM::bBuff;
		//m_bShadowLand = RANPARAM::bShadowLand;
		m_bRealReflect = RANPARAM::bRealReflect;
		m_dwSkinDetail = RANPARAM::dwSkinDetail;
		m_bGlow = RANPARAM::bGlow;
		m_bPost = RANPARAM::bPost;
		m_bRefract = RANPARAM::bRefract;
		m_bFrameLimit = RANPARAM::bFrameLimit;
		m_bTargetEffect = RENDERPARAM::bTargetEffect;
		m_bMineEffect   = RENDERPARAM::bMineEffect;
		m_bClickEffect	= RENDERPARAM::bClickEffect;
		m_bUseNormalMap = RENDERPARAM::bUseNormalMap;
        m_bVolumeLight  = RENDERPARAM::bVolumeLight;

		m_dwFogRange = RANPARAM::dwFogRange;
		m_dwTexFilter = static_cast<DWORD>( RENDERPARAM::g_emTextureFiltering );
		m_dwTexQuality = static_cast<DWORD>( RENDERPARAM::g_emTextureQuality );
		m_dwPhysXCloth = static_cast<DWORD>( RENDERPARAM::g_emPhysXCloth );

		APPLY ( pD3dApp );
	}

	void STESTSET::SetShadowChar( DWORD dwShadowChar )
	{
		m_dwShadowChar = dwShadowChar;
		RANPARAM::dwShadowChar = dwShadowChar;
		DxShadowMap::GetInstance().SetShadowDetail ( (ENUM_LAND_QUALITY) dwShadowChar );
	}

	void STESTSET::SetSkinDetail( DWORD dwSkinDetail )
	{
		m_dwSkinDetail = dwSkinDetail;
		RANPARAM::dwSkinDetail = dwSkinDetail;
		DxEffectMan::GetInstance().SetSkinDetail ( (EMSKINDETAIL) dwSkinDetail );
	}

	void STESTSET::SetVideoLevel( DWORD dwVideoLevel )
	{
		m_dwVideoLevel = dwVideoLevel;
		RANPARAM::dwVideoLevel = dwVideoLevel;
	}

	void STESTSET::VideoOKFromWidget()
	{
		GLGaeaClient* pGaeaClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();
		DxGlobalStage* pGlobalStage = pGaeaClient->GetGlobalStage();
		VideoOK( pGlobalStage->GetD3DApp(), pGaeaClient->GetSubPath(), pGaeaClient->GetServiceProvider() );
	}

	void STESTSET::GameOKFromWidget()
	{
		GLGaeaClient* pGaeaClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();
		GameOK( pGaeaClient->GetSubPath(), pGaeaClient->GetServiceProvider() );
	}

	void STESTSET::AudioOKFromWidget()
	{
		GLGaeaClient* pGaeaClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();
		DXPARAMSET::GetInstance().AudioOK( pGaeaClient->GetSubPath(), pGaeaClient->GetServiceProvider() );
	}
};

