/**
 * \date    2012/09/19
 * \author	shhan
 *
 * NSOcclusionOctree - Visible �Ǵ� Index�� ��Ƴ��� ������, ���� ������ �� �� �ִ��� ������ üũ�ؼ� ������ �Ѵ�.
 * NSOcclusionExceptIndex - Query�� Except �ؾ� �ϴ� Index ���� ��Ƴ���.
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

	// vecCharZoneIndex - ũ�⺰�� �� �� �ʿ�� ����, ���� ������ �ϳ��� �ϵ��� �Ѵ�.
	void SetExceptIndex( const VEC_DWORD& vecOctreeIndex, const VEC_DWORD& vecPieceIndex, const VEC_DWORD& vecCharZoneIndex );

	BOOL IsHaveExceptIndexPiece( int nIndex );
	BOOL IsHaveExceptIndexOctree( int nIndex );
	BOOL IsHaveExceptIndexCharZone( int nIndex );
};