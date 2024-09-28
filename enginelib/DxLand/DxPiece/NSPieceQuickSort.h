#pragma once

#include "../../DxTools/DxVertexFVF.h"
#include "../PieceInstancingThread.h"
#include "./DxPieceDef.h"

struct CLIPVOLUME;
class Entity;
class DxFrameMesh;
class DxStaticMesh;
class DxStaticMeshLightMap;
class EntityRenderList;
class DxStaticAniFrame;
struct PIECE_INSTANCING_FIRST;

//----------------------------------------------------------------------------------------------------------------------
//								D	x		P	i	e	c	e		Q	u	i	c	k		S	o	r	t
//	Note : ���� ��Ʈ�Ѵ��� �Ѹ��� ������� ����. ^^;
//			�Ѱ��� ����ְ� �迭�� ������ �ϴ� �� �ΰ� ?
//----------------------------------------------------------------------------------------------------------------------
namespace NSPieceQuickSort
{
	struct PIECEDISTANCE
	{
		float		m_fDistance;
		Entity*	m_pEntity;
	};

	extern BOOL				g_bViewAlphaLOD;	// ����̿� ���� ���̰� �Ⱥ��̰��� ���� Ȱ��ȭ
	extern BOOL				g_bBakeRAD;			// RAD Bake �� LOD �� �ȵǵ��� �Ѵ�. Alpha ��ġ�� �ٷ� 1 �� �����ؾ� �Ѵ�.
	extern BOOL				g_bPieceInstancing;

	extern float OBJECT_LENGTH_NEAR_0;
	extern float OBJECT_LENGTH_NEAR_1;
	extern float OBJECT_LENGTH_FAR_0;
	extern float OBJECT_LENGTH_FAR_1;

	extern float g_fElapsedTime;

	void SetArrayMaxSize( DWORD dwCount );																// �ִ� �迭 ������ ����
	void Reset();																						// ���� �迭 ������ �ʱ�ȭ.
	void InsertData( Entity* pPiece, 
					const D3DXVECTOR3& vFromPt, 
					const D3DXVECTOR3& vLookatPt, 
					const float& fLength, 
					BOOL bLodBlending, 
					BOOL bTimeBlend );																	// ������ Piece ����Ÿ

	void InsertDataForShadow( Entity* pEntity );

	void QuickSortProcess();																			// Quick Sort �۾�
	void FrameMove( const float fTime, const float fElapsedTime );
	void FrameMoveTOOL( const float fTime, const float fElapsedTime );
	void FrameMoveTHREAD();
	//void FrameMoveMAT( const float fTime, const float fElapsedTime );
	//void FrameMoveRAD( BOOL bDay );

	// Instancing �� ���� �ȵȴ�. 
	// m_pArrayPiece �� ��� m_vecPieceExceptInstancing �� �ű���.
	void InsertAllExceptInstancing();

	// Instancing ���� Data �� ������ �Ѵ�. Piece
	void InsertInstancing( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
							PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>* pPieceInstancing,
							float fHeight );
	void InsertInstancing( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
							PieceInstancingThread<DxStaticAniFrame*,DxPieceInstancingForShadow>* pPieceInstancing,
							float fHeight );

	void RenderOpaquePSF( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bWLD, BOOL bMPShading, BOOL bUsedMaterialSystem, BOOL bReflect, float fReflectHeight );
	void RenderHardAlphaPSF( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bWLD, BOOL bMPShading, BOOL bUsedMaterialSystem );
	void RenderDecalPSF();
	void RenderDecalPass2_PSF();
	void RenderWaterCustomSoftAlphaPSF();
	void RenderSoftAlphaPSF( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bWLD, BOOL bMPShading, BOOL bUsedMaterialSystem );
	void RenderEff_PSF( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bWLD, BOOL bUsedMaterialSystem );
	void RenderForLodBlend_PSF( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderSoftAlphaForLodBlend_PSF( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderSkinObjectPSF( BOOL bReflect, float fReflectHeight );

	//void RenderEffFrame();
	//void RenderEffFrameForDeffered( BOOL bLBuffer );
	//void RenderDecalAndLodBlendDecal( LPDIRECT3DDEVICEQ pd3dDevice );
	//void RenderSoftAlpha_Deffered( LPDIRECT3DDEVICEQ pd3dDevice );
	//void RenderWater_CustomSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	
	//void RenderEff_PickBlend_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap );

	void InitPickAlpha();
	void ReSizeArrayPiece();																			// ����� 2��� �ø���.
	void ReSizeArrayPieceONLY();																		// ����� 2��� �ø���.
	void FrameMoveONLY( const float fTime, const float fElapsedTime );

	//void RenderForLodBlendForDeffered( LPDIRECT3DDEVICEQ pd3dDevice );
	//void RenderSoftAlphaForLodBlendForDeffered( LPDIRECT3DDEVICEQ pd3dDevice );

	//void RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
	//						const CLIPVOLUME &sCV, 
	//						DxStaticMeshLightMap* pStaticMeshLightMap, 
	//						DxFrameMesh* pFrameMesh, 
	//						BOOL bPSF,
	//						BOOL bReflect,
	//						float fReflectHeight, 
	//						BOOL bFrameMove );

	//void Render_Query_Prepare( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	//void Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );

	void DxQuickSort( PIECEDISTANCE pArray[], int m, int n );
	void Swap( PIECEDISTANCE& pData1, PIECEDISTANCE& pData2 );

	void CheckOcclusionCulling( const VEC_DWORD* pvecDWORD );

	void CleanUp();


	//////////////////////////////////////////////////////////////////////////
	//
	void ExportPieceRenderList_THREAD( EntityRenderList* pDest );
	void ExportLandSkinRenderList_THREAD( EntityRenderList* pDest );
}
