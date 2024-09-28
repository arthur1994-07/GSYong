/**
 * \date    2012/09/03
 * \author	shhan
 *
 * OcclusionCulling 동작되는 Main
 * 생성, Save, Load, Index 보내주는 작업을 한다.
 */
#pragma once

#include "../DxVertexFVF.h"

class DxLandMan;
class NavigationMesh;

namespace NSOcclusionCulling
{
	extern BOOL		g_bRendering;
	extern BOOL		g_bBaking;			// Baking 중인가?
	extern int		g_nBakeCountCUR;
	extern DWORD	g_dwBakeCountALL;

	BOOL IsActive();
	BOOL IsActiveField();
	BOOL IsActivePiece();

	void SimulationPosition( NavigationMesh* pNavigationMesh, int nLength );
	void DrawSimulationPoint( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetOctreeBoxSize( int nLength );
	void DrawOctreeBox( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL Bake( LPDIRECT3DDEVICEQ pd3dDevice, 
				DxLandMan* pDxLandMan, 
				int nSumulationDetailLength, 
				int nOctreeBoxLength,
				BOOL bFieldOcclusion,
				BOOL bPieceOcclusion,
				BOOL bCharOcclusion );

	BOOL BakeOneCell( LPDIRECT3DDEVICEQ pd3dDevice, 
						DxLandMan* pDxLandMan, 
						const D3DXVECTOR3& vPos,
						int nSumulationDetailLength );

	BOOL GetRenderIndex( const D3DXVECTOR3& vPos, const VEC_DWORD** ppvecPieceIndex, const VEC_DWORD** ppvecOctreeIndex );

	BOOL VisibleOcclusionCharZone( const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin );

	BOOL FindChar( BOOL i, DWORD dwID );

	void Save( const TSTRING& strMakeTimeWLD, const char* szName );
	void Load( const char* szName );
	void IntegrityCheck( const TSTRING& strMakeTimeWLD );

	void CleanUp();
}