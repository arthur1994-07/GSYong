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

	DWORD		m_dwIndex;	// [shhan][2014.05.13] ���� ���ÿ����� VertexIndex ��. �� ���� Mesh ������ Vertex����ȭ �ɼ��� ���� ��� ������ ���� �� ����.
							//						�׳� (���ؽ��� ��ȿ�ϴ�. ��ȿ���� �ʴ�.) �θ� ���Ǹ� �ɲ� ����.

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

	MESHDATA_HLSL			m_sMeshDataHLSL;	// VisualMaterial, Overlay, TotalShader ���� Effect ���� ���δ�.
	//MESHDATA_HLSL			m_sMeshDataShadow;	// �׸��ڿ� Mesh. ������Ƽ Shadow�� ���ؼ� Shader �� �������� �ؾ� �Ѵ�.

	DWORD					m_dwNumAttributeGroups; 
	DWORD					m_dwNumInfl;
	LPD3DXBUFFER			m_pBoneCombinationBuf;
	//char*					szBoneName;
    TSTRING     			m_strBoneName;
	DWORD					m_dwNumBoneMatrix;
	//D3DXMATRIX**			m_ppBoneMatrixPtrs;
	D3DXMATRIX**			m_ppStaticMatrixPtrs;		// Bone �� �ƴ� Mesh �� ��� �⺻���� Matrix �� ������ �ֵ��� �Ѵ�.

	// [shhan][2014.10.08]
	// �������� ��Ų�ʿ��� Bone ������ �־���, �̸����� ���� Bone ��� ������ �Ͽ���.	m_ppBoneMatrixPtrs
	// m_ppBoneMatrixPtrs ��� Index �� ������ �Ǿ����� Bone ���� Index ����� ��Ī�� �ȵǾ���. ( AniThread ������ Index�� ���� )
	// ��Ī�� �� ���� ������, �ᱹ ���߰��� ��Ʈ������. ������ �ؿ� ������.
	//	Bone ���� Update �� �� ������, Skin �� �׷��� ���ϴ�.
	//	���� ������ ���Ҵ��ϴ��� �̷��� �Ǹ鼭, 2 ������ Index �� ������ �ȴ�.
	//	�׷��� 2���� ������� m_arrayBoneIndex �� ����ϰ� �ȴ�. ( BoneName ���� �����س��� )
	DWORD*					m_arrayBoneIndex;			// Thread �۾��Ǹ鼭 m_ppBoneMatrixPtrs �� ����ؼ� Bone Index �� ����ϵ��� �Ѵ�.
														
	D3DXMATRIX*				m_pBoneOffsetMatrices;
	DWORD					m_dwNumPaletteEntries;
	DWORD					m_iAttributeSW;			// used to denote the split between SW and HW if necessary for non-indexed skinning

	float*					m_arrayfInvWeight;		// ���� Cloth �ùķ��̼ǿ��� ����ϱ� ���� ������ �ִ� ������. (PxClothParticle::invWeight)

	// Note : Software Render ��. Lock �� �ϱ� ���� ���� ����.~!
	//BYTE*					m_pVB_DATA;
	//DWORD					m_dwFVF;
	//DWORD					m_dwVertexNUM;

	// Note ; LOD�� ��� �� ���̴�.
	//DxSMeshDATA				m_sMeshLOD[2];	// 0 �⺻, 1 LOD ����.~!

	//// Note : ī���ʿ��� ���̴� ���̴� . �ϳ��� NONINDEXED �� ��� �̰�, �ٸ� �ϳ��� HLSL �� ����̴�.
	//DxSkinVB_ATT*			m_pSlimMesh;
	//DxSkinVB_ATT*			m_pEdgeMeshHLSL;

	// Note : Normal Map�� ����ϴ� Mesh���� ���̴� ���̴�.
	//DxSkinVB_ATT*			m_pNormalMesh;

    // Note : Tangent Mesh��
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
	BOOL IsCartoon_CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, float fThickness, float fSizeADD );	// ī�� �޽��� ���ٸ� ������ �Ѵ�.
	void CreateNormalMapMESH( LPDIRECT3DDEVICEQ pd3dDevice );										// Normal Map�� �����Ѵ�.

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
