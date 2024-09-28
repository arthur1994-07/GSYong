/**
 * \date    2013/06/13
 * \author	shhan
 *
 * DxCubeMapTree - 
 * 
 */
#pragma once

#include <vector>

class DxLandMan;

struct DxCubeMapTree
{
private:
	enum
	{
		VERSION = 0x100,
	};

public:
	std::vector<D3DXVECTOR3>	m_vecPoint;			// 기준 위치.
	
public:
	LPDIRECT3DCUBETEXTUREQ m_pCubeTex;		// Save, Load	
	D3DXVECTOR3 m_vMax;						// Save, Load	
	D3DXVECTOR3 m_vMin;						// Save, Load	

public:
	DxCubeMapTree* m_pLeftChild;
	DxCubeMapTree* m_pRightChild;

public:
	void LastBoxNum( DWORD& dwBoxNum );

	void RenderBox( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos );

	void Bake( LPDIRECT3DDEVICEQ pd3dDevice, 
				DxLandMan* pDxLandMan,
				DWORD dwCubeMapSize,
				int& nCountCUR );

	void BakeOneCell( LPDIRECT3DDEVICEQ pd3dDevice, 
						DxLandMan* pDxLandMan, 
						DWORD dwCubeMapSize );

	void GetCubeMapTexture( LPDIRECT3DCUBETEXTUREQ& pCubeTexOUT, const D3DXVECTOR3& vPos );

public:
	void SaveMain( sc::SerialFile& SFile, const TSTRING& strMakeTimeWLD );
	void LoadMain( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, TSTRING& strMakeTimeWLD, DWORD dwCubeMapSize );

private:
	void Save( sc::SerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DWORD dwCubeMapSize );

public:
	DxCubeMapTree();
	~DxCubeMapTree();
};