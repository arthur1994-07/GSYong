#pragma once

#include <boost/scoped_ptr.hpp> 

#include "./DxPiece/DxPieceDef.h"
#include "./NSLandThreadDef.h"

class DxPieceAABB;
class DxStaticMeshLightMap;

template <typename T>
class DxPieceInstancingLodVB;


//////////////////////////////////////////////////////////////////////////
//							PieceInstancingSwap
template <typename T>
class PieceInstancingSwap
{
private:
	int		m_nUSED;	// 렌더 되는 번호
	int		m_nBackUp;	// Thread 에서 작업되는 번호
	BOOL	m_bDelete;	// Opaque 와 HardAlpha 가 없어서 지워져야 되는가~?

	// PieceInstancingSwap가 삭제되야할 경우가 생겼을 때 DxPiece 에 연결된 m_rPieceInstancingSwap, m_rPieceInstancingSwapForShadow 를 NULL 로 만들어 줘야 한다.
	std::vector<DxPiece*>		m_vecrPiece;

	boost::scoped_ptr<DxPieceInstancingLodVB<T>>	m_mapPieceInstancingSwap[2];

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

public:
	void Import_AddRef( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, DxPiece* pPiece );
	BOOL CreateInstancingLOD( LPDIRECT3DDEVICEQ pd3dDevice );
	void ResetInstancingLinkForPiece();
	void Swap();
	void CleanUp();
	BOOL IsDelete() const	{ return m_bDelete; }

	// Thread
public:
	void ResetRef_THREAD();
	void AddRef_THREAD();
	void Reset_THREAD();
	void Insert_THREAD( const DxPiece* pPiece );
	void InsertDataToVB_ExceptPiece_THREAD( VEC_PIECE& vecExceptInstancing );
	void InsertDataToVB_ExceptPiece_Reflect_THREAD( VEC_PIECE& vecExceptInstancing, float fHeight );

	// FrameMove
public:
	void FrameMoveStep( float fElapsedTime );

	// Render
public:
	void RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderDecal( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderDecalPass2( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	PieceInstancingSwap( DxStaticAniFrame* pStaticAniFrame );
	~PieceInstancingSwap();
};

//////////////////////////////////////////////////////////////////////////
//							PieceInstancingThread
template <typename T, typename T2>
class PieceInstancingThread
{
private:
	typedef std::map<T,boost::shared_ptr<PieceInstancingSwap<T2>>>				MAP_PIECEINSTANCINGSWAP;
	//typedef std::map<T,boost::shared_ptr<PieceInstancingSwap>>::iterator	MAP_PIECEINSTANCINGSWAP_ITER;
	MAP_PIECEINSTANCINGSWAP	m_mapPieceInstancingSwap;
	DWORD					m_dwFrameMoveStepIndex;

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

public:
	void Import_MeshLM( LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, DxPieceAABB* pPieceAABB );
	void Import_PureMesh( LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, DxPieceAABB* pPieceAABB );
	void Swap();
	void CleanUp();

private:
	void CreateInstancingLOD( LPDIRECT3DDEVICEQ pd3dDevice );

	// Thread
public:
	void ResetRef_THREAD();
	void AddRefMeshLM_THREAD( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const DxPiece* pPiece );
	void AddRefPureMesh_THREAD( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const DxPiece* pPiece );
	void Reset_THREAD();
	BOOL InsertInstancingMeshLM_THREAD( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const DxPiece* pPiece );
	BOOL InsertInstancingPureMesh_THREAD( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const DxPiece* pPiece );
	void InsertDataToVB_ExceptPiece_THREAD( VEC_PIECE& vecExceptInstancing );
	void InsertDataToVB_ExceptPiece_Reflect_THREAD( VEC_PIECE& vecExceptInstancing, float fHeight );

	// FrameMove
public:
	void FrameMoveStep( float fElapsedTime );

	// Render
public:
	void RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderDecal( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderDecalPass2( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	PieceInstancingThread();
	~PieceInstancingThread();
};

//////////////////////////////////////////////////////////////////////////
#include "./PieceInstancingThread.inl"

//////////////////////////////////////////////////////////////////////////