#pragma once

#include <string>

#include "../../DxTools/TextureManager.h"

#include "./DxEffProjBase.h"


class DxEffProjTexture : public DxEffProjBase
{
	//////////////////////////////////////////////////////////////////////////
	//								STATIC
private:
	static LPDIRECT3DSTATEBLOCK9 m_pSavedRS_STATIC;
	static LPDIRECT3DSTATEBLOCK9 m_pDrawRS_STATIC;

public:
	static void OnResetDevice_STATIC( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnLostDevice_STATIC();


	//////////////////////////////////////////////////////////////////////////
	//
private:
	TSTRING				m_strTexName;
	TextureResource		m_textureRes;


	//////////////////////////////////////////////////////////////////////////
	//							override
public:
	BOOL OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice ) override;
	void OnDestroyDevice() override;

	BOOL IsTextureShader() override		{ return FALSE; }

public:
	// BackBuffer 에 Screen 효과를 렌더링 하도록 한다.
	void RenderPostEffect( LPDIRECT3DDEVICEQ pd3dDevice ) override;


	//////////////////////////////////////////////////////////////////////////
	//
private:
	void RenderEX( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffProjTexture( const TCHAR* szName, float fCreatingTime, float fDestroyingTime, BOOL bEnableON );
	~DxEffProjTexture();
};