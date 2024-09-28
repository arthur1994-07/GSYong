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
#include "./MaterialFieldDef.h"

class DxDecal;
class DxTexEffBase;
class LightMapRenderMan;
class SVEC_LIGHTMAPOBJ;
class DxSingleMaterialMesh;
class DxTriangleLinkLight_LightMap;
struct LightMapBoxColorData;
struct PieceParameter;
struct MaterialValue;
class DxCullingMeshSwap;

namespace NSTexture16Bit
{
	struct DATA;
};

class DxSingleMaterialMesh : public OBJAABB
{
private:
	enum
	{
		VERSION = 0x0108,
		VERSION_LM = 0x0106,
	};

protected:
	TSTRING					m_strMaterialName;
	LightMapRenderMan*		m_pLightMapRenderMan;

	LPD3DXMESH	            m_pShadowMesh;	    	// 순수 렌더를 위해 쓰인다. (ps.1.1 ~ ps.1.4)

	BOOL					m_bCheckTex;
	TSTRING					m_strTextureName;
	TextureResource			m_spTextureRes;
	TextureResource			m_spMaskTexRes;
	DWORD					m_dwFlags;				// EN_SINGLE_TEX_MESH_FLAGS
	MaterialValue			m_sMaterialValue;		// 

	BOOL					m_bDefaultFX_EnableLightMap;	// DefaultFX를 사용할 경우 LightMap 을 쓸지 안 쓸지 알아야 하기 때문에 필요함.

	std::vector<D3DXVECTOR2>	m_vecLightMapUV_BackUp;

	MATERIAL_DATA			m_sMaterial_Data;

	PS_3_0_TYPE				m_emPS_3_0_Type_ForField;		// PS_3_0 버젼의 렌더타입 - 필드에서만 쓰인다.
	WORDWORD				m_wwLightID_ForField;			// 연관되는 Light ID - 필드에서만 쓰인다.



	//////////////////////////////////////////////////////////////////////////
	// Culling 으로 인한 fps 저하로 Thread 에서 작업 진행.
	// Thread 쪽에 등록을 한다.
	boost::shared_ptr<DxCullingMeshSwap>		m_spCullingMeshSwap[NSLandThread::TYPE_CULL_MODE_SIZE];



public:
	DxSingleMaterialMesh*	m_pNext;		// Tree 만들기 위한 임시 포인터

private:
	void CreateLightMap( const LPDIRECT3DDEVICEQ pd3dDevice, 
							const TCHAR* szMaterialName, 
							const TSTRING& strTextureName,
							const DWORD dwFaceNUM, 
							BYTE* pVertices,
							BOOL bCamColl,
							DWORD dwFVF,
							DWORD dwFlags,
							float fCellSize );

public:
	virtual void GetAABBSize ( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

public:
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
	{
		D3DXVECTOR3 vMax, vMin;
		GetAABBSize( vMax, vMin );
		return COLLISION::IsWithInPoint ( vDivMax, vDivMin, (vMax+vMin)/2 );
	}
	virtual OBJAABB* GetNext () { return m_pNext; }

	// bUnKnownTextureType - TRUE : Unknown 을 알 기 위해 작업된다.
	//								이때는 m_mapUnKnownTextureType 에 셋팅하면 안된다.
	BOOL RenderOrderSetting( LPDIRECT3DDEVICEQ pd3dDevice, DxSingleMaterialMeshRenderOrder* pRenderOrder, DWORD& dwTexType, BOOL bTool, BOOL bUnKnownTextureMode ) override;

	HRESULT Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool );

	HRESULT RenderDYNAMIC ( const LPDIRECT3DDEVICEQ pd3dDevice, 
							NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
							const CLIPVOLUME &sCV, 
							sc::SerialFile* const SFile ) override;

	virtual HRESULT Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );

	virtual HRESULT FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, sc::SerialFile* const SFile );

	// Field는 무조건 한번은 해야한다. Piece 는 할 필요 없다.
	// Thread 에 DxCullingMeshForThread 정보를 넘겨주도록 한다.
	void InsertDxCullingMeshForThreadInThread();

	HRESULT RenderThreadLoad ( const LPDIRECT3DDEVICEQ pd3dDevice, 
								NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
								ID3DXEffect* pFX );

	HRESULT RenderStaticWater( const LPDIRECT3DDEVICEQ pd3dDevice, 
								NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
								float fPrevHeight, 
								float &fHeight,
								float &fNearLength );

	HRESULT RenderCommonCustomMaterial( const LPDIRECT3DDEVICEQ pd3dDevice, 
											NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType );

	void RenderPiece( PieceParameter& sParameter, ID3DXEffect* pFX );
	void RenderPiece_Instancing( const PieceParameter& sParameter, ID3DXEffect* pFX );

	void RenderFixed( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType );

	void RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bAlphaTex );
	void RenderCascadeShadow_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex );

	// Tool 상에서 Occulusion 에 사용되기 위해 존재한다.
	void RenderFixedCV( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV );

	void ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld );

	VOID CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID DestroyQuery();
	void ResetOcclusionQuery();
	void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex );

	void ClipStaticMesh( DxDecal* pDecal );

	BOOL CheckOnlyVoxel();

	void SetLightID( const WORDWORD& wwLightID );

	BOOL IsTextureLoadingComplete();

//	// Note : Color Thread 를 위한 작업
//public:
//	virtual void InsertColorList( LOADINGDATALIST &listColorData );

public:
	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
		LPCSTR& szName, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const;

	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, 
		D3DXVECTOR3& vEnd, 
		D3DXVECTOR3 &vColl, 
		D3DXVECTOR3 &vNor, 
		EMCOLLMODE emCullMode, 
		BOOL bOnlyCamColl ) const;

	BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const;

	// Note : 충돌이 되면 바로 리턴한다.
	BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV );

	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor, const NSTexture16Bit::DATA* pDataDay );

	void GetVertexNum( DWORD& dwVertexNum );
	void GetVertices( std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV, std::vector<BOOL>& vecRceiveShadowLM );

	void CreateLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
								LightMapObject* pLightMapObject,
								WORDWORD wwBaseTexelXY,
								BOOL bRan_1_0,
								float fCellSize );

	void FlipLightMapRenderMan( DxSingleMaterialMesh* pSingleTexMesh );

	void CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice );

	void GetClosedPoint( const D3DXVECTOR3& vBasePos, D3DXVECTOR3& vCollisionOut, float& fDistance );

	void GetCollisionLineInTriListExceptY ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ); // 주병진 추가 메소드

	void Import( LPDIRECT3DDEVICEQ pd3dDevice, const DxSingleTexMesh* pSingleTexMesh, float fCellSize );
	void Import( LPDIRECT3DDEVICEQ pd3dDevice, LightMapRenderMan* pLightMapRenderMan, float fCellSize, bool &bEnableVertexColor );

	void ChangeDxStaticMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxSingleTexMesh* pSingleTexMesh, float fCellSize );

	void SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate );

public:
	void Save ( sc::SerialFile& SFile, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive );
	void Load ( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bDynamicLoad, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive );

	void ThreadLoadingTexture();
	void LoadTexture_NonThread( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL IsLightMapPiece();
	void SaveLightMapPiece( sc::SerialFile& SFile );
	void LoadLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

private:
	void Insert( MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH& mapMesh );
	void Insert( MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH& mapMesh );

public:
	BOOL IsExistCullingData( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType );

public:
	// nPrevPass -1 로 초기값을 줄 필요가 있음.
	BOOL IsRenderEnable_ChangeFX_Field( ID3DXEffect* pFX, 
										BOOL bMPShading, 
										TEXTURE_ALPHA emAlphaType, 
										UINT unPass, 
										BOOL& bChangePassOUT );

	// nPrevPass -1 로 초기값을 줄 필요가 있음.
	BOOL IsRenderEnable_ChangeFX( ID3DXEffect* pFX, 
									BOOL bMPShading, 
									TEXTURE_ALPHA emAlphaType, 
									UINT unPass, 
									BOOL& bChangePassOUT,
									PS_3_0_TYPE emPS_3_0_Type,
									WORDWORD wwLightID );

	void CreateDxCullingMeshSwap();

protected:
	void CleanUp();

public:
	DxSingleMaterialMesh();
	~DxSingleMaterialMesh();
};
