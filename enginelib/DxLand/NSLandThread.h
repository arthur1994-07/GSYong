//////////////////////////////////////////////////////////////////////////
// [shhan][2014.10.29]	���� - ������ CPU ����� Thread �� ������ ����.
//								���� Frame ������ ����۾��� �������� �Ѵ�. (�����)
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
	// ���̵��� ���� ���� ������ ��������� ���� ����

	// ����
	void DataCleanUp();




	//////////////////////////////////////////////////////////////////////////
	// Thread ������ ����

	// ����
	BOOL StartThread();

	// ������ ĳ���� ����� ���� ������Ѵ�.
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
	// Field ����
	void AddCullingMesh( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emType, boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread );


	//////////////////////////////////////////////////////////////////////////
	// LandSkin ����
	void SetLandSkinManager( const boost::shared_ptr<DxLandSkinManager>& spLandSkinManager );


	//////////////////////////////////////////////////////////////////////////
	// Piece ����
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