#pragma once

#include <boost/scoped_ptr.hpp>

#include "../../DxTools/DxVertexFVF.h"
#include "../AABBBinaryTree/AABBBinaryTreeBase.h"

class MaterialMesh;
class DxTexEffBase;


class LightMapRenderMat : public AABBBinaryTreeBase
{
private:
	enum
	{
		VERSION = 0x0101,
	};

public:
	boost::scoped_ptr<MaterialMesh>    m_scpMaterialMesh;

    // virtual
public:
	virtual BOOL IsLoad() const;
    virtual void DrawEff( const LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex );
	virtual void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, VEC_DWORD& vecOctreeIndex );
    virtual BOOL DynamicLoad( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bThread );
    virtual void Save( sc::SerialFile& SFile, BOOL bPiece );
	virtual void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece );
	virtual void RenderMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
								const MATERIAL_DATA& sMaterial_Data );
	virtual void RenderPieceMaterial_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, 
												ID3DXEffect* pFX, 
												const MATERIAL_DATA& sMaterial_Data );
	virtual void RenderMaterialWater( LPDIRECT3DDEVICEQ pd3dDevice, 
										const MATERIAL_DATA& sMaterial_Data, 
										float& fHeight, 
										float& fNearLength );
	virtual void MakeMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate );

public:
    void CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice );

public:
    void CleanUp();

public:
	LightMapRenderMat();
	virtual ~LightMapRenderMat();
};

