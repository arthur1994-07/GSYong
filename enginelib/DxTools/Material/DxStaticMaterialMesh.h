// DxStaticMeshCreate.h: interface for the CCollision class.
//
//	class DxStaticMaterialMesh : FrameMesh 데이타를 넣으면 AABB 트리 적용하고 Octree로 세세하고 나눈다.
//
//	Name : Sung-Hwan Han
//	Begin :2004/10/21
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <set>
#include <map>

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxEffect/Mesh/DxStaticMeshBase.h"
#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../DxLand/DxPiece/DxPieceTexEff.h"
#include "../../DxLand/LightMap/LightMapBoxColor.h"
#include "../../DxLand/LightMap/LightMapDef.h"
#include "../../DxLand/NSLandThreadDef.h"
#include "../../DxLand/DxLandDef.h"

class DxDecal;
class DxPieceEdit;
class DxTextureEffMan;
class DxSingleMaterialMesh;
class DxTextureEffNewFile;
class DxStaticMesh;
struct PieceParameter;

namespace NSTexture16Bit
{
	struct DATA;
};

class DxStaticMaterialMesh : public DxStaticMeshBase
{
private:
	enum
	{
		VERSION = 0x0105,           // 기본적인 Version
		VERSION_LM = 0x0101,        // LightMap 정보용 Version
	};

protected:
	enum
	{
		SINGLE_TEX_TYPE_NUM = 5,
	};

private:
	//TSTRING     m_strLightMap;
	TSTRING			m_strLightMapCombinedForFieldMesh;			// Piece 가 아닌 FieldMesh
	TSTRING			m_strLightMapDirectShadow1stForFieldMesh;	// Piece 가 아닌 FieldMesh
	TSTRING			m_strLightMapDirectShadow2ndForFieldMesh;	// Piece 가 아닌 FieldMesh
	TSTRING			m_strLightMapIndirectForFieldMesh;			// Piece 가 아닌 FieldMesh
	TextureResource m_textureResCombinedForFieldMesh;			// Piece 가 아닌 FieldMesh
	TextureResource m_textureResDirectShadow1stForFieldMesh;	// Piece 가 아닌 FieldMesh
	TextureResource m_textureResDirectShadow2ndForFieldMesh;	// Piece 가 아닌 FieldMesh
	TextureResource m_textureResIndirectForFieldMesh;			// Piece 가 아닌 FieldMesh
	WORDWORD		m_wwLightMapUserUV_TEXEL;	// 개발자가 만든 LightMap Texel 사이즈 

	// Voxel & AmbientCube
	D3DXVECTOR4			m_vVoxelColorForFieldMesh;
	D3DXVECTOR4			m_avAmbientCubeForFieldMesh[LightMapBoxColor::TEST_COUNT];

public:
	MMAPSINGLEMATERIALMESH	m_mmapMesh;

protected:
	DxSingleMaterialMesh*	m_pList;
	OBJAABBNode*			m_pMeshTree;

private:
	boost::shared_ptr<DxSingleMaterialMeshRenderOrder>	m_spRenderOrder;

	DWORD		m_dwTexType;

protected:
	D3DXVECTOR3		m_vMin;				// 최소값
	D3DXVECTOR3		m_vMax;				// 최대값

private:
	enum EMRENDERTYPE
	{
		EMRT_OCTREE = 0,
		EMRT_PIECE = 1,
	};

	EMRENDERTYPE	m_emRenderType;

public:
	BOOL IsMesh ()			{ return (BOOL)m_mmapMesh.size(); }

	//const TSTRING&  GetLightMapName()   const   { return m_strLightMap; }

protected:
	void SetAABBMaxMin ();

public:
	void RefreshLightMap( LPDIRECT3DDEVICEQ pd3dDevice );

	void ConvertMAPtoLIST( MMAPSINGLEMATERIALMESH& mapData, DxSingleMaterialMesh** ppList );

	void Import( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMesh* pStaticMesh, float fCellSize );

	void SetLightMapName( const TSTRING& strLightMapName, const TSTRING& strAddName, DWORD dwTextureID );	// Piece 가 아닌 Main 에서 필요한것


public:
	// Material 을 구한다.
	void Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool );

	// Tree 를 만들면서 그와 관련된 작업들을 정리한다.
	void CreateTree_AddWork();


public:
	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const;

	BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const override;

	void GetCollisionLineInTriListExceptY ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ); // 주병진 추가 메소드
	void GetCollisionLineInTriListExceptYReplacePiece ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ); // 주병진 추가 메소드

	BOOL GetLightMapColorForFieldMesh( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor );
	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor, const NSTexture16Bit::DATA* pDataDay );

	BOOL ChangeDxStaticMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMesh* pStaticMesh, float fCellSize );

public:
	void RenderOrderSetting( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void Render( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bMPShading, const D3DXMATRIX& matWorld );
	void RenderHardAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bMPShading, const D3DXMATRIX& matWorld );
	void RenderSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bMPShading, const D3DXMATRIX& matWorld );
	void RenderSoftAlpha_Deffered( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const D3DXMATRIX& matWorld );
	void RenderDecal( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bMPShading, const D3DXMATRIX& matWorld );
	void RenderDecalPass2( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const D3DXMATRIX& matWorld );
	void RenderCustomMaterialUNLIT_UNFOG( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType );
	void RenderCustomMaterialUNLIT_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, float fPrevHeight, float &fHeight, float &fNearLength );

	void RenderCascadeShadowOpaque( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType );
	void RenderCascadeShadowHardAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType );

	void Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV );

	VOID CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID DestroyQuery();
	void ResetOcclusionQuery();
	void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex );

	void ClipStaticMesh( DxDecal* pDecal );

	BOOL CheckOnlyVoxel();

	void RenderOrderSettingPiece( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool, DWORD& dwTexType );

	void LoadLightMapForFieldMesh( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate );






	// TriangleLinkLight
public:
	// Piece의 TriangleLinkLight 를 생성하도록 한다.
	virtual BOOL CreateTriangleLinkLightForPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
													DxStaticMeshCollBase* pStaticMesh, 
													DxPieceManager* pPieceManager, 
													std::vector<DWORD>& vecLightID, 
													const D3DXMATRIX& matWorld,
													const D3DLIGHTQ& sLight,
													float fAttEndLineValue ) const;

protected:
	void CleanUp();

public:
	void BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos, sc::SerialFile* const pSFile );
	void BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, sc::SerialFile* const pSFile );
	void BaseLoadPiece( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	void SingleTexMesh_Save( MMAPSINGLEMATERIALMESH& mapMesh, sc::SerialFile& SFile, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive );
	void SingleTexMesh_Load_Insert( LPDIRECT3DDEVICEQ pd3dDevice, MMAPSINGLEMATERIALMESH& mapMesh, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive );
	void Load_105( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece, boost::function<void (const TCHAR*)> pArchive );
	void Load_104( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece, boost::function<void (const TCHAR*)> pArchive );
	void Load_102_103( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece, boost::function<void (const TCHAR*)> pArchive );
	void Load_101( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece, boost::function<void (const TCHAR*)> pArchive );
	void Load_100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece, boost::function<void (const TCHAR*)> pArchive );

	void SingleTexMesh_SaveLightMapPiece( MMAPSINGLEMATERIALMESH& mapMesh, sc::SerialFile& SFile );
	void SingleTexMesh_LoadLightMapPiece_Insert( LPDIRECT3DDEVICEQ pd3dDevice, MMAPSINGLEMATERIALMESH& mapMesh, sc::SerialFile& SFile );

	//virtual
public:
	virtual D3DXVECTOR3 GetAABBMax ()	{ return m_vMax; }
	virtual D3DXVECTOR3 GetAABBMin ()	{ return m_vMin; }

	virtual void FlipLightMapRenderMan( DxStaticMeshBase* pStaticMeshBase );

	virtual void CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual void MergeLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
										const TSTRING& strLightMapName, 
										DxStaticMesh* pStaticMesh, 
										DxPieceManagerPSF* pPieceManager, 
										const D3DXMATRIX& matWorld, 
										VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
										DWORD& dwStartTextureSize,
										WORDWORD& wwBaseTexelXY,
										BOOL& bEnoughUsedPixelLightMap,
										const WORDWORD& wwLightID4Shadow );

	virtual void CreateLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
										const TSTRING& strLightMapName,
										TSTRING& strTextureCombined, 
										TSTRING& strTextureDirect_Shadow_1,
										TSTRING& strTextureDirect_Shadow_2,
										TSTRING& strTextureIndirect,
										VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
										WORDWORD wwBaseTexelXY );

	virtual void CreateLightMapUVPieceObject( LPDIRECT3DDEVICEQ pd3dDevice, 
												const TSTRING& strLightMapName,
												TSTRING& strTextureCombined, 
												TSTRING& strTextureDirect_Shadow_1,
												TSTRING& strTextureDirect_Shadow_2,
												TSTRING& strTextureIndirect,
												VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
												WORDWORD wwBaseTexelXY,
												BOOL bRan_1_0,
												float fCellSize );

	virtual void BakeRadiosityForPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
										DxLandMan* pLandMan,
										VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
										const D3DXMATRIX& matWorld,
										LPDIRECT3DTEXTUREQ pTextureSystem,
										float fReflectance, 
										float fFogBlend,
										DWORD dwFogLength,
										DWORD dwRadiosityIndex );

	//virtual void GetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3&  vMin );
	//virtual BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision, EMCOLLMODE emCullMode );

	virtual BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
									LPCSTR& szName, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const;

	// Note : 충돌이 되면 바로 리턴한다.
	virtual BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV );

	virtual void GetClosedPoint( D3DXVECTOR3& vCollisionInOut );

	virtual void ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld );

	virtual void FrameMove( const float fElapsedTime );

	virtual void Render_PIECE_( const LPDIRECT3DDEVICEQ pd3dDevice, 
								DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
								const D3DXMATRIX& matWorld, 
								const D3DXVECTOR2& vLightMapUV_Offset,
								float fAlpha, 
								int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
								LPDIRECT3DTEXTUREQ pTextureDay, 
								LPDIRECT3DTEXTUREQ pTextureNight,
								BOOL bMPShading )	{}

	virtual void Render_PIECE_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
									DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
									const D3DXMATRIX& matWorld, 
									const D3DXVECTOR2& vLightMapUV_Offset,
									float fAlpha, 
									int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
									LPDIRECT3DTEXTUREQ pTextureDay, 
									LPDIRECT3DTEXTUREQ pTextureNight,
									BOOL bMPShading )	{}

	virtual void Render_PIECE_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
										DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
										const D3DXMATRIX& matWorld, 
										const D3DXVECTOR2& vLightMapUV_Offset,
										float fAlpha, 
										int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
										LPDIRECT3DTEXTUREQ pTextureDay, 
										LPDIRECT3DTEXTUREQ pTextureNight,
										BOOL bMPShading )	{}

	void Render_PIECE_( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend );
	void Render_PIECE_Alpha( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend );
	void Render_PIECE_Decal( PieceParameter& sParameter );
	void Render_PIECE_SoftAlpha( PieceParameter& sParameter, BOOL bMPShading );
	void Render_PIECE_SoftAlphaForPSF( PieceParameter& sParameter );
	void Render_PIECE_DecalPass2( PieceParameter& sParameter );
	void Render_PIECE_Custom_UNLIT_UNFOG( PieceParameter& sParameter );
	void Render_PIECE_Water_CustomSoftAlpha( PieceParameter& sParameter );

	void Render_PIECE_Opaque_Instancing( const PieceParameter& sParameter );
	void Render_PIECE_HardAlpha_Instancing( const PieceParameter& sParameter );
	void Render_PIECE_Decal_Instancing( const PieceParameter& sParameter );
	void Render_PIECE_DecalPass2_Instancing( const PieceParameter& sParameter );

	void Render_PIECE_CascadeShadowOpaque( LPDIRECT3DDEVICEQ pd3dDevice );
	void Render_PIECE_CascadeShadowHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	void Render_PIECE_CascadeShadowOpaque_Instancing( LPDIRECT3DDEVICEQ pd3dDevice );
	void Render_PIECE_CascadeShadowHardAlpha_Instancing( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual void Render_CharShadow( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV );

	// Dynamic Loading	|| General <SFile==NULL> || Piece File			Loading 을 의미함.
	void Save( sc::SerialFile& SFile, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bDynamicLoad, const BOOL bPiece, boost::function<void (const TCHAR*)> pArchive );

	virtual BOOL IsLightMapPiece();
	virtual void SaveLightMapPiece( sc::SerialFile& SFile );
	virtual void LoadLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

public:	
	void StartThread( LPDIRECT3DDEVICEQ pd3dDevice );
	void EndThread();

	// 로그인 화면 같은 경우 다중으로 맵이 로딩이 되어 있고, 맵 전환이 되기 때문에 NSLandThread 정보값도 변경이 되야한다.
	void SetNSLandThread();

public:
	DxStaticMaterialMesh();
	~DxStaticMaterialMesh();
};