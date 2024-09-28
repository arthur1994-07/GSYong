#pragma once

#include "../../DxLand/AABBBinaryTree/AABBRenderStrategy.h"

struct MaterialValue;

// ���� - FrameMesh ���� StaticMesh �� ������� ����
//			����޴� Light ���� ������ �۾��� �ϵ��� �Ѵ�.
class DxTriangleLinkLight_LightMap : public AABBRenderStrategy
{
private:
	std::vector<std::tr1::shared_ptr<DxSingleTexMesh>>	    m_vecSingleTexMesh;

	// = 0
public:
	virtual AABBBinaryTreeBase* GetAABBBinaryBase()	{ return NULL; }

	virtual void GetAABBSize ( D3DXVECTOR3 &_vMax, D3DXVECTOR3 &_vMin )	{}
	virtual int Create( const LPDIRECT3DDEVICEQ pd3dDevice, 
						const DWORD dwFaceNUM, 
						BYTE* pVertices, 
						const DWORD dwFVF, 
						EM_LAND_TYPE emLandType, 
						BOOL bUserLightMapUV,
						BOOL bRan_1_0,
						AABB_BINARY_TREE_TYPE emType,
						float fCellSize )				{ return 0; }
	virtual void Render ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV ) {}
	void RenderCullingMesh( LPDIRECT3DDEVICEQ pd3dDevice, boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread ) override	{}
	virtual void RenderPiece ( const LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTextureDay, LPDIRECT3DTEXTUREQ pTextureNight )	{}
	virtual void Save ( sc::SerialFile& SFile, BOOL bPiece )	{}
	virtual void Load ( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece )	{}

	void RenderTHREAD_VisualMaterial( const LPDIRECT3DDEVICEQ pd3dDevice, 
										const CLIPVOLUME* pCV, 
										DxTexEffBase* pEff, 
										const D3DXMATRIX& matWorld ) override	{}

	void RenderPiece_VisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
										DxTexEffBase* pEff, 
										BOOL bRealPL, 
										const D3DXMATRIX& matWorld, 
										const D3DXVECTOR2& vLightMapUV_Offset,
										float fAlpha, 
										LPDIRECT3DTEXTUREQ pTextureDay, 
										LPDIRECT3DTEXTUREQ pTextureNight ) override	{}

	void RenderCullingMeshVM( LPDIRECT3DDEVICEQ pd3dDevice, 
								boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread,
								DxTexEffBase* pEff, 
								const D3DXMATRIX& matWorld ) override {}

	virtual void Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bAlphaTex )	{}

	virtual VOID CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )	{}
	virtual VOID DestroyQuery()	{}
	virtual void ResetOcclusionQuery()	{}
	virtual void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex )	{}

	virtual void GetVertexNum( DWORD& dwVertexNum ) {}
	virtual void GetVertices( std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV )	{}
	virtual void GetVertices( std::vector<VERTEXNORCOLORTEX5>& vecVertices )	{}

	// Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
	virtual BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, const NSTexture16Bit::DATA* pData16Bit, int nIndexUV )
			{ return FALSE; }

	virtual void ClipMesh( DxDecal* pDecal )	{}

	// DxStaticMaterialMesh ���� mmapMesh �� ������� ���ȴ�. 
	void Create_DxSingleMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice, MMAPSINGLEMATERIALMESH& mmapMesh, float fCellSize ) const;

public:
	int Create( const LPDIRECT3DDEVICEQ pd3dDevice, 
				const TCHAR* szMaterialName, 
				const TSTRING& strTextureName,
				const DWORD dwFaceNUM, 
				BYTE* pVertices, 
				DWORD dwFlags,
				const MaterialValue& sMaterialValue,
				const DWORD dwFVF, 
				const TSTRING& strTextureCombined,
				const TSTRING& strTextureDirect_Shadow_1,
				const TSTRING& strTextureDirect_Shadow_2,
				const TSTRING& strTextureIndirect,
				BOOL bUserLightMapUV,
				const std::vector<WORDWORD>& vecTriangle_wwLightID,
				float fCellSize );

	void CleanUp();

public:
	DxTriangleLinkLight_LightMap();
	virtual ~DxTriangleLinkLight_LightMap();
};