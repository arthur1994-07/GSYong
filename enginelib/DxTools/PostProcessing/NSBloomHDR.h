#pragma once

namespace NSBloomHDR
{
	// 내부 함수
	void SetFX( TSTRING& strFX );

    // 외부에서 셋팅해야 함.
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

	void OnRender( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL IsEnable();
}