// NsOcTree.h: interface for the CCollision class.
//
//	class DxAABBOctree : ���� �����Ϳ� �� ������ ������ Octree ������ ����ȭ�� OptimizeMesh �� �����.
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

	LPD3DXMESH	            m_pShadowMesh;	    	// ���� ������ ���� ���δ�. (ps.1.1 ~ ps.1.4)

	BOOL					m_bCheckTex;
	TSTRING					m_strTextureName;
	TextureResource			m_spTextureRes;
	TextureResource			m_spMaskTexRes;
	DWORD					m_dwFlags;				// EN_SINGLE_TEX_MESH_FLAGS
	MaterialValue			m_sMaterialValue;		// 

	BOOL					m_bDefaultFX_EnableLightMap;	// DefaultFX�� ����� ��� LightMap �� ���� �� ���� �˾ƾ� �ϱ� ������ �ʿ���.

	std::vector<D3DXVECTOR2>	m_vecLightMapUV_BackUp;

	MATERIAL_DATA			m_sMaterial_Data;

	PS_3_0_TYPE				m_emPS_3_0_Type_ForField;		// PS_3_0 ������ ����Ÿ�� - �ʵ忡���� ���δ�.
	WORDWORD				m_wwLightID_ForField;			// �����Ǵ� Light ID - �ʵ忡���� ���δ�.



	//////////////////////////////////////////////////////////////////////////
	// Culling ���� ���� fps ���Ϸ� Thread ���� �۾� ����.
	// Thread �ʿ� ����� �Ѵ�.
	boost::shared_ptr<DxCullingMeshSwap>		m_spCullingMeshSwap[NSLandThread::TYPE_CULL_MODE_SIZE];



public:
	DxSingleMaterialMesh*	m_pNext;		// Tree ����� ���� �ӽ� ������

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

	// bUnKnownTextureType - TRUE : Unknown �� �� �� ���� �۾��ȴ�.
	//								�̶��� m_mapUnKnownTextureType �� �����ϸ� �ȵȴ�.
	BOOL RenderOrderSetting( LPDIRECT3DDEVICEQ pd3dDevice, DxSingleMaterialMeshRenderOrder* pRenderOrder, DWORD& dwTexType, BOOL bTool, BOOL bUnKnownTextureMode ) override;

	HRESULT Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool );

	HRESULT RenderDYNAMIC ( const LPDIRECT3DDEVICEQ pd3dDevice, 
							NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
							const CLIPVOLUME &sCV, 
							sc::SerialFile* const SFile ) override;

	virtual HRESULT Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );

	virtual HRESULT FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, sc::SerialFile* const SFile );

	// Field�� ������ �ѹ��� �ؾ��Ѵ�. Piece �� �� �ʿ� ����.
	// Thread �� DxCullingMeshForThread ������ �Ѱ��ֵ��� �Ѵ�.
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

	// Tool �󿡼� Occulusion �� ���Ǳ� ���� �����Ѵ�.
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

//	// Note : Color Thread �� ���� �۾�
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

	// Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
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

	void GetCollisionLineInTriListExceptY ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ); // �ֺ��� �߰� �޼ҵ�

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
	// nPrevPass -1 �� �ʱⰪ�� �� �ʿ䰡 ����.
	BOOL IsRenderEnable_ChangeFX_Field( ID3DXEffect* pFX, 
										BOOL bMPShading, 
										TEXTURE_ALPHA emAlphaType, 
										UINT unPass, 
										BOOL& bChangePassOUT );

	// nPrevPass -1 �� �ʱⰪ�� �� �ʿ䰡 ����.
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
