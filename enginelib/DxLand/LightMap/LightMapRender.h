#pragma once

#include <boost/tr1/memory.hpp>

#include "../../DxTools/DxVertexFVF.h"
#include "../AABBBinaryTree/AABBBinaryTreeBase.h"

class DxTangentMesh;
class DxTexEffBase;


class LightMapRender : public AABBBinaryTreeBase
{
private:
	enum
	{
		VERSION = 0x0103,
	};

public:
    std::tr1::shared_ptr<DxTangentMesh>		m_spTangentMesh;

    // virtual
public:
	virtual BOOL IsLoad() const;
    virtual void DrawEff( const LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex );
	virtual void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, VEC_DWORD& vecOctreeIndex );
    virtual BOOL DynamicLoad( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bThread );
    virtual void Save( sc::SerialFile& SFile, BOOL bPiece );
	virtual void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece );
	virtual void Load_Prev( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece );
	virtual void RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
										DxTexEffBase* pEff, 
										const D3DXMATRIX& matWorld, 
										const D3DXVECTOR2& vLightMapUV_Offset,
										float fAlpha,
										LPDIRECT3DTEXTUREQ pTextureDay, 
										LPDIRECT3DTEXTUREQ pTextureNight );

public:
    void CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice );

public:
    void CleanUp();

public:
	LightMapRender();
	virtual ~LightMapRender();
};

