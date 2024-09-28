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
	// ������� �� Deffered (SoftAlpha Ȥ�� ������ ������) ���� ������� ������ ��ü.
	PIECEDISTANCE*				m_pArrayPiece(NULL);					// �迭
	DWORD						m_dwArraySize(0);						// �ִ� ������ �迭 ������
	DWORD						m_dwPieceCount(0);						// ���� �迭 ������

	// Instancing ���� ���� �����ϰ� ���� ��.
	VEC_PIECE					m_vecPieceExceptInstancing;	
	// �޸� ����ȭ�� ������ �ξ���.	��� ����ϰ� ����� ������.
	//PIECEDISTANCE*				m_pArrayPieceExceptInstancing(NULL);	// �迭
	//DWORD						m_dwArraySizeExceptInstancing(0);		// �ִ� ������ �迭 ������
	//DWORD						m_dwPieceCountExceptInstancing(0);		// ���� �迭 ������

	// ����� �� Deffered ���� ���� AlphaBlend �Ǵ� ��ü.
	PIECEDISTANCE*	m_pArrayPieceBlend = NULL;
	DWORD			m_dwArraySizeBlend = 0L;
	DWORD			m_dwPieceCountBlend = 0L;

	// Deffered ���� Point �� ������ų� �����Ǵ� ��ü.
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
	//// TnL_PS_2_0 �̻󿡼��� ȣ�� �ȴ�.
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
	//// TnL_PS_2_0 �̻󿡼��� ȣ�� �ȴ�.
	//// PickBlend �Ǵ� ��ü�� 1RenderTarget �� ���� �� �� �ִ� �͵� Rendering �ϱ�.
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
	////					������ ��ü�� LOD ó��
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
	////					SoftAlpha �� LOD ó��
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
	//// TnL_PS_1_1 ���Ͽ����� ȣ�� �ȴ�.
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

			// �ɼǻ����� ����� ���ϴ� LOD �Ÿ��� �������� ���δ�.
			// g_emGroundQulity �� wld �� material �ý��� ������ ���� ����.
			if ( RENDERPARAM::g_emTnLMode <= TnL_PS_1_1 )
			{
				VisibleRadius *= 0.5f;
				InvisibleRadius *= 0.5f;
			}

			vDir = pCur->GetCenter() - vFromPt;
			fTempLength = D3DXVec3Length( &vDir );	//vDir.x*vDir.x + vDir.y*vDir.y + vDir.z*vDir.z;

			// Note : Edit ���³� �ƴϳĿ� ���� Ȱ��ȭ �ɼ��� �ְ� �ƴ� ���� �ִ�.
			if( !g_bViewAlphaLOD )
			{
				pCur->SetAlpha( 1.f );
			}
			else if ( g_bBakeRAD )
			{
				// Voxel Color ��� �������� ���� �ʴ´�.
				if ( pCur->IsVoxelColor() )
					return;

				//// �Ÿ��� �ָ� �������� ���� �ʴ´�.
				//vDir = pCur->m_vCenter - vLookatPt;
				//fLengthLookat = D3DXVec3Length( &vDir );	// ī�޶� ������ �Ÿ�
				//fLengthLookat -= pCur->m_fRadius;			// ��ü�� ���������� ����� �ش�.
				//if ( 400.f <= fLengthLookat)
				//	return;

				pCur->SetAlpha( 1.f );
			}
			else
			{
				vDir = pCur->GetCenter() - vLookatPt;
				fLengthLookat = D3DXVec3Length( &vDir ); // ī�޶� ������ �Ÿ�
				fLengthLookat -= pCur->GetRadius(); // ��ü�� ���������� ����� �ش�.

				//// Note : �ָ��־ �����ؾ� �ϴ°�~?
				//if( pCur->m_bFarRender )
				//{
				//	pCur->m_bVisible = TRUE;
				//}
				//else

				if ( bLodBlending )
				{
					// Material System �� ���̴� �ʵ鸸 Timeblend �ý����� ���̵��� ����.
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

						// �ָ� �ִ� �ɷ� �������.
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
						fAlphaSpeed += 0.05f;		// 0.5 ~ 0.05 ~ 0.5					// ��ȭ �ӵ� ���� ����.
						fAlphaSpeed *= 1.5f;		// ��ġ2�� �뷫 1�� ���� �ɸ���.	// ��ȭ �ӵ� ���� ����.


						if ( pCur->IsAddAlpha() )
						{
							pCur->SetAlpha( pCur->GetAlpha() + (g_fElapsedTime*fAlphaSpeed) ) ;	// �þ��.

							if ( pCur->GetAlpha() > 1.f )
							{
								pCur->SetBlendAlpha( FALSE );
								pCur->SetAlpha( 1.f );
							}
						}
						else
						{
							pCur->SetAlpha( pCur->GetAlpha() - (g_fElapsedTime*fAlphaSpeed) ) ;	// ������.

							if ( pCur->GetAlpha() < 0.f )
							{
								pCur->SetAlpha( 0.f );
							}
						}

						// Note : ȭ���� �����ٸ� ����ó���� �Ѵٴ� �ɼ��� ����.
						if( pCur->GetPickAlpha() )
						{
							if( fLength > (fTempLength - pCur->GetRadius()) )
							{
								pCur->SetAlpha( pCur->GetAlpha() + (g_fElapsedTime*2.f) ) ;	// 1�ʿ� 2.f�� �þ��.

								//D3DXVECTOR3	m_vMax;			// ��ü�� Max
								//D3DXVECTOR3	m_vMin;			// ��ü�� Min
								// Note : Pick Check�� �Ѵ�.
								if ( COLLISION::IsCollisionLineToAABB( vFromPt, vLookatPt, pCur->GetStaticMax(), pCur->GetStaticMin() ) )	//	pCur->m_vMax, pCur->m_vMin ) )	//
								{
									pCur->SetBlendAlpha( TRUE );
									pCur->SetAlpha( pCur->GetAlpha() - (g_fElapsedTime*2.f*2.f) ) ;	//  1�ʿ� 2.f�� �پ���. // ��ܿ��� �÷����� �������� �Ѵ�.

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
						// Note : ��Ÿ���� �ϴ��� Ȯ���Ѵ�.	���ؾ� �ϴ� �ñ⸦ Ʋ���� ���ش�.
						// if m_bVisible is TRUE then Piece render opaque, else m_bVisible is FALSE then Piece render transparent.
						if ( fLengthLookat <= VisibleRadius )
						{
							// ���� ���� �ִٸ� ������ ���Ĵ� 1 �̴�.
							// ������ Pick �� Object�� ������ ȿ���� ���� if( pCur->GetPickAlpha() ) �κ��� �۾���.
							//pCur->GetAlpha() = 1.f;

							pCur->SetAlpha( pCur->GetAlpha() + (g_fElapsedTime*2.7f) ) ;	// 1�ʿ� 2.7f�� �þ��.

							// Note : ȭ���� �����ٸ� ����ó���� �Ѵٴ� �ɼ��� ����.
							if( pCur->GetPickAlpha() )
							{
								if( fLength > (fTempLength - pCur->GetRadius()) )
								{
									//D3DXVECTOR3	m_vMax;			// ��ü�� Max
									//D3DXVECTOR3	m_vMin;			// ��ü�� Min
									// Note : Pick Check�� �Ѵ�.
									if ( COLLISION::IsCollisionLineToAABB( vFromPt, vLookatPt, pCur->GetStaticMax(), pCur->GetStaticMin() ) )	//	pCur->m_vMax, pCur->m_vMin ) )	//
									{
										pCur->SetBlendAlpha( TRUE );
										pCur->SetAlpha( pCur->GetAlpha() - (g_fElapsedTime*2.7f*2.f) ) ;	// 1�ʿ� 2.7f�� �پ���. // ��ܿ��� �÷����� �������� �Ѵ�.

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
					pCur->SetAlpha( pCur->GetAlpha() + (g_fElapsedTime*2.7f) ) ;	// 1�ʿ� 2.7f�� �þ��.

					// Note : ȭ���� �����ٸ� ����ó���� �Ѵٴ� �ɼ��� ����.
					if( pCur->GetPickAlpha() )
					{
						if( fLength > (fTempLength - pCur->GetRadius()) )
						{
							//D3DXVECTOR3	m_vMax;			// ��ü�� Max
							//D3DXVECTOR3	m_vMin;			// ��ü�� Min
							// Note : Pick Check�� �Ѵ�.
							if ( COLLISION::IsCollisionLineToAABB( vFromPt, vLookatPt, pCur->GetStaticMax(), pCur->GetStaticMin() ) )	//	pCur->m_vMax, pCur->m_vMin ) )	//
							{
								pCur->SetBlendAlpha( TRUE );
								pCur->SetAlpha( pCur->GetAlpha() - (g_fElapsedTime*2.7f*2.f) ) ;	// 1�ʿ� 2.7f�� �پ���. // ��ܿ��� �÷����� �������� �Ѵ�.

								if( pCur->GetAlpha() < 0.24f )
								{
									pCur->SetAlpha( 0.24f );
								}
							}
						}
					}
				}
			}

			// Note : �ѷ��� �ϴ� �͵�.
			if( pCur->GetAlpha() > 0.f )
			{
				// Note : Pick AlphaBlending      
				//			�������� m_pArrayPiece ��, �������� m_pArrayPieceBlend ��, ������ ���� �ʴ´�.
				if( pCur->GetAlpha() < 1.f )
				{
					if ( pCur->IsBlendAlpha() )
					{
						// Note : �ִ� ���� ������ �Ѿ���.
						//		�迭 ũ�⸦ �÷��� �Ѵ�.
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
						// Note : �ִ� ���� ������ �Ѿ���.
						//		�迭 ũ�⸦ �÷��� �Ѵ�.
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

					// Note : �ִ� ���� ������ �Ѿ���.
					//		�迭 ũ�⸦ �÷��� �Ѵ�.
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
		// Note : �ִ� ���� ������ �Ѿ���.
		//		�迭 ũ�⸦ �÷��� �Ѵ�.
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

	// Instancing �� ���� �ȵȴ�. 
	// m_pArrayPiece �� ��� m_vecPieceExceptInstancing �� �ű���.
	void InsertAllExceptInstancing()
	{
		m_vecPieceExceptInstancing.clear();

		for ( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			DxPiece* pPiece = MIN_BOOST::polymorphic_downcast<DxPiece*>( m_pArrayPiece[i].m_pEntity );

			// Instancing �� �ȵǴ� �� �Ϲ����� �������� �ǵ��� �Ѵ�.
			m_vecPieceExceptInstancing.push_back( pPiece );
		}
	}

	// Instancing ���� Data �� ������ �Ѵ�. Piece
	void InsertInstancing( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
							PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>* pPieceInstancing,
							float fHeight )
	{
		m_vecPieceExceptInstancing.clear();

		// �� Instancing �׷츶�� ���° LodVB �� ����� �� �˾ƾ� �ؼ� ���� �˻簡 �ʿ��ϴ�.
		pPieceInstancing->ResetRef_THREAD();
		for ( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			DxPiece* pPiece = MIN_BOOST::polymorphic_downcast<DxPiece*>( m_pArrayPiece[i].m_pEntity );

			pPieceInstancing->AddRefMeshLM_THREAD( emCullingRenderType, pPiece );
		}

		// �ش� LodVB �ʿ� PieceParameter ���� �����س��´�. 
		pPieceInstancing->Reset_THREAD();

		// Instancing ������ �����Ѵ�.
		for ( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			DxPiece* pPiece = MIN_BOOST::polymorphic_downcast<DxPiece*>( m_pArrayPiece[i].m_pEntity );

			// Piece ���ؿ����� Instancing ���� ����. ( PickAlpha �� �ȴٴ��� �ϴ� ��� Instancing �Ұ� )
			if ( !pPiece->IsInstancingDynamicOption() )
			{
				// Instancing �� �ȵǴ� �� �Ϲ����� �������� �ǵ��� �Ѵ�.
				m_vecPieceExceptInstancing.push_back( pPiece );
				continue;
			}

			if ( !pPieceInstancing->InsertInstancingMeshLM_THREAD( emCullingRenderType, pPiece ) )
			{
				// Instancing �� �ȵǴ� �� �Ϲ����� �������� �ǵ��� �Ѵ�.
				m_vecPieceExceptInstancing.push_back( pPiece );
			}
		}

		// CreateVB �� �ϰ� ������ �ʹ� ���� Instancing�� ���ܵǴ� �͵� ���´�.
		
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

		// �� Instancing �׷츶�� ���° LodVB �� ����� �� �˾ƾ� �ؼ� ���� �˻簡 �ʿ��ϴ�.
		pPieceInstancing->ResetRef_THREAD();
		for ( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			DxPiece* pPiece = MIN_BOOST::polymorphic_downcast<DxPiece*>( m_pArrayPiece[i].m_pEntity );

			pPieceInstancing->AddRefPureMesh_THREAD( emCullingRenderType, pPiece );
		}

		// �ش� LodVB �ʿ� PieceParameter ���� �����س��´�. 
		pPieceInstancing->Reset_THREAD();

		// Instancing ������ �����Ѵ�.
		for ( DWORD i=0; i<m_dwPieceCount; ++i )
		{
			DxPiece* pPiece = MIN_BOOST::polymorphic_downcast<DxPiece*>( m_pArrayPiece[i].m_pEntity );

			if ( !pPieceInstancing->InsertInstancingPureMesh_THREAD( emCullingRenderType, pPiece ) )
			{
				// Instancing �� �ȵǴ� �� �Ϲ����� �������� �ǵ��� �Ѵ�.
				m_vecPieceExceptInstancing.push_back( pPiece );
			}
		}

		// CreateVB �� �ϰ� ������ �ʹ� ���� Instancing�� ���ܵǴ� �͵� ���´�.

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
		// ������ ��ü �۾�
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

		// ������ ��ü�� �۾�
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

		// ������ ��ü�� �۾�
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