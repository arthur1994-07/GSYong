#pragma once

#include <string>

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxVertexFVF.h"

#include "./DxEffProjBase.h"


class DxEffProjTextureShader : public DxEffProjBase
{
	//////////////////////////////////////////////////////////////////////////
	//								STATIC
private:
	static LPDIRECT3DSTATEBLOCK9			g_pSavedRS_STATIC;
	static LPDIRECT3DSTATEBLOCK9			g_pDrawRS_STATIC;
	static std::map<TSTRING,ID3DXEffect*>	g_mapCustomFX;

public:
	static void OnResetDevice_STATIC( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnLostDevice_STATIC();
	static void DeleteCustomFX_STATIC();


	//////////////////////////////////////////////////////////////////////////
	//
private:
	TSTRING				m_strTexName;
	TextureResource		m_textureRes;
	TSTRING				m_strShader;
	ID3DXEffect*		m_rFX;


	//////////////////////////////////////////////////////////////////////////
	//							override
public:
	BOOL OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice ) override;
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice ) override;
	void OnLostDevice() override;
	void OnDestroyDevice() override;

	BOOL IsTextureShader() override		{ return TRUE; }

public:
	// ������ ������ �� ���� �� ȿ���� ȭ�� ���� ȿ���� FX �� �۾��Ͽ� BackBuffer �� ������ �ǰ� �Ѵ�.
	BOOL RenderSourceBackBuffer( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSURFACEQ pSurface, LPDIRECT3DTEXTUREQ pTexture ) override;


	//////////////////////////////////////////////////////////////////////////
	//
private:
	void RenderEX( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffProjTextureShader( const TCHAR* szName, const TCHAR* szShader, float fCreatingTime, float fDestroyingTime, BOOL bEnableON );
	~DxEffProjTextureShader();
};