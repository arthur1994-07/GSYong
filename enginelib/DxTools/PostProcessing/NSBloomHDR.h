#pragma once

namespace NSBloomHDR
{
	// ���� �Լ�
	void SetFX( TSTRING& strFX );

    // �ܺο��� �����ؾ� ��.
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

	void OnRender( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL IsEnable();
}