// NsOcTree.h: interface for the CCollision class.
//
//	class DxAABBOctree : ���� �����Ϳ� �� ������ ������ Octree ������ ����ȭ�� OptimizeMesh �� �����.
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
	ENSTM_CAMCOLL					= 0x1,	// Note : ī�޶� �浹���θ� �����Ѵ�.
	ENSTM_DONT_CAST_SHADOW_LM		= 0x2,	//LightMap �׸��ڸ� �����ϴ� ��ü���� ���ܽ�Ű�� TRUE
	ENSTM_DONT_RECEIVE_SHADOW_LM	= 0x4,	//LightMap �׸��ڿ� ���� ���� �ʵ��� �Ϸ��� TRUE
	ENSTM_DECAL_ALPHABLEND			= 0x8,	// Decal ����.
	ENSTM_DECAL_ALPHATEST			= 0x10,	// Decal ����.
	ENSTM_DECAL_MULTIPLY			= 0x20,	// Decal ����.
	ENSTM_DECAL_ADD					= 0x40,	// Decal ����.
	ENSTM_DECAL_ORDER_1				= 0x100,	// Decal ����.	// ���� ����.
	ENSTM_DECAL_ORDER_2				= 0x200,	// Decal ����.
	ENSTM_DECAL_ORDER_3				= 0x300,	// Decal ����.
	ENSTM_DECAL_ORDER_MAX			= 0xf00,	// Decal ����.	0xf00 �� ������ �ǵ��� ���ÿ�.

	ENSTM_UNLIGHTMAP_AMBIENT_CUBE	= 0x1000,	// UnLightMap Type - Ambient Cube Mode
	ENSTM_UNLIGHTMAP_VOXEL_COLOR	= 0x2000,	// UnLightMap Type - Voxel Color Mode
	ENSTM_UNLIGHTMAP_MAX			= 0xf000,	// UnLightMap Type - 0xf000 �� ������ �ǵ��� ���ÿ�.

	ENSTM_VERTEX_COLOR				= 0x10000,	// Vertex Color �� ����ϴ°�~?		StaticMesh - Export �� ���õȴ�. Piece - pis->pie �� ���õȴ�.
	ENSTM_CUBEMAP					= 0x20000,	// CubeMap �� ���Ǵ°�~?
	ENSTM_MASK_CUBEMAP_SPECULARMAP	= 0x80000,	// Mask �� CubeMap & SpecularMap

	ENSTM_NORMALMAP					= 0x100000,	// NormalMap ���
	ENSTM_DECAL_NORMAL				= 0x200000,	// Decal ������ Normal �� ���Ǵ°�~?

	ENSTM_MASK_MAP					= ENSTM_MASK_CUBEMAP_SPECULARMAP | ENSTM_NORMALMAP,			// Mask ���� ����ϴ°�~? 
	ENSTM_DECAL_BEFORE				= ENSTM_DECAL_ALPHABLEND | ENSTM_DECAL_ALPHATEST | ENSTM_DECAL_MULTIPLY,	// Decal ��
	ENSTM_DECAL_AFTER				= ENSTM_DECAL_ADD,	// Decal ��
	ENSTM_DECAL_FULL				= ENSTM_DECAL_ALPHABLEND | ENSTM_DECAL_ALPHATEST | ENSTM_DECAL_MULTIPLY | ENSTM_DECAL_ADD,	// Decal ��
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
    AABBRenderStrategy* m_pAABBRenderStrategy;  // Strategy Pattern Ȱ��.
	TSTRING 			m_strTexName;
	TextureResource		m_textureRes;

	TSTRING				m_strMaterialName;

    //LPDIRECT3DTEXTUREQ	m_pTexture32BIT;		// LightMap ���鶧 ����.
	BOOL				m_bCheckTex;			// Texture �ε��� 1���� �õ��ϵ��� �Ѵ�.
	DWORD				m_dwFlags;				// EN_SINGLE_TEX_MESH_FLAGS
	MaterialValue		m_sMaterialValue;		// DxSingleMaterialMesh �� ����� ���� ��� ���̰� ����. Save, Load ���� ����.
	//BOOL                m_bCamColl;
	//BOOL				m_bDontCastShadowLM;	//LightMap �׸��ڸ� �����ϴ� ��ü���� ���ܽ�Ű�� TRUE
	//BOOL				m_bDontReceiveShadowLM;	//LightMap �׸��ڿ� ���� ���� �ʵ��� �Ϸ��� TRUE

	BOOL				m_bDefaultFX_EnableLightMap;	// DxSingleMaterialMesh-m_bDefaultFX_EnableLightMap �� ���� �����ϱ� ���� �ӽð�

	WORDWORD			m_wwLightID;			// �����Ǵ� Light ID

	

	//////////////////////////////////////////////////////////////////////////
	// Culling ���� ���� fps ���Ϸ� Thread ���� �۾� ����.
	// Thread �ʿ� ����� �Ѵ�.
	boost::shared_ptr<DxCullingMeshSwap>		m_spCullingMeshSwap[NSLandThread::TYPE_CULL_MODE_SIZE];



protected:
	DxTexEffBase*		m_pTexEffList;	// Texture Effect List.

public:
	DxSingleTexMesh*	m_pNext;		// Tree ����� ���� �ӽ� ������

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

    // TexEff �� �����´�.
    BOOL GetTexEff( LPDIRECT3DDEVICEQ pd3dDevice, VEC_TEXEFF& vecTexEff );

    // TexEff�� ����Ѵ�.
    void SetTextureEffNewFile( LPDIRECT3DDEVICEQ pd3dDevice, const VEC_TEXEFF& vecTexEff );

	void ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld );

//	// Note : Color Thread �� ���� �۾�
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

    // Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
    BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV );

	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor );

	BOOL IsLightMap();

	// �浹�޽��� ����� ���ؼ��� �ʿ���.
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
	const WORDWORD& GetLightID() const 	{ return m_wwLightID; }			// �����Ǵ� Light ID

	// DxStaticMaterialMesh ���� mmapMesh �� ������� ���ȴ�. 
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

	void GetCollisionLineInTriListExceptY ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ); // �ֺ��� �߰� �޼ҵ�

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
							int nRadiosityPassCUR,				// ���� �������� Pass
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

