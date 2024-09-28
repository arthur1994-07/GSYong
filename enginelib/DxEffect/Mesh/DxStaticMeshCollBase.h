#pragma once

#include "../../DxTools/Collision/CollisionDef.h"

class DxStaticMeshCollBase
{
public:
	// Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
	virtual BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV ) = 0;

public:
	DxStaticMeshCollBase() {}
	virtual ~DxStaticMeshCollBase() {}
};