#pragma once

struct DxAABBNode;

namespace COLLISION
{	
	//주병진 Start
	struct TRIANGLE
	{
		D3DXVECTOR3 vTri1;
		D3DXVECTOR3 vTri2;
		D3DXVECTOR3 vTri3;

		D3DXVECTOR3 vCollision;

		static void Functor_TriangleTransform( TRIANGLE& sSrc, const D3DXMATRIX& matTransform )
		{
			D3DXVec3TransformCoord(&sSrc.vTri1, &sSrc.vTri1, &matTransform);
			D3DXVec3TransformCoord(&sSrc.vTri2, &sSrc.vTri2, &matTransform);
			D3DXVec3TransformCoord(&sSrc.vTri3, &sSrc.vTri3, &matTransform); 

			D3DXVec3TransformCoord(&sSrc.vCollision, &sSrc.vCollision, &matTransform);
		}
	};
}

typedef DxAABBNode* PDXAABBNODE;

enum EMCOLLMODE
{
	EMCC_CULL_CCW			= 0x1,	// 뒷면을 검사안한다. ( 앞면을 검사한다. )
	EMCC_CULL_CW			= 0x2,	// 앞면을 검사안한다. ( 뒷면을 검사한다. )
	EMCC_EXCEPT_ALPHATEX	= 0x4,	// 투명도 있는 텍스쳐를 검사를 제외하겠다.
	EMCC_ONLY_WALL			= 0x8,	// 벽만 검사하도록 한다.
	EMCC_EXCEPT_WATER		= 0x10,	// 물은 제외하도록 한다.

	EMCC_CULL_NONE					= EMCC_CULL_CCW | EMCC_CULL_CW,						// 양면검사를 한다.
	EMCC_CULL_NONE_ONLY_WALL		= EMCC_CULL_NONE | EMCC_ONLY_WALL,					// 양면 검사를 하고 벽만 검사를 하도록 한다.
	EMCC_CULL_CCW_EXCEPT_ALPHATEX	= EMCC_CULL_CCW | EMCC_EXCEPT_ALPHATEX,				// 앞면만 검사에 알파텍스쳐를 검사하지 않는다.
	EMCC_CULL_NONE_EXCEPT_ALPHATEX	= EMCC_CULL_NONE | EMCC_EXCEPT_ALPHATEX,			// 양면 검사를 하고 알파텍스쳐를 검사하지 않는다.
	EMCC_CULL_CCW_EXCEPT_WATER		= EMCC_CULL_CCW | EMCC_EXCEPT_WATER,				// 앞면만 검사에 물은 검사하지 않는다.
	EMCC_CULL_NONE_EXCEPT_WATER		= EMCC_CULL_CCW | EMCC_CULL_CW | EMCC_EXCEPT_WATER,	// 양면검사를 하고, 물은 검사하지 않는다.
};