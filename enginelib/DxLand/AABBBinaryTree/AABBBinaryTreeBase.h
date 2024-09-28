#pragma once

#include "../../../SigmaCore/gassert.h"

#include "../../DxTools/DxVertexFVF.h"

#include "../DxLandDef.h"

struct DxAABBNode;
struct MATERIAL_DATA;
class DxTexEffBase;

namespace sc
{
    class SerialFile;
} // namespace sc

class AABBBinaryTreeBase
{
protected:
	int						m_nID;				// ID - Occlusion Culling �� ���� ���δ�.
    AABB_BINARY_TREE_TYPE   m_emType;
    BOOL		            m_bCheck;
	BOOL		            m_bLoad;
	DWORD		            m_dwFileCur;        // bDynamicLoading �� ���� ���ܵд�.
	D3DXVECTOR3	            m_vMax;
	D3DXVECTOR3	            m_vMin;
	D3DXVECTOR3	            m_vCenter;
    DWORD                   m_dwFVF;
    BYTE*		            m_pVertex;          // ����ȭ ������ ����� ���� ����
	DWORD		            m_dwFace;           // ����ȭ ������ ����� ���� ����
	LPD3DXMESH	            m_pOcMesh;	    	// ����ȭ ������ ����� �� Mesh�̴�. Render���� ������ �ʴ´�. �� T&L ��Ŀ����� ���̰� �ȴ�.
	BOOL					m_bAlphaTex;		// �����ؽ��ĸ� ���� Mesh�ΰ�~?
	LPD3DXMESH	            m_pShadowMesh;    	// �׸��ڸ� ������ �ϱ����� Position ���� ������ �ִ� Mesh. Ȥ�� Position + UV
    DxAABBNode*	            m_pCollsionTREE;

	DWORD					m_dwVertexNUM;
	DWORD					m_dwFaceNUM;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;	// m_pShadowMesh �� VB. Release �� ������� �Ѵ�.
	LPDIRECT3DINDEXBUFFER9	m_pIB;	// m_pShadowMesh �� IB. Release �� ������� �Ѵ�.

	// DxOcclusionQuery
	// Occlusion Culling �� ���鶧 �ٷ� ����� �����ϱ� ������ OnResetDevice, OnLostDevice ���� �۾����� �ʿ䰡 ����.
	// ��� Occlusion Culling �� ���鶧, CreateQuery, DestroyQuery �� ö���ϰ� ������.
protected:
	LPDIRECT3DQUERY9	m_pd3dQuery;
	BOOL				m_bQueryTest;
	DWORD				m_dwCullPixels;

public:
	VOID CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID DestroyQuery();
	HRESULT GetDataQuery( DWORD& dwCullPixels );
	void ResetOcclusionQuery();

public:
    AABBBinaryTreeBase *m_pLeftChild;
	AABBBinaryTreeBase *m_pRightChild;

    // Get
public:
	int GetID()						const;
    const D3DXVECTOR3& GetMax()     const;
    const D3DXVECTOR3& GetMin()     const;
	const D3DXVECTOR3& GetCenter()  const;
    const BYTE* GetVertices()       const;
    DWORD GetFaceNum()              const;
    BOOL  GetLoad()                 const;
    AABB_BINARY_TREE_TYPE GetType() const;
    LPD3DXMESH GetOcMesh()          const;
    LPD3DXMESH* GetppOcMesh();
    const DxAABBNode* GetCollisionTree() const;
    DxAABBNode** GetppCollisionTree();

    // Set
public:
    void SetLoad();
    void SetFaceNum_Zero();
    void SetFVF( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF );

    // �Ϲ�
public:
    void InitAABBBBinaryTreeBase( const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, BYTE* pVertex, DWORD dwFaceNum );
    void DeleteVertexArray();
    void CleanUp_Base();

	// PixelShader 1.1 �� �������� ���ϴ� �׷���ī�带 ���ؼ� �̷��� �۾���.
	void Render( LPDIRECT3DDEVICEQ pd3dDevice ) const;

	// �׸��ڸ� ������ �ϱ� ���� �̷��� ����.
	void RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex );

	// �׸��ڸ� ������ �ϱ� ���� �̷��� ����.
	void RenderCascadeShadow_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex );

	// �׸��� �������� Mesh �� �����Ѵ�.
	void CreateShadowMesh( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex );

    // virtual
public:

	// = 0
	virtual BOOL IsLoad() const = 0;
    virtual void DrawEff( const LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
	virtual void Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex ) = 0;
	virtual void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, VEC_DWORD& vecOctreeIndex ) = 0;
    virtual BOOL DynamicLoad( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bThread ) = 0;
    virtual void Save( sc::SerialFile& SFile, BOOL bPiece ) = 0;
	virtual void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece ) = 0;

	// 
	virtual void GetVertexNum( DWORD& dwVertexNum );
	virtual void GetVectorVertices( std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV );
	virtual void GetVectorVertices( std::vector<VERTEXNORCOLORTEX5>& vecVertices );
	virtual void Load_Prev( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece )	{}

	virtual void RenderOctreeEff( LPDIRECT3DDEVICEQ pd3dDevice, 
									DxTexEffBase* pEff )								{ GASSERT( 0 && "OBJOCTree �� �ƴѰ����� ȣ���" ); }

	virtual void RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
										DxTexEffBase* pEff, 
										const D3DXMATRIX& matWorld, 
										const D3DXVECTOR2& vLightMapUV_Offset,
										float fAlpha,
										LPDIRECT3DTEXTUREQ pTextureDay, 
										LPDIRECT3DTEXTUREQ pTextureNight )				{ GASSERT( 0 && "LightMapRender �� �ƴѰ����� ȣ���" ); }

	virtual void RenderMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
								const MATERIAL_DATA& sMaterial_Data )					{ GASSERT( 0 && "LightMapRenderMat �� �ƴѰ����� ȣ���" ); }

	virtual void RenderPieceMaterial_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, 
												ID3DXEffect* pFX, 
												const MATERIAL_DATA& sMaterial_Data )	{ GASSERT( 0 && "LightMapRenderMat �� �ƴѰ����� ȣ���" ); }

	virtual void RenderMaterialWater( LPDIRECT3DDEVICEQ pd3dDevice, 
										const MATERIAL_DATA& sMaterial_Data, 
										float& fHeight, 
										float& fNearLength )							{ GASSERT( 0 && "LightMapRenderMat �� �ƴѰ����� ȣ���" ); }

	virtual void MakeMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice )						{ GASSERT( 0 && "LightMapRenderMat �� �ƴѰ����� ȣ���" ); }

	virtual void SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate )				{ GASSERT( 0 && "LightMapRenderMat �� �ƴѰ����� ȣ���" ); }

    // static
public:
    static AABBBinaryTreeBase* CreateFactory( AABB_BINARY_TREE_TYPE emType );

public:
    AABBBinaryTreeBase( AABB_BINARY_TREE_TYPE emType );
    virtual ~AABBBinaryTreeBase();
};