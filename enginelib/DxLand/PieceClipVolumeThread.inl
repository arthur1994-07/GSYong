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

	// [shhan][2013.05.20] ���ʿ� LOD Blending �� �����Ͽ���.
	BOOL bLodBlending(TRUE);

	boost::shared_ptr<DxPieceAABB> spPieceAABB = m_wpPieceAABB.lock(); 
	if ( spPieceAABB ) 
	{
		NSPieceQuickSort::InitPickAlpha();
		NSPieceQuickSort::Reset();

		// NSPieceQuickSort �� ������ �Ǵ� Data ���� ����.
		spPieceAABB->InsertQuickSort
		( 
			m_sCV[emCullingRenderType], 
			m_vFromPt, 
			m_vLookatPt, 
			bLodBlending, 
			m_bTimeBlend, 
			emCullingRenderType==NSLandThread::TYPE_REFLECTION_CULL ? TRUE : FALSE 
		);

		// Instancing �ɸ� �� �͵��� �������� �Ѵ�.
		//
		// HW �� �����ϸ� TnL_PS_1_1 �� TnL_FIXED ���� Instancing �ǵ��� �ϸ� ������,
		// �׷��Ա��� �����ʿ�� ����δ�.
		//
		// Lightmap test �� �� ���� Instancing �� ���� �ʴ´�.
		//
		if ( pPieceInstancing )
		{
			if ( NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_TEXTURE_LIGHTMAP )
			{
				if ( (RENDERPARAM::g_emGroundQulityTHREAD >= TnL_PS_3_0_DF) )
				{
#ifdef _RELEASED
					if ( NSPieceQuickSort::g_bPieceInstancing )	// NSPieceQuickSort::g_bPieceInstaning �� WorldEdit ���� �׽�Ʈ ���̴�.
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

		// Piece ���� �۾��̴�.
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

		// Piece ���� �۾��̴�.
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

		// NSPieceQuickSort �� ������ �Ǵ� Data ���� ����.
		spPieceAABB->InsertQuickSortForShadow( m_sCV[emCullingRenderType] );

		// Instancing �ɸ� �� �͵��� �������� �Ѵ�.
		// ���� CastShadow �� ���� 3.0 �� �̻�Ǿ� ���۵ǹǷ� Instancing �� ����� �����ϴ�.
		if ( pPieceInstancing )
		{
#ifdef _RELEASED
			if ( NSPieceQuickSort::g_bPieceInstancing )	// NSPieceQuickSort::g_bPieceInstaning �� WorldEdit ���� �׽�Ʈ ���̴�.
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

		// Piece ���� �۾��̴�.
		NSPieceQuickSort::ExportPieceRenderList_THREAD( pDest );
	}

	boost::shared_ptr<DxLandSkinManager> spLandSkinManager = m_wpLandSkinManager.lock(); 
	if ( spLandSkinManager ) 
	{
		NSPieceQuickSort::InitPickAlpha();
		NSPieceQuickSort::Reset();

		// NSPieceQuickSort �� ������ �Ǵ� Data ���� ����.
		spLandSkinManager->InsertQuickSortForShadow( m_sCV[emCullingRenderType] );

		NSPieceQuickSort::ExportLandSkinRenderList_THREAD( pDest );
	}
}