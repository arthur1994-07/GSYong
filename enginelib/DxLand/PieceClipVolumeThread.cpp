#include "pch.h"

#include "./DxPiece/NSPieceQuickSort.h"
#include "./DxPiece/DxPieceAABB.h"
#include "./DxPiece/DxPieceDef.h"
#include "./EntityRenderList.h"

#include "PieceClipVolumeThread.h"


PieceClipVolumeThread::PieceClipVolumeThread()
	: m_vFromPt(0.f,0.f,0.f)
	, m_vLookatPt(0.f,0.f,0.f)
	, m_bTimeBlend(TRUE)
	, m_fHeight(0.f)
	, m_bWld(TRUE)
{
	for ( DWORD i=0; i<NSLandThread::TYPE_CULL_MODE_SIZE; ++i )
	{
		m_bEnable[i] = FALSE;
	}
}

PieceClipVolumeThread::~PieceClipVolumeThread()
{
}

void PieceClipVolumeThread::SetPieceAABB( const boost::shared_ptr<DxPieceAABB>& spPieceAABB )
{
	m_wpPieceAABB = spPieceAABB;
}

void PieceClipVolumeThread::SetLandSkinManager( const boost::shared_ptr<DxLandSkinManager>& spLandSkinManager )
{
	m_wpLandSkinManager = spLandSkinManager;
}

void PieceClipVolumeThread::InsertNeedData( const CLIPVOLUME* arraypCV[],
											const D3DXVECTOR3& vFromPt, 
											const D3DXVECTOR3& vLookatPt, 
											BOOL bTimeBlend,
											float fHeight,
											BOOL bWLD )
{
	m_vFromPt = vFromPt;
	m_vLookatPt = vLookatPt;
	m_bTimeBlend = bTimeBlend;
	m_fHeight = fHeight;
	m_bWld = bWLD;

	for ( DWORD i=0; i<NSLandThread::TYPE_CULL_MODE_SIZE; ++i )
	{
		if ( arraypCV[i] )
		{
			m_bEnable[i] = TRUE;
			m_sCV[i] = *arraypCV[i];
		}
		else
		{
			m_bEnable[i] = FALSE;
		}
	}
}

void PieceClipVolumeThread::CalculateRenderListTHREAD_Spot_Shadow( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
																  EntityRenderList* pDest )
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

		NSPieceQuickSort::InsertAllExceptInstancing();

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