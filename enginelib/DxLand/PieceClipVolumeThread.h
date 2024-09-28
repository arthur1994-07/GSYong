#pragma once

#include <boost/weak_ptr.hpp> 

#include "./DxPiece/DxPieceDef.h"
#include "./NSLandThreadDef.h"

class DxPieceAABB;
class DxStaticAniFrame;
class EntityRenderList;
class DxLandSkinManager;

template <typename T, typename T2>
class PieceInstancingThread;
//template <> class PieceInstancingThread<DxStaticAniFrame*,DxPieceInstancingForShadow>;


class PieceClipVolumeThread
{
private:
	boost::weak_ptr<DxPieceAABB>		m_wpPieceAABB;			// 참조만 할 생각이라 weak_ptr
	boost::weak_ptr<DxLandSkinManager>	m_wpLandSkinManager;	// 참조만 할 생각이라 weak_ptr


	BOOL		m_bEnable[NSLandThread::TYPE_CULL_MODE_SIZE];	// 현재 모드 작동하는가~?
	CLIPVOLUME	m_sCV[NSLandThread::TYPE_CULL_MODE_SIZE];
	D3DXVECTOR3 m_vFromPt;
	D3DXVECTOR3 m_vLookatPt;
	BOOL		m_bTimeBlend;
	float		m_fHeight;
	BOOL		m_bWld;

public:
	void SetPieceAABB( const boost::shared_ptr<DxPieceAABB>& spPieceAABB );
	void SetLandSkinManager( const boost::shared_ptr<DxLandSkinManager>& spLandSkinManager );

	void InsertNeedData( const CLIPVOLUME* arraypCV[],
						const D3DXVECTOR3& vFromPt, 
						const D3DXVECTOR3& vLookatPt, 
						BOOL bTimeBlend,
						float fHeight,
						BOOL bWLD);

	// Thread
public:
	template <typename T, typename T2>
	void CalculateRenderListTHREAD( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
									EntityRenderList* pDest,
									PieceInstancingThread<T,T2>* pPieceInstancing );

	template <typename T, typename T2>
	void CalculateRenderListTHREAD_Shadow( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
											EntityRenderList* pDest,
											PieceInstancingThread<T,T2>* pPieceInstancing );

	void CalculateRenderListTHREAD_Spot_Shadow( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
												EntityRenderList* pDest );

public:
	PieceClipVolumeThread();
	~PieceClipVolumeThread();
};


//////////////////////////////////////////////////////////////////////////
#include "./PieceClipVolumeThread.inl"

//////////////////////////////////////////////////////////////////////////