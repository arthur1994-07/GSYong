/**
 * \date    2012/09/19
 * \author	shhan
 *
 * NSOcclusionOctree - Visible 되는 Index를 모아놓은 것으로, 실제 렌더링 할 때 있는지 없는지 체크해서 렌더링 한다.
 * NSOcclusionExceptIndex - Query를 Except 해야 하는 Index 들을 모아놓음.
 */
#pragma once

namespace NSOcclusionOctree
{
	void InsertIndex( const VEC_DWORD* pvecOctreeIndex );
	BOOL Visible( int nIndex );
};

namespace NSOcclusionExceptIndex
{
	void Init();

	// vecCharZoneIndex - 크기별로 다 할 필요는 없고, 제일 작은거 하나만 하도록 한다.
	void SetExceptIndex( const VEC_DWORD& vecOctreeIndex, const VEC_DWORD& vecPieceIndex, const VEC_DWORD& vecCharZoneIndex );

	BOOL IsHaveExceptIndexPiece( int nIndex );
	BOOL IsHaveExceptIndexOctree( int nIndex );
	BOOL IsHaveExceptIndexCharZone( int nIndex );
};