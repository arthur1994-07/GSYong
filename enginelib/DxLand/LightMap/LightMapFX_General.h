#pragma once

#include "./LightMapFX_Base.h"
#include "./LightMapDef.h"

class DxTangentMesh;

class LightMapFX_General : public LightMapFX_Base
{
private:
	LPDIRECT3DVERTEXDECLARATION9	m_pDCRT;

public:
	virtual HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT OnLostDevice();

	virtual void SetFX( TSTRING& strFX );

	virtual void SetWorld( const D3DXMATRIX& matWorld, float fAlpha );

	void Render( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh );

public:
	LightMapFX_General();
	virtual ~LightMapFX_General();
};