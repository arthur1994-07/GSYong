#pragma once

#include "../../DxTools/TextureManager.h"

#include "../AABBBinaryTree/AABBRenderStrategy.h"

class DxDecal;
class LightMapRender;
class LightMapRenderMat;

namespace NSTexture16Bit
{
	struct DATA;
};


class LightMapRenderMan : public AABBRenderStrategy
{
private:
	enum
	{
		VERSION = 0x0104,
	};

private:
    DWORD               m_dwFVF;
    D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;
	TSTRING				m_strTextureDay;    	// 이전 렌더링 Type 에서 필요함. LightMap
	TSTRING				m_strTextureNight;  	// 이전 렌더링 Type 에서 필요함. LightMap
    TextureResource		m_textureResDay;		// 이전 렌더링 Type 에서 필요함.
    TextureResource		m_textureResNight;		// 이전 렌더링 Type 에서 필요함.

	AABBBinaryTreeBase*	    m_pLightMapTree;	// 실제 UV값, LightMap UV값이 들어 있고 실제 맵도 들어있다.

public:
	virtual AABBBinaryTreeBase* GetAABBBinaryBase();

    virtual int Create( const LPDIRECT3DDEVICEQ pd3dDevice, 
                        const DWORD dwFaceNUM, 
                        BYTE* pVertices, 
                        const DWORD dwFVF, 
                        EM_LAND_TYPE emLandType, 
						BOOL bUserLightMapUV,
						BOOL bRan_1_0,
						AABB_BINARY_TREE_TYPE emType,
						float fCellSize );

	virtual void SetTextureName( const TSTRING& strTextureCombined,
								const TSTRING& strTextureDirect_Shadow_1,
								const TSTRING& strTextureDirect_Shadow_2,
								const TSTRING& strTextureIndirect );

	//virtual void ReleaseLightMap();

    virtual void GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

    virtual void Render( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );

	void RenderCullingMesh( LPDIRECT3DDEVICEQ pd3dDevice, boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread ) override;

	// 저사양 
	virtual void RenderMaterialTnL( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV );
	virtual void RenderCascadeShadow( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV, BOOL bAlphaTex );

	virtual void RenderMaterial( LPDIRECT3DDEVICEQ pd3dDevice,
								const CLIPVOLUME* pCV, 
								const MATERIAL_DATA& sMaterial_Data );
	
	virtual void RenderMaterialWater( LPDIRECT3DDEVICEQ pd3dDevice,
										const CLIPVOLUME* pCV, 
										const MATERIAL_DATA& sMaterial_Data,
										float& fHeight,
										float& fNearLength );

    virtual void RenderPiece( const LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTextureDay, LPDIRECT3DTEXTUREQ pTextureNight );                     // 조각파일

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
	virtual VOID ResetOcclusionQuery();
	virtual void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex );

    virtual BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, EMCOLLMODE emCullMode ) const;

	virtual BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const;

    // Note : 충돌이 되면 바로 리턴한다.
    virtual BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, const NSTexture16Bit::DATA* pData16Bit, int nIndexUV );

	virtual void GetVertexNum( DWORD& dwVertexNum );
    virtual void GetVertices( std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV );
	virtual void GetVertices( std::vector<VERTEXNORCOLORTEX5>& vecVertices );

	virtual void ClipMesh( DxDecal* pDecal );

public:
	void LoadTexture( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor, const NSTexture16Bit::DATA* pDataDay );

	void SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate );

	void RenderMaterial_OtherFX( LPDIRECT3DDEVICEQ pd3dDevice,
								const MATERIAL_DATA& sMaterial_Data );

	void RenderPieceMaterial_Instancing( LPDIRECT3DDEVICEQ pd3dDevice,
										ID3DXEffect* pFX, 
										const MATERIAL_DATA& sMaterial_Data );

	void RenderCascadeShadow_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex );

public:
	virtual void Save( sc::SerialFile& SFile, BOOL bPiece );
	virtual void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece );

public:
	void Import( LPDIRECT3DDEVICEQ pd3dDevice, LightMapRenderMan* pSrc, float fCellSize, bool &bEnableVertexColor );

private:
	void CleanUp();

public:
	LightMapRenderMan();
	~LightMapRenderMan();
};