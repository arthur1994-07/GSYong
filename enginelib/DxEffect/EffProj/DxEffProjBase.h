#pragma once

#include "../../DxTools/DxVertexFVF.h"

class DxEffProjBase
{
private:
	BOOL		m_bEnableON;		// 활성화 ON / OFF
	float		m_fCreatingTime;
	float		m_fDestroyingTime;

protected:
	BOOL		m_bPlayON;			// m_bEnableON 가 켜지면 m_bPlayON 는 켜지고, m_bEnableON 가 꺼지면 m_bPlayON 는 시간이 다 될때 OFF 가 된다.

	float		m_fAlpha;
	VERTEXRHW	m_sVertex[4];

public:
	virtual BOOL OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void OnLostDevice()									{}
	virtual void OnDestroyDevice()								{}

	virtual BOOL IsTextureShader() = 0;

	// 기존에 렌더링 된 것을 물 효과나 화면 깨진 효과를 FX 로 작업하여 BackBuffer 에 렌더링 되게 한다.
	virtual BOOL RenderSourceBackBuffer( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSURFACEQ pSurface, LPDIRECT3DTEXTUREQ pTexture ) { return FALSE; }

	// BackBuffer 에 Screen 효과를 렌더링 하도록 한다.
	virtual void RenderPostEffect( LPDIRECT3DDEVICEQ pd3dDevice ) {}

public:
	void FrameMove( float fElapsedTime );

	// 활성 비활성화 시킨다.
	void EnableEFF( BOOL bUse );

	// 비활성화 후에 처리해야 될 것들을 모두 정리하고나면 TRUE 가 넘어온다.
	BOOL IsPlayOFF() const;

public:
	DxEffProjBase( BOOL bEnableON, float fCreatingTime, float fDestroyingTime );
	virtual ~DxEffProjBase();
};