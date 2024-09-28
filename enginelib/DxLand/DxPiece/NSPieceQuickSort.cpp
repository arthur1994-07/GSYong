#include "pch.h"

#include "../../DxTools/DxRenderStates.h"
#include "../../DxTools/DxSurfaceTex.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxTools/Collision.h"
#include "../PieceInstancingThread.h"
#include "../EntityRenderList.h"
#include "./DxPiece.h"

#include "NSPieceQuickSort.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSPieceQuickSort
{
	// 저사양쪽 및 Deffered (SoftAlpha 혹은 갯수가 적은것) 에서 원래대로 나오는 물체.
	PIECEDISTANCE*				m_pArrayPiece(NULL);					// 배열
	DWORD						m_dwArraySize(0);						// 최대 가능한 배열 사이즈
	DWORD						m_dwPieceCount(0);						// 현재 배열 사이즈

	// Instancing 으로 인해 제외하고 남은 것.
	VEC_PIECE					m_vecPieceExceptInstancing;	
	// 메모리 단편화로 전역에 두었다.	잠시 사용하고 재사용시 리셋함.
	//PIECEDISTANCE*				m_pArrayPieceExceptInstancing(NULL);	// 배열
	//DWORD						m_dwArraySizeExceptInstancing(0);		// 최대 가능한 배열 사이즈
	//DWORD						m_dwPieceCountExceptInstancing(0);		// 현재 배열 사이즈

	// 저사양 및 Deffered 에서 강제 AlphaBlend 되는 물체.
	PIECEDISTANCE*	m_pArrayPieceBlend = NULL;
	DWORD			m_dwArraySizeBlend = 0L;
	DWORD			m_dwPieceCountBlend = 0L;

	// Deffered 에서 Point 로 사라지거나 생성되는 물체.
	PIECEDISTANCE*	m_pArrayPiecePoint = NULL;
	DWORD			m_dwArraySizePoint = 0L;
	DWORD			m_dwPieceCountPoint = 0L;

	// 
	BOOL			g_bViewAlphaLOD = TRUE;
	BOOL			g_bBakeRAD = FALSE;
	BOOL			g_bPieceInstancing = TRUE;
	float			g_fTime(0.f);
	float			g_fElapsedTime(0.f);		// ElapsedTime,

	void CleanUp()
	{
		m_dwPieceCount = 0;
		m_dwArraySize = 0;
		SAFE_DELETE_ARRAY( m_pArrayPiece );

		m_dwPieceCountBlend = 0;
		m_dwArraySizeBlend = 0;
		SAFE_DELETE_ARRAY( m_pArrayPieceBlend );

		m_dwPieceCountPoint = 0;
		m_dwArraySizePoint = 0;
		SAFE_DELETE_ARRAY( m_pArrayPiecePoint );
	}

	void FrameMove( const float fTime, const float fElapsedTime )
	{
		g_fTime = fTime;
		g_fElapsedTime = fElapsedTime;
	}

	void FrameMoveTOOL( const float fTime, const float fElapsedTime )
	{
		g_fTime = fTime;
		g_fElapsedTime = fElapsedTime;

		for( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			m_pArrayPiece[i].m_pEntity->FrameMoveTOOL( fTime, fElapsedTime );
		}
	}

	void FrameMoveTHREAD()
	{
		for( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			m_pArrayPiece[i].m_pEntity->FrameMoveTHREAD( g_fElapsedTime );
		}
		for( DWORD i=0; i<m_dwPieceCountBlend; ++i )
		{
			m_pArrayPieceBlend[i].m_pEntity->FrameMoveTHREAD( g_fElapsedTime );
		}
		for( DWORD i=0; i<m_dwPieceCountPoint; ++i )
		{
			m_pArrayPiecePoint[i].m_pEntity->FrameMoveTHREAD( g_fElapsedTime );
		}
	}

	//void FrameMoveRAD( BOOL bDay )
	//{
	//	for( DWORD i=0; i<m_dwPieceCount; ++i )
	//	{
	//		m_pArrayPiece[i].m_pEntity->FrameMoveRAD( bDay );
	//	}
	//}

	//void FrameMoveMAT( const float fTime, const float fElapsedTime )
	//{
	//	g_fElapsedTime = fElapsedTime;

	//	for( DWORD i=0; i<m_dwPieceCount; ++i )
	//	{
	//		m_pArrayPiece[i].m_pEntity->FrameMoveMAT( fTime, fElapsedTime, TRUE );
	//	}
	//	for( DWORD i=0; i<m_dwPieceCountBlend; ++i )
	//	{
	//		m_pArrayPieceBlend[i].m_pEntity->FrameMoveMAT( fTime, fElapsedTime, TRUE );
	//	}
	//	for( DWORD i=0; i<m_dwPieceCountPoint; ++i )
	//	{
	//		m_pArrayPiecePoint[i].m_pEntity->FrameMoveMAT( fTime, fElapsedTime, TRUE );
	//	}
	//}	

	void RenderOpaquePSF( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bWLD, BOOL bMPShading, BOOL bUsedMaterialSystem, BOOL bReflect, float fReflectHeight )
	{
		DWORD dwFVF_SAVE(0L);
		pd3dDevice->GetFVF( &dwFVF_SAVE );
		pd3dDevice->SetFVF( 0 );

		if ( bUsedMaterialSystem )
		{
			if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 )
			{
				for( DWORD i=0; i<m_dwPieceCount; ++i )
				{
					m_pArrayPiece[i].m_pEntity->RenderMS_Opaque( bMPShading, bReflect, fReflectHeight, FALSE );
					m_pArrayPiece[i].m_pEntity->RenderMS_Opaque_Animation( bMPShading, bReflect, fReflectHeight, FALSE );
				}
			}
			else
			{
				for( DWORD i=0; i<m_dwPieceCount; ++i )
				{
					m_pArrayPiece[i].m_pEntity->RenderMS_Fixed();
				}
			}
		}
		else
		{
			for( DWORD i=0; i<m_dwPieceCount; ++i )
			{
				m_pArrayPiece[i].m_pEntity->Render( pd3dDevice, !bWLD );
			}
		}

		pd3dDevice->SetFVF( dwFVF_SAVE );
	}

	void RenderHardAlphaPSF( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bWLD, BOOL bMPShading, BOOL bUsedMaterialSystem )
	{
		DWORD dwFVF_SAVE(0L);
		pd3dDevice->GetFVF( &dwFVF_SAVE );
		pd3dDevice->SetFVF( 0 );

		DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );

		if ( bUsedMaterialSystem )
		{
			for( DWORD i=0; i<m_dwPieceCount; ++i )
			{
				m_pArrayPiece[i].m_pEntity->RenderMS_HardAlpha( bMPShading, FALSE );
				m_pArrayPiece[i].m_pEntity->RenderMS_HardAlpha_Animation( bMPShading, FALSE );
			}
		}
		else
		{
			for( DWORD i=0; i<m_dwPieceCount; ++i )
			{
				m_pArrayPiece[i].m_pEntity->RenderHardAlpha( pd3dDevice, !bWLD );
			}
		}

		DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );

		pd3dDevice->SetFVF( dwFVF_SAVE );
	}

	void RenderDecalPSF()
	{
		for( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			m_pArrayPiece[i].m_pEntity->RenderDecal();
		}
	}

	void RenderDecalPass2_PSF()
	{
		for( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			m_pArrayPiece[i].m_pEntity->RenderMS_DecalPass2();
		}
	}

	void RenderWaterCustomSoftAlphaPSF()
	{
		for( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			m_pArrayPiece[i].m_pEntity->Render_PIECE_Custom_UNLIT_UNFOG();
		}

		for( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			m_pArrayPiece[i].m_pEntity->Render_PIECE_Water_CustomSoftAlpha();
		}
	}

	void RenderSoftAlphaPSF( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bWLD, BOOL bMPShading, BOOL bUsedMaterialSystem )
	{
		DWORD dwFVF_SAVE(0L);
		pd3dDevice->GetFVF( &dwFVF_SAVE );
		pd3dDevice->SetFVF( 0 );

		DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );

		if ( bUsedMaterialSystem )
		{
			if ( bWLD )
			{
				for( DWORD i=0; i<m_dwPieceCount; ++i )
				{
					m_pArrayPiece[i].m_pEntity->RenderMS_SoftAlpha( bMPShading );
				}
			}
			else
			{
				//for( DWORD i=0; i<m_dwPieceCount; ++i )
				for( int i=(int)(m_dwPieceCount-1); i>=0; --i )
				{
					m_pArrayPiece[i].m_pEntity->RenderMS_SoftAlphaForPSF();
				}
			}
		}
		else
		{
			for( DWORD i=0; i<m_dwPieceCount; ++i )
			{
				m_pArrayPiece[i].m_pEntity->RenderSoftAlpha( pd3dDevice, !bWLD );
			}
		}

		DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );

		pd3dDevice->SetFVF( dwFVF_SAVE );
	}

	//////////////////////////////////////////////////////////////////////////
	//
	void RenderSkinObjectPSF( BOOL bReflect, float fReflectHeight )
	{
		for( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			m_pArrayPiece[i].m_pEntity->RenderSkinObject( bReflect, fReflectHeight );
		}
	}

	void RenderEff_PSF( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bWLD, BOOL bUsedMaterialSystem )
	{
		for( int i=(int)(m_dwPieceCount-1); i>=0; --i )
		{
			m_pArrayPiece[i].m_pEntity->RenderEff( pd3dDevice, pStaticMeshLightMap, pFrameMesh, !bWLD );
		}
	}

	void FrameMoveONLY( const float fTime, const float fElapsedTime )
	{
		for( DWORD i=0; i<m_dwPieceCountBlend; ++i )
		{
			m_pArrayPieceBlend[i].m_pEntity->FrameMoveTOOL( fTime, fElapsedTime );
		}

		for( DWORD i=0; i<m_dwPieceCountPoint; ++i )
		{
			m_pArrayPiecePoint[i].m_pEntity->FrameMoveTOOL( fTime, fElapsedTime );
		}
	}


	void RenderForLodBlend_PSF( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		// Note : SetRenderState
		DWORD dwAlphaTestEnable;
		pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );

		// Note : SetRenderState
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );

		Entity* pPiece(NULL);
		for( int i=(int)(m_dwPieceCountPoint-1); i>=0; --i )
		{
			pPiece = m_pArrayPiecePoint[i].m_pEntity;

			pPiece->RenderMS_Opaque( FALSE, FALSE, 0.f, FALSE );
			pPiece->RenderMS_HardAlpha( FALSE, FALSE );
		}

		// Note : ReSetRenderState
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	dwAlphaTestEnable );
	}





	void RenderSoftAlphaForLodBlend_PSF( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		// Note : SetRenderState
		DWORD dwAlphaTestEnable;
		pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );

		// Note : SetRenderState
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );

		Entity* pPiece(NULL);
		for( int i=(int)(m_dwPieceCountPoint-1); i>=0; --i )
		{
			pPiece = m_pArrayPiecePoint[i].m_pEntity;

			pPiece->RenderMS_SoftAlphaForPSF();
		}

		// Note : ReSetRenderState
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	dwAlphaTestEnable );
	}






	//void RenderEffFrame()
	//{
	//	for( DWORD i=0; i<m_dwPieceCount; ++i )
	//	{
	//		m_pArrayPiece[i].m_pEntity->RenderEffFrame();
	//	}
	//}

	//void RenderEffFrameForDeffered( BOOL bLBuffer )
	//{
	//	for( DWORD i=0; i<m_dwPieceCount; ++i )
	//	{
	//		m_pArrayPiece[i].m_pEntity->RenderEffFrameForDeffered( NULL, bLBuffer );
	//	}
	//}

	//void RenderDecalAndLodBlendDecal( LPDIRECT3DDEVICEQ pd3dDevice )
	//{
	//	for( DWORD i=0; i<m_dwPieceCount; ++i )
	//	{
	//		m_pArrayPiece[i].m_pEntity->RenderDecal();
	//	}

	//	// Note : SetRenderState
	//	DWORD dwAlphaTestEnable;
	//	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );

	//	// Note : SetRenderState
	//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );

	//	Entity* pPiece(NULL);
	//	for( int i=(int)(m_dwPieceCountPoint-1); i>=0; --i )
	//	{
	//		pPiece = m_pArrayPiecePoint[i].m_pEntity;

	//		pPiece->RenderDecal();
	//	}

	//	// Note : ReSetRenderState
	//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	dwAlphaTestEnable );
	//}

	////////////////////////////////////////////////////////////////////////////
	//// TnL_PS_2_0 이상에서만 호출 된다.
	//void RenderSoftAlpha_Deffered( LPDIRECT3DDEVICEQ pd3dDevice )
	//{
	//	// Base Rendering
	//	for( DWORD i=0; i<m_dwPieceCount; ++i )
	//	{
	//		m_pArrayPiece[i].m_pEntity->RenderMS_SoftAlphaForDeffered();
	//	}

	//	// LOD
	//	Entity* pPiece(NULL);
	//	for( int i=(int)(m_dwPieceCountBlend-1); i>=0; --i )
	//	{
	//		pPiece = m_pArrayPieceBlend[i].m_pPiece;

	//		pPiece->RenderMS( FALSE, FALSE, 0.f, TRUE );
	//		pPiece->RenderMS_HardAlpha( FALSE, TRUE );
	//		pPiece->RenderMS_SoftAlpha( FALSE );
	//	}
	//}

	////////////////////////////////////////////////////////////////////////////
	//// TnL_PS_2_0 이상에서만 호출 된다.
	//// PickBlend 되는 물체의 1RenderTarget 에 렌더 할 수 있는 것들 Rendering 하기.
	//void RenderEff_PickBlend_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap )
	//{
	//	// Base Rendering
	//	Entity* pPiece(NULL);
	//	for( int i=(int)(m_dwPieceCountPoint-1); i>=0; --i )
	//	{
	//		pPiece = m_pArrayPiecePoint[i].m_pEntity;
	//		pPiece->RenderMS_DecalPass2();
	//		pPiece->Render_PIECE_Water_CustomSoftAlpha();
	//		pPiece->RenderEff( pd3dDevice, pStaticMeshLightMap, NULL, FALSE );
	//	}

	//	// LOD
	//	for( int i=(int)(m_dwPieceCountBlend-1); i>=0; --i )
	//	{
	//		pPiece = m_pArrayPieceBlend[i].m_pPiece;

	//		pPiece->RenderDecal();
	//		pPiece->RenderMS_DecalPass2();
	//		pPiece->Render_PIECE_Water_CustomSoftAlpha();
	//		pPiece->RenderEff( pd3dDevice, pStaticMeshLightMap, NULL, FALSE );
	//	}
	//}

	////////////////////////////////////////////////////////////////////////////
	////					불투명 물체의 LOD 처리
	//void RenderForLodBlendForDeffered( LPDIRECT3DDEVICEQ pd3dDevice )
	//{
	//	// Note : SetRenderState
	//	DWORD dwAlphaTestEnable;
	//	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );

	//	// Note : SetRenderState
	//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );

	//	Entity* pPiece(NULL);
	//	for( int i=(int)(m_dwPieceCountPoint-1); i>=0; --i )
	//	{
	//		pPiece = m_pArrayPiecePoint[i].m_pEntity;

	//		pPiece->RenderMS( FALSE, FALSE, 0.f, FALSE );
	//		pPiece->RenderMS_HardAlpha( FALSE, FALSE );
	//		pPiece->RenderEffFrameForDeffered( NULL, FALSE );
	//	}

	//	// Note : ReSetRenderState
	//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	dwAlphaTestEnable );
	//}

	////////////////////////////////////////////////////////////////////////////
	////					SoftAlpha 의 LOD 처리
	//void RenderSoftAlphaForLodBlendForDeffered( LPDIRECT3DDEVICEQ pd3dDevice )
	//{
	//	// Note : SetRenderState
	//	DWORD dwAlphaTestEnable;
	//	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );

	//	// Note : SetRenderState
	//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );

	//	Entity* pPiece(NULL);
	//	for( int i=(int)(m_dwPieceCountPoint-1); i>=0; --i )
	//	{
	//		pPiece = m_pArrayPiecePoint[i].m_pEntity;

	//		pPiece->RenderMS_SoftAlphaForDeffered();
	//	}

	//	// Note : ReSetRenderState
	//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	dwAlphaTestEnable );
	//}

	////////////////////////////////////////////////////////////////////////////
	//// TnL_PS_1_1 이하에서만 호출 된다.
	//void RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
	//						const CLIPVOLUME &sCV, 
	//						DxStaticMeshLightMap* pStaticMeshLightMap, 
	//						DxFrameMesh* pFrameMesh, 
	//						BOOL bPSF, 
	//						BOOL bReflect,
	//						float fReflectHeight, 
	//						BOOL bFrameMove )
	//{
	//	// Note : SetRenderState
	//	DWORD dwAlphaBlendEnable, dwAlphaREF;
	//	pd3dDevice->GetRenderState( D3DRS_ALPHAREF,			&dwAlphaREF );
	//	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );

	//	// Note : SetRenderState
	//	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );
	//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
	//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

	//	// Note : SetTextureStageState
	//	DWORD dwAlphaOP, dwAlphaARG2;
	//	pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,	&dwAlphaOP );
	//	pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG2,	&dwAlphaARG2 );
	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );

	//	{
	//		Entity* pPiece(NULL);
	//		for( int i=(int)(m_dwPieceCountBlend-1); i>=0; --i )
	//		{
	//			pPiece = m_pArrayPieceBlend[i].m_pPiece;

	//			DWORD dwColor = static_cast<DWORD>(pPiece->GetAlpha() * 255.f);
	//			if ( dwColor == 0 )
	//				continue;

	//			dwColor = dwColor << 24;
	//			dwColor += 0xffffff;
	//			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwColor );

	//			if ( pPiece->IsMaterialMesh() )
	//			{
	//				pPiece->RenderMS( FALSE, FALSE, 0.f, FALSE );
	//				pPiece->RenderMS_HardAlpha( FALSE, FALSE );
	//				pPiece->RenderEffFrame();
	//				pPiece->RenderSkinObject( bReflect, fReflectHeight, bFrameMove );
	//				pPiece->RenderMS_SoftAlpha( FALSE );
	//				pPiece->RenderMS_DecalPass2();
	//				pPiece->Render_PIECE_Water_CustomSoftAlpha();
	//				pPiece->RenderEff( pd3dDevice, pStaticMeshLightMap, pFrameMesh, bPSF );					
	//			}
	//			else
	//			{
	//				pPiece->Render( pd3dDevice, bPSF );
	//				pPiece->RenderHardAlpha( pd3dDevice, bPSF );
	//				pPiece->RenderEffFrame();
	//				pPiece->RenderSkinObject( bReflect, fReflectHeight, bFrameMove );
	//				pPiece->RenderSoftAlpha( pd3dDevice, bPSF );
	//				pPiece->RenderEff( pd3dDevice, pStaticMeshLightMap, pFrameMesh, bPSF );
	//			}
	//		}
	//	}

	//	// Note : ReSetRenderState
	//	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			dwAlphaREF );
	//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );

	//	// Note : ReSetTextureStageState
	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	dwAlphaOP );
	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,	dwAlphaARG2 );
	//}

	//void Render_Query_Prepare( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
	//{
	//	// Opaque & HardAlpha & SoftAlpha
	//	for( int i=(int)(m_dwPieceCount-1); i>=0; --i )
	//	{
	//		m_pArrayPiece[i].m_pEntity->Render_Query_Prepare( pd3dDevice, sCV );
	//	}
	//}

	//void Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
	//{
	//	// Opaque & HardAlpha & SoftAlpha
	//	for( int i=(int)(m_dwPieceCount-1); i>=0; --i )
	//	{
	//		m_pArrayPiece[i].m_pEntity->Render_Query( pd3dDevice, sCV );
	//	}
	//}

	void SetArrayMaxSize( DWORD dwCount )
	{
		if ( dwCount == 0 )
			dwCount = 1;

		if( dwCount > m_dwArraySize )
		{
			SAFE_DELETE_ARRAY( m_pArrayPiece );

			m_dwArraySize = dwCount;
			m_pArrayPiece = new PIECEDISTANCE[m_dwArraySize];
		}

		//if( dwCount > m_dwArraySizeExceptInstancing )
		//{
		//	SAFE_DELETE_ARRAY( m_pArrayPieceExceptInstancing );

		//	m_dwArraySizeExceptInstancing = dwCount;
		//	m_pArrayPieceExceptInstancing = new PIECEDISTANCE[m_dwArraySizeExceptInstancing];
		//}

		if( dwCount > m_dwArraySizeBlend )
		{
			SAFE_DELETE_ARRAY( m_pArrayPieceBlend );

			m_dwArraySizeBlend = dwCount;
			m_pArrayPieceBlend = new PIECEDISTANCE[m_dwArraySizeBlend];
		}

		if( dwCount > m_dwArraySizePoint )
		{
			SAFE_DELETE_ARRAY( m_pArrayPiecePoint );

			m_dwArraySizePoint = dwCount;
			m_pArrayPiecePoint = new PIECEDISTANCE[m_dwArraySizePoint];
		}
	}

	void ReSizeArrayPieceCommon( PIECEDISTANCE** ppArrayPiece, DWORD& dwArraySize )
	{
		if ( dwArraySize == 0 )
		{
			dwArraySize = 1;
			SAFE_DELETE_ARRAY( (*ppArrayPiece) );
			(*ppArrayPiece) = new PIECEDISTANCE[ dwArraySize ];
		}
		else
		{
			DWORD dwTemp = dwArraySize;
			PIECEDISTANCE* pTemp = new PIECEDISTANCE[dwTemp];
			for( DWORD i=0; i<dwTemp; ++i )
			{
				pTemp[i].m_fDistance	= (*ppArrayPiece)[i].m_fDistance;
				pTemp[i].m_pEntity		= (*ppArrayPiece)[i].m_pEntity;
			}

			dwArraySize = dwTemp*2;
			SAFE_DELETE_ARRAY( (*ppArrayPiece) );
			(*ppArrayPiece) = new PIECEDISTANCE[ dwArraySize ];

			for( DWORD i=0; i<dwTemp; ++i )
			{
				(*ppArrayPiece)[i].m_fDistance	= pTemp[i].m_fDistance;
				(*ppArrayPiece)[i].m_pEntity		= pTemp[i].m_pEntity;
			}

			SAFE_DELETE_ARRAY( pTemp ); 
		}
	}

	void ReSizeArrayPiece()
	{
		ReSizeArrayPieceCommon( &m_pArrayPiece, m_dwArraySize );
	}

	//void ReSizeArrayPieceExcecptInstancing()
	//{
	//	ReSizeArrayPieceCommon( &m_pArrayPieceExceptInstancing, m_dwArraySizeExceptInstancing );
	//}

	void ReSizeArrayPieceONLY()
	{
		ReSizeArrayPieceCommon( &m_pArrayPieceBlend, m_dwArraySizeBlend );
	}

	void ReSizeArrayPiecePoint()
	{
		ReSizeArrayPieceCommon( &m_pArrayPiecePoint, m_dwArraySizePoint );
	}

	void Reset()
	{
		m_dwPieceCount = 0;
		m_vecPieceExceptInstancing.clear();
		//m_dwPieceCountExceptInstancing = 0;
	}

	void InitPickAlpha()
	{
		m_dwPieceCountBlend = 0;
		m_dwPieceCountPoint = 0;
	}

	void InsertData( Entity* pPiece, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength, BOOL bLodBlending, BOOL bTimeBlend )
	{
		float fTempLength(0.f);
		float fLengthLookat(0.f);
		D3DXVECTOR3 vDir(0.f,0.f,0.f);

		Entity* pCur(NULL);
		{
			pCur = pPiece;

			float VisibleRadius = pCur->GetVisibleRadius();
			float InvisibleRadius = pCur->GetInvisibleRadius();

			// 옵션상으로 저사양 이하는 LOD 거리를 절반으로 줄인다.
			// g_emGroundQulity 는 wld 의 material 시스템 기준을 보는 것임.
			if ( RENDERPARAM::g_emTnLMode <= TnL_PS_1_1 )
			{
				VisibleRadius *= 0.5f;
				InvisibleRadius *= 0.5f;
			}

			vDir = pCur->GetCenter() - vFromPt;
			fTempLength = D3DXVec3Length( &vDir );	//vDir.x*vDir.x + vDir.y*vDir.y + vDir.z*vDir.z;

			// Note : Edit 상태냐 아니냐에 따라 활성화 될수도 있고 아닐 수도 있다.
			if( !g_bViewAlphaLOD )
			{
				pCur->SetAlpha( 1.f );
			}
			else if ( g_bBakeRAD )
			{
				// Voxel Color 라면 렌더링을 하지 않는다.
				if ( pCur->IsVoxelColor() )
					return;

				//// 거리가 멀면 렌더링을 하지 않는다.
				//vDir = pCur->m_vCenter - vLookatPt;
				//fLengthLookat = D3DXVec3Length( &vDir );	// 카메라 까지의 거리
				//fLengthLookat -= pCur->m_fRadius;			// 물체의 반지름까지 계산해 준다.
				//if ( 400.f <= fLengthLookat)
				//	return;

				pCur->SetAlpha( 1.f );
			}
			else
			{
				vDir = pCur->GetCenter() - vLookatPt;
				fLengthLookat = D3DXVec3Length( &vDir ); // 카메라 까지의 거리
				fLengthLookat -= pCur->GetRadius(); // 물체의 반지름까지 계산해 준다.

				//// Note : 멀리있어도 렌더해야 하는가~?
				//if( pCur->m_bFarRender )
				//{
				//	pCur->m_bVisible = TRUE;
				//}
				//else

				if ( bLodBlending )
				{
					// Material System 이 쓰이는 맵들만 Timeblend 시스템이 쓰이도록 하자.
					if ( bTimeBlend && (RENDERPARAM::g_emGroundQulityTHREAD >= TnL_PS_3_0_DF) )
					{
						//if ( fLengthLookat <= VisibleRadius )
						//{
						//	pCur->SetAddAlpha( TRUE );
						//}
						//else if ( InvisibleRadius <= fLengthLookat )
						//{
						//	pCur->SetAddAlpha( FALSE );
						//}

						// 멀리 있는 걸로 계산하자.
						if ( InvisibleRadius >= fLengthLookat )
						{
							pCur->SetAddAlpha( g_fTime, TRUE );
						}
						else if ( InvisibleRadius <= fLengthLookat )
						{
							pCur->SetAddAlpha( g_fTime, FALSE );
						}

						float fAlphaSpeed = pCur->GetAlpha();
						fAlphaSpeed -= 0.5f;
						fAlphaSpeed = fabsf( fAlphaSpeed );
						fAlphaSpeed += 0.05f;		// 0.5 ~ 0.05 ~ 0.5					// 변화 속도 조절 가능.
						fAlphaSpeed *= 1.5f;		// 수치2는 대략 1초 정도 걸린다.	// 변화 속도 조절 가능.


						if ( pCur->IsAddAlpha() )
						{
							pCur->SetAlpha( pCur->GetAlpha() + (g_fElapsedTime*fAlphaSpeed) ) ;	// 늘어난다.

							if ( pCur->GetAlpha() > 1.f )
							{
								pCur->SetBlendAlpha( FALSE );
								pCur->SetAlpha( 1.f );
							}
						}
						else
						{
							pCur->SetAlpha( pCur->GetAlpha() - (g_fElapsedTime*fAlphaSpeed) ) ;	// 빠진다.

							if ( pCur->GetAlpha() < 0.f )
							{
								pCur->SetAlpha( 0.f );
							}
						}

						// Note : 화면을 가린다면 알파처리를 한다는 옵션이 있음.
						if( pCur->GetPickAlpha() )
						{
							if( fLength > (fTempLength - pCur->GetRadius()) )
							{
								pCur->SetAlpha( pCur->GetAlpha() + (g_fElapsedTime*2.f) ) ;	// 1초에 2.f가 늘어난다.

								//D3DXVECTOR3	m_vMax;			// 전체의 Max
								//D3DXVECTOR3	m_vMin;			// 전체의 Min
								// Note : Pick Check를 한다.
								if ( COLLISION::IsCollisionLineToAABB( vFromPt, vLookatPt, pCur->GetStaticMax(), pCur->GetStaticMin() ) )	//	pCur->m_vMax, pCur->m_vMin ) )	//
								{
									pCur->SetBlendAlpha( TRUE );
									pCur->SetAlpha( pCur->GetAlpha() - (g_fElapsedTime*2.f*2.f) ) ;	//  1초에 2.f가 줄어든다. // 상단에서 올렸으니 내리도록 한다.

									if( pCur->GetAlpha() < 0.5f )
									{
										pCur->SetAlpha( 0.5f );
									}
								}
							}
						}
					}
					else
					{
						// Note : 나타나야 하는지 확인한다.	변해야 하는 시기를 틀리게 해준다.
						// if m_bVisible is TRUE then Piece render opaque, else m_bVisible is FALSE then Piece render transparent.
						if ( fLengthLookat <= VisibleRadius )
						{
							// 내부 원에 있다면 무조건 알파는 1 이다.
							// 하지만 Pick 된 Object의 반투명 효과를 위해 if( pCur->GetPickAlpha() ) 부분이 작업됨.
							//pCur->GetAlpha() = 1.f;

							pCur->SetAlpha( pCur->GetAlpha() + (g_fElapsedTime*2.7f) ) ;	// 1초에 2.7f가 늘어난다.

							// Note : 화면을 가린다면 알파처리를 한다는 옵션이 있음.
							if( pCur->GetPickAlpha() )
							{
								if( fLength > (fTempLength - pCur->GetRadius()) )
								{
									//D3DXVECTOR3	m_vMax;			// 전체의 Max
									//D3DXVECTOR3	m_vMin;			// 전체의 Min
									// Note : Pick Check를 한다.
									if ( COLLISION::IsCollisionLineToAABB( vFromPt, vLookatPt, pCur->GetStaticMax(), pCur->GetStaticMin() ) )	//	pCur->m_vMax, pCur->m_vMin ) )	//
									{
										pCur->SetBlendAlpha( TRUE );
										pCur->SetAlpha( pCur->GetAlpha() - (g_fElapsedTime*2.7f*2.f) ) ;	// 1초에 2.7f가 줄어든다. // 상단에서 올렸으니 내리도록 한다.

										if( pCur->GetAlpha() < 0.24f )
										{
											pCur->SetAlpha( 0.24f );
										}
									}
								}
							}

							if( pCur->GetAlpha() > 1.f )
							{
								pCur->SetAlpha( 1.f );
							}
						}
						else
						{
							if ( InvisibleRadius <= fLengthLookat )
							{
								pCur->SetAlpha( 0.f );
							}
							else // VisibleRadius < fLengthLookat < InvisibleRadius
							{
								pCur->SetBlendAlpha( TRUE );
								float Diff = InvisibleRadius - VisibleRadius;
								pCur->SetAlpha( (InvisibleRadius - fLengthLookat) / Diff );
							}
						}
					}
				}
				else
				{
					pCur->SetAlpha( pCur->GetAlpha() + (g_fElapsedTime*2.7f) ) ;	// 1초에 2.7f가 늘어난다.

					// Note : 화면을 가린다면 알파처리를 한다는 옵션이 있음.
					if( pCur->GetPickAlpha() )
					{
						if( fLength > (fTempLength - pCur->GetRadius()) )
						{
							//D3DXVECTOR3	m_vMax;			// 전체의 Max
							//D3DXVECTOR3	m_vMin;			// 전체의 Min
							// Note : Pick Check를 한다.
							if ( COLLISION::IsCollisionLineToAABB( vFromPt, vLookatPt, pCur->GetStaticMax(), pCur->GetStaticMin() ) )	//	pCur->m_vMax, pCur->m_vMin ) )	//
							{
								pCur->SetBlendAlpha( TRUE );
								pCur->SetAlpha( pCur->GetAlpha() - (g_fElapsedTime*2.7f*2.f) ) ;	// 1초에 2.7f가 줄어든다. // 상단에서 올렸으니 내리도록 한다.

								if( pCur->GetAlpha() < 0.24f )
								{
									pCur->SetAlpha( 0.24f );
								}
							}
						}
					}
				}
			}

			// Note : 뿌려야 하는 것들.
			if( pCur->GetAlpha() > 0.f )
			{
				// Note : Pick AlphaBlending      
				//			불투명은 m_pArrayPiece 로, 반투명은 m_pArrayPieceBlend 로, 투명은 넣지 않는다.
				if( pCur->GetAlpha() < 1.f )
				{
					if ( pCur->IsBlendAlpha() )
					{
						// Note : 최대 제한 갯수를 넘었다.
						//		배열 크기를 늘려야 한다.
						if( m_dwPieceCountBlend >= m_dwArraySizeBlend )
						{
							ReSizeArrayPieceONLY();
						}

						m_pArrayPieceBlend[m_dwPieceCountBlend].m_fDistance = fTempLength;
						m_pArrayPieceBlend[m_dwPieceCountBlend].m_pEntity = pCur;
						++m_dwPieceCountBlend;
					}
					else
					{
						// Note : 최대 제한 갯수를 넘었다.
						//		배열 크기를 늘려야 한다.
						if( m_dwPieceCountPoint >= m_dwArraySizePoint )
						{
							ReSizeArrayPiecePoint();
						}

						m_pArrayPiecePoint[m_dwPieceCountPoint].m_fDistance = fTempLength;
						m_pArrayPiecePoint[m_dwPieceCountPoint].m_pEntity = pCur;
						++m_dwPieceCountPoint;
					}
				}
				else
				{
					pCur->SetAlpha( 1.f );

					// Note : 최대 제한 갯수를 넘었다.
					//		배열 크기를 늘려야 한다.
					if( m_dwPieceCount >= m_dwArraySize )
					{
						ReSizeArrayPiece();
					}

					m_pArrayPiece[m_dwPieceCount].m_fDistance = fTempLength;
					m_pArrayPiece[m_dwPieceCount].m_pEntity = pCur;
					++m_dwPieceCount;
				}
			}

			//pCur = pCur->m_pTreeNext;
		}
	}

	void InsertDataForShadow( Entity* pEntity )
	{
		// Note : 최대 제한 갯수를 넘었다.
		//		배열 크기를 늘려야 한다.
		if( m_dwPieceCount >= m_dwArraySize )
		{
			ReSizeArrayPiece();
		}

		m_pArrayPiece[m_dwPieceCount].m_pEntity = pEntity;
		++m_dwPieceCount;
	}



	//////////////////////////////////////////////////////////////////////////
	//
	void ExportPieceRenderList_THREAD( EntityRenderList* pDest )
	{
		pDest->ImportTHREAD( m_vecPieceExceptInstancing, m_pArrayPiece, m_dwPieceCount, m_pArrayPieceBlend, m_dwPieceCountBlend, m_pArrayPiecePoint, m_dwPieceCountPoint );
	}

	void ExportLandSkinRenderList_THREAD( EntityRenderList* pDest )
	{
		pDest->ImportTHREAD_LandSkin( m_pArrayPiece, m_dwPieceCount, m_pArrayPieceBlend, m_dwPieceCountBlend, m_pArrayPiecePoint, m_dwPieceCountPoint );
	}	

	// Instancing 이 동작 안된다. 
	// m_pArrayPiece 를 모두 m_vecPieceExceptInstancing 로 옮기자.
	void InsertAllExceptInstancing()
	{
		m_vecPieceExceptInstancing.clear();

		for ( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			DxPiece* pPiece = MIN_BOOST::polymorphic_downcast<DxPiece*>( m_pArrayPiece[i].m_pEntity );

			// Instancing 이 안되는 건 일반적인 렌더링이 되도록 한다.
			m_vecPieceExceptInstancing.push_back( pPiece );
		}
	}

	// Instancing 관련 Data 를 빼도록 한다. Piece
	void InsertInstancing( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
							PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>* pPieceInstancing,
							float fHeight )
	{
		m_vecPieceExceptInstancing.clear();

		// 각 Instancing 그룹마다 몇번째 LodVB 를 사용할 지 알아야 해서 선행 검사가 필요하다.
		pPieceInstancing->ResetRef_THREAD();
		for ( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			DxPiece* pPiece = MIN_BOOST::polymorphic_downcast<DxPiece*>( m_pArrayPiece[i].m_pEntity );

			pPieceInstancing->AddRefMeshLM_THREAD( emCullingRenderType, pPiece );
		}

		// 해당 LodVB 쪽에 PieceParameter 들을 정리해놓는다. 
		pPieceInstancing->Reset_THREAD();

		// Instancing 셋팅을 시작한다.
		for ( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			DxPiece* pPiece = MIN_BOOST::polymorphic_downcast<DxPiece*>( m_pArrayPiece[i].m_pEntity );

			// Piece 기준에서의 Instancing 가능 여부. ( PickAlpha 가 된다던가 하는 경우 Instancing 불가 )
			if ( !pPiece->IsInstancingDynamicOption() )
			{
				// Instancing 이 안되는 건 일반적인 렌더링이 되도록 한다.
				m_vecPieceExceptInstancing.push_back( pPiece );
				continue;
			}

			if ( !pPieceInstancing->InsertInstancingMeshLM_THREAD( emCullingRenderType, pPiece ) )
			{
				// Instancing 이 안되는 건 일반적인 렌더링이 되도록 한다.
				m_vecPieceExceptInstancing.push_back( pPiece );
			}
		}

		// CreateVB 를 하고 갯수가 너무 적어 Instancing에 제외되는 것도 얻어온다.
		
		if ( emCullingRenderType == NSLandThread::TYPE_REFLECTION_CULL )
		{
			pPieceInstancing->InsertDataToVB_ExceptPiece_Reflect_THREAD( m_vecPieceExceptInstancing, fHeight );
		}
		else
		{
			pPieceInstancing->InsertDataToVB_ExceptPiece_THREAD( m_vecPieceExceptInstancing );
		}
	}

	void InsertInstancing( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
							PieceInstancingThread<DxStaticAniFrame*,DxPieceInstancingForShadow>* pPieceInstancing,
							float fHeight )
	{
		m_vecPieceExceptInstancing.clear();

		// 각 Instancing 그룹마다 몇번째 LodVB 를 사용할 지 알아야 해서 선행 검사가 필요하다.
		pPieceInstancing->ResetRef_THREAD();
		for ( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			DxPiece* pPiece = MIN_BOOST::polymorphic_downcast<DxPiece*>( m_pArrayPiece[i].m_pEntity );

			pPieceInstancing->AddRefPureMesh_THREAD( emCullingRenderType, pPiece );
		}

		// 해당 LodVB 쪽에 PieceParameter 들을 정리해놓는다. 
		pPieceInstancing->Reset_THREAD();

		// Instancing 셋팅을 시작한다.
		for ( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			DxPiece* pPiece = MIN_BOOST::polymorphic_downcast<DxPiece*>( m_pArrayPiece[i].m_pEntity );

			if ( !pPieceInstancing->InsertInstancingPureMesh_THREAD( emCullingRenderType, pPiece ) )
			{
				// Instancing 이 안되는 건 일반적인 렌더링이 되도록 한다.
				m_vecPieceExceptInstancing.push_back( pPiece );
			}
		}

		// CreateVB 를 하고 갯수가 너무 적어 Instancing에 제외되는 것도 얻어온다.

		pPieceInstancing->InsertDataToVB_ExceptPiece_THREAD( m_vecPieceExceptInstancing );
	}




	void QuickSortProcess()
	{
		//DxQuickSort( m_pArrayPiece, 0, m_dwPieceCount-1 );
		DxQuickSort( m_pArrayPieceBlend, 0, m_dwPieceCountBlend-1 );
		//DxQuickSort( m_pArrayPiecePoint, 0, m_dwPieceCountPoint-1 );
	}

	void DxQuickSort( PIECEDISTANCE pArray[], int ilo, int ihi ) 
	{
		float pivot;	// pivot value for partitioning array
		int ulo, uhi;	// indices at ends of unpartitioned region
		int ieq;		// least index of array entry with value equal to pivot
	 //   int tempEntry;	// temporary entry used for swapping

		if (ilo >= ihi) return;

		// Select a pivot value.
		pivot = pArray[(ilo + ihi)/2].m_fDistance;
		// Initialize ends of unpartitioned region and least index of entry
		// with value equal to pivot.
		ieq = ulo = ilo;
		uhi = ihi;
		// While the unpartitioned region is not empty, try to reduce its size.
		while( ulo <= uhi ) 
		{
			if( pArray[uhi].m_fDistance > pivot ) 
			{
				// Here, we can reduce the size of the unpartitioned region and
				// try again.
				uhi--;
			} 
			else 
			{
				// Here, A[uhi] <= pivot, so swap entries at indices ulo and
				// uhi.
				//tempEntry = pArray[ulo];
				//pArray[ulo] = pArray[uhi];
				//pArray[uhi] = tempEntry;

				Swap( pArray[uhi], pArray[ulo] );

				// After the swap, A[ulo] <= pivot.
				if (pArray[ulo].m_fDistance < pivot) 
				{
					// Swap entries at indices ieq and ulo.
					//tempEntry = pArray[ieq];
					//pArray[ieq] = pArray[ulo];
					//pArray[ulo] = tempEntry;
					Swap( pArray[ieq], pArray[ulo] );

					// After the swap, A[ieq] < pivot, so we need to change
					// ieq.
					ieq++;
					// We also need to change ulo, but we also need to do
					// that when A[ulo] = pivot, so we do it after this if
					// statement.
				}
				// Once again, we can reduce the size of the unpartitioned
				// region and try again.
				ulo++;
			}
		}

		// Now, all entries from index ilo to ieq - 1 are less than the pivot
		// and all entries from index uhi to ihi + 1 are greater than the
		// pivot.  So we have two regions of the array that can be sorted
		// recursively to put all of the entries in order.
		DxQuickSort( pArray, ilo, ieq - 1 );
		DxQuickSort( pArray, uhi + 1, ihi );
	}


	void Swap( PIECEDISTANCE& pData1, PIECEDISTANCE& pData2 )
	{
		PIECEDISTANCE pTemp;
		pTemp.m_fDistance = pData1.m_fDistance;
		pTemp.m_pEntity = pData1.m_pEntity;

		pData1.m_fDistance = pData2.m_fDistance;
		pData1.m_pEntity = pData2.m_pEntity;

		pData2.m_fDistance = pTemp.m_fDistance;
		pData2.m_pEntity = pTemp.m_pEntity;
	}

	void CheckOcclusionCulling( const VEC_DWORD* pvecDWORD )
	{
		// 불투명 물체 작업
		{
			std::vector<PIECEDISTANCE> vecPieceDistance;
			if ( pvecDWORD )
			{
				for( DWORD i=0; i<m_dwPieceCount; ++i )
				{
					if ( m_pArrayPiece[i].m_pEntity->CheckOcclusionCulling( pvecDWORD ) )
					{
						vecPieceDistance.push_back( m_pArrayPiece[i] );
					}
				}
			}

			m_dwPieceCount = static_cast<DWORD>( vecPieceDistance.size() );
			for ( DWORD i=0; i<m_dwPieceCount; ++i )
			{
				m_pArrayPiece[i] = vecPieceDistance[i];
			}
		}

		// 반투명 물체도 작업
		{
			std::vector<PIECEDISTANCE> vecPieceDistance;
			if ( pvecDWORD )
			{
				for( DWORD i=0; i<m_dwPieceCountBlend; ++i )
				{
					if ( m_pArrayPieceBlend[i].m_pEntity->CheckOcclusionCulling( pvecDWORD ) )
					{
						vecPieceDistance.push_back( m_pArrayPieceBlend[i] );
					}
				}
			}

			m_dwPieceCountBlend = static_cast<DWORD>( vecPieceDistance.size() );
			for ( DWORD i=0; i<m_dwPieceCountBlend; ++i )
			{
				m_pArrayPieceBlend[i] = vecPieceDistance[i];
			}
		}

		// 반투명 물체도 작업
		{
			std::vector<PIECEDISTANCE> vecPieceDistance;
			if ( pvecDWORD )
			{
				for( DWORD i=0; i<m_dwPieceCountPoint; ++i )
				{
					if ( m_pArrayPiecePoint[i].m_pEntity->CheckOcclusionCulling( pvecDWORD ) )
					{
						vecPieceDistance.push_back( m_pArrayPiecePoint[i] );
					}
				}
			}

			m_dwPieceCountPoint = static_cast<DWORD>( vecPieceDistance.size() );
			for ( DWORD i=0; i<m_dwPieceCountPoint; ++i )
			{
				m_pArrayPiecePoint[i] = vecPieceDistance[i];
			}
		}
	}
}