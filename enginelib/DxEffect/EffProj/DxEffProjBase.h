#pragma once

#include "../../DxTools/DxVertexFVF.h"

class DxEffProjBase
{
private:
	BOOL		m_bEnableON;		// Ȱ��ȭ ON / OFF
	float		m_fCreatingTime;
	float		m_fDestroyingTime;

protected:
	BOOL		m_bPlayON;			// m_bEnableON �� ������ m_bPlayON �� ������, m_bEnableON �� ������ m_bPlayON �� �ð��� �� �ɶ� OFF �� �ȴ�.

	float		m_fAlpha;
	VERTEXRHW	m_sVertex[4];

public:
	virtual BOOL OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void OnLostDevice()									{}
	virtual void OnDestroyDevice()								{}

	virtual BOOL IsTextureShader() = 0;

	// ������ ������ �� ���� �� ȿ���� ȭ�� ���� ȿ���� FX �� �۾��Ͽ� BackBuffer �� ������ �ǰ� �Ѵ�.
	virtual BOOL RenderSourceBackBuffer( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSURFACEQ pSurface, LPDIRECT3DTEXTUREQ pTexture ) { return FALSE; }

	// BackBuffer �� Screen ȿ���� ������ �ϵ��� �Ѵ�.
	virtual void RenderPostEffect( LPDIRECT3DDEVICEQ pd3dDevice ) {}

public:
	void FrameMove( float fElapsedTime );

	// Ȱ�� ��Ȱ��ȭ ��Ų��.
	void EnableEFF( BOOL bUse );

	// ��Ȱ��ȭ �Ŀ� ó���ؾ� �� �͵��� ��� �����ϰ��� TRUE �� �Ѿ�´�.
	BOOL IsPlayOFF() const;

public:
	DxEffProjBase( BOOL bEnableON, float fCreatingTime, float fDestroyingTime );
	virtual ~DxEffProjBase();
};