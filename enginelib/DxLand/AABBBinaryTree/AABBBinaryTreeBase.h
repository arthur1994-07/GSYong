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
	int						m_nID;				// ID - Occlusion Culling 을 위해 쓰인다.
    AABB_BINARY_TREE_TYPE   m_emType;
    BOOL		            m_bCheck;
	BOOL		            m_bLoad;
	DWORD		            m_dwFileCur;        // bDynamicLoading 을 위해 남겨둔다.
	D3DXVECTOR3	            m_vMax;
	D3DXVECTOR3	            m_vMin;
	D3DXVECTOR3	            m_vCenter;
    DWORD                   m_dwFVF;
    BYTE*		            m_pVertex;          // 최적화 파일을 만들기 위해 존재
	DWORD		            m_dwFace;           // 최적화 파일을 만들기 위해 존재
	LPD3DXMESH	            m_pOcMesh;	    	// 최적화 파일을 만들기 전 Mesh이다. Render에는 쓰이지 않는다. 단 T&L 방식에서는 쓰이게 된다.
	BOOL					m_bAlphaTex;		// 알파텍스쳐를 가진 Mesh인가~?
	LPD3DXMESH	            m_pShadowMesh;    	// 그림자를 렌더링 하기위한 Position 값만 가지고 있는 Mesh. 혹은 Position + UV
    DxAABBNode*	            m_pCollsionTREE;

	DWORD					m_dwVertexNUM;
	DWORD					m_dwFaceNUM;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;	// m_pShadowMesh 의 VB. Release 를 잘해줘야 한다.
	LPDIRECT3DINDEXBUFFER9	m_pIB;	// m_pShadowMesh 의 IB. Release 를 잘해줘야 한다.

	// DxOcclusionQuery
	// Occlusion Culling 을 만들때 바로 만들고 삭제하기 때문에 OnResetDevice, OnLostDevice 에서 작업해줄 필요가 없다.
	// 대신 Occlusion Culling 을 만들때, CreateQuery, DestroyQuery 를 철저하게 해주자.
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

    // 일반
public:
    void InitAABBBBinaryTreeBase( const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, BYTE* pVertex, DWORD dwFaceNum );
    void DeleteVertexArray();
    void CleanUp_Base();

	// PixelShader 1.1 도 지원하지 못하는 그래픽카드를 위해서 이렇게 작업됨.
	void Render( LPDIRECT3DDEVICEQ pd3dDevice ) const;

	// 그림자를 렌더링 하기 위해 이렇게 쓴다.
	void RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex );

	// 그림자를 렌더링 하기 위해 이렇게 쓴다.
	void RenderCascadeShadow_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex );

	// 그림자 렌더링용 Mesh 를 생성한다.
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
									DxTexEffBase* pEff )								{ GASSERT( 0 && "OBJOCTree 이 아닌곳에서 호출됨" ); }

	virtual void RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
										DxTexEffBase* pEff, 
										const D3DXMATRIX& matWorld, 
										const D3DXVECTOR2& vLightMapUV_Offset,
										float fAlpha,
										LPDIRECT3DTEXTUREQ pTextureDay, 
										LPDIRECT3DTEXTUREQ pTextureNight )				{ GASSERT( 0 && "LightMapRender 이 아닌곳에서 호출됨" ); }

	virtual void RenderMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
								const MATERIAL_DATA& sMaterial_Data )					{ GASSERT( 0 && "LightMapRenderMat 이 아닌곳에서 호출됨" ); }

	virtual void RenderPieceMaterial_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, 
												ID3DXEffect* pFX, 
												const MATERIAL_DATA& sMaterial_Data )	{ GASSERT( 0 && "LightMapRenderMat 이 아닌곳에서 호출됨" ); }

	virtual void RenderMaterialWater( LPDIRECT3DDEVICEQ pd3dDevice, 
										const MATERIAL_DATA& sMaterial_Data, 
										float& fHeight, 
										float& fNearLength )							{ GASSERT( 0 && "LightMapRenderMat 이 아닌곳에서 호출됨" ); }

	virtual void MakeMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice )						{ GASSERT( 0 && "LightMapRenderMat 이 아닌곳에서 호출됨" ); }

	virtual void SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate )				{ GASSERT( 0 && "LightMapRenderMat 이 아닌곳에서 호출됨" ); }

    // static
public:
    static AABBBinaryTreeBase* CreateFactory( AABB_BINARY_TREE_TYPE emType );

public:
    AABBBinaryTreeBase( AABB_BINARY_TREE_TYPE emType );
    virtual ~AABBBinaryTreeBase();
};