#pragma once

namespace NSGlow
{
	// ���� �Լ�
	void SetFX( TSTRING& strFX );

	// �ܺο��� �����ؾ� ��.
	void SetPath( const TCHAR* szFullPath );
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void OnDestroyDevice();
	void OnRender( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSURFACE9 pBackBuffer );

    void SetActiveGlow();
}