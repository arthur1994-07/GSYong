#pragma once

struct DxAABBNode;

namespace COLLISION
{	
	//�ֺ��� Start
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
	EMCC_CULL_CCW			= 0x1,	// �޸��� �˻���Ѵ�. ( �ո��� �˻��Ѵ�. )
	EMCC_CULL_CW			= 0x2,	// �ո��� �˻���Ѵ�. ( �޸��� �˻��Ѵ�. )
	EMCC_EXCEPT_ALPHATEX	= 0x4,	// ���� �ִ� �ؽ��ĸ� �˻縦 �����ϰڴ�.
	EMCC_ONLY_WALL			= 0x8,	// ���� �˻��ϵ��� �Ѵ�.
	EMCC_EXCEPT_WATER		= 0x10,	// ���� �����ϵ��� �Ѵ�.

	EMCC_CULL_NONE					= EMCC_CULL_CCW | EMCC_CULL_CW,						// ���˻縦 �Ѵ�.
	EMCC_CULL_NONE_ONLY_WALL		= EMCC_CULL_NONE | EMCC_ONLY_WALL,					// ��� �˻縦 �ϰ� ���� �˻縦 �ϵ��� �Ѵ�.
	EMCC_CULL_CCW_EXCEPT_ALPHATEX	= EMCC_CULL_CCW | EMCC_EXCEPT_ALPHATEX,				// �ո鸸 �˻翡 �����ؽ��ĸ� �˻����� �ʴ´�.
	EMCC_CULL_NONE_EXCEPT_ALPHATEX	= EMCC_CULL_NONE | EMCC_EXCEPT_ALPHATEX,			// ��� �˻縦 �ϰ� �����ؽ��ĸ� �˻����� �ʴ´�.
	EMCC_CULL_CCW_EXCEPT_WATER		= EMCC_CULL_CCW | EMCC_EXCEPT_WATER,				// �ո鸸 �˻翡 ���� �˻����� �ʴ´�.
	EMCC_CULL_NONE_EXCEPT_WATER		= EMCC_CULL_CCW | EMCC_CULL_CW | EMCC_EXCEPT_WATER,	// ���˻縦 �ϰ�, ���� �˻����� �ʴ´�.
};