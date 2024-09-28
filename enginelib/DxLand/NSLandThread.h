//////////////////////////////////////////////////////////////////////////
// [shhan][2014.10.29]	목적 - 지형의 CPU 계산을 Thread 로 돌리기 위함.
//								다음 Frame 까지는 모든작업이 끝나도록 한다. (대기함)
//						
#pragma once

#include "NSLandThreadDef.h"

class DxPieceAABB;
class LightMapRenderMan;
class DxCullingMeshSwap;
class EntityRenderListSwap;
class DxStaticAniFrame;
class DxLandSkinManager;
class DxPieceInstancing;
class DxPieceInstancingForShadow;
struct PIECE_INSTANCING_FIRST;
struct CLIPVOLUME;

template <typename T, typename T2>
class PieceInstancingThread;


namespace NSLandThread
{
	//////////////////////////////////////////////////////////////////////////
	// 맵이동과 관련 이전 데이터 사라짐으로 인한 정리

	// 정리
	void DataCleanUp();




	//////////////////////////////////////////////////////////////////////////
	// Thread 생성과 삭제

	// 생성
	BOOL StartThread();

	// 삭제는 캐릭터 지우기 전에 해줘야한다.
	void EndThread();




	//////////////////////////////////////////////////////////////////////////
	// 

	void Wait_FrameMove( const boost::shared_ptr<EntityRenderListSwap> spPieceRenderListSwap[],
						const CLIPVOLUME &cv,
						float fTime, 
						float fElapsedTime, 
						BOOL bUsedMaterialSystem,
						BOOL bUsedMaterialSystemPS_3_0,
						float fWaterHeight,
						BOOL bNotRendering );

	void Wait_FrameMoveRAD( const boost::shared_ptr<EntityRenderListSwap> spPieceRenderListSwap[],
							const CLIPVOLUME &cv );

	void FrameMoveRAD( const boost::shared_ptr<EntityRenderListSwap> spPieceRenderListSwap[], const CLIPVOLUME &cv, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt );

	BOOL IsCompleteData();

	void AddRenderList_CALCULATE_DATA_MODE( const boost::shared_ptr<EntityRenderListSwap> spPieceRenderListSwap[],
											const CLIPVOLUME* arraypCV[],
											const D3DXVECTOR3& vFromPt, 
											const D3DXVECTOR3& vLookatPt, 
											BOOL bTimeBlend, 
											float fHeight,
											BOOL bWLD );

	void Swap();



	//////////////////////////////////////////////////////////////////////////
	// Field 관련
	void AddCullingMesh( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emType, boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread );


	//////////////////////////////////////////////////////////////////////////
	// LandSkin 관련
	void SetLandSkinManager( const boost::shared_ptr<DxLandSkinManager>& spLandSkinManager );


	//////////////////////////////////////////////////////////////////////////
	// Piece 관련
	void SetPieceAABB( const boost::shared_ptr<DxPieceAABB>& spPieceAABB );

	void SetPieceInstancingMeshLM( LPDIRECT3DDEVICEQ pd3dDevice, 
									DxPieceAABB* pPieceAABB,
									const boost::shared_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>& spPieceInstancingThread );

	void SetPieceInstancingMeshReflection( LPDIRECT3DDEVICEQ pd3dDevice, 
											DxPieceAABB* pPieceAABB,
											const boost::shared_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>& spPieceInstancingThread );

	void SetPieceInstancingPureMesh( LPDIRECT3DDEVICEQ pd3dDevice, 
										DxPieceAABB* pPieceAABB,
										NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emType, 
										const boost::shared_ptr<PieceInstancingThread<DxStaticAniFrame*,DxPieceInstancingForShadow>>& spPieceInstancingThread );
};