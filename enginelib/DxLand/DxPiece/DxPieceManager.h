#pragma once

#include "../../DxTools/Collision/CollisionDef.h"

class DxPieceManager
{
public:
	virtual BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV ) = 0;

public:
	DxPieceManager();
	virtual ~DxPieceManager();
};