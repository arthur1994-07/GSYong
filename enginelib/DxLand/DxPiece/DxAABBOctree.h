// NsOcTree.h: interface for the CCollision class.
//
//	class DxAABBOctree : 정점 데이터와 면 갯수를 넣으면 Octree 형식의 최적화된 OptimizeMesh 를 만든다.
//
//	Name : Sung-Hwan Han
//	Begin :2004/10/21
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../AABBBinaryTree/AABBRenderStrategy.h"
#include "../DxLandDef.h"

class OBJOCTree;
class DxTexEffBase;
class DxTexEffFlowUV;
class DxTexEffRotate;
class AABBBinaryTreeBase;
class DxDecal;


class DxAABBOctree : public AABBRenderStrategy
{
protected:
	static const DWORD	VERSION;

protected:
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;
	OBJOCTree*			m_pOcTree;

public:
	virtual AABBBinaryTreeBase* GetOBJOCTree();
	virtual AABBBinaryTreeBase* GetAABBBinaryBase();

public:
	virtual void GetAABBSize ( D3DXVECTOR3 &_vMax, D3DXVECTOR3 &_vMin )
	{
		_vMax = m_vMax; _vMin = m_vMin;
	}
	virtual void FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, sc::SerialFile* const SFile );
	virtual void RenderTHREAD_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxTexEffFlowUV* pEff );
	virtual void RenderTHREAD_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxTexEffRotate* pEff );
    virtual void DynamicLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, sc::SerialFile* const SFile );
	virtual void Render ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void RenderCullingMesh( LPDIRECT3DDEVICEQ pd3dDevice, boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread ) override;
	virtual void RenderPiece ( const LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTextureDay, LPDIRECT3DTEXTUREQ pTextureNight );									// 조각파일
	virtual void Render_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffFlowUV* pEff );		// 조각파일
	virtual void Render_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffRotate* pEff );		// 조각파일
    virtual void Render_VisualMaterialColor( LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffBase* pEff, BOOL bRealPL, const D3DXMATRIX& matWorld );

    void RenderTHREAD_VisualMaterial( const LPDIRECT3DDEVICEQ pd3dDevice, 
                                        const CLIPVOLUME* pCV, 
                                        DxTexEffBase* pEff, 
										const D3DXMATRIX& matWorld ) override;

    void RenderPiece_VisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
                                      DxTexEffBase* pEff, 
                                      BOOL bRealPL, 
                                      const D3DXMATRIX& matWorld, 
                                      const D3DXVECTOR2& vLightMapUV_Offset,
                                      float fAlpha, 
									  LPDIRECT3DTEXTUREQ pTextureDay, 
									  LPDIRECT3DTEXTUREQ pTextureNight ) override;

	void RenderCullingMeshVM( LPDIRECT3DDEVICEQ pd3dDevice, 
								boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread,
								DxTexEffBase* pEff, 
								const D3DXMATRIX& matWorld ) override;

	virtual void Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bAlphaTex );

	virtual VOID CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual VOID DestroyQuery();
	virtual void ResetOcclusionQuery();
	virtual void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex );

	virtual void ClipMesh( DxDecal* pDecal );

//	// Note : Color Thread 를 위한 작업
//public:
//	virtual void InsertColorList( LOADINGDATALIST &listColorData );

public:
	virtual int Create( const LPDIRECT3DDEVICEQ pd3dDevice, 
						const DWORD dwFaceNUM, 
						BYTE* pVertices, 
						const DWORD dwFVF, 
						EM_LAND_TYPE emLandType, 
						BOOL bUserLightMapUV,
						BOOL bRan_1_0,
						AABB_BINARY_TREE_TYPE emType,
						float fCellSize );

	virtual void GetVertexNum( DWORD& dwVertexNum );
    virtual void GetVertices( std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV );
	virtual void GetVertices( std::vector<VERTEXNORCOLORTEX5>& vecVertices );

public:
	virtual BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const;

	virtual BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, EMCOLLMODE emCullMode ) const;

    // Note : 충돌이 되면 바로 리턴한다.
    virtual BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, const NSTexture16Bit::DATA* pData16Bit, int nIndexUV );

    virtual void GetCollisionLineInTriListExceptY ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ); // 주병진 추가 메소드

    virtual void GetClosedPoint( const D3DXVECTOR3& vBasePos, D3DXVECTOR3& vCollisionOut, float& fDistance );

public:
	virtual void Save ( sc::SerialFile& SFile, BOOL bPiece );
	virtual void Load ( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece );
	virtual void Load_VER100( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, std::string& szName );

public:
    void ThreadLoad( const CLIPVOLUME &sCV, LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading );
	BOOL IsHaveLightMapUV();

protected:
	void CleanUp();

public:
	DxAABBOctree();
	~DxAABBOctree();
};
typedef DxAABBOctree* PDXAABBOCTREE;
