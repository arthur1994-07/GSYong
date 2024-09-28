#pragma once

struct AABB_DATA
{
	D3DXVECTOR3 vMin;
	D3DXVECTOR3 vMax;
};

typedef std::vector<AABB_DATA>	VEC_AABB_DATA;
typedef VEC_AABB_DATA::iterator VEC_AABB_DATA_ITER;

struct POS_DIR_DATA
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vDir;
};

typedef std::vector<POS_DIR_DATA>	VEC_POS_DIR_DATA;
typedef VEC_POS_DIR_DATA::iterator	VEC_POS_DIR_DATA_ITER;

struct LockParticle
{
	DWORD		m_dwIndex;		// VertexList 의 위치.
	D3DXVECTOR3	m_vPosition;	// Bone 계산까지 다 된 Position
};