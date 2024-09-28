#pragma once

#include "./LightMapFX_Base.h"
#include "./LightMapDef.h"

class DxTangentMesh;

class LightMapFX_Test : public LightMapFX_Base
{
public:
	virtual HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT OnLostDevice();

	virtual void SetFX( TSTRING& strFX );

	BOOL Render( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bVertexColor, DxTangentMesh* pTangentMesh, const DWORD dwAttrib );

public:
	LightMapFX_Test();
	virtual ~LightMapFX_Test();
};