#pragma once

namespace NSLightShafts
{
    // ���� �Լ�
    void SetFX( TSTRING& strFX );

    // �ܺο��� �����ؾ� ��.
	void SetPath( const TCHAR* szFullPath );
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void OnDestroyDevice();
	void OnRender( LPDIRECT3DDEVICEQ pd3dDevice );

	// [2010.08.24]  �� �κ��� �����ؾ� �ٸ� �۾��鵵 �� �� �� �ִ�.
	BOOL OnRenderStart( LPDIRECT3DDEVICEQ pd3dDevice );

	// [2010.08.24]  OnRenderStart ���� �ؾ� �۵���.
	// Shafts ��ġ ����
	void OnSetting
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		float fAlpha, 
		float fLightShaftsWeight,
		float fLightShaftsDecay, 
		float fLightShaftsLast, 
		const D3DXVECTOR3& vLightShaftsDir 
	);

    // [2010.09.28]  �� ���� LightShafts�� ����.
    void SetLowLevel( float fAlpha );

	// [2010.08.24]  OnRenderStart ���� �ؾ� �۵���.
	void OnRenderEnd( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSURFACEQ	pSrcZBuffer );
}