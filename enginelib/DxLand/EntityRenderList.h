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
// ����		: World �ʿ� Entity ���� RenderList �� ��´�.
// �۾����� : ������ �ǽð����� Tree �� Ÿ�鼭 ������ ������ �˻縦 �ߴ�.
//				CPU ���ϰ� �߻��� �ذ��ϱ� ����.



//////////////////////////////////////////////////////////////////////////
// �۾��Ǵ� Source Data
class EntityRenderList
{
private:
	BOOL m_bWLD;

private:
	// m_vecEntityBlend, m_vecEntityPoint ���� ��ü ������� ������ ��ü.
	std::vector<NSPieceQuickSort::PIECEDISTANCE>		m_vecEntity;					// �⺻.
	VEC_PIECE											m_vecEntityExceptInstancing;	// Instancing ���� ���� ���ܵ� ������ ��ü��

	// ����� �� Deffered ���� ���� AlphaBlend �Ǵ� ��ü.
	std::vector<NSPieceQuickSort::PIECEDISTANCE>		m_vecEntityBlend;

	// Deffered ���� Point �� ������ų� �����Ǵ� ��ü.
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
	// ������ DF �� �����ȴٸ� �׿� �´� �ɼǰ����� ��ȭ��Ų��.
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
	void ReSizeArrayPiece();																			// ����� 2��� �ø���.
	void ReSizeArrayPieceONLY();																		// ����� 2��� �ø���.
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
	// TnL_PS_3_0_CSM �̻�
	void RenderCacadeOpaque( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderCascadeHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderCascadeShadowReplace();
	// TnL_PS_3_0_CSM �̻�
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
// Game �ʿ��� ������ �ִ� Source Data
// Swap �� ���� �ִ�.
class EntityRenderListSwap
{
	// Swap �ʿ��� ���� �� Data
private:
	int m_nUSED;
	int m_nBackUp;
	boost::shared_ptr<EntityRenderList>					m_EntityRenderList[2];		// Swap �Ǹ鼭 ����.

public:
	void CleanUp();
	void Swap();
	boost::shared_ptr<EntityRenderList>& GetEntityRenderListForThread();




	void TestCode( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bLodBlending, BOOL bTimeBlend, BOOL bReflect,
					BOOL bUsedMaterialSystem, float fTime, float fElapsedTime, BOOL bWLD );


	// ������ DF �� �����ȴٸ� �׿� �´� �ɼǰ����� ��ȭ��Ų��.
	void SetData_Force_TnL_PS_3_0_DF();

public:
	void FrameMoveMAT( const float fTime, const float fElapsedTime );
	void FrameMoveLandSkin( float fElapsedTime );

	//////////////////////////////////////////////////////////////////////////
	// Render ���� �ڵ�
public:
	//////////////////////////////////////////////////////////////////////////
	// ��ü �� ȣ��ȴ�.
	void RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice, 
						BOOL bMPShading,
						BOOL bUsedMaterialSystem,
						BOOL bReflect,
						float fReflectHeight,
						BOOL bWLD );
	void RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMPShading, BOOL bUsedMaterialSystem, BOOL bWLD );
	void RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bWLD, BOOL bUsedMaterialSystem );
	void RenderSkinObject( BOOL bReflect, float fReflectHeight );
	// ��ü �� ȣ��ȴ�.
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// TnL_PS_1_1 ���Ͽ����� ȣ�� �ȴ�.
	void RenderEffFrame();
	void RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWLD, BOOL bMPShading, BOOL bUsedMaterialSystem );
	void RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxStaticMeshLightMap* pStaticMeshLightMap, 
							DxFrameMesh* pFrameMesh,
							BOOL bReflect,
							float fReflectHeight );
	//void Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF );

	// ENSTM_DECAL_AFTER �� ������ �۾��� �ȴ�.
	void RenderDecal();	
	// TnL_PS_1_1 ���Ͽ����� ȣ�� �ȴ�.
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// Deffered ��,��,�ֻ� �ɼ�	
	void RenderDecalAndLodBlendDecal( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderDecalPass2();
	void RenderSoftAlpha_Deffered( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderEff_PickBlend_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap );
	void RenderForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflect, float fReflectHeight );
	void RenderSoftAlphaForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderEffFrameForDeffered( BOOL bLBuffer, BOOL bReflect );
	void RenderCustom_UNLIT_UNFOG();
	void RenderWaterCustomSoftAlpha();
	// Deffered ��,��,�ֻ� �ɼ�	
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// TnL_PS_3_0_CSM �̻�
	void RenderCacadeOpaque( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderCascadeHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderCascadeShadowReplace();
	// TnL_PS_3_0_CSM �̻�
	//////////////////////////////////////////////////////////////////////////

public:
	EntityRenderListSwap();
	~EntityRenderListSwap();
};


//
////////////////////////////////////////////////////////////////////////////
//// Thread ���ο��� ���Ǵ� Data
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