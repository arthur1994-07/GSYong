#include "pch.h"

#include <string>


#include "../../DxEffect/Single/DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "../../Common/StlFunctions.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "./DxEffectParticleSys.h"

#include "../Char/DxEffcharData.h"
#include "./DxEffGroupPlayer.h"

#include "../../G-Logic/GLOGIC.h"

#include ".\dxeffgamehelp.h"
#include ".\Util\DxEffFanwise.h"
#include ".\Util\DxEffPlane.h"
#include ".\Util\DxEffCircle.h"

#include "../../DxTools/RENDERPARAM.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxEffGameHelp& DxEffGameHelp::GetInstance()
{
	static DxEffGameHelp Instance;
	return Instance;
}


DxEffGameHelp::DxEffGameHelp(void)
	: m_emTargetEffRender(EMTARGET_EFFECT_NULL)
	, m_pScopeRenderStats(NULL)
	, m_bOverEffRender(FALSE)
	, m_bClickEffRender(FALSE)
	, m_bScopeEffCircle(FALSE)
	, m_bScopeEffFanwise(FALSE)
{
	D3DXMatrixIdentity(&m_matScopeFanswise);
}

DxEffGameHelp::~DxEffGameHelp(void)
{
	RemoveAllEff();
}


SPDXEFFSINGLEGROUP DxEffGameHelp::NewEffGroup ( DxEffSinglePropGroupResource spPropGroup, const D3DXMATRIX &matTrans, const STARGETID* pTargetID )
{
	//	Note : Eff Group instance 생성.
	//
	SPDXEFFSINGLEGROUP spEffSingleGroup = spPropGroup.get()->NEWEFFGROUP ();
	if ( spEffSingleGroup.get() == NULL )   return SPDXEFFSINGLEGROUP();

	//	Note : 타겟지정.
	//
	spEffSingleGroup->SetTargetID ( pTargetID );

	//	Note : Eff Group의 매트릭스 트랜스폼 지정.
	//
	spEffSingleGroup->m_matWorld = matTrans;

	//	Note : Eff Group Create Dev.
	//
	spEffSingleGroup->Create ( m_pd3dDevice );

	return spEffSingleGroup;
}

SPDXEFFSINGLEGROUP DxEffGameHelp::NewEffGroup ( const char* szFileName, const D3DXMATRIX &matTrans, const STARGETID* pTargetID, BOOL bThread )
{
	GASSERT(m_pd3dDevice);

	std::string strFileName = szFileName;
	std::transform ( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

	//	Note : Prop Group 을 가져옴.
	//
	DxEffSinglePropGroupResource spPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( szFileName, bThread, FALSE );
	if ( !spPropGroup.IsValid() )
        return SPDXEFFSINGLEGROUP();

	//	Note : Eff Group instance 생성.
	//
	return NewEffGroup ( spPropGroup, matTrans, pTargetID );
}

SPDXEFFFANWISE DxEffGameHelp::NewFanwiseEff ( const FANWISE_PROPERTY &sProperty )
{
	GASSERT(m_pd3dDevice);

	SPDXEFFFANWISE sNewEff( new DxEffFanwise );
	sNewEff->m_dwRunFlag = (EFF_PLY_PLAY | EFF_CFG_NEVERDIE);
	sNewEff->InitDeviceObjects( m_pd3dDevice );
	sNewEff->RestoreDeviceObjects( m_pd3dDevice );
	sNewEff->UpdateProperty(sProperty);

	return sNewEff;
}

SPDXEFFPLANE DxEffGameHelp::NewPlaneEff ( const PLANE_PROPERTY &sProperty )
{
	GASSERT(m_pd3dDevice);

	SPDXEFFPLANE sNewEff( new DxEffPlane );
	sNewEff->m_dwRunFlag = (EFF_PLY_PLAY | EFF_CFG_NEVERDIE);
	sNewEff->InitDeviceObjects( m_pd3dDevice );
	sNewEff->RestoreDeviceObjects( m_pd3dDevice );
	sNewEff->UpdateProperty(sProperty);

	return sNewEff;
}

SPDXEFFCIRCLE DxEffGameHelp::NewCircleEff ( const CIRCLE_PROPERTY &sProperty )
{
	GASSERT(m_pd3dDevice);

	SPDXEFFCIRCLE sNewEff( new DxEffCircle );
	sNewEff->m_dwRunFlag = (EFF_PLY_PLAY | EFF_CFG_NEVERDIE);
	sNewEff->InitDeviceObjects( m_pd3dDevice );
	sNewEff->RestoreDeviceObjects( m_pd3dDevice );
	sNewEff->UpdateProperty(sProperty);

	return sNewEff;
}

HRESULT DxEffGameHelp::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	if( m_pOverEff.get() )
		m_pOverEff->InitDeviceObjects( pd3dDevice );
	if( m_pClickEff.get() )
        m_pClickEff->InitDeviceObjects( pd3dDevice );

	if( m_pScopeEffCircle.get() )
		m_pScopeEffCircle->InitDeviceObjects( pd3dDevice );
	if( m_pScopeEffCircle_OutLine.get() )
		m_pScopeEffCircle_OutLine->InitDeviceObjects( pd3dDevice );
	if( m_pScopeEffFanwise.get() )
		m_pScopeEffFanwise->InitDeviceObjects( pd3dDevice );

	if( m_pTargetEff_Default.get() )
		m_pTargetEff_Default->InitDeviceObjects( pd3dDevice );
	if( m_pTargetEffCircle_Enemy.get() )
		m_pTargetEffCircle_Enemy->InitDeviceObjects( pd3dDevice );
	if( m_pTargetEffArrow_Enemy.get() )
		m_pTargetEffArrow_Enemy->InitDeviceObjects( pd3dDevice );
	if( m_pTargetEffCircle_Npc.get() )
		m_pTargetEffCircle_Npc->InitDeviceObjects( pd3dDevice );
	if( m_pTargetEffArrow_Npc.get() )
		m_pTargetEffArrow_Npc->InitDeviceObjects( pd3dDevice );
	if( m_pTargetEffCircle_Alliance.get() )
		m_pTargetEffCircle_Alliance->InitDeviceObjects( pd3dDevice );
	if( m_pTargetEffArrow_Alliance.get() )
		m_pTargetEffArrow_Alliance->InitDeviceObjects( pd3dDevice );

    SPDXEFFSINGLEGROUP spEffect;
	MAP_MINEEFF_ITER iter = m_mapMineEff.begin();
	MAP_MINEEFF_ITER iter_end = m_mapMineEff.end();
	for ( ; iter!=iter_end; ++iter )
	{
        spEffect = iter->second;
		spEffect->InitDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffGameHelp::RestoreDeviceObjects ()
{
	if( m_pOverEff.get() )
		m_pOverEff->RestoreDeviceObjects( m_pd3dDevice );
	if( m_pClickEff.get() )
        m_pClickEff->RestoreDeviceObjects( m_pd3dDevice );

	if( m_pScopeEffCircle.get() )
		m_pScopeEffCircle->RestoreDeviceObjects( m_pd3dDevice );
	if( m_pScopeEffCircle_OutLine.get() )
		m_pScopeEffCircle_OutLine->RestoreDeviceObjects( m_pd3dDevice );
	if( m_pScopeEffFanwise.get() )
		m_pScopeEffFanwise->RestoreDeviceObjects( m_pd3dDevice );

	if( m_pTargetEff_Default.get() )
		m_pTargetEff_Default->RestoreDeviceObjects( m_pd3dDevice );
	if( m_pTargetEffCircle_Enemy.get() )
		m_pTargetEffCircle_Enemy->RestoreDeviceObjects( m_pd3dDevice );
	if( m_pTargetEffArrow_Enemy.get() )
		m_pTargetEffArrow_Enemy->RestoreDeviceObjects( m_pd3dDevice );
	if( m_pTargetEffCircle_Npc.get() )
		m_pTargetEffCircle_Npc->RestoreDeviceObjects( m_pd3dDevice );
	if( m_pTargetEffArrow_Npc.get() )
		m_pTargetEffArrow_Npc->RestoreDeviceObjects( m_pd3dDevice );
	if( m_pTargetEffCircle_Alliance.get() )
		m_pTargetEffCircle_Alliance->RestoreDeviceObjects( m_pd3dDevice );
	if( m_pTargetEffArrow_Alliance.get() )
		m_pTargetEffArrow_Alliance->RestoreDeviceObjects( m_pd3dDevice );

    SPDXEFFSINGLEGROUP spEffect;
	MAP_MINEEFF_ITER iter = m_mapMineEff.begin();
	MAP_MINEEFF_ITER iter_end = m_mapMineEff.end();
	for ( ; iter!=iter_end; ++iter )
	{
        spEffect = iter->second;
		spEffect->RestoreDeviceObjects (m_pd3dDevice);
	}

	m_pd3dDevice->CreateStateBlock( D3DSBT_ALL, &m_pScopeRenderStats );

	return S_OK;
}

HRESULT DxEffGameHelp::InvalidateDeviceObjects ()
{
	if( m_pOverEff.get() )
		m_pOverEff->InvalidateDeviceObjects();
	if( m_pClickEff.get() )
        m_pClickEff->InvalidateDeviceObjects();

	if( m_pScopeEffCircle.get() )
		m_pScopeEffCircle->InvalidateDeviceObjects();
	if( m_pScopeEffCircle_OutLine.get() )
		m_pScopeEffCircle_OutLine->InvalidateDeviceObjects();
	if( m_pScopeEffFanwise.get() )
		m_pScopeEffFanwise->InvalidateDeviceObjects();

	if( m_pTargetEff_Default.get() )
		m_pTargetEff_Default->InvalidateDeviceObjects();
	if( m_pTargetEffCircle_Enemy.get() )
		m_pTargetEffCircle_Enemy->InvalidateDeviceObjects();
	if( m_pTargetEffArrow_Enemy.get() )
		m_pTargetEffArrow_Enemy->InvalidateDeviceObjects();
	if( m_pTargetEffCircle_Npc.get() )
		m_pTargetEffCircle_Npc->InvalidateDeviceObjects();
	if( m_pTargetEffArrow_Npc.get() )
		m_pTargetEffArrow_Npc->InvalidateDeviceObjects();
	if( m_pTargetEffCircle_Alliance.get() )
		m_pTargetEffCircle_Alliance->InvalidateDeviceObjects();
	if( m_pTargetEffArrow_Alliance.get() )
		m_pTargetEffArrow_Alliance->InvalidateDeviceObjects();

    SPDXEFFSINGLEGROUP spEffect;
	MAP_MINEEFF_ITER iter = m_mapMineEff.begin();
	MAP_MINEEFF_ITER iter_end = m_mapMineEff.end();
	for ( ; iter!=iter_end; ++iter )
	{
        spEffect = iter->second;
		spEffect->InvalidateDeviceObjects ();
	}

	SAFE_RELEASE( m_pScopeRenderStats );

	return S_OK;
}

HRESULT DxEffGameHelp::DeleteDeviceObjects ()
{
	RemoveAllEff();

	return S_OK;
}

HRESULT DxEffGameHelp::FrameMove ( float fTime, float fElapsedTime )
{
	if( m_pOverEff.get() && m_bOverEffRender )
		m_pOverEff->FrameMove( fTime, fElapsedTime );

	if( m_pClickEff.get() && m_bClickEffRender && RENDERPARAM::bClickEffect ) 
		m_pClickEff->FrameMove( fTime, fElapsedTime );

	if( m_pScopeEffFanwise.get() && m_bScopeEffFanwise ) 
		m_pScopeEffFanwise->FrameMove( fTime, fElapsedTime );

	if ( m_bScopeEffCircle )
	{
		if( m_pScopeEffCircle.get() ) 
			m_pScopeEffCircle->FrameMove( fTime, fElapsedTime );
		if( m_pScopeEffCircle_OutLine.get() ) 
			m_pScopeEffCircle_OutLine->FrameMove( fTime, fElapsedTime );
	}

	if ( RENDERPARAM::bTargetEffect )
	{
		switch ( m_emTargetEffRender )
		{
		case EMTARGET_EFFECT_NULL:
		default:
			break;

		case EMTARGET_EFFECT_DEFAULT:
			{
				if( m_pTargetEff_Default.get() ) 
					m_pTargetEff_Default->FrameMove( fTime, fElapsedTime );
			}
			break;

		case EMTARGET_EFFECT_ENEMY:
			{
				if( m_pTargetEffCircle_Enemy.get() ) 
					m_pTargetEffCircle_Enemy->FrameMove( fTime, fElapsedTime );
				if( m_pTargetEffArrow_Enemy.get() ) 
					m_pTargetEffArrow_Enemy->FrameMove( fTime, fElapsedTime );
			}
			break;

		case EMTARGET_EFFECT_NPC:
			{
				if( m_pTargetEffCircle_Npc.get() ) 
					m_pTargetEffCircle_Npc->FrameMove( fTime, fElapsedTime );
				if( m_pTargetEffArrow_Npc.get() ) 
					m_pTargetEffArrow_Npc->FrameMove( fTime, fElapsedTime );
			}
			break;

		case EMTARGET_EFFECT_ALLIANCE:
			{
				if( m_pTargetEffCircle_Alliance.get() ) 
					m_pTargetEffCircle_Alliance->FrameMove( fTime, fElapsedTime );
				if( m_pTargetEffArrow_Alliance.get() ) 
					m_pTargetEffArrow_Alliance->FrameMove( fTime, fElapsedTime );
			}
			break;
		}
	}

	if( RENDERPARAM::bMineEffect )
	{
        SPDXEFFSINGLEGROUP spEffect;
		MAP_MINEEFF_ITER iter = m_mapMineEff.begin();
		MAP_MINEEFF_ITER iter_end = m_mapMineEff.end();
		for ( ; iter!=iter_end; ++iter )
		{
            spEffect = iter->second;
			spEffect->FrameMove ( fTime, fElapsedTime );
		}
	}

	return S_OK;
}

HRESULT	DxEffGameHelp::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_pOverEff.get() && m_bOverEffRender )
		m_pOverEff->Render( pd3dDevice, D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), 1.0f );

	if( m_pClickEff.get() && m_bClickEffRender && RENDERPARAM::bClickEffect ) 
		m_pClickEff->Render( pd3dDevice, D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), 1.0f );

	if ( RENDERPARAM::bTargetEffect )
	{
		switch ( m_emTargetEffRender )
		{
		case EMTARGET_EFFECT_NULL:
		default:
			break;

		case EMTARGET_EFFECT_DEFAULT:
			{
				if( m_pTargetEff_Default.get() ) 
					m_pTargetEff_Default->Render(pd3dDevice,m_vSelectTargetSize, 1.f );
			}
			break;

		case EMTARGET_EFFECT_ENEMY:
			{
				if( m_pTargetEffCircle_Enemy.get() ) 
					m_pTargetEffCircle_Enemy->Render(pd3dDevice,m_vSelectTargetSize, 1.f );
				if( m_pTargetEffArrow_Enemy.get() ) 
					m_pTargetEffArrow_Enemy->Render(pd3dDevice,m_vSelectTargetSize, 1.f );
			}
			break;

		case EMTARGET_EFFECT_NPC:
			{
				if( m_pTargetEffCircle_Npc.get() ) 
					m_pTargetEffCircle_Npc->Render(pd3dDevice,m_vSelectTargetSize, 1.f );
				if( m_pTargetEffArrow_Npc.get() ) 
					m_pTargetEffArrow_Npc->Render(pd3dDevice,m_vSelectTargetSize, 1.f );
			}
			break;

		case EMTARGET_EFFECT_ALLIANCE:
			{
				if( m_pTargetEffCircle_Alliance.get() ) 
					m_pTargetEffCircle_Alliance->Render(pd3dDevice,m_vSelectTargetSize, 1.f );
				if( m_pTargetEffArrow_Alliance.get() ) 
					m_pTargetEffArrow_Alliance->Render(pd3dDevice,m_vSelectTargetSize, 1.f );
			}
			break;
		}
	}

	//! 범위지정만 렌더스테이트를 바꾼다.
	if ( m_bScopeEffCircle || m_bScopeEffFanwise )
	{
		GASSERT(m_pScopeRenderStats);
		m_pScopeRenderStats->Capture();

		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );

		if( m_pScopeEffCircle_OutLine.get() && m_bScopeEffCircle ) 
			m_pScopeEffCircle_OutLine->Render( pd3dDevice, m_matScopeCircle, 1.f );

		if( m_pScopeEffFanwise.get() && m_bScopeEffFanwise ) 
			m_pScopeEffFanwise->Render( pd3dDevice, m_matScopeFanswise, 1.f );

		pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

		if( m_pScopeEffCircle.get() && m_bScopeEffCircle ) 
			m_pScopeEffCircle->Render( pd3dDevice, m_matScopeCircle, 1.f );

		m_pScopeRenderStats->Apply();
	}


	if( RENDERPARAM::bMineEffect )
	{
        SPDXEFFSINGLEGROUP spEffect;
		MAP_MINEEFF_ITER iter = m_mapMineEff.begin();
		MAP_MINEEFF_ITER iter_end = m_mapMineEff.end();
		for ( ; iter!=iter_end; ++iter )
		{
            spEffect = iter->second;
			spEffect->Render( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), 1.f );
		}
	}

	return S_OK;
}

HRESULT DxEffGameHelp::UpdateGameHelpEffect(
    const GAMEHELP_EFFECTTYPE emEffectType,
    const D3DXMATRIX& matTrans,
    const STARGETID& sTargetID,
    const D3DXVECTOR3& vTargetSize)
{
	if ( sTargetID.GaeaId==EMTARGET_NULL )
		return E_FAIL;

	std::string strFileName;

	switch( emEffectType )
	{
	case EMTARGET_EFFECT_NULL:
		{
			return S_FALSE;
		}
		break;

	case EMTARGET_EFFECT_DEFAULT:
	case EMTARGET_EFFECT_ENEMY:
	case EMTARGET_EFFECT_NPC:
	case EMTARGET_EFFECT_ALLIANCE:
		{
			m_emTargetEffRender = emEffectType;
			m_vSelectTargetSize = vTargetSize;

			SPDXEFFSINGLEGROUP* pspTargetEffCircle = NULL;
			SPDXEFFSINGLEGROUP* pspTargetEffArrow  = NULL;
			std::string*		pstrFilenameCircle = NULL;
			std::string*		pstrFilenameArrow  = NULL;

			switch ( emEffectType )
			{
			case EMTARGET_EFFECT_DEFAULT:
				{
					pspTargetEffCircle = &m_pTargetEff_Default;
					pstrFilenameCircle = &m_sEffectFilenameCaps.strTargetEff_Default;
				}
				break;

			case EMTARGET_EFFECT_ENEMY:
				{
					pspTargetEffCircle = &m_pTargetEffCircle_Enemy;
					pspTargetEffArrow  = &m_pTargetEffArrow_Enemy;
					pstrFilenameCircle = &m_sEffectFilenameCaps.strTargetEff_Circle_Enemy;
					pstrFilenameArrow  = &m_sEffectFilenameCaps.strTargetEff_Arrow_Enemy;
				}
				break;

			case EMTARGET_EFFECT_NPC:
				{
					pspTargetEffCircle = &m_pTargetEffCircle_Npc;
					pspTargetEffArrow  = &m_pTargetEffArrow_Npc;
					pstrFilenameCircle = &m_sEffectFilenameCaps.strTargetEff_Circle_Npc;
					pstrFilenameArrow  = &m_sEffectFilenameCaps.strTargetEff_Arrow_Npc;
				}
				break;

			case EMTARGET_EFFECT_ALLIANCE:
				{
					pspTargetEffCircle = &m_pTargetEffCircle_Alliance;
					pspTargetEffArrow  = &m_pTargetEffArrow_Alliance;
					pstrFilenameCircle = &m_sEffectFilenameCaps.strTargetEff_Circle_Alliance;
					pstrFilenameArrow  = &m_sEffectFilenameCaps.strTargetEff_Arrow_Alliance;
				}
				break;

			default:
				return E_FAIL;
			};

			if( pspTargetEffCircle && (*pspTargetEffCircle) ) 
			{
				(*pspTargetEffCircle)->SetTargetID( &sTargetID );
				(*pspTargetEffCircle)->m_matWorld = matTrans;
			}
			else if ( pspTargetEffCircle )
			{
				strFileName = (*pstrFilenameCircle);

				std::transform ( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

				(*pspTargetEffCircle) = NewEffGroup( strFileName.c_str(), matTrans, &sTargetID, FALSE );
				if( (*pspTargetEffCircle).get() == NULL )
					return E_FAIL;
			}

			if( pspTargetEffArrow && (*pspTargetEffArrow) ) 
			{
				(*pspTargetEffArrow)->SetTargetID( &sTargetID );
				(*pspTargetEffArrow)->m_matWorld = matTrans;
			}
			else if ( pspTargetEffArrow )
			{
				strFileName = (*pstrFilenameArrow);

				std::transform ( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

				(*pspTargetEffArrow) = NewEffGroup( strFileName.c_str(), matTrans, &sTargetID, FALSE );
				if( (*pspTargetEffArrow).get() == NULL )
					return E_FAIL;
			}
		}
		break;

	case EMOVER_EFFECT:
		{
			m_bOverEffRender = TRUE;

			if( m_pOverEff.get() )
			{
				m_pOverEff->SetTargetID( &sTargetID );
				m_pOverEff->m_matWorld = matTrans;
			}
			else
			{
				strFileName = m_sEffectFilenameCaps.strOverEff;
				std::transform( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

				m_pOverEff = NewEffGroup( strFileName.c_str(), matTrans, &sTargetID, FALSE );
				if( m_pOverEff.get() == NULL )
					return E_FAIL;
			}
		}
		break;

	case EMCLICK_EFFECT:
		{
			m_bClickEffRender = TRUE;

			if( m_pClickEff.get() )
			{
				m_pClickEff->SetTargetID( &sTargetID );
				m_pClickEff->m_matWorld = matTrans; 			
			}
			else
			{
				strFileName = m_sEffectFilenameCaps.strClickEff;
				std::transform( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

				m_pClickEff = NewEffGroup( strFileName.c_str(), matTrans, &sTargetID, FALSE );
				if( m_pClickEff.get() == NULL )
					return E_FAIL;
			}		
		}
		break;

	// 스코프 원형 이펙트의 경우 파일에서 읽지 않는다.
	case EMSCOPE_CIRCLE_EFFECT:
		{
			PLANE_PROPERTY  sProperty;
			CIRCLE_PROPERTY sProperty_OutLine;
			return UpdateScopePlaneEffect( sProperty, sProperty_OutLine, matTrans );
		}
		break;

	//! 스코프 부채꼴 이펙트의 경우 파일에서 읽지 않는다.
	case EMSCOPE_FANWISE_EFFECT:
		{
			FANWISE_PROPERTY sProperty;
			return UpdateFanwiseEffect( sProperty, matTrans );
		}
		break;

	default:
		{
			strFileName = m_sEffectFilenameCaps.strMineEff;
			std::transform( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

			SPDXEFFSINGLEGROUP spNewEffect = NewEffGroup( strFileName.c_str(), matTrans, &sTargetID, FALSE );
			if( spNewEffect.get() == NULL )
				return E_FAIL;

			m_mapMineEff.insert( std::make_pair(sTargetID,spNewEffect) );
		}
		break;
	};

	return S_OK;
}

HRESULT DxEffGameHelp::UpdateTargetEffect( const GAMEHELP_EFFECTTYPE	emEffectType, 
										   const STARGETID& 			sTargetID, 
										   const D3DXMATRIX&			matCircleTrans, 
										   const D3DXMATRIX&			matArrowTrans,
										   const D3DXVECTOR3&			vTargetSize )
{
	if ( sTargetID.GaeaId==EMTARGET_NULL )
		return E_FAIL;

	std::string				strFileName;

	SPDXEFFSINGLEGROUP* 	pspTargetEffCircle = NULL;
	SPDXEFFSINGLEGROUP* 	pspTargetEffArrow  = NULL;
	std::string*			pstrFilenameCircle = NULL;
	std::string*			pstrFilenameArrow  = NULL;

	switch( emEffectType )
	{
	case EMTARGET_EFFECT_DEFAULT:
		pspTargetEffCircle = &m_pTargetEff_Default;
		pstrFilenameCircle = &m_sEffectFilenameCaps.strTargetEff_Default;
		break;

	case EMTARGET_EFFECT_ENEMY:
		pspTargetEffCircle = &m_pTargetEffCircle_Enemy;
		pspTargetEffArrow  = &m_pTargetEffArrow_Enemy;
		pstrFilenameCircle = &m_sEffectFilenameCaps.strTargetEff_Circle_Enemy;
		pstrFilenameArrow  = &m_sEffectFilenameCaps.strTargetEff_Arrow_Enemy;
		break;

	case EMTARGET_EFFECT_NPC:
		pspTargetEffCircle = &m_pTargetEffCircle_Npc;
		pspTargetEffArrow  = &m_pTargetEffArrow_Npc;
		pstrFilenameCircle = &m_sEffectFilenameCaps.strTargetEff_Circle_Npc;
		pstrFilenameArrow  = &m_sEffectFilenameCaps.strTargetEff_Arrow_Npc;
		break;

	case EMTARGET_EFFECT_ALLIANCE:
		pspTargetEffCircle = &m_pTargetEffCircle_Alliance;
		pspTargetEffArrow  = &m_pTargetEffArrow_Alliance;
		pstrFilenameCircle = &m_sEffectFilenameCaps.strTargetEff_Circle_Alliance;
		pstrFilenameArrow  = &m_sEffectFilenameCaps.strTargetEff_Arrow_Alliance;
		break;

	default:
		return E_FAIL;
	}; //switch

	m_emTargetEffRender = emEffectType;
	m_vSelectTargetSize = vTargetSize;

	if( pspTargetEffCircle && (*pspTargetEffCircle) ) 
	{
		(*pspTargetEffCircle)->SetTargetID( &sTargetID );
		(*pspTargetEffCircle)->m_matWorld = matCircleTrans;
	}
	else if( pspTargetEffCircle )
	{
		strFileName = (*pstrFilenameCircle);

		std::transform( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

		(*pspTargetEffCircle) = NewEffGroup( strFileName.c_str(), matCircleTrans, &sTargetID, FALSE );
		if( (*pspTargetEffCircle).get() == NULL )
			return E_FAIL;
	} //if..else

	if( pspTargetEffArrow && (*pspTargetEffArrow) ) 
	{
		(*pspTargetEffArrow)->SetTargetID( &sTargetID );
		(*pspTargetEffArrow)->m_matWorld = matArrowTrans;
	}
	else if( pspTargetEffArrow )
	{
		strFileName = (*pstrFilenameArrow);

		std::transform( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

		(*pspTargetEffArrow) = NewEffGroup( strFileName.c_str(), matArrowTrans, &sTargetID, FALSE );
		if( (*pspTargetEffArrow).get() == NULL )
			return E_FAIL;
	} //if..else

	return S_OK;
}

HRESULT DxEffGameHelp::UpdateFanwiseEffect( const FANWISE_PROPERTY &sProperty, const D3DXMATRIX &matTrans )
{
	FANWISE_PROPERTY sClone(sProperty);

	m_bScopeEffFanwise = TRUE;
	m_matScopeFanswise = matTrans; 	

	StringCchCopy( sClone.m_szTexture, sizeof(sClone.m_szTexture), m_sTextrueFilenameCaps.strScopeFanwise.c_str() );

	if ( !m_pScopeEffFanwise.get() )
	{
		m_pScopeEffFanwise = NewFanwiseEff( sClone );

		if( m_pScopeEffFanwise.get() == NULL )
			return E_FAIL;
	}
	else
	{
		m_pScopeEffFanwise->UpdateProperty( sClone );
	}

	return S_OK;
}

HRESULT DxEffGameHelp::UpdateScopePlaneEffect( const PLANE_PROPERTY &sProperty, const CIRCLE_PROPERTY &sPropertyOutLine, const D3DXMATRIX &matTrans )
{
	m_bScopeEffCircle = TRUE;
	m_matScopeCircle  = matTrans; 	

	{
		PLANE_PROPERTY sClone(sProperty);

		StringCchCopy( sClone.m_szTexture, sizeof(sClone.m_szTexture), m_sTextrueFilenameCaps.strScopeCircle.c_str() );

		if ( !m_pScopeEffCircle.get() )
		{
			m_pScopeEffCircle = NewPlaneEff( sClone );

			if( m_pScopeEffCircle.get() == NULL )
				return E_FAIL;
		}
		else
		{
			m_pScopeEffCircle->UpdateProperty( sClone );
		}
	}

	{
		CIRCLE_PROPERTY sClone(sPropertyOutLine);
		sClone.m_dwColor = NULL;

		if ( !m_pScopeEffCircle_OutLine.get() )
		{
			m_pScopeEffCircle_OutLine = NewCircleEff( sClone );

			if( m_pScopeEffCircle_OutLine.get() == NULL )
				return E_FAIL;
		}
		else
		{
			m_pScopeEffCircle_OutLine->UpdateProperty( sClone );
		}
	}

	return S_OK;
}

HRESULT DxEffGameHelp::BlindEff(const GAMEHELP_EFFECTTYPE emEffectType, const STARGETID &sTargetID )
{
	if( sTargetID.GaeaId == EMTARGET_NULL )
		return E_FAIL;

	MAP_MINEEFF_ITER iter = m_mapMineEff.find( sTargetID );
	if( iter != m_mapMineEff.end() )
		m_mapMineEff.erase( iter );

	return S_OK;
}

HRESULT DxEffGameHelp::BlindEff(const GAMEHELP_EFFECTTYPE emEffectType )
{
	switch( emEffectType )
	{
	case EMTARGET_EFFECT_DEFAULT:
	case EMTARGET_EFFECT_ENEMY:
	case EMTARGET_EFFECT_NPC:
	case EMTARGET_EFFECT_ALLIANCE:
		{
			m_emTargetEffRender = EMTARGET_EFFECT_NULL;
		}
		break;

	case EMOVER_EFFECT:
		{
			m_bOverEffRender = FALSE;
		}
		break;

	case EMCLICK_EFFECT:
		{
			m_bClickEffRender = FALSE;
		}
		break;
		
	case EMSCOPE_CIRCLE_EFFECT:
		{
			m_bScopeEffCircle = FALSE;
		}
		break;

	case EMSCOPE_FANWISE_EFFECT:
		{
			m_bScopeEffFanwise = FALSE;
		}
		break;

	};

	return S_OK;
}

HRESULT DxEffGameHelp::BlindTargetEff ()
{
	m_emTargetEffRender = EMTARGET_EFFECT_NULL;
	return S_OK;
}

HRESULT DxEffGameHelp::BlindScopeEff()
{
	m_bScopeEffCircle  = FALSE;
	m_bScopeEffFanwise = FALSE;
	return S_OK;
}

void DxEffGameHelp::RemoveAllEff ()
{
	if( m_pOverEff.get() )
		m_pOverEff->CleanUp();
	if( m_pClickEff.get() )
        m_pClickEff->CleanUp();

	if( m_pScopeEffCircle.get() )
		m_pScopeEffCircle->CleanUp();
	if( m_pScopeEffCircle_OutLine.get() )
		m_pScopeEffCircle_OutLine->CleanUp();
	if( m_pScopeEffFanwise.get() )
		m_pScopeEffFanwise->CleanUp();

	if( m_pTargetEff_Default.get() )
		m_pTargetEff_Default->CleanUp();
	if( m_pTargetEffCircle_Enemy.get() )
		m_pTargetEffCircle_Enemy->CleanUp();
	if( m_pTargetEffArrow_Enemy.get() )
		m_pTargetEffArrow_Enemy->CleanUp();
	if( m_pTargetEffCircle_Npc.get() )
		m_pTargetEffCircle_Npc->CleanUp();
	if( m_pTargetEffArrow_Npc.get() )
		m_pTargetEffArrow_Npc->CleanUp();
	if( m_pTargetEffCircle_Alliance.get() )
		m_pTargetEffCircle_Alliance->CleanUp();
	if( m_pTargetEffArrow_Alliance.get() )
		m_pTargetEffArrow_Alliance->CleanUp();

	SPDXEFFSINGLEGROUP spEffect;
	MAP_MINEEFF_ITER iter = m_mapMineEff.begin();
	MAP_MINEEFF_ITER iter_end = m_mapMineEff.end();
	for ( ; iter!=iter_end; ++iter )
	{
		spEffect = iter->second;
		if( spEffect.get() )
			spEffect->CleanUp();
	}

	m_pOverEff.reset();
	m_pClickEff.reset();
	m_pScopeEffCircle.reset();
	m_pScopeEffCircle_OutLine.reset();
	m_pScopeEffFanwise.reset();
	m_pTargetEff_Default.reset();
	m_pTargetEffCircle_Enemy.reset();
	m_pTargetEffArrow_Enemy.reset();
	m_pTargetEffCircle_Npc.reset();
	m_pTargetEffArrow_Npc.reset();
	m_pTargetEffCircle_Alliance.reset();
	m_pTargetEffArrow_Alliance.reset();

	m_mapMineEff.clear();
}