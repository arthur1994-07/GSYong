#pragma once

#include "./LightMapDef.h"

class DxTangentMesh;

class LightMapFX_Base
{
protected:
	ID3DXEffect*	m_pFX;
	D3DXMATRIX		m_matVP;

public:
    void CleanUp();

	// 순수 가상함수
    virtual HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
    virtual HRESULT OnLostDevice() = 0;
	virtual void SetFX( TSTRING& strFX ) = 0;

	// LoadFX
    HRESULT LoadFX( LPDIRECT3DDEVICEQ pd3dDevice );	

    // FX Param 셋팅.
    void FrameMove( const D3DXMATRIX& matVP );
	void SetBaseTexture( LPDIRECT3DTEXTUREQ pTexture );
    void SetLightMapTexture( LPDIRECT3DTEXTUREQ pTextureDay, LPDIRECT3DTEXTUREQ pTextureNight, const D3DXVECTOR4& vUV_Multiply );
    void SetLightMapUV_Offset( const D3DXVECTOR2& vLightMapUV_Offset );
    virtual void SetWorld( const D3DXMATRIX& matWorld, float fAlpha );	// LightMapFX_General 는 구현이 약간 다르다.

    // Edit
    void ShortBlackFog();

public:
	LightMapFX_Base();
	virtual ~LightMapFX_Base();
};