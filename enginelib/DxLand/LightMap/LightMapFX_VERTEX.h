#pragma once

#include "./LightMapFX_Base.h"
#include "./LightMapDef.h"

class DxTangentMesh;

class LightMapFX_VERTEX : public LightMapFX_Base
{
private:
	LPDIRECT3DVERTEXDECLARATION9	m_pDCRT;

public:
	virtual HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT OnLostDevice();

	virtual void SetFX( TSTRING& strFX );

	virtual void SetWorld( const D3DXMATRIX& matWorld, float fAlpha );

	BOOL Render( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, DWORD dwMaterials );
	BOOL RenderInexact( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, DWORD dwMaterials, UINT nIndex );
	BOOL Render( LPDIRECT3DDEVICEQ pd3dDevice, 
				LPDIRECT3DVERTEXBUFFERQ	pVB,
				LPDIRECT3DINDEXBUFFERQ pIB,
				int nFVFSize,
				DWORD dwFVF,
				DWORD dwVertNum, 
				DWORD dwFaceNum );

public:
	LightMapFX_VERTEX();
	virtual ~LightMapFX_VERTEX();
};