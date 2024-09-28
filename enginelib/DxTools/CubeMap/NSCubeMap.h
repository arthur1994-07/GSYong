#pragma once

class DxLandMan;

namespace NSCubeMap
{
	void CreateLocalCubeMap( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName, DWORD dwSize, const D3DXVECTOR3& vPos, DxLandMan* pLandMan );

	void CleanUp();
};