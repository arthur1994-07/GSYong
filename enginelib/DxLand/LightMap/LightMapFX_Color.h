#pragma once

#include "./LightMapFX_Base.h"
#include "./LightMapDef.h"

class DxTangentMesh;

class LightMapFX_Color : public LightMapFX_Base
{
private:
	LPDIRECT3DVERTEXDECLARATION9	m_pDCRT;

public:
	virtual HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT OnLostDevice();
	virtual void SetFX( TSTRING& strFX );

	void Render( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh );

public:
	LightMapFX_Color();
	virtual ~LightMapFX_Color();
};