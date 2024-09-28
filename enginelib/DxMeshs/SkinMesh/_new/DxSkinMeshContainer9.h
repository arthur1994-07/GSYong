#pragma once

#include <boost/tr1/memory.hpp>

#include "./DxSkinMesh9.h"

namespace sc {
    class BaseStream;
    class SerialFile;
}

struct DxSkinVB_ATT;
struct SMATERIAL_PIECE;
struct DXMATERIAL_CHAR_EFF;
class DxTangentMesh;
class DxMultiMaterialSkinMesh;

#include "DxSMeshDATA.h"

struct DXMATERIALEX
{
	enum
	{
		FLAG_ALPHA_HARD		= 0x0001,
		FLAG_ALPHA_SOFT		= 0x0002,
	};

	DWORD				dwFlags;
	DWORD				m_dwTexID;
	TextureResource		m_textureRes;

	DXMATERIALEX ()
		: dwFlags(NULL)
		, m_dwTexID(0)
	{
	}
};

//typedef struct D3DXMATERIAL {
//    D3DMATERIAL9 MatD3D;
//    LPSTR pTextureFilename;
//} D3DXMATERIAL;

//typedef enum D3DXMESHDATATYPE {
//    D3DXMESHTYPE_MESH = 0x001,
//    D3DXMESHTYPE_PMESH = 0x002,
//    D3DXMESHTYPE_PATCHMESH = 0x003,
//    D3DXEDT_FORCE_DWORD = 0x7fffffff
//} D3DXMESHDATATYPE;

//typedef struct D3DXMESHDATA {
//
//    D3DXMESHDATATYPE Type;
//	
//    union
//    {
//    LPD3DXMESH      pMesh;
//    LPD3DXPMESH     pPMesh;
//    LPD3DXPATCHMESH pPatchMesh;
//    }
//	
//} D3DXMESHDATA, *LPD3DXMESHDATA;


//typedef struct _D3DXMESHCONTAINER {
//    LPSTR Name;
//    D3DXMESHDATA MeshData;
//    LPD3DXMATERIAL pMaterials;
//    LPD3DXEFFECTINSTANCE pEffects;
//    DWORD NumMaterials;
//    DWORD *pAdjacency;
//    LPD3DXSKININFO pSkinInfo;
//    struct _D3DXMESHCONTAINER *pNextMeshContainer;
//} D3DXMESHCONTAINER, *LPD3DXMESHCONTAINER;


#define COL_ERR (-1)


struct SVERTEXINFLU
{
	enum { MAX_BONE = 6, };
	const static DWORD	VERSION;

	DWORD		m_dwIndex;	// [shhan][2014.05.13] 이전 셋팅에서의 VertexIndex 값. 이 값은 Mesh 생성시 Vertex최적화 옵션이 변할 경우 문제가 생길 수 있음.
							//						그냥 (버텍스가 유효하다. 유효하지 않다.) 로만 사용되면 될꺼 같음.

	D3DXVECTOR3	m_vVector;
	D3DXVECTOR3	m_vNormal;

	DWORD		m_dwNumBone;
	DWORD		m_pBone[MAX_BONE];
	float		m_pWeights[MAX_BONE];

	BOOL SaveFile ( sc::SerialFile &SFile );
	BOOL LoadFile ( sc::BaseStream &SFile );

	SVERTEXINFLU ();
	void SetBoneNum ( DWORD _dwBone );
};

struct SBoneInfluence
{
	enum { MAX_BONE = 6, };
	DWORD	dwInfue;
	DWORD	pVertices[MAX_BONE];
	float	pWeights[MAX_BONE];

	SBoneInfluence ();
	void SetInfue ( DWORD _dwInfue );
};

//--------------------------------------------------------------------------------------
// Name: struct D3DXMESHCONTAINER_DERIVED
// Desc: Structure derived from D3DXMESHCONTAINER so we can add some app-specific
//       info that will be stored with each mesh
//--------------------------------------------------------------------------------------

struct MESHDATA_HLSL
{
	DWORD					m_NumAttributeGroups; 
	DWORD					m_NumInfl;
	LPD3DXBUFFER			m_pBoneCombinationBuf;
	D3DXMESHDATA            m_MeshData;

	void CleanUp();

	MESHDATA_HLSL();
	~MESHDATA_HLSL();
};

struct SMeshContainer : public D3DXMESHCONTAINER
{
	static	DWORD			VERSION;
	static	DWORD			g_dwStaticBoneMatrixNum;

	DXMATERIALEX*			m_pMaterialEx;

	// SkinMesh info
	LPD3DXMESH				m_pOrigMesh;
	//LPD3DXATTRIBUTERANGE	pAttributeTable;
	//METHOD					emMethod;

	MESHDATA_HLSL			m_sMeshDataHLSL;	// VisualMaterial, Overlay, TotalShader 등의 Effect 에서 쓰인다.
	//MESHDATA_HLSL			m_sMeshDataShadow;	// 그림자용 Mesh. 고퀄리티 Shadow를 위해서 Shader 로 렌더링을 해야 한다.

	DWORD					m_dwNumAttributeGroups; 
	DWORD					m_dwNumInfl;
	LPD3DXBUFFER			m_pBoneCombinationBuf;
	//char*					szBoneName;
    TSTRING     			m_strBoneName;
	DWORD					m_dwNumBoneMatrix;
	//D3DXMATRIX**			m_ppBoneMatrixPtrs;
	D3DXMATRIX**			m_ppStaticMatrixPtrs;		// Bone 이 아닌 Mesh 의 경우 기본적인 Matrix 를 가지고 있도록 한다.

	// [shhan][2014.10.08]
	// 이전에는 스킨쪽에서 Bone 순서가 있었고, 이름으로 실제 Bone 들과 연결을 하였다.	m_ppBoneMatrixPtrs
	// m_ppBoneMatrixPtrs 대신 Index 로 변경이 되었지만 Bone 쪽의 Index 들과는 매칭이 안되었다. ( AniThread 때문에 Index로 변경 )
	// 매칭이 될 수도 있지만, 결국 나중가면 흐트러진다. 이유는 밑에 적었다.
	//	Bone 쪽이 Update 될 수 있지만, Skin 은 그렇지 못하다.
	//	만약 이전에 같았다하더라도 이렇게 되면서, 2 관계의 Index 가 깨지게 된다.
	//	그래서 2개의 연결고리로 m_arrayBoneIndex 를 사용하게 된다. ( BoneName 으로 연결해놓음 )
	DWORD*					m_arrayBoneIndex;			// Thread 작업되면서 m_ppBoneMatrixPtrs 을 대신해서 Bone Index 를 기록하도록 한다.
														
	D3DXMATRIX*				m_pBoneOffsetMatrices;
	DWORD					m_dwNumPaletteEntries;
	DWORD					m_iAttributeSW;			// used to denote the split between SW and HW if necessary for non-indexed skinning

	float*					m_arrayfInvWeight;		// 현재 Cloth 시뮬레이션에서 사용하기 위해 가지고 있는 데이터. (PxClothParticle::invWeight)

	// Note : Software Render 시. Lock 안 하기 위한 버퍼 생성.~!
	//BYTE*					m_pVB_DATA;
	//DWORD					m_dwFVF;
	//DWORD					m_dwVertexNUM;

	// Note ; LOD를 사용 할 것이다.
	//DxSMeshDATA				m_sMeshLOD[2];	// 0 기본, 1 LOD 적용.~!

	//// Note : 카툰쪽에서 쓰이는 것이다 . 하나는 NONINDEXED 일 경우 이고, 다른 하나는 HLSL 일 경우이다.
	//DxSkinVB_ATT*			m_pSlimMesh;
	//DxSkinVB_ATT*			m_pEdgeMeshHLSL;

	// Note : Normal Map을 사용하는 Mesh에서 쓰이는 것이다.
	//DxSkinVB_ATT*			m_pNormalMesh;

    // Note : Tangent Mesh용
    std::tr1::shared_ptr<DxTangentMesh>				m_spTangentMesh;
	std::tr1::shared_ptr<DxMultiMaterialSkinMesh>	m_spMultiMaterialSkinMesh;

	SMeshContainer			*m_pGlobalNext;

public:
	SMeshContainer ()
        : m_pMaterialEx( NULL )

        , m_pOrigMesh( NULL )
        //, pAttributeTable( NULL )
        //, emMethod( D3DNONINDEXED )

        , m_dwNumAttributeGroups( 0 )
        , m_dwNumInfl( 0 )
        , m_pBoneCombinationBuf( NULL )
        , m_dwNumBoneMatrix( 0 )
        //, m_ppBoneMatrixPtrs( NULL )
		, m_ppStaticMatrixPtrs(NULL)
		, m_arrayBoneIndex( NULL )
        , m_pBoneOffsetMatrices( NULL )
        , m_dwNumPaletteEntries( 0 )
        //, UseSoftwareVP( false )
        , m_iAttributeSW( 0 )

		, m_arrayfInvWeight(NULL)

       // , m_pVB_DATA( NULL )
       // , m_dwFVF( 0 )
       // , m_dwVertexNUM( 0 )

        //, m_pSlimMesh( NULL )
        //, m_pEdgeMeshHLSL( NULL )

       // , m_pNormalMesh( NULL )

        , m_pGlobalNext( NULL )
	  {
          Name = NULL;

          memset( &MeshData, 0, sizeof( D3DXMESHDATA ) );

          pMaterials = NULL;
		  NumMaterials = 0;

          pEffects = NULL;
          pAdjacency = NULL;
          pSkinInfo = NULL;

          pNextMeshContainer = NULL;
	  }

	  ~SMeshContainer();

public:
	HRESULT GenerateSkinnedMesh ( IDirect3DDevice9 *pd3dDevice );
	HRESULT GenerateObjectMesh ( IDirect3DDevice9 *pd3dDevice );
	BOOL IsCartoon_CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, float fThickness, float fSizeADD );	// 카툰 메쉬가 없다면 생성을 한다.
	void CreateNormalMapMESH( LPDIRECT3DDEVICEQ pd3dDevice );										// Normal Map을 생성한다.

public:
	LPDIRECT3DTEXTUREQ SetMaterial ( LPDIRECT3DDEVICEQ pd3dDevice, 
									DWORD nIndex, 
									BOOL& bZBiasChange, 
									SMATERIAL_PIECE *pmtrlPiece=NULL, 
									DXMATERIAL_CHAR_EFF* pmtrlSpecular=NULL,
									LPDIRECT3DTEXTUREQ* apOverlayTex=NULL );

	HRESULT ReSetMaterial ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	BOOL IsAlphaTex_HARD ( DWORD nMtIndex );
	BOOL IsAlphaTex_SOFT ( DWORD nMtIndex );

public:
	//BOOL IsCollision ( const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vDetectPos, DWORD &dwDetectFace ) const;
	BOOL GetClosedPoint ( const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vDetectPos, DWORD &dwDetectIndex,
		D3DXVECTOR3& vCLOSED_POINT, D3DXVECTOR3& vCLOSED_NORMAL, const DxSkeleton* pSkeleton );
	BOOL GetClosedPoint2 ( const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vDetectPos, DWORD &dwDetectIndex,
		D3DXVECTOR3& vCLOSED_POINT, D3DXVECTOR3& vCLOSED_NORMAL, const DxSkeleton* pSkeleton, const D3DXMATRIX& matStaticLinkBone );
	HRESULT GetVertexInfluences ( const DWORD dwIndex, SVERTEXINFLU *pVertInflu );

	HRESULT CalculateVertexInfluences( const SVERTEXINFLU& sVertInflu, 
										D3DXVECTOR3 &vVecOutput, 
										D3DXVECTOR3 &vNorOutput,
										const LPD3DXMATRIX pBoneMatrice, 
										BOOL bIdentity, 
										const DxSkeletonMaintain& sSkeleton, 
										const D3DXMATRIX& matStaticLinkBone );

	HRESULT CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

	BOOL GetStaticVertexData( std::vector<VERTEXNORCOLORTEX4>& vecVertices, const DxSkeleton* pSkeleton, const D3DXMATRIX& matStaticLinkBone );

public:
	DWORD GetNumMaterials ()		{	return NumMaterials; }

public:
	HRESULT UpdateSkinnedMesh( PBYTE pbData, DWORD dwFVF, LPD3DXMATRIX pBoneMatrices, const DxSkeleton* pSkeleton );
	void GenerateSkinnedMeshNormalMap( IDirect3DDevice9 *pd3dDevice );
	void GenerateSkinnedMeshNormalMap( IDirect3DDevice9 *pd3dDevice, MESHDATA_HLSL &sMeshData, DWORD dwFVF_ExceptPOSITION_MASK );
	//void CreateShadowForMaterial( IDirect3DDevice9 *pd3dDevice );
    //void GenerateSkinnedMeshNON_public( IDirect3DDevice9 *pd3dDevice );

private:
	void GenerateSkinnedMeshNON( IDirect3DDevice9 *pd3dDevice );
	void GenerateSkinnedMeshHLSL( IDirect3DDevice9 *pd3dDevice, bool &bUseSoftwareVP );
	void GenerateSkinnedMeshCPU( IDirect3DDevice9 *pd3dDevice );
	void GenerateSkinnedMeshMaterial( IDirect3DDevice9 *pd3dDevice );
	void GenerateObjectMeshMaterial( IDirect3DDevice9 *pd3dDevice );

private:
	void CreateLOD_SMeshNON( IDirect3DDevice9* pd3dDevice, LPD3DXMESH pMesh, DWORD* pAdjacencySRC, LPD3DXMESH& pLOD_HIGH, LPD3DXMESH& pLOD_LOW );
	void CreateLOD_SMeshHLSL( IDirect3DDevice9* pd3dDevice, LPD3DXMESH pMesh, LPD3DXMESH& pLOD_LOW, LPD3DXMESH& pLOD_HIGH );
	void CreateLOD_SMeshCPU( IDirect3DDevice9* pd3dDevice, LPD3DXMESH pMesh, DWORD* pAdjacencySRC, LPD3DXMESH& pLOD_HIGH, LPD3DXMESH& pLOD_LOW );

public:
	void SaveNORMAL( sc::SerialFile& SFile );
	void SaveHLSL( sc::SerialFile& SFile );
	void SaveNormalMesh( sc::SerialFile& SFile );
	void LoadNORMAL( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );
	void LoadHLSL( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );
	void LoadNormalMesh( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

	void SaveLOD_SW( sc::SerialFile& SFile );
	void LoadLOD_SW( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );
};

typedef SMeshContainer*		PSMESHCONTAINER;
