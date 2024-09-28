#include "pch.h"

#include "../../DxEffect/Mesh/DxStaticMeshBase.h"
#include "../../DxEffect/Mesh/DxStaticMesh.h"
#include "../../DxEffect/Mesh/DxFrameAniMan.h"
#include "../../DxTools/Occlusion/NSOcclusionCulling.h"
#include "../../DxTools/Material/DxStaticMaterialMeshFrameAni.h"
#include "../../DxTools/Material/DxStaticMaterialMeshAniMan.h"
#include "../../DxTools/Material/DxStaticMaterialMesh.h"
#include "../../DxTools/DxRenderStates.h"
#include "./DxPieceEffMan.h"
#include "./DxPieceDef.h"

#include "./DxPieceStaticAni.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
//								D	X		R	E	P	L	A	C	E	P	I	E	C	E		E	X
//----------------------------------------------------------------------------------------------------------------------
DxStaticAniFrame::DxStaticAniFrame()
	: m_bFrameMoveLock(TRUE)
	, m_dwEffectCount(0L)
	, m_pEffectHead(NULL)
	, m_pStaticMesh(NULL)
	, m_pFrameAniMan(NULL)
	, m_nRefCount(0)
	, m_dwTexType(0L)
	, m_pNext(NULL)
{
}

DxStaticAniFrame::~DxStaticAniFrame()
{
	CleanUp();
}

void DxStaticAniFrame::CleanUp()
{
	m_dwEffectCount = 0L;
	SAFE_DELETE( m_pEffectHead );
	SAFE_DELETE( m_pStaticMesh );
	SAFE_DELETE( m_pFrameAniMan );

	m_vecAnimationMatrixPointer.clear();

	SAFE_DELETE( m_pNext );	// 음 조심해야 한다.
}

void DxStaticAniFrame::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	PDXEFFECTBASE pCur = m_pEffectHead;
	while( pCur )
	{
		pCur->RestoreDeviceObjects( pd3dDevice );
		pCur = pCur->pEffectNext;
	}
}

void DxStaticAniFrame::OnLostDevice()
{
	PDXEFFECTBASE pCur = m_pEffectHead;
	while( pCur )
	{
		pCur->InvalidateDeviceObjects();
		pCur = pCur->pEffectNext;
	}
}

DxFrame* DxStaticAniFrame::GetAniFrame( const char* szName )
{
	if (!m_pFrameAniMan)
        return NULL;
    else
	    return m_pFrameAniMan->GetAniFrame( szName );
}

DxAnimationMan*	DxStaticAniFrame::GetAniManHead()
{
	if (!m_pFrameAniMan)
        return NULL;
    else
	    return m_pFrameAniMan->GetAniManHead();
}

void DxStaticAniFrame::MakeAniMan( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh* pFrameMesh )
{
	m_pFrameAniMan = new DxFrameAniMan;
	m_pFrameAniMan->MakeAniMan( pd3dDevice, pFrameMesh );
}

DxStaticMesh* DxStaticAniFrame::GetStaticMesh() 
{ 
	return dynamic_cast<DxStaticMesh*>( m_pStaticMesh ); 
}

void DxStaticAniFrame::MergeLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
										 const TSTRING& strLightMapName, 
										 DxStaticMesh* pStaticMesh, 
										 DxPieceManagerPSF* pPieceManager,
										 const D3DXMATRIX& matWorld,
										 VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
										 DWORD& dwStartTextureSize,
										 WORDWORD& wwBaseTexelXY,
										 BOOL& bEnoughUsedPixelLightMap,
										 const WORDWORD& wwLightID4Shadow )
{
	if ( !m_pStaticMesh )
		return;

	m_pStaticMesh->MergeLightMapUVPiece
	( 
		pd3dDevice, 
		strLightMapName, 
		pStaticMesh, 
		pPieceManager, 
		matWorld, 
		vecsvecLightMapObj, 
		dwStartTextureSize,
		wwBaseTexelXY,
		bEnoughUsedPixelLightMap,
		wwLightID4Shadow
	);
}

void DxStaticAniFrame::CreateLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
											   const TSTRING& strLightMapName,
											   TSTRING& strTextureCombined, 
											   TSTRING& strTextureDirect_Shadow_1,
											   TSTRING& strTextureDirect_Shadow_2,
											   TSTRING& strTextureIndirect,
											   VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
											   WORDWORD wwBaseTexelXY )
{
	if ( !m_pStaticMesh )
		return;

	m_pStaticMesh->CreateLightMapUVPiece( pd3dDevice, 
											strLightMapName,
											strTextureCombined, 
											strTextureDirect_Shadow_1,
											strTextureDirect_Shadow_2,
											strTextureIndirect,
											vecsvecLightMapObj,
											wwBaseTexelXY );
}

void DxStaticAniFrame::CreateLightMapUVPieceObject( LPDIRECT3DDEVICEQ pd3dDevice, 
											 const TSTRING& strLightMapName,
											 TSTRING& strTextureCombined, 
											 TSTRING& strTextureDirect_Shadow_1,
											 TSTRING& strTextureDirect_Shadow_2,
											 TSTRING& strTextureIndirect,
											 VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
											 WORDWORD wwBaseTexelXY,
											 BOOL bRan_1_0, 
											 float fCellSize )
{
	if ( !m_pStaticMesh )
		return;

	m_pStaticMesh->CreateLightMapUVPieceObject( pd3dDevice, 
												strLightMapName,
												strTextureCombined, 
												strTextureDirect_Shadow_1,
												strTextureDirect_Shadow_2,
												strTextureIndirect,
												vecsvecLightMapObj,
												wwBaseTexelXY,
												bRan_1_0, 
												fCellSize );
}

void DxStaticAniFrame::SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate )
{
	if ( !m_pStaticMesh )
		return;

	DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
	if ( pStaticMesh )
	{
		pStaticMesh->SetLightMapUV_Rate( vLightMapUV_Rate );
	}
}

void DxStaticAniFrame::LoadLightMapOLD( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if ( !m_pStaticMesh )
        return;

	DxStaticMesh* pStaticMesh = dynamic_cast<DxStaticMesh*>( m_pStaticMesh );
	if ( pStaticMesh )
	{
		pStaticMesh->LoadLightMapOLD( pd3dDevice );
	}
}

// Radiosity
void DxStaticAniFrame::BakeRadiosityForPiece( LPDIRECT3DDEVICEQ pd3dDevice,
											 DxLandMan* pLandMan,
											 VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
											 const D3DXMATRIX& matWorld, 
											 LPDIRECT3DTEXTUREQ pTextureSystem,
											 float fReflectance, 
											 float fFogBlend,
											 DWORD dwFogLength,
											 DWORD dwRadiosityIndex )
{
	if ( !m_pStaticMesh )
		return;

	m_pStaticMesh->BakeRadiosityForPiece( pd3dDevice, 
										pLandMan,
										vecsvecLightMapObj,
										matWorld, 
										pTextureSystem,
										fReflectance, 
										fFogBlend,
										dwFogLength,
										dwRadiosityIndex );
}

BOOL DxStaticAniFrame::CheckOnlyVoxel()
{
	if ( !m_pStaticMesh )
		return FALSE;

	DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
	if( !pStaticMesh )
		return FALSE;

	return pStaticMesh->CheckOnlyVoxel();
}

void DxStaticAniFrame::FrameMoveTOOL( LPDIRECT3DDEVICEQ pd3dDevice, const float fElapsedTime )
{
	if( !m_bFrameMoveLock )	
		return;	// Note : Lock Off	.. No FrameMove

	DWORD dwTexType(0L);

	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->RenderOrderSettingPiece( pd3dDevice, TRUE, dwTexType );
		}

		m_pStaticMesh->FrameMove( fElapsedTime );
	}

	if ( m_spStaticMaterialMeshAniMan.get() )
	{
		m_spStaticMaterialMeshAniMan->RenderOrderSettingPiece( pd3dDevice, TRUE, dwTexType );
	}

	// 여기 놔두면 심하게 흔들린다.
	PDXEFFECTBASE pCur = m_pEffectHead;
	while( pCur )
	{
		pCur->FrameMove( 0.f, fElapsedTime );
		pCur = pCur->pEffectNext;
	}

	// Note : Lock Off
	m_bFrameMoveLock = FALSE;
}

void DxStaticAniFrame::FrameMoveMAT( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWLD )
{
	if( !m_bFrameMoveLock )	
		return;	// Note : Lock Off	.. No FrameMove

	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->RenderOrderSettingPiece( pd3dDevice, !bWLD, m_dwTexType );
		}
	}

	if ( m_spStaticMaterialMeshAniMan.get() )
	{
		DWORD dwTexType(0);
		m_spStaticMaterialMeshAniMan->RenderOrderSettingPiece( pd3dDevice, !bWLD, dwTexType );

		// EMTTF_OPAQUE -> EMTTF_FRAMEANI_OPAQUE
		// EMTTF_ALPHA_HARD -> EMTTF_FRAMEANI_ALPHA_HARD
		// EMTTF_ALPHA_SOFT -> EMTTF_FRAMEANI_ALPHA_SOFT
		dwTexType *= EMTTF_FRAMEANI_OPAQUE;	// 
		m_dwTexType |= dwTexType;
	}

	// Note : Lock Off
	m_bFrameMoveLock = FALSE;
}

void DxStaticAniFrame::Render( LPDIRECT3DDEVICEQ pd3dDevice, 
                              const D3DXMATRIX& matWorld, 
                              const float fAniTime, 
                              DxTextureEffMan::MAPTEXEFF* pmapTexEff,
                              const D3DXMATRIX& matTexScale,
                              float fAlpha,
							  int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
							  LPDIRECT3DTEXTUREQ pTextureDay, 
							  LPDIRECT3DTEXTUREQ pTextureNight,
							  BOOL bMPShading )
{
	// Note : FrameAnimation은 Alpha가 분리 되지 않는다....
	if( m_pFrameAniMan )
	{
		m_pFrameAniMan->Render( pd3dDevice, matWorld, fAniTime, pmapTexEff );
	}

	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	if( m_pStaticMesh )
	{
        // 텍스쳐 Matrix 셋팅.
        pd3dDevice->SetTransform( D3DTS_TEXTURE1, &matTexScale );
        pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );

        // 렌더
        D3DXVECTOR2 vLightMapUV_Offset( matTexScale._31, matTexScale._32 );
		m_pStaticMesh->Render_PIECE_( pd3dDevice, pmapTexEff, matWorld, vLightMapUV_Offset, fAlpha, nRenderMode, pTextureDay, pTextureNight, bMPShading );

        // 텍스쳐 Matrix 되돌림.
        DxResetTextureMatrix( pd3dDevice, 1 );
        pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	}

	PDXEFFECTBASE pCur = m_pEffectHead;
	while( pCur )
	{
		(*pCur->m_pmatLocal) = matWorld;	// Note : World Matrix 음.. 어렵네.

		pCur->Render( NULL, pd3dDevice );
		pCur = pCur->pEffectNext;
	}

	// Note : Lock ON
	m_bFrameMoveLock = TRUE;
}

void DxStaticAniFrame::Render_HardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
										const D3DXMATRIX& matWorld, 
										DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
										const D3DXMATRIX& matTexScale,
										float fAlpha,
										int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
										LPDIRECT3DTEXTUREQ pTextureDay, 
										LPDIRECT3DTEXTUREQ pTextureNight,
										BOOL bMPShading )
{
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	if( m_pStaticMesh )
	{
		// 텍스쳐 Matrix 셋팅.
		pd3dDevice->SetTransform( D3DTS_TEXTURE1, &matTexScale );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );

		// 렌더
		D3DXVECTOR2 vLightMapUV_Offset( matTexScale._31, matTexScale._32 );
		m_pStaticMesh->Render_PIECE_Alpha( pd3dDevice, pmapTexEff, matWorld, vLightMapUV_Offset, fAlpha, nRenderMode, pTextureDay, pTextureNight, bMPShading );

		// 텍스쳐 Matrix 되돌림.
		DxResetTextureMatrix( pd3dDevice, 1 );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	}
}

void DxStaticAniFrame::Render_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
										const D3DXMATRIX& matWorld, 
										DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
										const D3DXMATRIX& matTexScale,
										float fAlpha,
										int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
										LPDIRECT3DTEXTUREQ pTextureDay, 
										LPDIRECT3DTEXTUREQ pTextureNight,
										BOOL bMPShading )
{
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	if( m_pStaticMesh )
	{
        // 텍스쳐 Matrix 셋팅.
        pd3dDevice->SetTransform( D3DTS_TEXTURE1, &matTexScale );
        pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );

        // 렌더
        D3DXVECTOR2 vLightMapUV_Offset( matTexScale._31, matTexScale._32 );
		m_pStaticMesh->Render_PIECE_SoftAlpha( pd3dDevice, pmapTexEff, matWorld, vLightMapUV_Offset, fAlpha, nRenderMode, pTextureDay, pTextureNight, bMPShading );

        // 텍스쳐 Matrix 되돌림.
        DxResetTextureMatrix( pd3dDevice, 1 );
        pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	}
}

void DxStaticAniFrame::RenderMS_Opaque( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_( sParameter, bMPShading, bBlend );
		}
	}

	// Note : Lock ON
	m_bFrameMoveLock = TRUE;
}

void DxStaticAniFrame::RenderMS_Opaque_Animation( PieceParameter& sParameter, BOOL bMPShading, BOOL bReflect, float fHeight, BOOL bBlend, bool bAniLoop )
{
	if ( m_spStaticMaterialMeshAniMan.get() )
	{
		if ( bMPShading == FALSE )
		{
			if ( bReflect )
			{
				m_spStaticMaterialMeshFrameAni->UpdateMirrorMatrix( fHeight );
			}
			else
			{
				m_spStaticMaterialMeshFrameAni->UpdateMatrix( sParameter.m_fAniTime, *sParameter.m_rmatWorld, bAniLoop );
			}
		}
		m_spStaticMaterialMeshAniMan->Render_PIECE_( sParameter, bMPShading, bBlend );
	}

	// Note : Lock ON
	m_bFrameMoveLock = TRUE;
}

void DxStaticAniFrame::RenderMS_HardAlpha( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_Alpha( sParameter, bMPShading, bBlend );
		}
	}
}

void DxStaticAniFrame::RenderMS_HardAlpha_Animation( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend )
{
	if ( m_spStaticMaterialMeshAniMan.get() )
	{
		m_spStaticMaterialMeshAniMan->Render_PIECE_Alpha( sParameter, bMPShading, bBlend );
	}
}

void DxStaticAniFrame::RenderDecal( PieceParameter& sParameter )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_Decal( sParameter );
		}
	}

	if ( m_spStaticMaterialMeshAniMan.get() )
	{
		m_spStaticMaterialMeshAniMan->Render_PIECE_Decal( sParameter );
	}
}

void DxStaticAniFrame::RenderMS_SoftAlpha( PieceParameter& sParameter, BOOL bMPShading )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_SoftAlpha( sParameter, bMPShading );
		}
	}

	if ( m_spStaticMaterialMeshAniMan.get() )
	{
		m_spStaticMaterialMeshAniMan->Render_PIECE_SoftAlpha( sParameter, bMPShading );
	}
}

void DxStaticAniFrame::RenderMS_SoftAlphaForPSF( PieceParameter& sParameter )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_SoftAlphaForPSF( sParameter );
		}
	}

	if ( m_spStaticMaterialMeshAniMan.get() )
	{
		m_spStaticMaterialMeshAniMan->Render_PIECE_SoftAlphaForPSF( sParameter );
	}
}

void DxStaticAniFrame::RenderMS_DecalPass2( PieceParameter& sParameter )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_DecalPass2( sParameter );
		}	
	}

	if ( m_spStaticMaterialMeshAniMan.get() )
	{
		m_spStaticMaterialMeshAniMan->Render_PIECE_DecalPass2( sParameter );
	}
}

void DxStaticAniFrame::Render_PIECE_Custom_UNLIT_UNFOG( PieceParameter& sParameter )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_Custom_UNLIT_UNFOG( sParameter );
		}	
	}

	if ( m_spStaticMaterialMeshAniMan.get() )
	{
		m_spStaticMaterialMeshAniMan->Render_PIECE_Custom_UNLIT_UNFOG( sParameter );
	}
}

void DxStaticAniFrame::Render_PIECE_Water_CustomSoftAlpha( PieceParameter& sParameter )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_Water_CustomSoftAlpha( sParameter );
		}	
	}

	if ( m_spStaticMaterialMeshAniMan.get() )
	{
		m_spStaticMaterialMeshAniMan->Render_PIECE_Water_CustomSoftAlpha( sParameter );
	}
}

void DxStaticAniFrame::RenderOpaque_Instancing( const PieceParameter& sParameter )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_Opaque_Instancing( sParameter );
		}
	}

	// Note : Lock ON
	m_bFrameMoveLock = TRUE;
}

void DxStaticAniFrame::RenderHardAlpha_Instancing( const PieceParameter& sParameter )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_HardAlpha_Instancing( sParameter );
		}
	}

	// Note : Lock ON
	m_bFrameMoveLock = TRUE;
}

void DxStaticAniFrame::RenderDecal_Instancing( const PieceParameter& sParameter )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_Decal_Instancing( sParameter );
		}
	}

	// Note : Lock ON
	m_bFrameMoveLock = TRUE;
}

void DxStaticAniFrame::RenderDecalPass2_Instancing( const PieceParameter& sParameter )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_DecalPass2_Instancing( sParameter );
		}
	}

	// Note : Lock ON
	m_bFrameMoveLock = TRUE;
}

void DxStaticAniFrame::Render_CharShadow( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV )
{
	if( m_pStaticMesh )
	{
		m_pStaticMesh->Render_CharShadow( pd3dDevice, pCV );
	}

	// Note : Lock ON
	m_bFrameMoveLock = TRUE;
}

void DxStaticAniFrame::RenderCascadeShadowOpaque( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_CascadeShadowOpaque( pd3dDevice );
		}
	}

	// Note : Lock ON
	m_bFrameMoveLock = TRUE;
}

void DxStaticAniFrame::RenderCascadeShadowHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_CascadeShadowHardAlpha( pd3dDevice );
		}
	}
}

void DxStaticAniFrame::RenderCascadeShadowOpaque_Instancing( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_CascadeShadowOpaque_Instancing( pd3dDevice );
		}
	}

	// Note : Lock ON
	m_bFrameMoveLock = TRUE;
}

void DxStaticAniFrame::RenderCascadeShadowHardAlpha_Instancing( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_PIECE_CascadeShadowHardAlpha_Instancing( pd3dDevice );
		}
	}
}

void DxStaticAniFrame::RenderCascadeShadowOpaque_Animation( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_spStaticMaterialMeshAniMan.get() )
	{
		m_spStaticMaterialMeshAniMan->Render_PIECE_CascadeShadowOpaque( pd3dDevice );
	}
}

void DxStaticAniFrame::RenderCascadeShadowHardAlpha_Animation( LPDIRECT3DDEVICEQ pd3dDevice)
{
	if ( m_spStaticMaterialMeshAniMan.get() )
	{
		m_spStaticMaterialMeshAniMan->Render_PIECE_CascadeShadowHardAlpha( pd3dDevice );
	}
}

void DxStaticAniFrame::Render_EDIT( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld )
{
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	if( m_pFrameAniMan )
	{
		m_pFrameAniMan->Render_EDIT( pd3dDevice );
	}
}

void DxStaticAniFrame::Render_Query( PieceParameter& sParameter )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Render_Query( sParameter.m_pd3dDevice, NULL );
		}
	}

	// Note : Lock ON
	m_bFrameMoveLock = TRUE;
}

void DxStaticAniFrame::Save( sc::SerialFile& SFile )
{
	SFile << static_cast<DWORD>( VERSION );
	SFile.BeginBlock();

	DxStaticMesh* pStaticMesh = dynamic_cast<DxStaticMesh*>( m_pStaticMesh );
	if( pStaticMesh )
	{
		SFile << (BOOL)TRUE;
		pStaticMesh->Save( SFile, TRUE );
	}
	else
	{
		SFile << (BOOL)FALSE;
	}

	DxStaticMaterialMesh* pStaticMaterialMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
	if( pStaticMaterialMesh )
	{
		SFile << (BOOL)TRUE;
		pStaticMaterialMesh->Save( SFile, TRUE, NULL );
	}
	else
	{
		SFile << (BOOL)FALSE;
	}

	// m_spStaticMaterialMeshAniMan, m_spStaticMaterialMeshFrameAni - Save Load 순서가 중요하다.
	// Animation 관련 데이터
	if ( m_spStaticMaterialMeshAniMan.get() )
	{
		SFile << (BOOL)TRUE;
		m_spStaticMaterialMeshAniMan->Save( SFile, TRUE, NULL );
	}
	else
	{
		SFile << (BOOL)FALSE;
	}

	// Animation 관련 데이터
	if ( m_spStaticMaterialMeshFrameAni.get() )
	{
		SFile << (BOOL)TRUE;
		m_spStaticMaterialMeshFrameAni->Save( SFile );
	}
	else
	{
		SFile << (BOOL)FALSE;
	}

	// Note : Replace Effect
	{
		DxEffectBase* pEffCur;
		DWORD dwEffCount = 0;
		
		pEffCur = m_pEffectHead;
		while ( pEffCur )
		{
			dwEffCount++;
			pEffCur = pEffCur->pEffectNext;
		}

		SFile << dwEffCount;

		pEffCur = m_pEffectHead;
		while ( pEffCur )
		{
			pEffCur->SaveFile( SFile );
			pEffCur = pEffCur->pEffectNext;
		}
	}

	SFile.EndBlock();
}

void DxStaticAniFrame::Load_0100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	// Note : Static Mesh
	BOOL bExist;
	SFile >> bExist;
	if ( bExist )
	{
		DxStaticMesh* pStaticMesh = new DxStaticMesh;
		pStaticMesh->Load( pd3dDevice, SFile, FALSE, TRUE );
		m_pStaticMesh = pStaticMesh;
	}

	// Note : Animation Frame
	SFile >> bExist;
	if ( bExist )
	{
		m_pFrameAniMan = new DxFrameAniMan;
		m_pFrameAniMan->Load( pd3dDevice, SFile );
	}
}

void DxStaticAniFrame::Load_0101( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	// Note : Static Mesh
	BOOL bExist;
	SFile >> bExist;
	if ( bExist )
	{
		DxStaticMesh* pStaticMesh = new DxStaticMesh;
		pStaticMesh->Load( pd3dDevice, SFile, FALSE, TRUE );
		m_pStaticMesh = pStaticMesh;
	}

	// Note : Animation Frame
	SFile >> bExist;
	if ( bExist )
	{
		m_pFrameAniMan = new DxFrameAniMan;
		m_pFrameAniMan->Load( pd3dDevice, SFile );
	}

	// Note : Replace Effect
	{
		DWORD dwMaxCount = 0;
		SFile >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			HRESULT hr = S_OK;
			hr = EffectLoadToList( pd3dDevice, SFile, m_pEffectHead );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount++;
		}
	}
}

void DxStaticAniFrame::Load_0102( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	// Note : Static Mesh
	BOOL bExist;

	// Note : Animation Frame
	SFile >> bExist;
	if ( bExist )
	{
		m_pFrameAniMan = new DxFrameAniMan;
		m_pFrameAniMan->Load( pd3dDevice, SFile );
	}

	SFile >> bExist;
	if ( bExist )
	{
		DxStaticMesh* pStaticMesh = new DxStaticMesh;
		pStaticMesh->Load( pd3dDevice, SFile, FALSE, TRUE );
		m_pStaticMesh = pStaticMesh;
	}

	// Note : Replace Effect
	{
		DWORD dwMaxCount = 0;
		SFile >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			HRESULT hr = S_OK;
			hr = EffectLoadToList( pd3dDevice, SFile, m_pEffectHead );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount++;
		}
	}
}

void DxStaticAniFrame::Load_0103( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, boost::function<void (const TCHAR*)> pArchive )
{
	// Note : Static Mesh
	BOOL bExist;

	// Note : Animation Frame
	SFile >> bExist;
	if ( bExist )
	{
		m_pFrameAniMan = new DxFrameAniMan;
		m_pFrameAniMan->Load( pd3dDevice, SFile );
	}

	SFile >> bExist;
	if ( bExist )
	{
		DxStaticMesh* pStaticMesh = new DxStaticMesh;
		pStaticMesh->Load( pd3dDevice, SFile, FALSE, TRUE );
		m_pStaticMesh = pStaticMesh;
	}

	SFile >> bExist;
	if ( bExist )
	{
		DxStaticMaterialMesh* pStaticMesh = new DxStaticMaterialMesh;
		pStaticMesh->Load( pd3dDevice, SFile, FALSE, TRUE, pArchive );
		m_pStaticMesh = pStaticMesh;
	}

	// Note : Replace Effect
	{
		DWORD dwMaxCount = 0;
		SFile >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			HRESULT hr = S_OK;
			hr = EffectLoadToList( pd3dDevice, SFile, m_pEffectHead );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount++;
		}
	}
}

void DxStaticAniFrame::Load_0104( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, boost::function<void (const TCHAR*)> pArchive )
{
	// Note : Static Mesh
	BOOL bExist;

	SFile >> bExist;
	if ( bExist )
	{
		DxStaticMesh* pStaticMesh = new DxStaticMesh;
		pStaticMesh->Load( pd3dDevice, SFile, FALSE, TRUE );
		m_pStaticMesh = pStaticMesh;
	}

	SFile >> bExist;
	if ( bExist )
	{
		DxStaticMaterialMesh* pStaticMesh = new DxStaticMaterialMesh;
		pStaticMesh->Load( pd3dDevice, SFile, FALSE, TRUE, pArchive );
		m_pStaticMesh = pStaticMesh;
	}

	// m_spStaticMaterialMeshAniMan, m_spStaticMaterialMeshFrameAni - Save Load 순서가 중요하다.
	SFile >> bExist;
	if ( bExist )
	{
		m_spStaticMaterialMeshAniMan = std::tr1::shared_ptr<DxStaticMaterialMeshAniMan>(new DxStaticMaterialMeshAniMan);
		m_spStaticMaterialMeshAniMan->Load( pd3dDevice, SFile, TRUE, pArchive );
	}

	SFile >> bExist;
	if ( bExist )
	{
		//////////////////////////////////////////////////////////////////////////
		//		다른 곳에서 애니메이션 제어를 위한 vector. clear
		m_vecAnimationMatrixPointer.clear();



		//////////////////////////////////////////////////////////////////////////
		//						Loading
		m_spStaticMaterialMeshFrameAni = std::tr1::shared_ptr<DxStaticMaterialMeshFrameAni>(new DxStaticMaterialMeshFrameAni);
		m_spStaticMaterialMeshFrameAni->Load( SFile, m_spStaticMaterialMeshAniMan.get(), m_vecAnimationMatrixPointer );
	}

	// Note : Replace Effect
	{
		DWORD dwMaxCount = 0;
		SFile >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			HRESULT hr = S_OK;
			hr = EffectLoadToList( pd3dDevice, SFile, m_pEffectHead );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount++;
		}
	}
}

void DxStaticAniFrame::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, boost::function<void (const TCHAR*)> pArchive )
{
	CleanUp();		// 초기화

	DWORD dwVer;
	DWORD dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	switch ( dwVer )
	{
	case 0x0100:	Load_0100( pd3dDevice, SFile );	break;
	case 0x0101:	Load_0101( pd3dDevice, SFile );	break;
	case 0x0102:	Load_0102( pd3dDevice, SFile );	break;
	case 0x0103:	Load_0103( pd3dDevice, SFile, pArchive );	break;
	case VERSION:	Load_0104( pd3dDevice, SFile, pArchive );	break;
	default:
		{
			DWORD dwCur = SFile.GetfTell();
			SFile.SetOffSet( dwCur+dwBufferSize );
		}
		break;
	};
}

BOOL DxStaticAniFrame::IsLightMapPiece()
{
    if ( !m_pStaticMesh )
        return FALSE;

    return m_pStaticMesh->IsLightMapPiece();
}

void DxStaticAniFrame::SaveLightMapPiece( sc::SerialFile& SFile )
{
    SFile << static_cast<DWORD>( VERSION_LM );

    SFile.BeginBlock();
    {
		DxStaticMesh* pStaticMesh = dynamic_cast<DxStaticMesh*>( m_pStaticMesh );
		if( pStaticMesh )
		{
			SFile << (BOOL)TRUE;
			pStaticMesh->SaveLightMapPiece( SFile );
		}
		else
		{
			SFile << (BOOL)FALSE;
		}

		DxStaticMaterialMesh* pStaticMaterialMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if( pStaticMaterialMesh )
		{
			SFile << (BOOL)TRUE;
			pStaticMaterialMesh->SaveLightMapPiece( SFile );
		}
		else
		{
			SFile << (BOOL)FALSE;
		}
    }
    SFile.EndBlock();
}

void DxStaticAniFrame::LoadLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
    DWORD dwVer, dwBufferSize;
    SFile >> dwVer;
    SFile >> dwBufferSize;

	if ( dwVer == 0x0101 )
	{
		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			m_pStaticMesh = new DxStaticMesh;
			m_pStaticMesh->LoadLightMapPiece( pd3dDevice, SFile );
		}

		SFile >> bExist;
		if ( bExist )
		{
			m_pStaticMesh = new DxStaticMaterialMesh;
			m_pStaticMesh->LoadLightMapPiece( pd3dDevice, SFile );
		}
	}
	else if ( dwVer == 0x0100 )
	{
		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			SAFE_DELETE( m_pStaticMesh );
			m_pStaticMesh = new DxStaticMesh;
			m_pStaticMesh->LoadLightMapPiece( pd3dDevice, SFile );
		}
	}
}

void DxStaticAniFrame::LoadBasicAABB( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pStaticMesh )
		return;

	DxStaticMaterialMesh* pDynamic = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
	if ( !pDynamic )
		return;

	pDynamic->BaseLoadPiece( pd3dDevice );
}

void DxStaticAniFrame::FlipLightMapRenderMan( DxStaticAniFrame* pStaticAniFrame )
{
	if ( !m_pStaticMesh )
		return;

    m_pStaticMesh->FlipLightMapRenderMan( pStaticAniFrame->m_pStaticMesh );
}

void DxStaticAniFrame::CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pStaticMesh )
		return;

    m_pStaticMesh->CreateAlphaTexture32Bit( pd3dDevice );
}

void DxStaticAniFrame::Export( LPDIRECT3DDEVICEQ pd3dDevice, 
							  DxFrameMesh* pFrameMesh, 
							  LIGHTMAPUV_MAIN* pLightMapUV_MAIN, 
							  const DxTextureEffMan* pTextureEffMan, 
							  const CMaterialSetting* pMaterialSetting, 
							  BOOL bRan_1_0,
							  float fCellSize )
{
	//	Note : Optimum Octree..
	DxStaticMesh* pStaticMesh = new DxStaticMesh;
	pStaticMesh->MakeAABBOCTree( pd3dDevice, pFrameMesh, NULL, pLightMapUV_MAIN, FALSE, TRUE, FALSE, _T(""), pTextureEffMan, pMaterialSetting, bRan_1_0, fCellSize );	// Modify

	// DxStaticMaterialMesh 로 변경
	SAFE_DELETE( m_pStaticMesh );
	if ( bRan_1_0 )
	{
		m_pStaticMesh = pStaticMesh;
	}
	else
	{
		DxStaticMaterialMesh* pStaticMaterialMesh = new DxStaticMaterialMesh;
		pStaticMaterialMesh->Import( pd3dDevice, pStaticMesh, fCellSize );
		SAFE_DELETE( pStaticMesh );

		m_pStaticMesh = pStaticMaterialMesh;
	}



	//////////////////////////////////////////////////////////////////////////
	//		다른 곳에서 애니메이션 제어를 위한 vector. clear
	m_vecAnimationMatrixPointer.clear();



	//////////////////////////////////////////////////////////////////////////
	//						본격적으로 시작
	m_spStaticMaterialMeshAniMan = std::tr1::shared_ptr<DxStaticMaterialMeshAniMan>(new DxStaticMaterialMeshAniMan);
	m_spStaticMaterialMeshFrameAni = std::tr1::shared_ptr<DxStaticMaterialMeshFrameAni>(new DxStaticMaterialMeshFrameAni);
	m_spStaticMaterialMeshFrameAni->Import( pd3dDevice, 
											pFrameMesh->GetFrameRoot(), 
											pFrameMesh->GetUnitTime(), 
											m_spStaticMaterialMeshAniMan.get(), 
											pFrameMesh->GetMaterialSetting_const(),
											m_vecAnimationMatrixPointer,
											fCellSize );

	// Animation 되는 물체가 있는지 없는지 확인 후 없다면 m_spStaticMaterialMeshAniMan, m_spStaticMaterialMeshFrameAni를 delete 한다.
	if ( !m_spStaticMaterialMeshAniMan->IsExist() )
	{
		m_spStaticMaterialMeshAniMan.reset();
		m_spStaticMaterialMeshFrameAni.reset();
	}

	//SAFE_DELETE( m_pFrameAniMan );
	//m_pFrameAniMan = new DxFrameAniMan;
	//m_pFrameAniMan->MakeAniMan( pd3dDevice, pFrameMesh );

	// Note : Replace ^^
	DxEffectBase* pEffCur = pFrameMesh->GetEffectList();
	while ( pEffCur )
	{
		if( !pEffCur->GetEachRender() )
		{
			DWORD TypeID = pEffCur->GetTypeID();
			char* szFrameName = pEffCur->GetAdaptFrameName();

			PBYTE pProperty;
			DWORD dwSize;
			DWORD dwVer;
			pEffCur->GetProperty ( pProperty, dwSize, dwVer );
			LPDXAFFINEPARTS pAffineParts = pEffCur->m_pAffineParts;

			DxFrame* pFrame = pFrameMesh->GetFrameRoot();
			EffectAdaptToList( pFrame, TypeID, szFrameName, pd3dDevice, pProperty, dwSize, dwVer, pAffineParts, pEffCur, pMaterialSetting );
		}

		pEffCur = pEffCur->pEffectNext;
	}
}

HRESULT DxStaticAniFrame::EffectAdaptToList( DxFrame *pFrameRoot, DWORD TypeID, char* szFrame, LPDIRECT3DDEVICEQ pd3dDevice,
									PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts, DxEffectBase* pEffectSrc, const CMaterialSetting* pMaterialSetting )
{
	HRESULT hr;

	DxFrame *pDxFrame = pFrameRoot->FindFrame( szFrame );

	DxEffectBase *pEffect = DxPieceEffMan::GetInstance().CreateEffInstance( TypeID );
	if ( !pEffect ) return NULL;

	hr = pEffect->AdaptToEffList( pDxFrame, pd3dDevice );

	if ( hr == S_FALSE )
	{
		char szMsg[256] = "";
		StringCchPrintf( szMsg, 256, "단독 효과(%s)의 Adapt 메소드가 존재하지 않습니다.", pEffect->GetName() );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
		return E_FAIL;
	}

	if ( FAILED(hr) )
	{
		char szMsg[256] = "";
		StringCchPrintf( szMsg, 256, "단독 효과(%s)의 Adapt 도중 오류를 발생하였습니다.", pEffect->GetName() );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
		return E_FAIL;
	}

	//	Note : Property, Affine  값을 대입.
	if ( pProperty )	pEffect->SetProperty ( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffect->IsUseAffineMatrix() )		pEffect->SetAffineValue ( pAffineParts );

	//	Note : 대량의 버퍼를 복제하기 위한 메소드.
	if ( pEffectSrc )	pEffect->CloneDataMAT( pEffectSrc, pd3dDevice, pMaterialSetting );

	hr = pEffect->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffect);
		return E_FAIL;
	}

	m_dwEffectCount++;
	pEffect->pEffectNext = m_pEffectHead;
	m_pEffectHead = pEffect;

	return S_OK;
}

HRESULT DxStaticAniFrame::EffectLoadToList( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile, PDXEFFECTBASE &pEffList )
{
	BOOL	bResult = FALSE;
	HRESULT	hr = E_FAIL;
	char	*szName = NULL;
	int		StrLength;

	SFile >> bResult;
	if ( bResult )
	{
		SFile >> StrLength;
		szName = new char [ StrLength ];
		SFile.ReadBuffer ( szName, StrLength );
	}

    DWORD	TypeID;
	SFile >> TypeID;
	
	DWORD	dwSize = 0;
	BYTE	*pProperty = NULL;
	DWORD	dwVer = 0;

	SFile >> dwVer;
	SFile >> dwSize;
	if ( dwSize )
	{
		pProperty = new BYTE [ dwSize ];
		SFile.ReadBuffer ( pProperty, sizeof ( BYTE ) * dwSize );
	}

	LPDXAFFINEPARTS	pAffineParts = NULL;
	SFile >> bResult;
	if ( bResult )
	{
		pAffineParts = new DXAFFINEPARTS;
		SFile.ReadBuffer ( pAffineParts, sizeof ( DXAFFINEPARTS ) );
	}

	DxEffectBase *pEffect = DxPieceEffMan::GetInstance().CreateEffInstance ( TypeID );
	if ( !pEffect ) return E_FAIL;

	//	Note : Property, Affine  값을 대입.
	//
	if ( pProperty )	pEffect->SetProperty ( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffect->IsUseAffineMatrix() )	pEffect->SetAffineValue ( pAffineParts );

	//	Note : LoadBuffer(), 특정 Effect 에서만 사용되는 메모리 버퍼를 읽는다.
	//
	pEffect->LoadBuffer ( SFile, dwVer, pd3dDevice );

	//	Note : Create Device.
	//
	hr = pEffect->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffect);
		return E_FAIL;
	}

    SAFE_DELETE_ARRAY ( pProperty );
	SAFE_DELETE ( pAffineParts );
	SAFE_DELETE_ARRAY ( szName );

	//	Note : Insert To List.
	//
	pEffect->pEffectNext = pEffList;
	pEffList = pEffect;

	// Note : 값 뒤집기	< List 순서를 유지하기 위해서 >
	DxEffectBase* pTemp = NULL;
	DxEffectBase* pCur	= pEffList;
	DxEffectBase* pThis = pEffList;
	while ( pCur )
	{
		pThis = pCur;
		pCur = pCur->pEffectNext;

		pThis->pEffectNext = pTemp;
		pTemp = pThis;
	}
	pEffList = pTemp;

	return S_OK;
}

void DxStaticAniFrame::GetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3&  vMin )
{
	if( !m_pStaticMesh )	return;

	vMax = m_pStaticMesh->GetAABBMax();
	vMin = m_pStaticMesh->GetAABBMin();

	if ( m_spStaticMaterialMeshFrameAni.get() )
	{
		m_spStaticMaterialMeshFrameAni->GetAABBBox( vMax, vMin );
	}
}

BOOL DxStaticAniFrame::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3* pNor, D3DXVECTOR3& vCollision, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const
{
	if( !m_pStaticMesh )	return FALSE;

	const char* szName = NULL;
	if ( pNor )
	{
		return m_pStaticMesh->IsCollisionLine( vStart, vEnd, vCollision, *pNor, szName, emCullMode, bOnlyCamColl );
	}
	else
	{
		D3DXVECTOR3 vNewNormal(0.f,1.f,0.f);
		return m_pStaticMesh->IsCollisionLine( vStart, vEnd, vCollision, vNewNormal, szName, emCullMode, bOnlyCamColl );
	}
}

BOOL DxStaticAniFrame::IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const
{
	if( !m_pStaticMesh )	
		return FALSE;

	return m_pStaticMesh->IsCollisionLineSimple( vStart, vEnd, emCullMode );
}

// Note : 충돌이 되면 바로 리턴한다.
BOOL DxStaticAniFrame::IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV )
{
	if( !m_pStaticMesh )	
        return FALSE;

	return m_pStaticMesh->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, emCullMode, nIndexUV );
}

BOOL DxStaticAniFrame::GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor, const NSTexture16Bit::DATA* pDataDay )
{
	if( !m_pStaticMesh )	
		return FALSE;

	DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
	if ( pStaticMesh )
	{
		return pStaticMesh->GetLightMapColor( pd3dDevice, vStart, vEnd, dwColor, pDataDay );
	}

	return FALSE;
}

void DxStaticAniFrame::GetClosedPoint( D3DXVECTOR3& vCollisionInOut )
{
    if ( !m_pStaticMesh )	
		return;

	m_pStaticMesh->GetClosedPoint( vCollisionInOut );
}

void DxStaticAniFrame::ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld )
{
	if ( !m_pStaticMesh )	
		return;

	m_pStaticMesh->ExportXFile( pxofsave, pDataObjectRoot, strPieceName, matWorld );
}

BOOL DxStaticAniFrame::IsMaterialMesh() const
{
	if ( !m_pStaticMesh )	
		return FALSE;

	DxStaticMaterialMesh* pStaticMaterialMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
	if( pStaticMaterialMesh )
		return TRUE;

	return FALSE;
}

BOOL DxStaticAniFrame::ChangeDxStaticMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice, float fCellSize )
{
	// 움직이는 물체일 수가 있다.
	if ( !m_pStaticMesh )	
		return TRUE;

	DxStaticMesh* pStaticMesh = GetStaticMesh();
	if ( !pStaticMesh )
		return FALSE;

	DxStaticMaterialMesh* pStaticMaterialMesh = new DxStaticMaterialMesh;
	if( !pStaticMaterialMesh->ChangeDxStaticMaterialMesh( pd3dDevice, pStaticMesh, fCellSize ) )
	{
		return FALSE;
	}

	SAFE_DELETE ( m_pStaticMesh );
	m_pStaticMesh = pStaticMaterialMesh;
	return TRUE;
}

// Piece의 TriangleLinkLight 를 생성하도록 한다.
BOOL DxStaticAniFrame::CreateTriangleLinkLightPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
												   DxStaticMeshCollBase* pStaticMeshColl, 
												   DxPieceManager* pPieceManager, 
												   std::vector<DWORD>& vecLightID, 
												   const D3DXMATRIX& matWorld,
												   const D3DLIGHTQ& sLight,
												   float fAttEndLineValue ) const
{
	if( !m_pStaticMesh )
		return FALSE;

	return m_pStaticMesh->CreateTriangleLinkLightForPiece
	( 
		pd3dDevice, 
		pStaticMeshColl, 
		pPieceManager, 
		vecLightID, 
		matWorld,
		sLight,
		fAttEndLineValue
	);
}



//////////////////////////////////////////////////////////////////////////
//	Animation 을 Pointer 로 가지고 있어서 다른곳에서 제어할 수 있도록 함.
const std::vector<D3DXMATRIX*>& DxStaticAniFrame::GetAnimationMatrixPointer() const
{
	return m_vecAnimationMatrixPointer;
}

void DxStaticAniFrame::SetAnimationMatrix( const std::vector<D3DXMATRIX>& vecAnimationMatrix )
{
	DWORD dwSize = m_vecAnimationMatrixPointer.size();
	if ( vecAnimationMatrix.size() != dwSize )
		return;

	for ( DWORD i=0; i<dwSize; ++i )
	{
		*m_vecAnimationMatrixPointer[i] = vecAnimationMatrix[i];
	}
}

//////////////////////////////////////////////////////////////////////////
// Material 을 구한다.
void DxStaticAniFrame::Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool )
{
	if( m_pStaticMesh )
	{
		DxStaticMaterialMesh* pStaticMesh = dynamic_cast<DxStaticMaterialMesh*>( m_pStaticMesh );
		if ( pStaticMesh )
		{
			pStaticMesh->Active_MATERIAL_DATA( pd3dDevice, bTool );
		}
	}

	if ( m_spStaticMaterialMeshAniMan.get() )
	{
		m_spStaticMaterialMeshAniMan->Active_MATERIAL_DATA( pd3dDevice, bTool );
	}
}