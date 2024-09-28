#pragma once

#include "../../../SigmaCore/gassert.h"

#include "../../DxTools/Collision/CollisionDef.h"
#include "../../DxTools/DxCustomTypes.h"
#include "../../DxTools/DxVertexFVF.h"
#include "../DxLandDef.h"

class AABBBinaryTreeBase;
class DxTexEffFlowUV;
class DxTexEffRotate;
class DxTexEffBase;
class DxDecal;
class DxCullingMeshSwap;
struct MATERIAL_DATA;

namespace NSTexture16Bit
{
	struct DATA;
};


class AABBRenderStrategy
{
public:
    enum TYPE_RENDER
    {
        TYPE_OCJOCTREE = 0,
        TYPE_LIGHTMAP = 1,
		TYPE_TRIANGLE_LINK_LIGHT_LIGHTMAP = 2,
    };

protected:
    TYPE_RENDER m_emTypeRender;

// 일반
public:
    TYPE_RENDER GetTypeRender();
	BOOL GetLightMapColor( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR2& vUV );

    // = 0
public:
	virtual AABBBinaryTreeBase* GetAABBBinaryBase() = 0;

    virtual void GetAABBSize ( D3DXVECTOR3 &_vMax, D3DXVECTOR3 &_vMin ) = 0;
	virtual int Create( const LPDIRECT3DDEVICEQ pd3dDevice, 
						const DWORD dwFaceNUM, 
						BYTE* pVertices, 
						const DWORD dwFVF, 
						EM_LAND_TYPE emLandType, 
						BOOL bUserLightMapUV,
						BOOL bRan_1_0,
						AABB_BINARY_TREE_TYPE emType,
						float fCellSize ) = 0;
    virtual void Render ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV ) = 0;
	virtual void RenderCullingMesh( LPDIRECT3DDEVICEQ pd3dDevice, boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread ) = 0;
    virtual void RenderPiece ( const LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTextureDay, LPDIRECT3DTEXTUREQ pTextureNight ) = 0;                     // 조각파일
    virtual void Save ( sc::SerialFile& SFile, BOOL bPiece ) = 0;
	virtual void Load ( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece ) = 0;

    virtual void RenderTHREAD_VisualMaterial( const LPDIRECT3DDEVICEQ pd3dDevice, 
                                                const CLIPVOLUME* pCV, 
                                                DxTexEffBase* pEff, 
												const D3DXMATRIX& matWorld ) = 0;

    virtual void RenderPiece_VisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
                                              DxTexEffBase* pEff, 
                                              BOOL bRealPL, 
                                              const D3DXMATRIX& matWorld, 
                                              const D3DXVECTOR2& vLightMapUV_Offset,
                                              float fAlpha, 
											  LPDIRECT3DTEXTUREQ pTextureDay, 
											  LPDIRECT3DTEXTUREQ pTextureNight ) = 0;

	virtual void RenderCullingMeshVM( LPDIRECT3DDEVICEQ pd3dDevice, 
										boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread,
										DxTexEffBase* pEff, 
										const D3DXMATRIX& matWorld ) = 0;

	virtual void Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bAlphaTex ) = 0;

	virtual VOID CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
	virtual VOID DestroyQuery() = 0;
	virtual void ResetOcclusionQuery() = 0;
	virtual void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex ) = 0;

	virtual void GetVertexNum( DWORD& dwVertexNum ) = 0;
    virtual void GetVertices( std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV ) = 0;
	virtual void GetVertices( std::vector<VERTEXNORCOLORTEX5>& vecVertices ) = 0;

	virtual BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const								{ return FALSE; }

    virtual BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, EMCOLLMODE emCullMode ) const	{ return FALSE; }

    // Note : 충돌이 되면 바로 리턴한다.
	virtual BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, const NSTexture16Bit::DATA* pData16Bit, int nIndexUV ) = 0;

	virtual void ClipMesh( DxDecal* pDecal ) = 0;

    // DxAABBOctree 만 사용
public:
    virtual AABBBinaryTreeBase* GetOBJOCTree()                                                                                                      { return NULL; }
    virtual void FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, sc::SerialFile* const SFile )     {}
    virtual void ThreadLoad( const CLIPVOLUME &sCV, LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading )                             {}
	virtual void RenderTHREAD_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxTexEffFlowUV* pEff )                             {}
	virtual void RenderTHREAD_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxTexEffRotate* pEff )                             {}
    virtual void DynamicLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, sc::SerialFile* const SFile )                              {}
	virtual void Render_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffFlowUV* pEff )                                                          {}	// 조각파일
	virtual void Render_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffRotate* pEff )                                                          {}	// 조각파일
    virtual void Render_VisualMaterialColor( LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffBase* pEff, BOOL bRealPL, const D3DXMATRIX& matWorld )           {}
    //virtual void InsertColorList( LOADINGDATALIST &listColorData )                                                                                  {} // Note : Color Thread 를 위한 작업
    virtual void GetCollisionLineInTriListExceptY ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ) {} // 주병진 추가 메소드
    virtual void Load_VER100( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, std::string& szName )                                      {}
    virtual void GetClosedPoint( const D3DXVECTOR3& vBasePos, D3DXVECTOR3& vCollisionOut, float& fDistance )                                        {}

    // LightMapRenderMan 만 사용
public:
	virtual void RenderMaterial( const LPDIRECT3DDEVICEQ pd3dDevice )						{ GASSERT( 0 && "LightMapRenderMan 이 아닌곳에서 호출됨" ); }
	virtual void RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice )								{ GASSERT( 0 && "LightMapRenderMan 이 아닌곳에서 호출됨" ); }

    //virtual void LoadTexture( LPDIRECT3DDEVICEQ pd3dDevice )								{ GASSERT( 0 && "LightMapRenderMan 이 아닌곳에서 호출됨" ); }
	//virtual void ReleaseLightMap()															{ GASSERT( 0 && "LightMapRenderMan 이 아닌곳에서 호출됨" ); }
	virtual void SetTextureName( const TSTRING& strTextureCombined,
								const TSTRING& strTextureDirect_Shadow_1,
								const TSTRING& strTextureDirect_Shadow_2,
								const TSTRING& strTextureIndirect )							{ GASSERT( 0 && "LightMapRenderMan 이 아닌곳에서 호출됨" ); }

	virtual void RenderMaterialTnL( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )	{ GASSERT( 0 && "LightMapRenderMan 이 아닌곳에서 호출됨" ); }

	virtual void RenderMaterial( LPDIRECT3DDEVICEQ pd3dDevice,
								const CLIPVOLUME* pCV, 
								const MATERIAL_DATA& sMaterial_Data )							{ GASSERT( 0 && "LightMapRenderMan 이 아닌곳에서 호출됨" ); }

	virtual void RenderMaterialWater( LPDIRECT3DDEVICEQ pd3dDevice,
										const CLIPVOLUME* pCV, 
										const MATERIAL_DATA& sMaterial_Data,
										float& fHeight,
										float& fNearLength )									{ GASSERT( 0 && "LightMapRenderMan 이 아닌곳에서 호출됨" ); }

    // static
public:
    static AABBRenderStrategy* CreateFactory( TYPE_RENDER emType );

public:
    AABBRenderStrategy();
    virtual ~AABBRenderStrategy();
};
