#include "pch.h"

#include "../DxTools/DxRenderStates.h"
#include "../DxTools/RENDERPARAM.h"
#include "./DxPiece/DxPieceAABB.h"
#include "./DxLandMan.h"

#include "EntityRenderList.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



//////////////////////////////////////////////////////////////////////////
//
EntityRenderList::EntityRenderList()
	:m_bWLD(TRUE)
{
}

EntityRenderList::~EntityRenderList()
{
}

void EntityRenderList::ClearPieceList()
{
	m_vecEntity.clear();
	m_vecEntityExceptInstancing.clear();
	m_vecEntityBlend.clear();
	m_vecEntityPoint.clear();
}

void EntityRenderList::ClearLandSkinList()
{
	m_vecLandSkin.clear();
	m_vecLandSkinBlend.clear();
	m_vecLandSkinPoint.clear();
}

void EntityRenderList::SetData_Force_TnL_PS_3_0_DF()
{
	for( DWORD i=0; i<m_vecEntityExceptInstancing.size(); ++i )
	{
		m_vecEntityExceptInstancing[i]->SetData_Force_TnL_PS_3_0_DF();
	}
	for( DWORD i=0; i<m_vecEntityBlend.size(); ++i )
	{
		m_vecEntityBlend[i].m_pEntity->SetData_Force_TnL_PS_3_0_DF();
	}
	for( DWORD i=0; i<m_vecLandSkinPoint.size(); ++i )
	{
		m_vecLandSkinPoint[i].m_pEntity->SetData_Force_TnL_PS_3_0_DF();
	}
}

void EntityRenderList::FrameMoveMAT( const float fTime, const float fElapsedTime )
{
	for( DWORD i=0; i<m_vecEntityExceptInstancing.size(); ++i )
	{
		m_vecEntityExceptInstancing[i]->FrameMoveMAT_OnlyStaticAniFrame( fElapsedTime );
	}
	for( DWORD i=0; i<m_vecEntity.size(); ++i )
	{
		m_vecEntity[i].m_pEntity->FrameMoveMAT_OnlyPieceEff( fTime, fElapsedTime );
	}
	for( DWORD i=0; i<m_vecEntityBlend.size(); ++i )
	{
		m_vecEntityBlend[i].m_pEntity->FrameMoveMAT( fTime, fElapsedTime );
	}
	for( DWORD i=0; i<m_vecLandSkinPoint.size(); ++i )
	{
		m_vecLandSkinPoint[i].m_pEntity->FrameMoveMAT( fTime, fElapsedTime );
	}
}

void EntityRenderList::FrameMoveLandSkin( float fElapsedTime )
{
	for( DWORD i=0; i<m_vecLandSkin.size(); ++i )
	{
		m_vecLandSkin[i].m_pEntity->FrameMoveEntity( fElapsedTime );
	}
	for( DWORD i=0; i<m_vecLandSkinBlend.size(); ++i )
	{
		m_vecLandSkinBlend[i].m_pEntity->FrameMoveEntity( fElapsedTime );
	}
	for( DWORD i=0; i<m_vecLandSkinPoint.size(); ++i )
	{
		m_vecLandSkinPoint[i].m_pEntity->FrameMoveEntity( fElapsedTime );
	}
}

//////////////////////////////////////////////////////////////////////////
// Render
void EntityRenderList::RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWLD, BOOL bMPShading, BOOL bUsedMaterialSystem, BOOL bReflect, float fReflectHeight )
{
	DWORD dwFVF_SAVE(0L);
	pd3dDevice->GetFVF( &dwFVF_SAVE );
	pd3dDevice->SetFVF( 0 );

	if ( bUsedMaterialSystem )
	{
		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 )
		{
			if ( bMPShading )
			{
				// MPShading 시 Instancing 이 사용되지 않는다.
				for( DWORD i=0; i<m_vecEntity.size(); ++i )
				{
					m_vecEntity[i].m_pEntity->RenderMS_Opaque( bMPShading, bReflect, fReflectHeight, FALSE );
				}

				// Animation 은 Instancing 에서 렌더링 되지 않는다.
				for( DWORD i=0; i<m_vecEntity.size(); ++i )
				{
					m_vecEntity[i].m_pEntity->RenderMS_Opaque_Animation( bMPShading, bReflect, fReflectHeight, FALSE );
				}
			}
			else
			{
				// Instancing 에서 일부 렌더링 될 수 있다.
				for( DWORD i=0; i<m_vecEntityExceptInstancing.size(); ++i )
				{
					m_vecEntityExceptInstancing[i]->RenderMS_Opaque( bMPShading, bReflect, fReflectHeight, FALSE );
				}

				// Animation 은 Instancing 에서 렌더링 되지 않는다.
				for( DWORD i=0; i<m_vecEntity.size(); ++i )
				{
					m_vecEntity[i].m_pEntity->RenderMS_Opaque_Animation( bMPShading, bReflect, fReflectHeight, FALSE );
				}
			}
		}
		else
		{
			for( DWORD i=0; i<m_vecEntity.size(); ++i )
			{
				m_vecEntity[i].m_pEntity->RenderMS_Fixed();
			}
		}
	}
	else
	{
		for( DWORD i=0; i<m_vecEntity.size(); ++i )
		{
			m_vecEntity[i].m_pEntity->Render( pd3dDevice, !bWLD );
		}
	}

	pd3dDevice->SetFVF( dwFVF_SAVE );
}

void EntityRenderList::RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWLD, BOOL bMPShading, BOOL bUsedMaterialSystem )
{
	DWORD dwFVF_SAVE(0L);
	pd3dDevice->GetFVF( &dwFVF_SAVE );
	pd3dDevice->SetFVF( 0 );

	//DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );

	if ( bUsedMaterialSystem )
	{
		if ( bMPShading )
		{
			// MPShading 시 Instancing 이 사용되지 않는다.
			for( DWORD i=0; i<m_vecEntity.size(); ++i )
			{
				m_vecEntity[i].m_pEntity->RenderMS_HardAlpha( bMPShading, FALSE );
			}

			// Animation 은 Instancing 에서 렌더링 되지 않는다.
			for( DWORD i=0; i<m_vecEntity.size(); ++i )
			{
				m_vecEntity[i].m_pEntity->RenderMS_HardAlpha_Animation( bMPShading, FALSE );
			}
		}
		else
		{
			// Instancing 에서 일부 렌더링 될 수 있다.
			for( DWORD i=0; i<m_vecEntityExceptInstancing.size(); ++i )
			{
				m_vecEntityExceptInstancing[i]->RenderMS_HardAlpha( bMPShading, FALSE );
			}

			// Animation 은 Instancing 에서 렌더링 되지 않는다.
			for( DWORD i=0; i<m_vecEntity.size(); ++i )
			{
				m_vecEntity[i].m_pEntity->RenderMS_HardAlpha_Animation( bMPShading, FALSE );
			}
		}
	}
	else
	{
		for( DWORD i=0; i<m_vecEntity.size(); ++i )
		{
			m_vecEntity[i].m_pEntity->RenderHardAlpha( pd3dDevice, !bWLD );
		}
	}

	//DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );

	pd3dDevice->SetFVF( dwFVF_SAVE );
}

void EntityRenderList::RenderEffFrame()
{
	for( DWORD i=0; i<m_vecEntity.size(); ++i )
	{
		m_vecEntity[i].m_pEntity->RenderEffFrame();
	}
}

void EntityRenderList::RenderEffFrameForDeffered( BOOL bLBuffer, BOOL bReflect )
{
	for( DWORD i=0; i<m_vecEntity.size(); ++i )
	{
		m_vecEntity[i].m_pEntity->RenderEffFrameForDeffered( bLBuffer, bReflect );
	}
}

void EntityRenderList::RenderDecal()
{
	for( DWORD i=0; i<m_vecEntity.size(); ++i )
	{
		m_vecEntity[i].m_pEntity->RenderDecal();
	}
}

void EntityRenderList::RenderDecalAndLodBlendDecal( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( DWORD i=0; i<m_vecEntityExceptInstancing.size(); ++i )
	{
		m_vecEntityExceptInstancing[i]->RenderDecal();
	}

	// Note : SetRenderState
	DWORD dwAlphaTestEnable;
	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );

	// Note : SetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );

	Entity* pPiece(NULL);
	for( int i=(int)(m_vecEntityPoint.size())-1; i>=0; --i )
	{
		pPiece = m_vecEntityPoint[i].m_pEntity;

		pPiece->RenderDecal();
	}

	// Note : ReSetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	dwAlphaTestEnable );
}

void EntityRenderList::RenderDecalPass2()
{
	for( DWORD i=0; i<m_vecEntityExceptInstancing.size(); ++i )
	{
		m_vecEntityExceptInstancing[i]->RenderMS_DecalPass2();
	}
}

void EntityRenderList::RenderCustom_UNLIT_UNFOG()
{
	for( DWORD i=0; i<m_vecEntity.size(); ++i )
	{
		m_vecEntity[i].m_pEntity->Render_PIECE_Custom_UNLIT_UNFOG();
	}
}

void EntityRenderList::RenderWaterCustomSoftAlpha()
{
	for( DWORD i=0; i<m_vecEntity.size(); ++i )
	{
		m_vecEntity[i].m_pEntity->Render_PIECE_Water_CustomSoftAlpha();
	}
}

void EntityRenderList::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWLD, BOOL bMPShading, BOOL bUsedMaterialSystem )
{
	DWORD dwFVF_SAVE(0L);
	pd3dDevice->GetFVF( &dwFVF_SAVE );
	pd3dDevice->SetFVF( 0 );

	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );

	if ( bUsedMaterialSystem )
	{
		if ( bWLD )
		{
			for( DWORD i=0; i<m_vecEntity.size(); ++i )
			{
				m_vecEntity[i].m_pEntity->RenderMS_SoftAlpha( bMPShading );
			}
		}
		else
		{
			//for( DWORD i=0; i<m_vecEntity.size(); ++i )
			for( int i=(int)(m_vecEntity.size())-1; i>=0; --i )
			{
				m_vecEntity[i].m_pEntity->RenderMS_SoftAlphaForPSF();
			}
		}
	}
	else
	{
		for( DWORD i=0; i<m_vecEntity.size(); ++i )
		{
			m_vecEntity[i].m_pEntity->RenderSoftAlpha( pd3dDevice, !bWLD );
		}
	}

	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );

	pd3dDevice->SetFVF( dwFVF_SAVE );
}




//////////////////////////////////////////////////////////////////////////
// TnL_PS_2_0 이상에서만 호출 된다.
void EntityRenderList::RenderSoftAlpha_Deffered( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Base Rendering
	for( DWORD i=0; i<m_vecEntity.size(); ++i )
	{
		m_vecEntity[i].m_pEntity->RenderMS_SoftAlphaForDeffered();
	}

	// LOD
	Entity* pPiece(NULL);
	for( int i=(int)(m_vecEntityBlend.size())-1; i>=0; --i )
	{
		pPiece = m_vecEntityBlend[i].m_pEntity;

		pPiece->RenderMS_Opaque( FALSE, FALSE, 0.f, TRUE );
		pPiece->RenderMS_Opaque_Animation( FALSE, FALSE, 0.f, TRUE );
		pPiece->RenderMS_HardAlpha( FALSE, TRUE );
		pPiece->RenderMS_HardAlpha_Animation( FALSE, TRUE );
		pPiece->RenderMS_SoftAlpha( FALSE );
	}
}





//////////////////////////////////////////////////////////////////////////
//
void EntityRenderList::RenderSkinObject( BOOL bReflect, float fReflectHeight )
{
	for( DWORD i=0; i<m_vecLandSkin.size(); ++i )
	{
		m_vecLandSkin[i].m_pEntity->RenderSkinObject( bReflect, fReflectHeight );
	}
	for( DWORD i=0; i<m_vecLandSkinBlend.size(); ++i )
	{
		m_vecLandSkinBlend[i].m_pEntity->RenderSkinObject( bReflect, fReflectHeight );
	}
	for( DWORD i=0; i<m_vecLandSkinPoint.size(); ++i )
	{
		m_vecLandSkinPoint[i].m_pEntity->RenderSkinObject( bReflect, fReflectHeight );
	}
}

void EntityRenderList::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bWLD, BOOL bUsedMaterialSystem )
{
	for( int i=(int)(m_vecEntity.size())-1; i>=0; --i )
	{
		m_vecEntity[i].m_pEntity->RenderEff( pd3dDevice, pStaticMeshLightMap, pFrameMesh, !bWLD );
	}
}



//////////////////////////////////////////////////////////////////////////
// TnL_PS_2_0 이상에서만 호출 된다.
// PickBlend 되는 물체의 1RenderTarget 에 렌더 할 수 있는 것들 Rendering 하기.
void EntityRenderList::RenderEff_PickBlend_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap )
{
	// Base Rendering
	Entity* pPiece(NULL);
	for( int i=(int)(m_vecEntityPoint.size())-1; i>=0; --i )
	{
		pPiece = m_vecEntityPoint[i].m_pEntity;
		pPiece->RenderMS_DecalPass2();
		pPiece->Render_PIECE_Custom_UNLIT_UNFOG();
	}

	for( int i=(int)(m_vecEntityPoint.size())-1; i>=0; --i )
	{
		pPiece = m_vecEntityPoint[i].m_pEntity;
		pPiece->Render_PIECE_Water_CustomSoftAlpha();
		pPiece->RenderEff( pd3dDevice, pStaticMeshLightMap, NULL, FALSE );
	}

	// LOD
	for( int i=(int)(m_vecEntityBlend.size())-1; i>=0; --i )
	{
		pPiece = m_vecEntityBlend[i].m_pEntity;

		pPiece->RenderDecal();
		pPiece->RenderMS_DecalPass2();
		pPiece->Render_PIECE_Custom_UNLIT_UNFOG();
	}

	for( int i=(int)(m_vecEntityBlend.size())-1; i>=0; --i )
	{
		pPiece = m_vecEntityBlend[i].m_pEntity;

		pPiece->Render_PIECE_Water_CustomSoftAlpha();
		pPiece->RenderEff( pd3dDevice, pStaticMeshLightMap, NULL, FALSE );
	}
}



void EntityRenderList::FrameMoveONLY( const float fTime, const float fElapsedTime )
{
	for( DWORD i=0; i<m_vecEntityBlend.size(); ++i )
	{
		m_vecEntityBlend[i].m_pEntity->FrameMoveTOOL( fTime, fElapsedTime );
	}

	for( DWORD i=0; i<m_vecEntityPoint.size(); ++i )
	{
		m_vecEntityPoint[i].m_pEntity->FrameMoveTOOL( fTime, fElapsedTime );
	}
}



//////////////////////////////////////////////////////////////////////////
//					불투명 물체의 LOD 처리
void EntityRenderList::RenderForLodBlendForDeffered( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflect, float fReflectHeight )
{
	// Note : SetRenderState
	DWORD dwAlphaTestEnable;
	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );

	// Note : SetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );

	Entity* pPiece(NULL);
	for( int i=(int)(m_vecEntityPoint.size())-1; i>=0; --i )
	{
		pPiece = m_vecEntityPoint[i].m_pEntity;

		pPiece->RenderMS_Opaque( FALSE, bReflect, fReflectHeight, FALSE );
		pPiece->RenderMS_Opaque_Animation( FALSE, bReflect, fReflectHeight, FALSE );
		pPiece->RenderMS_HardAlpha( FALSE, FALSE );
		pPiece->RenderMS_HardAlpha_Animation( FALSE, FALSE );
		pPiece->RenderEffFrameForDeffered( FALSE, bReflect );
	}

	// Note : ReSetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	dwAlphaTestEnable );
}

void EntityRenderList::RenderForLodBlend_PSF( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : SetRenderState
	DWORD dwAlphaTestEnable;
	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );

	// Note : SetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );

	Entity* pPiece(NULL);
	for( int i=(int)(m_vecEntityPoint.size())-1; i>=0; --i )
	{
		pPiece = m_vecEntityPoint[i].m_pEntity;

		pPiece->RenderMS_Opaque( FALSE, FALSE, 0.f, FALSE );
		pPiece->RenderMS_Opaque_Animation( FALSE, FALSE, 0.f, FALSE );
		pPiece->RenderMS_HardAlpha( FALSE, FALSE );
		pPiece->RenderMS_HardAlpha_Animation( FALSE, FALSE );
	}

	// Note : ReSetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	dwAlphaTestEnable );
}



//////////////////////////////////////////////////////////////////////////
//					SoftAlpha 의 LOD 처리
void EntityRenderList::RenderSoftAlphaForLodBlendForDeffered( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : SetRenderState
	DWORD dwAlphaTestEnable;
	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );

	// Note : SetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );

	Entity* pPiece(NULL);
	for( int i=(int)(m_vecEntityPoint.size())-1; i>=0; --i )
	{
		pPiece = m_vecEntityPoint[i].m_pEntity;

		pPiece->RenderMS_SoftAlphaForDeffered();
	}

	// Note : ReSetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	dwAlphaTestEnable );
}

void EntityRenderList::RenderSoftAlphaForLodBlend_PSF( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : SetRenderState
	DWORD dwAlphaTestEnable;
	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );

	// Note : SetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );

	Entity* pPiece(NULL);
	for( int i=(int)(m_vecEntityPoint.size())-1; i>=0; --i )
	{
		pPiece = m_vecEntityPoint[i].m_pEntity;

		pPiece->RenderMS_SoftAlphaForPSF();
	}

	// Note : ReSetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	dwAlphaTestEnable );
}



//////////////////////////////////////////////////////////////////////////
// TnL_PS_1_1 이하에서만 호출 된다.
void EntityRenderList::RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
										 DxStaticMeshLightMap* pStaticMeshLightMap, 
										 DxFrameMesh* pFrameMesh, 
										 BOOL bPSF, 
										 BOOL bReflect,
										 float fReflectHeight, 
										 BOOL bFrameMove )
{
	// Note : SetRenderState
	DWORD dwAlphaBlendEnable, dwAlphaREF;
	pd3dDevice->GetRenderState( D3DRS_ALPHAREF,			&dwAlphaREF );
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );

	// Note : SetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

	// Note : SetTextureStageState
	DWORD dwAlphaOP, dwAlphaARG2;
	pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,	&dwAlphaOP );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG2,	&dwAlphaARG2 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );

	{
		Entity* pPiece(NULL);
		for( int i=(int)(m_vecEntityBlend.size())-1; i>=0; --i )
		{
			pPiece = m_vecEntityBlend[i].m_pEntity;

			DWORD dwColor = static_cast<DWORD>(pPiece->GetAlpha() * 255.f);
			if ( dwColor == 0 )
				continue;

			dwColor = dwColor << 24;
			dwColor += 0xffffff;
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwColor );

			if ( pPiece->IsMaterialMesh() )
			{
				pPiece->RenderMS_Opaque( FALSE, FALSE, 0.f, FALSE );
				pPiece->RenderMS_Opaque_Animation( FALSE, FALSE, 0.f, FALSE );
				pPiece->RenderMS_HardAlpha( FALSE, FALSE );
				pPiece->RenderMS_HardAlpha_Animation( FALSE, FALSE );
				pPiece->RenderEffFrame();
				pPiece->RenderSkinObject( bReflect, fReflectHeight );
				pPiece->RenderMS_DecalPass2();
				pPiece->Render_PIECE_Custom_UNLIT_UNFOG();
				pPiece->Render_PIECE_Water_CustomSoftAlpha();
				pPiece->RenderMS_SoftAlpha( FALSE );
				pPiece->RenderEff( pd3dDevice, pStaticMeshLightMap, pFrameMesh, bPSF );					
			}
			else
			{
				pPiece->Render( pd3dDevice, bPSF );
				pPiece->RenderHardAlpha( pd3dDevice, bPSF );
				pPiece->RenderEffFrame();
				pPiece->RenderSkinObject( bReflect, fReflectHeight );
				pPiece->RenderSoftAlpha( pd3dDevice, bPSF );
				pPiece->RenderEff( pd3dDevice, pStaticMeshLightMap, pFrameMesh, bPSF );
			}
		}
	}

	// Note : ReSetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			dwAlphaREF );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );

	// Note : ReSetTextureStageState
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	dwAlphaOP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,	dwAlphaARG2 );
}

//////////////////////////////////////////////////////////////////////////
// TnL_PS_3_0_CSM 이상
void EntityRenderList::RenderCacadeOpaque( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( DWORD i=0; i<m_vecEntityExceptInstancing.size(); ++i )
	{
		m_vecEntityExceptInstancing[i]->RenderCascadeShadowOpaque();
	}
}

void EntityRenderList::RenderCascadeHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( DWORD i=0; i<m_vecEntityExceptInstancing.size(); ++i )
	{
		m_vecEntityExceptInstancing[i]->RenderCascadeShadowHardAlpha();
	}
}

void EntityRenderList::RenderCascadeShadowReplace()
{
	// 천 종류 렌더링. (DxEffectBase)
	for( DWORD i=0; i<m_vecEntity.size(); ++i )
	{
		m_vecEntity[i].m_pEntity->RenderCascadeShadowReplace();
	}
}
// TnL_PS_3_0_CSM 이상
//////////////////////////////////////////////////////////////////////////

void EntityRenderList::Render_Query_Prepare( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	// Opaque & HardAlpha & SoftAlpha
	for( int i=(int)(m_vecEntity.size())-1; i>=0; --i )
	{
		m_vecEntity[i].m_pEntity->Render_Query_Prepare( pd3dDevice, sCV );
	}
}

void EntityRenderList::Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	// Opaque & HardAlpha & SoftAlpha
	for( int i=(int)(m_vecEntity.size())-1; i>=0; --i )
	{
		m_vecEntity[i].m_pEntity->Render_Query( pd3dDevice, sCV );
	}
}

//////////////////////////////////////////////////////////////////////////
// Thread
void EntityRenderList::FrameMoveTHREAD( float fElapsedTime )
{
	for( DWORD i=0; i<m_vecEntity.size(); ++i )
	{
		m_vecEntity[i].m_pEntity->FrameMoveTHREAD( fElapsedTime );
	}
	for( DWORD i=0; i<m_vecEntityBlend.size(); ++i )
	{
		m_vecEntityBlend[i].m_pEntity->FrameMoveTHREAD( fElapsedTime );
	}
	for( DWORD i=0; i<m_vecEntityPoint.size(); ++i )
	{
		m_vecEntityPoint[i].m_pEntity->FrameMoveTHREAD( fElapsedTime );
	}

	//if ( NSPieceQuickSort::g_bBakeRAD )
	//{
	//	BOOL bDay = TRUE;
	//	for( DWORD i=0; i<m_vecEntity.size(); ++i )
	//	{
	//		m_vecEntity[i].m_pEntity->FrameMoveRAD( bDay );
	//	}
	//}
	//else
	//{
	//	if ( bUsedMaterialSystem )
	//	{
	//		for( DWORD i=0; i<m_vecEntity.size(); ++i )
	//		{
	//			m_vecEntity[i].m_pEntity->FrameMoveMAT( fTime, fElapsedTime, bWLD );
	//		}
	//		for( DWORD i=0; i<m_vecEntityBlend.size(); ++i )
	//		{
	//			m_vecEntityBlend[i].m_pEntity->FrameMoveMAT( fTime, fElapsedTime, bWLD );
	//		}
	//		for( DWORD i=0; i<m_vecEntityPoint.size(); ++i )
	//		{
	//			m_vecEntityPoint[i].m_pEntity->FrameMoveMAT( fTime, fElapsedTime, bWLD );
	//		}
	//	}
	//	else
	//	{
	//		for( DWORD i=0; i<m_vecEntity.size(); ++i )
	//		{
	//			m_vecEntity[i].m_pEntity->FrameMoveTOOL( fTime, fElapsedTime );
	//		}
	//		for( DWORD i=0; i<m_vecEntityBlend.size(); ++i )
	//		{
	//			m_vecEntityBlend[i].m_pEntity->FrameMoveTOOL( fTime, fElapsedTime );
	//		}
	//		for( DWORD i=0; i<m_vecEntityPoint.size(); ++i )
	//		{
	//			m_vecEntityPoint[i].m_pEntity->FrameMoveTOOL( fTime, fElapsedTime );
	//		}
	//	}
	//}
}

//void EntityRenderList::CalculateRenderListTHREAD( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bTimeBlend, BOOL bReflect,
//												 BOOL bUsedMaterialSystem, float fTime, float fElapsedTime, BOOL bWLD )
//{
//	boost::shared_ptr<DxPieceAABB> spPieceAABB = m_wpPieceAABB.lock(); 
//	if ( spPieceAABB ) 
//	{
//		NSPieceQuickSort::InitPickAlpha();
//		NSPieceQuickSort::Reset();
//
//		// [shhan][2013.05.20] 모든맵에 LOD Blending 을 적용하였음.
//		BOOL bLodBlending(TRUE);
//
//		// NSPieceQuickSort 에 보여야 되는 Data 들이 들어간다.
//		spPieceAABB->InsertQuickSort( sCV, vFromPt, vLookatPt, bLodBlending, bTimeBlend, bReflect );
//
//		// Piece 관련 작업이다.
//		NSPieceQuickSort::QuickSortProcess();
//		NSPieceQuickSort::ExportPieceRenderList_THREAD( this );
//
//		// 현재 Piece Alpha 관련 변경을 위해 사용된다.
//		FrameMoveTHREAD( bUsedMaterialSystem, fTime, fElapsedTime, bWLD );
//
//		// Instancing 관련 작업.
//		//spPieceAABB->InitInstancing()
//	}
//
//	//// mat 이 아닌것.
//	//boost::shared_ptr<DxStaticMeshLightMap> spStaticMeshLightMap = m_wpStaticMeshLightMap.lock(); 
//	//if ( spStaticMeshLightMap ) 
//	//{
//	//	//spStaticMeshLightMap->InsertRenderListTHREAD();
//	//}
//
//	//// mat 을 쓴것
//	//boost::shared_ptr<DxStaticMaterialMeshLightMap> spStaticMaterialMeshLightMap = m_wpStaticMaterialMeshLightMap.lock(); 
//	//if ( spStaticMaterialMeshLightMap ) 
//	//{
//	//	spStaticMaterialMeshLightMap->InsertRenderListTHREAD();
//	//}
//}

void EntityRenderList::ImportTHREAD( const VEC_PIECE& vecPieceExceptInstancing,
										NSPieceQuickSort::PIECEDISTANCE* pArrayPiece, DWORD dwEntityCount, 
										NSPieceQuickSort::PIECEDISTANCE* pArrayPieceBlend, DWORD dwEntityCountBlend, 
										NSPieceQuickSort::PIECEDISTANCE* pArrayPiecePoint, DWORD dwEntityCountPoint )
{
	ClearPieceList();	

	for ( DWORD i=0; i<dwEntityCount; ++i )
	{
		m_vecEntity.push_back( pArrayPiece[i] );
	}

	m_vecEntityExceptInstancing = vecPieceExceptInstancing;

	for ( DWORD i=0; i<dwEntityCountBlend; ++i )
	{
		m_vecEntityBlend.push_back( pArrayPieceBlend[i] );
	}

	for ( DWORD i=0; i<dwEntityCountPoint; ++i )
	{
		m_vecEntityPoint.push_back( pArrayPiecePoint[i] );
	}
}

void EntityRenderList::ImportTHREAD_LandSkin( NSPieceQuickSort::PIECEDISTANCE* pArrayPiece, DWORD dwEntityCount, 
											NSPieceQuickSort::PIECEDISTANCE* pArrayPieceBlend, DWORD dwEntityCountBlend, 
											NSPieceQuickSort::PIECEDISTANCE* pArrayPiecePoint, DWORD dwEntityCountPoint )
{
	ClearLandSkinList();	

	for ( DWORD i=0; i<dwEntityCount; ++i )
	{
		m_vecLandSkin.push_back( pArrayPiece[i] );
	}

	for ( DWORD i=0; i<dwEntityCountBlend; ++i )
	{
		m_vecLandSkinBlend.push_back( pArrayPieceBlend[i] );
	}

	for ( DWORD i=0; i<dwEntityCountPoint; ++i )
	{
		m_vecLandSkinPoint.push_back( pArrayPiecePoint[i] );
	}
}

void EntityRenderList::GetInstancingEntity( std::vector<Entity*>& vecrEntity )
{
	// vecrEntity 에 넣어주세요.
}


//////////////////////////////////////////////////////////////////////////
//
EntityRenderListSwap::EntityRenderListSwap()
	: m_nUSED(0)
	, m_nBackUp(1)
{
	for ( DWORD i=0; i<2; ++i )
	{
		m_EntityRenderList[i] = boost::shared_ptr<EntityRenderList>(new EntityRenderList());
	}
}

EntityRenderListSwap::~EntityRenderListSwap()
{
}

void EntityRenderListSwap::CleanUp()
{
	for ( DWORD i=0; i<2; ++i )
	{
		m_EntityRenderList[i]->ClearPieceList();
		m_EntityRenderList[i]->ClearLandSkinList();
	}
}

void EntityRenderListSwap::Swap()
{
	if ( m_nUSED == 0 )
	{
		m_nUSED = 1;
		m_nBackUp = 0;
	}
	else
	{
		m_nUSED = 0;
		m_nBackUp = 1;
	}
}

boost::shared_ptr<EntityRenderList>& EntityRenderListSwap::GetEntityRenderListForThread()
{
	return m_EntityRenderList[m_nBackUp];
}




//void EntityRenderListSwap::TestCode( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bLodBlending, BOOL bTimeBlend, BOOL bReflect,
//				BOOL bUsedMaterialSystem, float fTime, float fElapsedTime, BOOL bWLD )
//{
//	m_EntityRenderList[m_nUSED]->CalculateRenderListTHREAD( sCV, vFromPt, vLookatPt, bLodBlending, bTimeBlend, bReflect,
//															bUsedMaterialSystem, fTime, fElapsedTime, bWLD );
//}
//


void EntityRenderListSwap::SetData_Force_TnL_PS_3_0_DF()
{
	m_EntityRenderList[m_nUSED]->SetData_Force_TnL_PS_3_0_DF();
}

//////////////////////////////////////////////////////////////////////////
// 전체 다 호출된다.
void EntityRenderListSwap::FrameMoveLandSkin( float fElapsedTime )
{
	m_EntityRenderList[m_nUSED]->FrameMoveLandSkin( fElapsedTime );
}

void EntityRenderListSwap::FrameMoveMAT( const float fTime, const float fElapsedTime )
{
	m_EntityRenderList[m_nUSED]->FrameMoveMAT( fTime, fElapsedTime );
}

void EntityRenderListSwap::RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice, 
										  BOOL bMPShading,
										  BOOL bUsedMaterialSystem,
										  BOOL bReflect,
										  float fReflectHeight,
										  BOOL bWLD )
{
	m_EntityRenderList[m_nUSED]->RenderOpaque( pd3dDevice, bWLD, bMPShading, bUsedMaterialSystem, bReflect, fReflectHeight );
}

void EntityRenderListSwap::RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMPShading, BOOL bUsedMaterialSystem, BOOL bWLD )
{
	m_EntityRenderList[m_nUSED]->RenderHardAlpha( pd3dDevice, bWLD, bMPShading, bUsedMaterialSystem );
}

void EntityRenderListSwap::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bWLD, BOOL bUsedMaterialSystem )
{
	m_EntityRenderList[m_nUSED]->RenderEff( pd3dDevice, pStaticMeshLightMap, pFrameMesh, bWLD, bUsedMaterialSystem );
}

void EntityRenderListSwap::RenderSkinObject( BOOL bReflect, float fReflectHeight )
{
	m_EntityRenderList[m_nUSED]->RenderSkinObject( bReflect, fReflectHeight );
}
// 전체 다 호출된다.
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
// TnL_PS_1_1 이하에서만 호출 된다.
void EntityRenderListSwap::RenderEffFrame()
{
	m_EntityRenderList[m_nUSED]->RenderEffFrame();
}

void EntityRenderListSwap::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWLD, BOOL bMPShading, BOOL bUsedMaterialSystem )
{
	m_EntityRenderList[m_nUSED]->RenderSoftAlpha( pd3dDevice, bWLD, bMPShading, bUsedMaterialSystem );
}

void EntityRenderListSwap::RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
											 DxStaticMeshLightMap* pStaticMeshLightMap, 
											 DxFrameMesh* pFrameMesh,
											 BOOL bReflect,
											 float fReflectHeight )
{
	m_EntityRenderList[m_nUSED]->RenderPickAlpha( pd3dDevice, pStaticMeshLightMap, pFrameMesh, FALSE, bReflect, fReflectHeight, TRUE );
}

//void EntityRenderListSwap::Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )
//{
//	if ( bWLD )
//	{
//		m_EntityRenderList[m_nUSED]->Render_Reflect( pd3dDevice );
//	}
//	else
//	{
//		m_EntityRenderList[m_nUSED]->RenderSoftAlphaForLodBlend_PSF( pd3dDevice );
//	}
//
//	m_EntityRenderList[m_nUSED]->Render_Reflect( pd3dDevice, bPSF );
//}

// ENSTM_DECAL_AFTER 는 렌더링 작업이 된다.
void EntityRenderListSwap::RenderDecal()
{
	m_EntityRenderList[m_nUSED]->RenderDecal();
}
// TnL_PS_1_1 이하에서만 호출 된다.
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
// Deffered 중,상,최상 옵션	
void EntityRenderListSwap::RenderDecalAndLodBlendDecal( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_EntityRenderList[m_nUSED]->RenderDecalAndLodBlendDecal( pd3dDevice );
}

void EntityRenderListSwap::RenderDecalPass2()
{
	m_EntityRenderList[m_nUSED]->RenderDecalPass2();
}

void EntityRenderListSwap::RenderSoftAlpha_Deffered( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_EntityRenderList[m_nUSED]->RenderSoftAlpha_Deffered( pd3dDevice );
}

void EntityRenderListSwap::RenderEff_PickBlend_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap )
{
	m_EntityRenderList[m_nUSED]->RenderEff_PickBlend_Deffered( pd3dDevice, pStaticMeshLightMap );
}

void EntityRenderListSwap::RenderForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflect, float fReflectHeight )
{
	m_EntityRenderList[m_nUSED]->RenderForLodBlendForDeffered( pd3dDevice, bReflect, fReflectHeight );
}

void EntityRenderListSwap::RenderSoftAlphaForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_EntityRenderList[m_nUSED]->RenderSoftAlphaForLodBlendForDeffered( pd3dDevice );
}

void EntityRenderListSwap::RenderEffFrameForDeffered( BOOL bLBuffer, BOOL bReflect )
{
	m_EntityRenderList[m_nUSED]->RenderEffFrameForDeffered( bLBuffer, bReflect );
}

void EntityRenderListSwap::RenderCustom_UNLIT_UNFOG()
{
	m_EntityRenderList[m_nUSED]->RenderCustom_UNLIT_UNFOG();
}

void EntityRenderListSwap::RenderWaterCustomSoftAlpha()
{
	m_EntityRenderList[m_nUSED]->RenderWaterCustomSoftAlpha();
}
// Deffered 중,상,최상 옵션	
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
// TnL_PS_3_0_CSM 이상
void EntityRenderListSwap::RenderCacadeOpaque( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_EntityRenderList[m_nUSED]->RenderCacadeOpaque( pd3dDevice );
}

void EntityRenderListSwap::RenderCascadeHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_EntityRenderList[m_nUSED]->RenderCascadeHardAlpha( pd3dDevice );
}

void EntityRenderListSwap::RenderCascadeShadowReplace()
{
	m_EntityRenderList[m_nUSED]->RenderCascadeShadowReplace();
}
// TnL_PS_3_0_CSM 이상
//////////////////////////////////////////////////////////////////////////