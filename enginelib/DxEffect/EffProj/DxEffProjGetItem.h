#pragma once

#include <string>

#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/TextureManager.h"

class DxEffProjGetItem
{
protected:
	LPDIRECT3DSTATEBLOCK9 m_pSavedRS;
	LPDIRECT3DSTATEBLOCK9 m_pDrawRS;
	float m_fElapsedTime;

protected:
	D3DMATERIALQ		m_sMaterial;
	std::string			m_strTexName;
	TextureResource m_textureRes;

protected:
	BOOL			m_bEnableON;
	BOOL			m_bPlayON;
	float			m_fAlpha;
	VERTEXRHW		m_sVertex[4];

protected:
	int		m_nCount00;	// ������ ��ȣ
	int		m_nCount01;	// ������ ��ȣ

public:
	void EnableEFF( BOOL bUse );

public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnDestroyDevice();

public:
	void FrameMove( float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	void SequnceMake( int nCount );
	void RenderEX( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffProjGetItem();
	~DxEffProjGetItem();
};