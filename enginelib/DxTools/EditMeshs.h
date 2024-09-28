#ifndef EDITMESHS_H_
#define EDITMESHS_H_

#pragma once

#include "../../InternalCommonLib/dxstdafx.h"

namespace EDITMESHS
{
	extern const DWORD dwNumSphereVertex;
	extern D3DXVECTOR3 SphereVertex[114];
	extern const DWORD dwNumSphereFaces;
	extern WORD SphereIndex[224*3];

	VOID InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID DeleteDeviceObjects();

	VOID DrawMeshSphere();
	VOID DrawMeshCylinder();

	D3DXVECTOR3 GETAABB_POINT ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, int nindex );

	void RENDERAABB ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin, DWORD dwColor=0xffffffff );
	void RENDERLINE ( D3DXVECTOR3 &vPoint1, D3DXVECTOR3 &vPoint2, DWORD dwColor=0xffffffff );
	void RENDERLINE ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vPoint1, D3DXVECTOR3 &vPoint2, DWORD dwColor=0xffffffff );
	void RENDERSPHERE ( const D3DXVECTOR3& vPos, const D3DXVECTOR3& vScale, BOOL bFill = FALSE, D3DXMATRIX* pRotateMAT=NULL, DWORD dwColor=0xffffffff );
	void RENDERSPHERE ( LPDIRECT3DDEVICEQ pd3dDevice, D3DVECTOR vPos, float fScale, D3DXMATRIX* pRotateMAT=NULL, DWORD dwColor=0xffffffff );
	void RENDERSPHERE ( LPDIRECT3DDEVICEQ pd3dDevice, D3DVECTOR vPos, D3DXVECTOR3 vScale, BOOL bFill = FALSE, D3DXMATRIX* pRotateMAT=NULL, DWORD dwColor=0xffffffff );
	void RENDERPLANE ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vCenter, D3DXVECTOR3 &vSize, D3DXVECTOR3 &vSnap );
	void RENDERPLANE( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vVertex[], DWORD dwColor, BOOL bWireFrame=FALSE );
	void RENDERARROW ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vCenter, D3DXVECTOR3 &vSize, D3DXVECTOR3 &vSnap );

	VOID RENDERBOX( D3DXVECTOR3 vPos, FLOAT fScale, DWORD dwColor = 0xffffffff );
	VOID RENDERBOX( D3DXVECTOR3 vPos, FLOAT fRadius, float fHeight, DWORD dwColor, const D3DXMATRIX* pmatRotate );
	VOID RENDERBOX( D3DXVECTOR3 vPos, float fPosX, float fHeight, float fPosZ, DWORD dwColor, const D3DXMATRIX* pmatRotate );
	VOID RENDERBOX_LIST( D3DXVECTOR3* pPosList, DWORD dwListCnt, FLOAT fScale, DWORD dwColor = 0xffffffff );

	VOID RENDERCYLINDER( D3DXVECTOR3 vPos, float fRadius, float fHeight, DWORD dwColor, const D3DXMATRIX* pmatRotate, BOOL bWireFrame=TRUE );

	VOID RENDERCONE( D3DXVECTOR3 vCenter, D3DXVECTOR3 vDir, float fLength, float fConeAngleHalf, DWORD dwColor );
	VOID RENDERCONE( D3DXVECTOR3 vCenter, const D3DXVECTOR3 &vDirX, const D3DXVECTOR3 &vDirY, const D3DXVECTOR3 &vDirZ, float fLength, float fConeAngleHalfY, float fConeAngleHalfZ, DWORD dwColor );
	VOID RENDERCIRCLE( D3DXVECTOR3 &vCenter, D3DXVECTOR3 &vDir, float fLength, DWORD dwColor );
}

#endif // EDITMESHS_H_