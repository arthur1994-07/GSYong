#pragma once

namespace NSGlow
{
	// 내부 함수
	void SetFX( TSTRING& strFX );

	// 외부에서 셋팅해야 함.
	void SetPath( const TCHAR* szFullPath );
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void OnDestroyDevice();
	void OnRender( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSURFACE9 pBackBuffer );

    void SetActiveGlow();
}