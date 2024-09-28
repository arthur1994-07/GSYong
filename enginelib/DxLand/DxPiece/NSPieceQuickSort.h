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
//	Note : 괜히 소트한다음 뿌리면 느릴까봐 걱정. ^^;
//			한곳에 집어넣고 배열로 정렬을 하는 것 인가 ?
//----------------------------------------------------------------------------------------------------------------------
namespace NSPieceQuickSort
{
	struct PIECEDISTANCE
	{
		float		m_fDistance;
		Entity*	m_pEntity;
	};

	extern BOOL				g_bViewAlphaLOD;	// 뷰길이에 따른 보이고 안보이고의 동작 활성화
	extern BOOL				g_bBakeRAD;			// RAD Bake 면 LOD 는 안되도록 한다. Alpha 수치도 바로 1 로 변경해야 한다.
	extern BOOL				g_bPieceInstancing;

	extern float OBJECT_LENGTH_NEAR_0;
	extern float OBJECT_LENGTH_NEAR_1;
	extern float OBJECT_LENGTH_FAR_0;
	extern float OBJECT_LENGTH_FAR_1;

	extern float g_fElapsedTime;

	void SetArrayMaxSize( DWORD dwCount );																// 최대 배열 사이즈 셋팅
	void Reset();																						// 현재 배열 사이즈 초기화.
	void InsertData( Entity* pPiece, 
					const D3DXVECTOR3& vFromPt, 
					const D3DXVECTOR3& vLookatPt, 
					const float& fLength, 
					BOOL bLodBlending, 
					BOOL bTimeBlend );																	// 무작위 Piece 데이타

	void InsertDataForShadow( Entity* pEntity );

	void QuickSortProcess();																			// Quick Sort 작업
	void FrameMove( const float fTime, const float fElapsedTime );
	void FrameMoveTOOL( const float fTime, const float fElapsedTime );
	void FrameMoveTHREAD();
	//void FrameMoveMAT( const float fTime, const float fElapsedTime );
	//void FrameMoveRAD( BOOL bDay );

	// Instancing 이 동작 안된다. 
	// m_pArrayPiece 를 모두 m_vecPieceExceptInstancing 로 옮기자.
	void InsertAllExceptInstancing();

	// Instancing 관련 Data 를 빼도록 한다. Piece
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
	void ReSizeArrayPiece();																			// 사이즈를 2배로 늘린다.
	void ReSizeArrayPieceONLY();																		// 사이즈를 2배로 늘린다.
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
