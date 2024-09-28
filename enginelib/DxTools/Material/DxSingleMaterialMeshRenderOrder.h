// NsOcTree.h: interface for the CCollision class.
//
//	class DxAABBOctree : 정점 데이터와 면 갯수를 넣으면 Octree 형식의 최적화된 OptimizeMesh 를 만든다.
//
//	Name : Sung-Hwan Han
//	Begin :2004/10/21
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../../DxLand/DxPiece/DxPieceTexEff.h"
#include "../../DxLand/LightMap/LightMapDef.h"
#include "../../DxLand/NSLandThreadDef.h"
#include "../../DxLand/DxLandDef.h"
#include "../Collision.h"
#include "./MaterialDef.h"

class DxDecal;
class DxTexEffBase;
class SVEC_LIGHTMAPOBJ;
class DxSingleMaterialMesh;
struct LightMapBoxColorData;
struct PieceParameter;

struct DxSingleMaterialMeshRenderOrder
{
	enum
	{
		DECAL_LAYER_NUM = 3,
	};

	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH	m_mapUnKnownTextureType;

	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH	m_mapOpaque;
	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH	m_mapHardAlpha;
	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH	m_mapSoftAlpha00;
	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH	m_mapSoftAlpha01;
	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH	m_mapSoftAlpha02;

	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH	m_mapDecalTest[DECAL_LAYER_NUM];
	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH	m_mapDecalBlend[DECAL_LAYER_NUM];
	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH	m_mapDecalMutiply[DECAL_LAYER_NUM];
	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH	m_mapDecalPass2;

	MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH	m_mapWater;		// 실시간 반사 가능
	MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH	m_mapCustomSoftAlpha;	// 실시간 반사 불가능

	MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH	m_mapCustomUNLIT_UNFOG;		// 라이트를 안 받도록 함

	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH	m_mapLightMap;	// 라이트맵 정보 렌더링

	//std::set<ID3DXEffect*>	m_setFX;	// 참조로만. 새로운 것이 들어온다면 그 때 SetFX_CommonParameter 를 호출하기 위해 존재함.

	void RenderMesh( const LPDIRECT3DDEVICEQ pd3dDevice, 
					NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
					LPDIRECT3DTEXTUREQ pTextureCombined, 
					LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_1,
					LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_2,
					LPDIRECT3DTEXTUREQ pTextureIndirect,
					Material* pMaterial,
					MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh,
					BOOL bMPShading,
					UINT unPass,
					BOOL bLightMapTest,
					D3DXVECTOR4* pvAmbientCube,
					D3DXVECTOR4& vVoxelColor,
					const D3DXMATRIX& matWorld,
					TEXTURE_ALPHA emAlphaType,
					BOOL bDecal );

	void RenderMesh_NEWFX( const LPDIRECT3DDEVICEQ pd3dDevice, 
							NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
							LPDIRECT3DTEXTUREQ pTextureCombined, 
							Material* pMaterial,
							MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh,
							UINT unPass,
							D3DXVECTOR4* pvAmbientCube,
							D3DXVECTOR4& vVoxelColor,
							const D3DXMATRIX& matWorld,
							ID3DXEffect* pFX,
							NSMaterialManager::FX_TYPE emFxType );

	void RenderMeshWater( const LPDIRECT3DDEVICEQ pd3dDevice, 
							NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
							Material* pMaterial,
							VEC_SINGLE_MATERIAL_MESH& vecMesh,
							float fPrevHeight, 
							float &fHeight,
							float &fNearLength, 
							const D3DXMATRIX& matWorld );

	void RenderMeshCustomMaterial( const LPDIRECT3DDEVICEQ pd3dDevice, 
									NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
									Material* pMaterial,
									VEC_SINGLE_MATERIAL_MESH& vecMesh, 
									const D3DXMATRIX& matWorld );

	void RenderPiece( PieceParameter& sParameter,
						Material* pMaterial,
						MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh,
						BOOL bMPShading,
						UINT unPass,
						BOOL bLightMapTest,
						TEXTURE_ALPHA emAlphaType,
						BOOL bDecal );

	void RenderPiece_NEWFX( PieceParameter& sParameter,
							MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh,
							UINT unPass,
							ID3DXEffect* pFX,
							NSMaterialManager::FX_TYPE emFxType );

	void RenderMeshFixed( const LPDIRECT3DDEVICEQ pd3dDevice, 
							NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
							MAP_TYPE_VEC_SINGLE_MATERIAL_MESH* pmapType_vecMesh,
							VEC_SINGLE_MATERIAL_MESH* pvecMesh );

	void RenderPiece_Instancing( const PieceParameter& sParameter,
									Material* pMaterial,
									MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh,
									UINT unPass,
									BOOL bDecal );

	void RenderPiece_Instancing_NEWFX( const PieceParameter& sParameter,
										MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapType_vecMesh,
										UINT unPass,
										ID3DXEffect* pFX,
										NSMaterialManager::FX_TYPE emFxType );

	void RenderOpaque( const LPDIRECT3DDEVICEQ pd3dDevice, 
						NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
						LPDIRECT3DTEXTUREQ pTextureCombined, 
						LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_1,
						LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_2,
						LPDIRECT3DTEXTUREQ pTextureIndirect,
						BOOL bMPShading,
						D3DXVECTOR4* pvAmbientCube,
						D3DXVECTOR4& vVoxelColor,
						const D3DXMATRIX& matWorld );

	void RenderHardAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
						NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
						LPDIRECT3DTEXTUREQ pTextureCombined, 
						LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_1,
						LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_2,
						LPDIRECT3DTEXTUREQ pTextureIndirect,
						BOOL bMPShading,
						D3DXVECTOR4* pvAmbientCube,
						D3DXVECTOR4& vVoxelColor,
						const D3DXMATRIX& matWorld );

	void RenderSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
						NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
						LPDIRECT3DTEXTUREQ pTextureCombined, 
						LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_1,
						LPDIRECT3DTEXTUREQ pTextureDirect_Shadow_2,
						LPDIRECT3DTEXTUREQ pTextureIndirect,
						BOOL bMPShading,
						D3DXVECTOR4* pvAmbientCube,
						D3DXVECTOR4& vVoxelColor,
						const D3DXMATRIX& matWorld );

	void RenderDecal( const LPDIRECT3DDEVICEQ pd3dDevice, 
						NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
						BOOL bMPShading,
						D3DXVECTOR4* pvAmbientCube,
						D3DXVECTOR4& vVoxelColor,
						const D3DXMATRIX& matWorld );

	void RenderDecalPass2( const LPDIRECT3DDEVICEQ pd3dDevice, 
							NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
							D3DXVECTOR4* pvAmbientCube,
							D3DXVECTOR4& vVoxelColor,
							const D3DXMATRIX& matWorld );

	void RenderWater( const LPDIRECT3DDEVICEQ pd3dDevice, 
						NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
						float fPrevHeight, 
						float &fHeight, 
						float &fNearLength, 
						const D3DXMATRIX& matWorld );

	void RenderCustomUNLIT_UNFOG( const LPDIRECT3DDEVICEQ pd3dDevice, 
									NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
									const D3DXMATRIX& matWorld );

	void RenderCustomSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
								NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
								const D3DXMATRIX& matWorld );

	void RenderPieceOpaque( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend );
	void RenderPieceHardAlpha( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend );
	void RenderPieceSoftAlpha( PieceParameter& sParameter, BOOL bMPShading );
	void RenderPieceSoftAlphaForPSF( PieceParameter& sParameter );
	void RenderPieceDecal( PieceParameter& sParameter );
	void RenderPieceDecalPass2( PieceParameter& sParameter );
	void RenderPieceCustomUNLIT_UNFOG( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld );
	void RenderPieceWaterCustomSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld );

	void RenderPieceOpaque_Instancing( const PieceParameter& sParameter );
	void RenderPieceHardAlpha_Instancing( const PieceParameter& sParameter );
	void RenderPieceDecal_Instancing( const PieceParameter& sParameter );
	void RenderPieceDecalPass2_Instancing( const PieceParameter& sParameter );

	void RenderCascadeShadowOpaque( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType );
	void RenderCascadeShadowHardAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType );
	void RenderCascadeShadowOpaque_Instancing( const LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderCascadeShadowHardAlpha_Instancing( const LPDIRECT3DDEVICEQ pd3dDevice );

	void Render_CharShadow( const LPDIRECT3DDEVICEQ pd3dDevice, 
							const CLIPVOLUME* pCV );

	void Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, 
						const CLIPVOLUME* pCV );

	void SetTextureTypeForUnKnownTexture( LPDIRECT3DDEVICEQ pd3dDevice, DWORD& dwAddTexType );
	BOOL IsTextureTypeComplete();

	//BOOL IsValidData();

	void CleanUp()
	{
		m_mapUnKnownTextureType.clear();

		m_mapOpaque.clear();
		m_mapHardAlpha.clear();
		m_mapSoftAlpha00.clear();
		m_mapSoftAlpha01.clear();
		m_mapSoftAlpha02.clear();

		for ( DWORD i=0; i<DECAL_LAYER_NUM; ++i )
		{
			m_mapDecalTest[i].clear();
			m_mapDecalBlend[i].clear();
			m_mapDecalMutiply[i].clear();
		}
		m_mapDecalPass2.clear();

		m_mapWater.clear();
		m_mapCustomSoftAlpha.clear();
		m_mapCustomUNLIT_UNFOG.clear();

		m_mapLightMap.clear();
		//m_setFX.clear();
	}
};
