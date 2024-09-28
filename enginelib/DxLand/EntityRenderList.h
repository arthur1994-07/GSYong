#pragma once

#include <boost/weak_ptr.hpp> 

#include "../DxTools/DxCustomTypes.h"
#include "./DxPiece/NSPieceQuickSort.h"

class DxPiece;
class DxLandMan;
class DxPieceAABB;
class DxLandSkinManager;
class DxStaticMeshLightMap;
class DxStaticMaterialMeshLightMap;
struct CLIPVOLUME;

namespace NSPieceQuickSort
{
	struct PIECEDISTANCE;
};

//////////////////////////////////////////////////////////////////////////
//		EntityRenderList	&	EntityRenderListSwap	&	EntityRenderListThread
//
// 설명		: World 쪽에 Entity 들의 RenderList 를 얻는다.
// 작업이유 : 기존은 실시간으로 Tree 를 타면서 렌더링 조건을 검사를 했다.
//				CPU 부하가 발생을 해결하기 위함.



//////////////////////////////////////////////////////////////////////////
// 작업되는 Source Data
class EntityRenderList
{
private:
	BOOL m_bWLD;

private:
	// m_vecEntityBlend, m_vecEntityPoint 제외 물체 원래대로 나오는 물체.
	std::vector<NSPieceQuickSort::PIECEDISTANCE>		m_vecEntity;					// 기본.
	VEC_PIECE											m_vecEntityExceptInstancing;	// Instancing 으로 인한 제외된 상태의 물체들

	// 저사양 및 Deffered 에서 강제 AlphaBlend 되는 물체.
	std::vector<NSPieceQuickSort::PIECEDISTANCE>		m_vecEntityBlend;

	// Deffered 에서 Point 로 사라지거나 생성되는 물체.
	std::vector<NSPieceQuickSort::PIECEDISTANCE>		m_vecEntityPoint;

	//////////////////////////////////////////////////////////////////////////
	//						LandSkinObject
	std::vector<NSPieceQuickSort::PIECEDISTANCE>		m_vecLandSkin;
	std::vector<NSPieceQuickSort::PIECEDISTANCE>		m_vecLandSkinBlend;
	std::vector<NSPieceQuickSort::PIECEDISTANCE>		m_vecLandSkinPoint;

public:
	void ClearPieceList();
	void ClearLandSkinList();

public:
	// 강제로 DF 로 렌더된다면 그에 맞는 옵션값들을 변화시킨다.
	void SetData_Force_TnL_PS_3_0_DF();

public:
	void FrameMoveMAT( const float fTime, const float fElapsedTime );
	void RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWLD, BOOL bMPShading, BOOL bUsedMaterialSystem, BOOL bReflect, float fReflectHeight );
	void RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWLD, BOOL bMPShading, BOOL bUsedMaterialSystem );
	void RenderEffFrame();
	void RenderEffFrameForDeffered( BOOL bLBuffer, BOOL bReflect );
	void RenderDecal();
	void RenderDecalAndLodBlendDecal( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderDecalPass2();
	void RenderCustom_UNLIT_UNFOG();
	void RenderWaterCustomSoftAlpha();
	void RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWLD, BOOL bMPShading, BOOL bUsedMaterialSystem );
	void RenderSoftAlpha_Deffered( LPDIRECT3DDEVICEQ pd3dDevice );
	//void RenderWater_CustomSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bWLD, BOOL bUsedMaterialSystem );
	void RenderEff_PickBlend_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap );

	void InitPickAlpha();
	void ReSizeArrayPiece();																			// 사이즈를 2배로 늘린다.
	void ReSizeArrayPieceONLY();																		// 사이즈를 2배로 늘린다.
	void FrameMoveONLY( const float fTime, const float fElapsedTime );

	void RenderForLodBlendForDeffered( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflect, float fReflectHeight );
	void RenderSoftAlphaForLodBlendForDeffered( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderForLodBlend_PSF( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderSoftAlphaForLodBlend_PSF( LPDIRECT3DDEVICEQ pd3dDevice );

	void RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxStaticMeshLightMap* pStaticMeshLightMap, 
							DxFrameMesh* pFrameMesh, 
							BOOL bPSF,
							BOOL bReflect,
							float fReflectHeight, 
							BOOL bFrameMove );

	//////////////////////////////////////////////////////////////////////////
	// TnL_PS_3_0_CSM 이상
	void RenderCacadeOpaque( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderCascadeHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderCascadeShadowReplace();
	// TnL_PS_3_0_CSM 이상
	//////////////////////////////////////////////////////////////////////////

	void Render_Query_Prepare( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );

	//////////////////////////////////////////////////////////////////////////
	// LandSkinObject
	void FrameMoveLandSkin( float fElapsedTime );
	void RenderSkinObject( BOOL bReflect, float fReflectHeight );

	//////////////////////////////////////////////////////////////////////////
	// Thread
public:
	//void CalculateRenderListTHREAD( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bTimeBlend, BOOL bReflect,
	//								BOOL bUsedMaterialSystem, float fTime, float fElapsedTime, BOOL bWLD );

	void ImportTHREAD( const VEC_PIECE& vecPieceExceptInstancing,
						NSPieceQuickSort::PIECEDISTANCE* pArrayPiece, DWORD dwEntityCount, 
						NSPieceQuickSort::PIECEDISTANCE* pArrayPieceBlend, DWORD dwEntityCountBlend, 
						NSPieceQuickSort::PIECEDISTANCE* pArrayPiecePoint, DWORD dwEntityCountPoint );

	void ImportTHREAD_LandSkin( NSPieceQuickSort::PIECEDISTANCE* pArrayPiece, DWORD dwEntityCount, 
								NSPieceQuickSort::PIECEDISTANCE* pArrayPieceBlend, DWORD dwEntityCountBlend, 
								NSPieceQuickSort::PIECEDISTANCE* pArrayPiecePoint, DWORD dwEntityCountPoint );

	void GetInstancingEntity( std::vector<Entity*>& vecrEntity );

	void FrameMoveTHREAD( float fElapsedTime );

public:
	EntityRenderList();
	~EntityRenderList();
};



//////////////////////////////////////////////////////////////////////////
// Game 쪽에서 가지고 있는 Source Data
// Swap 을 위해 있다.
class EntityRenderListSwap
{
	// Swap 필요한 정보 및 Data
private:
	int m_nUSED;
	int m_nBackUp;
	boost::shared_ptr<EntityRenderList>					m_EntityRenderList[2];		// Swap 되면서 사용됨.

public:
	void CleanUp();
	void Swap();
	boost::shared_ptr<EntityRenderList>& GetEntityRenderListForThread();




	void TestCode( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bLodBlending, BOOL bTimeBlend, BOOL bReflect,
					BOOL bUsedMaterialSystem, float fTime, float fElapsedTime, BOOL bWLD );


	// 강제로 DF 로 렌더된다면 그에 맞는 옵션값들을 변화시킨다.
	void SetData_Force_TnL_PS_3_0_DF();

public:
	void FrameMoveMAT( const float fTime, const float fElapsedTime );
	void FrameMoveLandSkin( float fElapsedTime );

	//////////////////////////////////////////////////////////////////////////
	// Render 관련 코드
public:
	//////////////////////////////////////////////////////////////////////////
	// 전체 다 호출된다.
	void RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice, 
						BOOL bMPShading,
						BOOL bUsedMaterialSystem,
						BOOL bReflect,
						float fReflectHeight,
						BOOL bWLD );
	void RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMPShading, BOOL bUsedMaterialSystem, BOOL bWLD );
	void RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bWLD, BOOL bUsedMaterialSystem );
	void RenderSkinObject( BOOL bReflect, float fReflectHeight );
	// 전체 다 호출된다.
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// TnL_PS_1_1 이하에서만 호출 된다.
	void RenderEffFrame();
	void RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWLD, BOOL bMPShading, BOOL bUsedMaterialSystem );
	void RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxStaticMeshLightMap* pStaticMeshLightMap, 
							DxFrameMesh* pFrameMesh,
							BOOL bReflect,
							float fReflectHeight );
	//void Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF );

	// ENSTM_DECAL_AFTER 는 렌더링 작업이 된다.
	void RenderDecal();	
	// TnL_PS_1_1 이하에서만 호출 된다.
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// Deffered 중,상,최상 옵션	
	void RenderDecalAndLodBlendDecal( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderDecalPass2();
	void RenderSoftAlpha_Deffered( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderEff_PickBlend_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap );
	void RenderForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflect, float fReflectHeight );
	void RenderSoftAlphaForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderEffFrameForDeffered( BOOL bLBuffer, BOOL bReflect );
	void RenderCustom_UNLIT_UNFOG();
	void RenderWaterCustomSoftAlpha();
	// Deffered 중,상,최상 옵션	
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// TnL_PS_3_0_CSM 이상
	void RenderCacadeOpaque( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderCascadeHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderCascadeShadowReplace();
	// TnL_PS_3_0_CSM 이상
	//////////////////////////////////////////////////////////////////////////

public:
	EntityRenderListSwap();
	~EntityRenderListSwap();
};


//
////////////////////////////////////////////////////////////////////////////
//// Thread 내부에서 사용되는 Data
//struct EntityRenderListThread
//{
//	boost::weak_ptr<EntityRenderList>	m_wpEntityRenderList;
//
//	EntityRenderListThread()
//	{
//	}
//
//	void Import( const boost::shared_ptr<EntityRenderList>& spPieceRenderList, 
//				const CLIPVOLUME* pCV )
//	{
//		if ( pCV )
//		{
//			//m_sCV = *pCV;
//			m_wpEntityRenderList = spPieceRenderList;
//		}
//		else
//		{
//			m_wpEntityRenderList.reset();
//		}
//	}
//
//	//void CalculateRenderListTHREAD()
//	//{
//	//	boost::shared_ptr<EntityRenderList> spPieceRenderList = m_wpEntityRenderList.lock();
//	//	if ( spPieceRenderList )
//	//	{
//	//		spPieceRenderList->CalculateRenderListTHREAD( m_sCV, m_vFromPt, m_vLookatPt, m_bTimeBlend, m_bReflect, m_bUsedMaterialSystem, m_fTime, m_fElapsedTime, m_bWld );
//	//	}
//	//}
//
//	boost::shared_ptr<EntityRenderList> GetEntityRenderList()
//	{
//		return m_wpEntityRenderList.lock();
//	}
//};