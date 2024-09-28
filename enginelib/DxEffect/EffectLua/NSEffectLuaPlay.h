#pragma once

struct STARGETID;

namespace NSEffectPlay
{
	//////////////////////////////////////////////////////////////////////////
	// RanLogicClient 에서 사용하면 됨.
	void StartPlay( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const D3DXMATRIX &matStart, const STARGETID &sTargetID );
	void Delete( const char* szName );
	// RanLogicClient 에서 사용하면 됨.
	//////////////////////////////////////////////////////////////////////////




	//////////////////////////////////////////////////////////////////////////
	// Engine 에서 사용하면 됨.
	void DeleteALL();

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void OnDestroyDevice();

	BOOL IsTextureShader();

	void FrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime );

	// 기존에 렌더링 된 것을 물 효과나 화면 깨진 효과를 FX 로 작업하여 BackBuffer 에 렌더링 되게 한다.
	void RenderSourceBackBuffer( LPDIRECT3DDEVICEQ pd3dDevice, 
									LPDIRECT3DSURFACEQ pSurface1,			
									LPDIRECT3DTEXTUREQ pTexture1, 
									LPDIRECT3DSURFACEQ pSurface2, 
									LPDIRECT3DTEXTUREQ pTexture2, 
									LPDIRECT3DSURFACEQ& pCurrentSurfaceOUT );	// pCurrentSurface 는 복사 된 값을 받아서 원본을 훼손시키지 않도록 한다.
																				// 처음 입력받는 값은 pSurface1 을 복사한 값을 사용하도록 한다.

	// BackBuffer 에 Screen 효과를 렌더링 하도록 한다.
	void RenderPostEffect( LPDIRECT3DDEVICEQ pd3dDevice );
	// Engine 에서 사용하면 됨.
	//////////////////////////////////////////////////////////////////////////
};