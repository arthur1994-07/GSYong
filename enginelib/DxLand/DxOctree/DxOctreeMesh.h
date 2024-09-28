#ifndef	__DX_OCTREE_MESH__
#define	__DX_OCTREE_MESH__

#include "../../DxTools/DxCustomTypes.h"
#include "../../DxTools/Collision.h" // 주병진
#include "../../DxTools/TextureManager.h"

#include <vector>    // 주병진

class sc::SerialFile;

struct DxAABBNode;
struct DxMeshes;

struct DxOctreeMesh
{
	enum
	{
		VERSION = 0x100,
	};

	DWORD							m_dwFVF;

	DWORD							m_dwNumVertices;
	LPDIRECT3DVERTEXBUFFERQ			m_pVB;

	DWORD							m_dwNumFaces;
	LPDIRECT3DINDEXBUFFERQ			m_pIB;

	DWORD							m_dwAttribTableSize;
	LPD3DXATTRIBUTERANGE			m_pAttribTable;	

	DxAABBNode						*m_pAABBTreeRoot;

	DWORD GetFVF ()			{ return m_dwFVF; }
	DWORD GetNumVertices ()	{ return m_dwNumVertices; }
	DWORD GetNumFaces ()	{ return m_dwNumFaces; }

public:
	DxOctreeMesh () :
		m_dwNumVertices(0),
		m_pVB(NULL),
		m_dwNumFaces(0),
		m_pIB(NULL),
		m_dwAttribTableSize(0),
		m_pAttribTable(NULL),
		m_pAABBTreeRoot(NULL)
	{
	}
	virtual	~DxOctreeMesh();

	HRESULT CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, D3DXMATRIX *pmatComb );
	HRESULT CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxOctreeMesh* pMesh, D3DXMATRIX *pmatComb );

	HRESULT	CreateDxOctreeMesh_OLD ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD NumFaces, DWORD NumVertices, DWORD FVF );
	HRESULT	CreateDxOctreeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD NumFaces, DWORD NumVertices, DWORD FVF );
	HRESULT	ReleaseDxOctreeMesh	();	

	HRESULT	LockVertexBuffer ( VOID** ppbData );
	HRESULT	LockVertexBuffer ( DWORD dwFlag, VOID** ppbData );

	HRESULT	LockIndexBuffer ( VOID** ppbData );
	HRESULT	LockIndexBuffer ( DWORD dwFlag, VOID** ppbData );

	HRESULT	UnlockVertexBuffer	();
	HRESULT	UnlockIndexBuffer	();

	BOOL					CreateAttribute	( DWORD		AttribTableSize );	
	LPD3DXATTRIBUTERANGE	GetAttributeTbl	();
	BOOL					ReleaseAttribure();	

	HRESULT GetAttributeTable ( LPD3DXATTRIBUTERANGE pAttribTable, DWORD* pdwSize )
	{
		if ( !pAttribTable )
		{
			*pdwSize = m_dwAttribTableSize;
			return S_FALSE;
		}

		memcpy ( pAttribTable, m_pAttribTable, sizeof(D3DXATTRIBUTERANGE)*m_dwAttribTableSize );

		return S_OK;
	}

	HRESULT	DrawSubset			( DWORD	AttribID, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : AABB 충돌 처리 부분.
	//
protected:
	HRESULT MakeAABBNode ( DxAABBNode *pNode, D3DXMATRIX &matComb, WORD *pIndices,
		PBYTE pbPoints, WORD *pwFaces, DWORD dwNumFaces, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

	void IsCollision_DxOctreeMesh( DxAABBNode *pAABBCur, const D3DXVECTOR3& vStartOrg, D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2, D3DXVECTOR3 &vCollision, 
									D3DXVECTOR3 &vNormal, DWORD& dwAttribid, EMCOLLMODE emCullMode, IN OUT BOOL& bColl );

	BOOL IsCollisionLineSimple( DxAABBNode *pAABBCur, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode );

public:
	HRESULT	MakeAABBTree ( D3DXMATRIX &matComb );
	void IsCollision_OctreeMesh( const D3DXVECTOR3& vStartOrg, D3DXVECTOR3 &vPoint1, D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
					D3DXVECTOR3 &vNormal, DWORD& dwAttribid, EMCOLLMODE emCullMode );

	BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode );

	void GetCollisionLineInTriListExceptY2 ( DxAABBNode *pAABBCur, const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ); // 주병진 추가 메소드
	void GetCollisionLineInTriListExceptY  ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList );                       // 주병진 추가 메소드

public:
	BOOL		SaveFile ( sc::SerialFile &SFile );
	BOOL		LoadFile_NEW ( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL		LoadFile_OLD ( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
};

struct DXOCMATERIAL
{
	D3DMATERIALQ		rgMaterial;
	TextureResource m_textureRes;
	char				szTexture[MAX_PATH];

	DXOCMATERIAL ()
	{
		memset( szTexture, 0, sizeof(char)*MAX_PATH );
	}

    void SaveFile( sc::SerialFile &SFile );
    void LoadFile( sc::SerialFile &SFile );
};

struct DxOcMeshes
{
	DxOctreeMesh*		pOcMesh;

	DWORD				cMaterials;
	DXOCMATERIAL*		pMaterials;

	DxOcMeshes*			m_pNext;

	HRESULT CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pMesh, D3DXMATRIX *pmatComb );
	HRESULT CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxOcMeshes *pMesh, D3DXMATRIX *pmatComb );
	void ClearAll ();

	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL		SaveFile ( sc::SerialFile &SFile );
	BOOL		LoadFile ( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

	DxOcMeshes () :
		pOcMesh(0),
		cMaterials(0),
		pMaterials(0),
		m_pNext(0)
	{
	}

	~DxOcMeshes ()
	{
		ClearAll ();

		SAFE_DELETE(m_pNext);
	}
};

#endif	//	__DX_OCTREE_MESH__