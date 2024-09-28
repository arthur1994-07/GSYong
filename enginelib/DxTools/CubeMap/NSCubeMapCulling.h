/**
 * \date    2013/06/13
 * \author	shhan
 *
 * CubeMapCulling 동작되는 Main
 * 생성, Save, Load 작업을 한다.
 */
#pragma once

#include "../DxVertexFVF.h"

class DxLandMan;
class NavigationMesh;

namespace NSCubeMapCulling
{
	extern int		g_nBakeCountCUR;
	extern DWORD	g_dwBakeCountALL;

	void SimulationPosition( NavigationMesh* pNavigationMesh, int nLength );
	void DrawSimulationPoint( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetOctreeBoxSize( int nLength );
	void DrawOctreeBox( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL Bake( LPDIRECT3DDEVICEQ pd3dDevice, 
				DxLandMan* pDxLandMan, 
				int nSumulationDetailLength, 
				int nOctreeBoxLength,
				DWORD dwCubeMapSize );

	LPDIRECT3DCUBETEXTUREQ GetCubeMapTexture( const D3DXVECTOR3& vPos );

	void Save( const TSTRING& strMakeTimeWLD, const char* szName );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName );
	void IntegrityCheck( const TSTRING& strMakeTimeWLD );

	void CleanUp();
}

namespace NSCubeMapMaker
{
	template<class TREE>
	void MakeTree 
	( 
		TREE** ppTree, 
		const std::vector<D3DXVECTOR3>& vecPoint, 
		const D3DXVECTOR3& vMax, 
		const D3DXVECTOR3& vMin, 
		float fSize 
	);
};