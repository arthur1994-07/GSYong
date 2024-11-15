#ifndef _DX_CUBE_MAP_H_
#define _DX_CUBE_MAP_H_

#pragma once

#include "DxViewPort.h"
#include "./TextureManager.h"

class DxCubeMap
{
protected:
	struct VERTEX
	{
		D3DXVECTOR3	vPos;
		D3DCOLOR	cDiff;
		D3DXVECTOR2	vTex;
		static const	DWORD	FVF;

		VERTEX () :
			vPos(0.f,0.f,0.f),
			cDiff(0xffffffff),
			vTex(0.f,0.f)
		{
		}
	};

	struct SKYBOXVERTEX
	{
		D3DXVECTOR4	vPos;
		D3DXVECTOR2	vTex;
		static const	DWORD	FVF;

		SKYBOXVERTEX () :
			vPos(0.f,0.f,0.f,1.f),
			vTex(0.f,0.f)
		{
		}
	};

	static LPDIRECT3DSTATEBLOCK9		m_pSavedSkyBoxSB;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawSkyBoxSB;

	static LPDIRECT3DSTATEBLOCK9		m_pSavedLightSB;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawLightSB;

protected:
	BOOL	m_bGraphicCardEnable;	// 그래픽 카드에서 가능/불가능
	BOOL	m_bOptionEnable;		// 옵션에서의 활성화
	BOOL	m_bActive;				// 동작/미동작

protected:
	LPDIRECT3DVERTEXBUFFERQ	m_pVB;
	LPDIRECT3DINDEXBUFFERQ	m_pIB;

	char					m_szSkyBoxTex[MAX_PATH];
	TextureResource m_textureResSkyBox;
	char					m_szLightTex[MAX_PATH];
	TextureResource m_textureResLight;

	//LPDIRECT3DCUBETEXTUREQ	m_pCubeTex;
    TextureResource m_textureResTest;

public:
	BOOL	IsGraphicCardEnable	()				{ return m_bGraphicCardEnable; }
	BOOL	IsOptionEnable ()					{ return m_bOptionEnable; }	
	BOOL	Is_GraphicCard_Option_Enable	()	{ return (m_bOptionEnable&&m_bGraphicCardEnable); }	// 그래픽카드 사용가능, 옵션 활성화

	void	SetOptionEnable ( BOOL bUse )		{ m_bOptionEnable = bUse; }
	void	SetActiveON ()						{ m_bActive = TRUE; }

public:
	//LPDIRECT3DCUBETEXTUREQ	GetCubeTexture()	{ return m_pCubeTex; }
	LPDIRECT3DCUBETEXTUREQ	GetCubeTexTEST()	{ return m_textureResTest.GetCubeTexture(); }

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice, D3DSURFACE_DESC &d3dsdBackBuffer );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects();

	//HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	void	ComputeVB ();
	void	RenderLight ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX& matView );
	void	RenderSkybox ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	DxCubeMap();

public:
	virtual ~DxCubeMap();

public:
	static DxCubeMap& GetInstance();
};

#endif // _DX_CUBE_MAP_H_