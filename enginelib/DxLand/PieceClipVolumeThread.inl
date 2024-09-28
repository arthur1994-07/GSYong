#pragma once

#include "./LightMap/NSLightMapFX.h"
#include "./DxPiece/NSPieceQuickSort.h"
#include "./DxPiece/DxPieceAABB.h"
#include "./EntityRenderList.h"
#include "./DXLANDSKINOBJ.h"

template <typename T, typename T2>
void PieceClipVolumeThread::CalculateRenderListTHREAD( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
														  EntityRenderList* pDest,
														  PieceInstancingThread<T,T2>* pPieceInstancing )
{
	if ( !pDest )
		return;

	if ( !m_bEnable[emCullingRenderType] )
		return;

	// [shhan][2013.05.20] 모든맵에 LOD Blending 을 적용하였음.
	BOOL bLodBlending(TRUE);

	boost::shared_ptr<DxPieceAABB> spPieceAABB = m_wpPieceAABB.lock(); 
	if ( spPieceAABB ) 
	{
		NSPieceQuickSort::InitPickAlpha();
		NSPieceQuickSort::Reset();

		// NSPieceQuickSort 에 보여야 되는 Data 들이 들어간다.
		spPieceAABB->InsertQuickSort
		( 
			m_sCV[emCullingRenderType], 
			m_vFromPt, 
			m_vLookatPt, 
			bLodBlending, 
			m_bTimeBlend, 
			emCullingRenderType==NSLandThread::TYPE_REFLECTION_CULL ? TRUE : FALSE 
		);

		// Instancing 될만 한 것들을 빼내도록 한다.
		//
		// HW 만 가능하면 TnL_PS_1_1 나 TnL_FIXED 에서 Instancing 되도록 하면 되지만,
		// 그렇게까지 해줄필요는 없어보인다.
		//
		// Lightmap test 로 볼 때는 Instancing 을 하지 않는다.
		//
		if ( pPieceInstancing )
		{
			if ( NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_TEXTURE_LIGHTMAP )
			{
				if ( (RENDERPARAM::g_emGroundQulityTHREAD >= TnL_PS_3_0_DF) )
				{
#ifdef _RELEASED
					if ( NSPieceQuickSort::g_bPieceInstancing )	// NSPieceQuickSort::g_bPieceInstaning 는 WorldEdit 에서 테스트 용이다.
					{
						NSPieceQuickSort::InsertInstancing( emCullingRenderType, pPieceInstancing, m_fHeight );
					}
					else
					{
						NSPieceQuickSort::InsertAllExceptInstancing();
						pPieceInstancing->Reset_THREAD();
					}
#else
					NSPieceQuickSort::InsertInstancing( emCullingRenderType, pPieceInstancing, m_fHeight );
#endif		
				}
				else
				{
					NSPieceQuickSort::InsertAllExceptInstancing();
					pPieceInstancing->Reset_THREAD();
				}
			}
			else
			{
				NSPieceQuickSort::InsertAllExceptInstancing();
				pPieceInstancing->Reset_THREAD();
			}
		}
		else
		{
			NSPieceQuickSort::InsertAllExceptInstancing();
		}

		// Piece 관련 작업이다.
		NSPieceQuickSort::QuickSortProcess();
		NSPieceQuickSort::ExportPieceRenderList_THREAD( pDest );
	}

	boost::shared_ptr<DxLandSkinManager> spLandSkinManager = m_wpLandSkinManager.lock(); 
	if ( spLandSkinManager ) 
	{
		NSPieceQuickSort::InitPickAlpha();
		NSPieceQuickSort::Reset();

		spLandSkinManager->InsertQuickSort
		( 
			m_sCV[emCullingRenderType], 
			m_vFromPt, 
			m_vLookatPt, 
			bLodBlending, 
			m_bTimeBlend, 
			emCullingRenderType==NSLandThread::TYPE_REFLECTION_CULL ? TRUE : FALSE 
		);

		// Piece 관련 작업이다.
		NSPieceQuickSort::QuickSortProcess();
		NSPieceQuickSort::ExportLandSkinRenderList_THREAD( pDest );
	}
}

template <typename T, typename T2>
void PieceClipVolumeThread::CalculateRenderListTHREAD_Shadow( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
															  EntityRenderList* pDest,
															  PieceInstancingThread<T,T2>* pPieceInstancing )
{
	if ( !pDest )
		return;

	if ( !m_bEnable[emCullingRenderType] )
		return;

	boost::shared_ptr<DxPieceAABB> spPieceAABB = m_wpPieceAABB.lock(); 
	if ( spPieceAABB ) 
	{
		NSPieceQuickSort::InitPickAlpha();
		NSPieceQuickSort::Reset();

		// NSPieceQuickSort 에 보여야 되는 Data 들이 들어간다.
		spPieceAABB->InsertQuickSortForShadow( m_sCV[emCullingRenderType] );

		// Instancing 될만 한 것들을 빼내도록 한다.
		// 지형 CastShadow 는 버젼 3.0 은 이상되야 동작되므로 Instancing 이 충분히 가능하다.
		if ( pPieceInstancing )
		{
#ifdef _RELEASED
			if ( NSPieceQuickSort::g_bPieceInstancing )	// NSPieceQuickSort::g_bPieceInstaning 는 WorldEdit 에서 테스트 용이다.
			{
				NSPieceQuickSort::InsertInstancing( emCullingRenderType, pPieceInstancing, 0.f );
			}
			else
			{
				NSPieceQuickSort::InsertAllExceptInstancing();
				pPieceInstancing->Reset_THREAD();
			}
#else
			NSPieceQuickSort::InsertInstancing( emCullingRenderType, pPieceInstancing, 0.f );
#endif		
		}
		else
		{
			NSPieceQuickSort::InsertAllExceptInstancing();
		}

		// Piece 관련 작업이다.
		NSPieceQuickSort::ExportPieceRenderList_THREAD( pDest );
	}

	boost::shared_ptr<DxLandSkinManager> spLandSkinManager = m_wpLandSkinManager.lock(); 
	if ( spLandSkinManager ) 
	{
		NSPieceQuickSort::InitPickAlpha();
		NSPieceQuickSort::Reset();

		// NSPieceQuickSort 에 보여야 되는 Data 들이 들어간다.
		spLandSkinManager->InsertQuickSortForShadow( m_sCV[emCullingRenderType] );

		NSPieceQuickSort::ExportLandSkinRenderList_THREAD( pDest );
	}
}