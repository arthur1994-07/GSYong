#pragma once

namespace NSLightShafts
{
    // 내부 함수
    void SetFX( TSTRING& strFX );

    // 외부에서 셋팅해야 함.
	void SetPath( const TCHAR* szFullPath );
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void OnDestroyDevice();
	void OnRender( LPDIRECT3DDEVICEQ pd3dDevice );

	// [2010.08.24]  이 부분이 성공해야 다른 작업들도 다 할 수 있다.
	BOOL OnRenderStart( LPDIRECT3DDEVICEQ pd3dDevice );

	// [2010.08.24]  OnRenderStart 성공 해야 작동함.
	// Shafts 수치 셋팅
	void OnSetting
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		float fAlpha, 
		float fLightShaftsWeight,
		float fLightShaftsDecay, 
		float fLightShaftsLast, 
		const D3DXVECTOR3& vLightShaftsDir 
	);

    // [2010.09.28]  더 엷게 LightShafts를 만듬.
    void SetLowLevel( float fAlpha );

	// [2010.08.24]  OnRenderStart 성공 해야 작동함.
	void OnRenderEnd( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSURFACEQ	pSrcZBuffer );
}