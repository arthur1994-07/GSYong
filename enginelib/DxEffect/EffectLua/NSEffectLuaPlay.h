#pragma once

struct STARGETID;

namespace NSEffectPlay
{
	//////////////////////////////////////////////////////////////////////////
	// RanLogicClient ���� ����ϸ� ��.
	void StartPlay( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const D3DXMATRIX &matStart, const STARGETID &sTargetID );
	void Delete( const char* szName );
	// RanLogicClient ���� ����ϸ� ��.
	//////////////////////////////////////////////////////////////////////////




	//////////////////////////////////////////////////////////////////////////
	// Engine ���� ����ϸ� ��.
	void DeleteALL();

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void OnDestroyDevice();

	BOOL IsTextureShader();

	void FrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime );

	// ������ ������ �� ���� �� ȿ���� ȭ�� ���� ȿ���� FX �� �۾��Ͽ� BackBuffer �� ������ �ǰ� �Ѵ�.
	void RenderSourceBackBuffer( LPDIRECT3DDEVICEQ pd3dDevice, 
									LPDIRECT3DSURFACEQ pSurface1,			
									LPDIRECT3DTEXTUREQ pTexture1, 
									LPDIRECT3DSURFACEQ pSurface2, 
									LPDIRECT3DTEXTUREQ pTexture2, 
									LPDIRECT3DSURFACEQ& pCurrentSurfaceOUT );	// pCurrentSurface �� ���� �� ���� �޾Ƽ� ������ �Ѽս�Ű�� �ʵ��� �Ѵ�.
																				// ó�� �Է¹޴� ���� pSurface1 �� ������ ���� ����ϵ��� �Ѵ�.

	// BackBuffer �� Screen ȿ���� ������ �ϵ��� �Ѵ�.
	void RenderPostEffect( LPDIRECT3DDEVICEQ pd3dDevice );
	// Engine ���� ����ϸ� ��.
	//////////////////////////////////////////////////////////////////////////
};