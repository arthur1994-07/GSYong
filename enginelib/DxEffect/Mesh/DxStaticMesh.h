// DxStaticMeshCreate.h: interface for the CCollision class.
//
//	class DxStaticMesh : FrameMesh ����Ÿ�� ������ AABB Ʈ�� �����ϰ� Octree�� �����ϰ� ������.
//
//	Name : Sung-Hwan Han
//	Begin :2004/10/21
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <set>
#include <map>

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../DxLand/DxPiece/DxPieceTexEff.h"
#include "../../DxLand/LightMap/LightMapDef.h"
#include "../../DxLand/DxLandDef.h"
#include "./DxStaticMeshBase.h"

class DxPieceEdit;
class DxTextureEffMan;
class DxSingleTexMesh;
class CMaterialSetting;
class DxPieceManager;
class DxPieceManagerPSF;
class DxTextureEffNewFile;

class DxStaticMesh : public DxStaticMeshBase
{
private:
	enum
    {
        VERSION = 0x0105,           // �⺻���� Version
        VERSION_LM = 0x0101,        // LightMap ������ Version
    };

protected:
	//sc::SerialFile m_SFile;		// Thread Loading �� �ʿ���.

    enum
    {
        SINGLE_TEX_TYPE_NUM = 5,
    };

private:
    BOOL        m_bLightMap;
    TSTRING     m_strLightMap;
    float       m_fLightMapDetail;
	WORDWORD	m_wwLightMapUserUV_TEXEL;	// �����ڰ� ���� LightMap Texel ������ 

public:
	MAPSINGLETEXMESH	m_mapMesh;
	MAPSINGLETEXMESH	m_mapMeshALPHA;
	MAPSINGLETEXMESH	m_mapMeshSOFTALPHA;
	MAPSINGLETEXMESH	m_mapMeshSOFTALPHA01;
	MAPSINGLETEXMESH	m_mapMeshSOFTALPHA02;

protected:
	DxSingleTexMesh*	m_pList;
	DxSingleTexMesh*	m_pAlphaList;
	DxSingleTexMesh*	m_pAlphaSoftList;
	DxSingleTexMesh*	m_pAlphaSoftList01;
	DxSingleTexMesh*	m_pAlphaSoftList02;

	OBJAABBNode*		m_pMeshTree;
	OBJAABBNode*		m_pMeshAlphaTree;
	OBJAABBNode*		m_pMeshSoftAlphaTree;
	OBJAABBNode*		m_pMeshSoftAlphaTree01;
	OBJAABBNode*		m_pMeshSoftAlphaTree02;

protected:
	D3DXVECTOR3		m_vMin;				// �ּҰ�
	D3DXVECTOR3		m_vMax;				// �ִ밪

//private:
//	DxStaticMesh_THREAD	m_sThread;			// Loading�� ���� Thread
//	LOADINGDATALIST		m_listLoadingData;	// �ּ� ����Ʈ
//	CRITICAL_SECTION	m_CSLockLoading;	// m_listLoadingData�� ��ȭ�� �ߺ����� �ʵ��� �ϱ� ����.

private:
	enum EMRENDERTYPE
	{
		EMRT_OCTREE = 0,
		EMRT_PIECE = 1,
	};

	EMRENDERTYPE	m_emRenderType;

public:
	BOOL IsMesh ()			{ return (BOOL)m_mapMesh.size(); }
	BOOL IsMeshALPHA ()		{ return (BOOL)m_mapMeshALPHA.size(); }
	BOOL IsMeshSOFTALPHA ()	{ return (BOOL)m_mapMeshSOFTALPHA.size(); }

    BOOL            ActiveLightMap()    const   { return m_bLightMap; }
    const TSTRING&  GetLightMapName()   const   { return m_strLightMap; }
    float           GetLightMapDetail() const   { return m_fLightMapDetail; }

	virtual D3DXVECTOR3 GetAABBMax ()	{ return m_vMax; }
	virtual D3DXVECTOR3 GetAABBMin ()	{ return m_vMin; }

	WORDWORD GetLightMapUserUV_TEXEL()	{ return m_wwLightMapUserUV_TEXEL; }

protected:
	void SetAABBMaxMin ();

public:
	virtual void MakeAABBOCTree( const LPDIRECT3DDEVICEQ pd3dDevice, 
								DxFrameMesh* const pFrameMesh, 
								DxPieceManagerPSF* pPieceManager, 
								LIGHTMAPUV_MAIN* pLightMapUV_MAIN,
								const BOOL bProgress, 
								BOOL bPiece, 
								BOOL bLightMap, 
								const TSTRING& strLightMapName,
								const DxTextureEffMan* pTextureEffMan,
								const CMaterialSetting* pMaterialSetting,
								BOOL Ran_1_0,
								float fCellSize );

	void MakeAABBOCTree( LPDIRECT3DDEVICEQ pd3dDevice, 
						DxMeshes *pmsMeshs,
						const CMaterialSetting *pMaterialSetting,
						float fCellSize );

    //void MakeLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
				//					 const DxPieceEdit* pPieceEdit,
				//					 const TSTRING& strLightMapName, 
				//					 DxStaticMesh* pStaticMesh, 
				//					 DxPieceManager* pPieceManager, 
				//					 const D3DXMATRIX& matWorld, 
				//					 VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
				//					 DWORD& dwStartTextureSize,
				//					 WORDWORD& wwBaseTexelXY,
				//					 float& fLightMapDetail );

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

    //void CreateLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
    //                            const TSTRING& strLightMapName,
    //                            TSTRING& strTextureDay, 
    //                            TSTRING& strTextureNight,
    //                            TSTRING& strTextureAvgDay,
    //                            TSTRING& strTextureAvgNight,
    //                            VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
    //                            WORDWORD wwBaseTexelXY );

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

	void LoadLightMapOLD( LPDIRECT3DDEVICEQ pd3dDevice );

    virtual void FlipLightMapRenderMan( DxStaticMeshBase* pStaticMeshBase );

    virtual void CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice );

    void GetClosedPoint( D3DXVECTOR3& vCollisionInOut );

	void LoadLightMapTexture( LPDIRECT3DDEVICEQ pd3dDevice );

	void ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld );

    // TexEff �� �����´�.
public:
    void GetTexEff( LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffNewFile& sTexEffNewFile );
    void SetTextureEffNewFile( LPDIRECT3DDEVICEQ pd3dDevice, const DxTextureEffNewFile& sTexEffNewFile );

private:
    void GetTexEff( LPDIRECT3DDEVICEQ pd3dDevice, MAPSINGLETEXMESH& mapMesh, DxTextureEffNewFile& sTexEffNewFile );
    void SetTextureEffNewFile( LPDIRECT3DDEVICEQ pd3dDevice, MAPSINGLETEXMESH& mapMesh, const TSTRING& strName, const VEC_TEXEFF& vecTexEff );

public:
	virtual BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
									LPCSTR& szName, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const;
	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const;

	BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const override;

    // Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
    virtual BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV );

	void GetCollisionLineInTriListExceptY ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ); // �ֺ��� �߰� �޼ҵ�
	void GetCollisionLineInTriListExceptYReplacePiece ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ); // �ֺ��� �߰� �޼ҵ�

	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor );

	BOOL IsLightMap();

public:
	virtual void FrameMove( const float fElapsedTime );

	// Thread Loading Render
	void Render_THREAD_( const LPDIRECT3DDEVICEQ pd3dDevice, 
						NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
						const CLIPVOLUME &sCV );

	void Render_THREAD_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
								NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
								const CLIPVOLUME &sCV );

	void Render_THREAD_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
									NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
									const CLIPVOLUME &sCV );

	//// Dynamic Loading Render	|| General Render <SFile==NULL>
	//void Render_DYNAMIC_( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, sc::SerialFile* const SFile );
	//void Render_DYNAMIC_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, sc::SerialFile* const SFile );
	//void Render_DYNAMIC_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, sc::SerialFile* const SFile );

	// Piece File Render
	virtual void Render_PIECE_( const LPDIRECT3DDEVICEQ pd3dDevice, 
								DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
								const D3DXMATRIX& matWorld, 
								const D3DXVECTOR2& vLightMapUV_Offset,
								float fAlpha, 
								int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
								LPDIRECT3DTEXTUREQ pTextureDay, 
								LPDIRECT3DTEXTUREQ pTextureNight,
								BOOL bMPShading );

	virtual void Render_PIECE_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
									DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
									const D3DXMATRIX& matWorld, 
									const D3DXVECTOR2& vLightMapUV_Offset,
									float fAlpha, 
									int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
									LPDIRECT3DTEXTUREQ pTextureDay, 
									LPDIRECT3DTEXTUREQ pTextureNight,
									BOOL bMPShading );

	virtual void Render_PIECE_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
										DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
										const D3DXMATRIX& matWorld, 
										const D3DXVECTOR2& vLightMapUV_Offset,
										float fAlpha, 
										int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
										LPDIRECT3DTEXTUREQ pTextureDay, 
										LPDIRECT3DTEXTUREQ pTextureNight,
										BOOL bMPShading );

	virtual void Render_CharShadow( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV );

	void Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );

	VOID CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID DestroyQuery();
	void ResetOcclusionQuery();
	void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex );

	//// Piece File Render
	//void Render( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV );
	//void RenderAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV );
	//void RenderSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV );
	//void RenderSoftAlpha01( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV );
	//void RenderSoftAlpha02( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV );

	// TriangleLinkLight
public:
	// Piece�� TriangleLinkLight �� �����ϵ��� �Ѵ�.
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

	//// Thread Loading �� �ǹ���.
	//void Save( const char* szName );
	//void Load( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName );

	// Dynamic Loading	|| General <SFile==NULL> || Piece File			Loading �� �ǹ���.
	void Save( sc::SerialFile& SFile, BOOL bPiece );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bDynamicLoad, const BOOL bPiece );

    virtual BOOL IsLightMapPiece();
    virtual void SaveLightMapPiece( sc::SerialFile& SFile );
    virtual void LoadLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

protected:
	void SingleTexMesh_Save( MAPSINGLETEXMESH& mapMesh, sc::SerialFile& SFile, BOOL bPiece );
	void SingleTexMesh_Load_Insert( LPDIRECT3DDEVICEQ pd3dDevice, MAPSINGLETEXMESH& mapMesh, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece );
    void SingleTexMesh_Load_Insert_OLD( LPDIRECT3DDEVICEQ pd3dDevice, MAPSINGLETEXMESH& mapMesh, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece );
	void Load_101( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece );
	void Load_102( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece );
	void Load_103( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece );	// ������ ���� ���� ����
    void Load_104( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece );	// MAPSINGLETEXMESH �����.
	void Load_105( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece );	// m_wwLightMapUserUV_TEXEL �߰���.

    void SingleTexMesh_SaveLightMapPiece( MAPSINGLETEXMESH& mapMesh, sc::SerialFile& SFile );
    void SingleTexMesh_LoadLightMapPiece_Insert( LPDIRECT3DDEVICEQ pd3dDevice, MAPSINGLETEXMESH& mapMesh, sc::SerialFile& SFile );
    void SingleTexMesh_LoadLightMapPiece_Insert_VER100( LPDIRECT3DDEVICEQ pd3dDevice, MAPSINGLETEXMESH& mapMesh, sc::SerialFile& SFile );

public:	
	void StartThread( LPDIRECT3DDEVICEQ pd3dDevice );
	void EndThread();

	// �α��� ȭ�� ���� ��� �������� ���� �ε��� �Ǿ� �ְ�, �� ��ȯ�� �Ǳ� ������ NSLandThread �������� ������ �Ǿ��Ѵ�.
	void SetNSLandThread();

public:
	DxStaticMesh();
	~DxStaticMesh();
};

typedef std::vector<LPD3DXMESH>     VEC_D3DXMESH;
typedef VEC_D3DXMESH::iterator      VEC_D3DXMESH_ITER;

namespace NSSTATICMESH
{
	struct STATICDATA
	{
		D3DXVECTOR3	vMin;
		D3DXVECTOR3	vMax;

        std::vector<VERTEX_WORLD>	m_vecVertices;
		std::vector<D3DXVECTOR2>	m_vecLightMapUV;
        VEC_LIGHTMAPOBJ             m_vecLightMapObject;
		std::vector<WORDWORD>		m_vecTriangle_wwLightID;
        TSTRING                     m_strTextureCombined;
        TSTRING                     m_strTextureDirect_Shadow_1;
		TSTRING                     m_strTextureDirect_Shadow_2;
        TSTRING                     m_strTextureIndirect;

		STATICDATA()
            : vMin(FLT_MAX,FLT_MAX,FLT_MAX)
			, vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX)
            , m_strTextureCombined(_T(""))
            , m_strTextureDirect_Shadow_1(_T(""))
			, m_strTextureDirect_Shadow_2(_T(""))
            , m_strTextureIndirect(_T(""))
		{
		};
		~STATICDATA()
		{
			m_vecVertices.clear();
			m_vecLightMapUV.clear();
			m_vecTriangle_wwLightID.clear();
		};
	};

	typedef std::map<NAME_LMID,STATICDATA*>			    MAPSTATICDATA;
	typedef std::map<NAME_LMID,STATICDATA*>::iterator	MAPSTATICDATA_ITER;

	void ExportMaterialList 
    ( 
        const LPDIRECT3DDEVICEQ pd3dDevice, 
        DxFrameMesh* const pFrameMesh, 
        DxStaticMesh* pStaticMesh, 
		LIGHTMAPUV_MAIN* pLightMapUV_MAIN,
        BOOL bProgress, 
        BOOL bPiece, 
        BOOL bLightMap, 
        DxStaticMesh* pStaticMeshColl,
        DxPieceManagerPSF* pPieceManager, 
        const TSTRING& strLightMapName,
        float fLightMapDetail, 
		const DxTextureEffMan* pTextureEffMan, 
		const CMaterialSetting* pMaterialSetting,
		BOOL bRan_1_0,
		float fCellSize
    );

	// Ư���� Mesh ���� ���� �۾�. ( Animation �Ǵ� Mesh )
	void ExportMaterialList ( LPDIRECT3DDEVICEQ pd3dDevice, 
								DxMeshes *pmsMeshs,
								DxStaticMesh *pStaticMesh, 
								const CMaterialSetting *pMaterialSetting,
								float fCellSize );

	void ConvertMAPtoLIST ( MAPSINGLETEXMESH& mapData, PDXSINGLETEXMESH& pList );
	void ConvertTexEffMesh( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEff, MAPSINGLETEXMESH& mapData );
    void SeperateMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMeshIn, VEC_D3DXMESH& vecD3DXMESH );

	void RenderRadiosityForFrameMesh
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		DxFrameMesh* const pFrameMesh, 
		DxLandMan* pLandMan,
		LIGHTMAPUV_MAIN* pLightMapUV_MAIN,
		LPDIRECT3DTEXTUREQ pTextureSystem, 
		float fRadiosityReflectance,
		float fRadiosityFogBlend,
		DWORD dwRadiosityFogLength
	);
}