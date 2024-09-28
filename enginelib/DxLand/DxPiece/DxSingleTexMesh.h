// NsOcTree.h: interface for the CCollision class.
//
//	class DxAABBOctree : 정점 데이터와 면 갯수를 넣으면 Octree 형식의 최적화된 OptimizeMesh 를 만든다.
//
//	Name : Sung-Hwan Han
//	Begin :2004/10/21
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../../DxTools/Material/MaterialFieldDef.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/Collision.h"
#include "../LightMap/LightMapDef.h"
#include "../DxPiece/DxPieceTexEff.h"
#include "../NSLandThreadDef.h"
#include "../DxLandDef.h"

class DxTexEffBase;
class AABBRenderStrategy;
class SVEC_LIGHTMAPOBJ;
class DxPieceManagerPSF;
class DxCullingMeshSwap;

enum EN_SINGLE_TEX_MESH_FLAGS
{
	ENSTM_CAMCOLL					= 0x1,	// Note : 카메라 충돌여부를 설정한다.
	ENSTM_DONT_CAST_SHADOW_LM		= 0x2,	//LightMap 그림자를 생성하는 물체에서 제외시키면 TRUE
	ENSTM_DONT_RECEIVE_SHADOW_LM	= 0x4,	//LightMap 그림자에 영향 받지 않도록 하려면 TRUE
	ENSTM_DECAL_ALPHABLEND			= 0x8,	// Decal 셋팅.
	ENSTM_DECAL_ALPHATEST			= 0x10,	// Decal 셋팅.
	ENSTM_DECAL_MULTIPLY			= 0x20,	// Decal 셋팅.
	ENSTM_DECAL_ADD					= 0x40,	// Decal 셋팅.
	ENSTM_DECAL_ORDER_1				= 0x100,	// Decal 셋팅.	// 순서 셋팅.
	ENSTM_DECAL_ORDER_2				= 0x200,	// Decal 셋팅.
	ENSTM_DECAL_ORDER_3				= 0x300,	// Decal 셋팅.
	ENSTM_DECAL_ORDER_MAX			= 0xf00,	// Decal 셋팅.	0xf00 쪽 라인은 건들지 마시오.

	ENSTM_UNLIGHTMAP_AMBIENT_CUBE	= 0x1000,	// UnLightMap Type - Ambient Cube Mode
	ENSTM_UNLIGHTMAP_VOXEL_COLOR	= 0x2000,	// UnLightMap Type - Voxel Color Mode
	ENSTM_UNLIGHTMAP_MAX			= 0xf000,	// UnLightMap Type - 0xf000 쪽 라인은 건들지 마시오.

	ENSTM_VERTEX_COLOR				= 0x10000,	// Vertex Color 를 사용하는가~?		StaticMesh - Export 시 셋팅된다. Piece - pis->pie 시 셋팅된다.
	ENSTM_CUBEMAP					= 0x20000,	// CubeMap 이 사용되는가~?
	ENSTM_MASK_CUBEMAP_SPECULARMAP	= 0x80000,	// Mask 맵 CubeMap & SpecularMap

	ENSTM_NORMALMAP					= 0x100000,	// NormalMap 사용
	ENSTM_DECAL_NORMAL				= 0x200000,	// Decal 에서의 Normal 이 사용되는가~?

	ENSTM_MASK_MAP					= ENSTM_MASK_CUBEMAP_SPECULARMAP | ENSTM_NORMALMAP,			// Mask 맵을 사용하는가~? 
	ENSTM_DECAL_BEFORE				= ENSTM_DECAL_ALPHABLEND | ENSTM_DECAL_ALPHATEST | ENSTM_DECAL_MULTIPLY,	// Decal 합
	ENSTM_DECAL_AFTER				= ENSTM_DECAL_ADD,	// Decal 합
	ENSTM_DECAL_FULL				= ENSTM_DECAL_ALPHABLEND | ENSTM_DECAL_ALPHATEST | ENSTM_DECAL_MULTIPLY | ENSTM_DECAL_ADD,	// Decal 합
};

class DxSingleTexMesh : public OBJAABB
{
private:
	enum
    {
        VERSION = 0x0106,
        VERSION_LM = 0x0100,
    };

protected:
	DWORD				m_dwEffectFlag;
    AABBRenderStrategy* m_pAABBRenderStrategy;  // Strategy Pattern 활용.
	TSTRING 			m_strTexName;
	TextureResource		m_textureRes;

	TSTRING				m_strMaterialName;

    //LPDIRECT3DTEXTUREQ	m_pTexture32BIT;		// LightMap 만들때 사용됨.
	BOOL				m_bCheckTex;			// Texture 로딩은 1번만 시도하도록 한다.
	DWORD				m_dwFlags;				// EN_SINGLE_TEX_MESH_FLAGS
	MaterialValue		m_sMaterialValue;		// DxSingleMaterialMesh 을 만들기 위해 잠시 쓰이고 버림. Save, Load 하지 않음.
	//BOOL                m_bCamColl;
	//BOOL				m_bDontCastShadowLM;	//LightMap 그림자를 생성하는 물체에서 제외시키면 TRUE
	//BOOL				m_bDontReceiveShadowLM;	//LightMap 그림자에 영향 받지 않도록 하려면 TRUE

	BOOL				m_bDefaultFX_EnableLightMap;	// DxSingleMaterialMesh-m_bDefaultFX_EnableLightMap 에 값을 전달하기 위한 임시값

	WORDWORD			m_wwLightID;			// 연관되는 Light ID

	

	//////////////////////////////////////////////////////////////////////////
	// Culling 으로 인한 fps 저하로 Thread 에서 작업 진행.
	// Thread 쪽에 등록을 한다.
	boost::shared_ptr<DxCullingMeshSwap>		m_spCullingMeshSwap[NSLandThread::TYPE_CULL_MODE_SIZE];



protected:
	DxTexEffBase*		m_pTexEffList;	// Texture Effect List.

public:
	DxSingleTexMesh*	m_pNext;		// Tree 만들기 위한 임시 포인터

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

	HRESULT RenderTHREAD ( const LPDIRECT3DDEVICEQ pd3dDevice, 
							NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
							const CLIPVOLUME &sCV,
							BOOL bMPShading ) override;

	HRESULT RenderDYNAMIC ( const LPDIRECT3DDEVICEQ pd3dDevice,
							NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
							const CLIPVOLUME &sCV, 
							sc::SerialFile* const SFile ) override;

	virtual HRESULT Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	virtual HRESULT FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, sc::SerialFile* const SFile );
	virtual void	FrameMove( const float fElpasedTime );

	HRESULT Render ( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const CLIPVOLUME& sCV );

	void	RenderPiece( const LPDIRECT3DDEVICEQ pd3dDevice, 
                        DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
                        const D3DXMATRIX& matWorld, 
                        BOOL bRealPL, 
                        const D3DXVECTOR2& vLightMapUV_Offset,
						float fAlpha, 
						LPDIRECT3DTEXTUREQ pTextureDay, 
						LPDIRECT3DTEXTUREQ pTextureNight );

	VOID CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID DestroyQuery();
	void ResetOcclusionQuery();
	void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex );

    // TexEff 만 가져온다.
    BOOL GetTexEff( LPDIRECT3DDEVICEQ pd3dDevice, VEC_TEXEFF& vecTexEff );

    // TexEff를 등록한다.
    void SetTextureEffNewFile( LPDIRECT3DDEVICEQ pd3dDevice, const VEC_TEXEFF& vecTexEff );

	void ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld );

//	// Note : Color Thread 를 위한 작업
//public:
//	virtual void InsertColorList( LOADINGDATALIST &listColorData );
	
public:
	BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const;

	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
						LPCSTR& szName, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const;

	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, 
                            D3DXVECTOR3& vEnd, 
                            D3DXVECTOR3 &vColl, 
                            D3DXVECTOR3 &vNor, 
                            EMCOLLMODE emCullMode, 
                            BOOL bOnlyCamColl ) const;

    // Note : 충돌이 되면 바로 리턴한다.
    BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV );

	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor );

	BOOL IsLightMap();

	// 충돌메쉬를 만들기 위해서도 필요함.
	int Create( const LPDIRECT3DDEVICEQ pd3dDevice, 
					const TCHAR* szMaterialName, 
					const TSTRING& strTextureName,
                    const DWORD dwFaceNUM, 
                    BYTE* pVertices, 
                    const DWORD dwFVF, 
                    BOOL bPiece, 
                    DWORD dwFlags, 
					const MaterialValue& sMaterialValue,
					BOOL bUserLightMapUV,
					float fCellSize );

    int CreateLightMap( const LPDIRECT3DDEVICEQ pd3dDevice, 
							const TCHAR* szMaterialName, 
							const TSTRING& strTextureName,
							const DWORD dwFaceNUM, 
							BYTE* pVertices, 
							DWORD dwFlags,
							const MaterialValue& sMaterialValue,
							DWORD dwFVF,
							const TSTRING& strTextureCombined,
							const TSTRING& strTextureDirect_Shadow_1,
							const TSTRING& strTextureDirect_Shadow_2,
							const TSTRING& strTextureIndirect,
							BOOL bUserLightMapUV,
							BOOL bRan_1_0,
							float fCellSize );

	int CreateLightMapForTriangleLinkLight( const LPDIRECT3DDEVICEQ pd3dDevice, 
											const TCHAR* szMaterialName, 
											const TSTRING& strTextureName,
											const DWORD dwFaceNUM, 
											BYTE* pVertices,
											DWORD dwFlags,
											const MaterialValue& sMaterialValue,
											DWORD dwFVF,
											const TSTRING& strTextureCombined,
											const TSTRING& strTextureDirect_Shadow_1,
											const TSTRING& strTextureDirect_Shadow_2,
											const TSTRING& strTextureIndirect,
											BOOL bUserLightMapUV,
											const std::vector<WORDWORD>& vecTriangle_wwLightID,
											float fCellSize );

	void SetLightID( const WORDWORD& vecLightID );
	const WORDWORD& GetLightID() const 	{ return m_wwLightID; }			// 연관되는 Light ID

	// DxStaticMaterialMesh 에서 mmapMesh 을 얻기위해 사용된다. 
	void Create_DxSingleMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice, MMAPSINGLEMATERIALMESH& mmapMesh, float fCellSize ) const;

    void LoadLightMap( LPDIRECT3DDEVICEQ pd3dDevice );

    //void MakeLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
    //                        const TSTRING& strLightMapName, 
    //                        DxStaticMesh* pStaticMesh, 
    //                        DxPieceManager* pPieceManager, 
    //                        const D3DXMATRIX& matWorld, 
    //                        SVEC_LIGHTMAPOBJ* psvecLightMapObj,
    //                        DWORD& dwStartTextureSize,
    //                        float fLightMapDetail,
    //                        BOOL bDontMakeShadowLM );

	void GetVertexNum( DWORD& dwVertexNum );
	void GetVertices( std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV, std::vector<BOOL>& vecRceiveShadowLM );

    //void CreateLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
    //                            const TSTRING& strLightMapName,
    //                            TSTRING& strTextureDay, 
    //                            TSTRING& strTextureNight,
    //                            TSTRING& strTextureAvgDay,
    //                            TSTRING& strTextureAvgNight,
    //                            SVEC_LIGHTMAPOBJ* psvecLightMapObj,
    //                            WORDWORD wwBaseTexelXY );

	void CreateLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
								LightMapObject* pLightMapObject,
								TSTRING& strTextureCombined, 
								TSTRING& strTextureDirect_Shadow_1,
								TSTRING& strTextureDirect_Shadow_2,
								TSTRING& strTextureIndirect,
								WORDWORD wwBaseTexelXY,
								BOOL bRan_1_0,
								float fCellSize );

    void FlipLightMapRenderMan( DxSingleTexMesh* pSingleTexMesh );

    void CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice );

    void GetClosedPoint( const D3DXVECTOR3& vBasePos, D3DXVECTOR3& vCollisionOut, float& fDistance );

	void ConvertTexEffMesh( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEff );

	void GetCollisionLineInTriListExceptY ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ); // 주병진 추가 메소드

	const TSTRING& GetTextureName() const			{ return m_strTexName; }
	const TSTRING& GetMaterialName() const			{ return m_strMaterialName; }
	BOOL GetDefaultFX_EnableLightMap() const		{ return m_bDefaultFX_EnableLightMap; }
	TextureResource& GetTextureRes()				{ return m_textureRes; }
	DWORD GetFlags() const 							{ return m_dwFlags; }
	const MaterialValue& GetMaterialValue() const 	{ return m_sMaterialValue; }
    
	void RenderRadiosity( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxLandMan* pLandMan,
							LPDIRECT3DTEXTUREQ pTextureSystem, 
							const TSTRING& strLightMapName,
							int nRadiosityPassCUR,				// 현재 진행중인 Pass
							float fRadiosityReflectance,
							float fRadiosityFogBlend,
							DWORD dwRadiosityFogLength );
public:
    AABBRenderStrategy* GetAABBRenderStrategy() const { return m_pAABBRenderStrategy; }
	//DxAABBOctree* GetAABBOctree() { return m_pAABBOctree; }

public:
	void CreateDxCullingMeshSwap();

public:
	void Save ( sc::SerialFile& SFile, BOOL bPiece );
	void Load ( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bDynamicLoad, BOOL bPiece, BOOL bDummy );
	void Load_VER100 ( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDummy );

    BOOL IsLightMapPiece();
    void SaveLightMapPiece( sc::SerialFile& SFile );
    void LoadLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

protected:
	void CleanUp();

public:
	DxSingleTexMesh();
	~DxSingleTexMesh();
};

