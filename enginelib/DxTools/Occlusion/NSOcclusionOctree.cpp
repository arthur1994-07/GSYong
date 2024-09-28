#include "pch.h"

#include "./NSOcclusionCharZone.h"
#include "./NSOcclusionCulling.h"

#include "NSOcclusionOctree.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace NSOcclusionOctree
{
	const VEC_DWORD* g_pvecRenderIndex(NULL);

	void InsertIndex( const VEC_DWORD* pvecOctreeIndex )
	{
		g_pvecRenderIndex = NULL;
		if ( pvecOctreeIndex )
		{
			g_pvecRenderIndex = pvecOctreeIndex;
		}
	}

	BOOL Visible( int nIndex )
	{
		// 값이 없는 경우는 계산에 들어가지 않도록 한다.
		if ( nIndex == -1 )
			return TRUE;

		// NSOcclusionCulling 이 동작되지 않을 경우는 무조건 렌더링 되도록 한다.
		if ( !NSOcclusionCulling::IsActiveField() )
			return TRUE;

		if ( !g_pvecRenderIndex )
			return FALSE;

		if ( std::binary_search( g_pvecRenderIndex->begin(), g_pvecRenderIndex->end(), static_cast<DWORD>(nIndex) ) )
			return TRUE;

		return FALSE;
	}
};


namespace NSOcclusionExceptIndex
{
	SET_DWORD g_setPieceIndex;
	SET_DWORD g_setOctreeIndex;
	SET_DWORD g_setCharZoneIndex;

	void Init()
	{
		g_setPieceIndex.clear();
		g_setOctreeIndex.clear();
		g_setCharZoneIndex.clear();
	}

	void SetExceptIndex( const VEC_DWORD& vecOctreeIndex, const VEC_DWORD& vecPieceIndex, const VEC_DWORD& vecCharZoneIndex )
	{
		Init();

		for ( DWORD i=0; i<vecPieceIndex.size(); ++i )
		{
			g_setPieceIndex.insert( vecPieceIndex[i] );
		}
		for ( DWORD i=0; i<vecOctreeIndex.size(); ++i )
		{
			g_setOctreeIndex.insert( vecOctreeIndex[i] );
		}
		for ( DWORD i=0; i<vecCharZoneIndex.size(); ++i )
		{
			g_setCharZoneIndex.insert( vecCharZoneIndex[i] );
		}
	}

	BOOL IsHaveExceptIndexPiece( int nIndex )
	{
		// 값이 없는 경우는 가지고 있지 않다고 셋팅한다.
		if ( nIndex == -1 )
			return FALSE;

		SET_DWORD_CITER iter = g_setPieceIndex.find( nIndex );
		if ( iter != g_setPieceIndex.end() )
			return TRUE;

		return FALSE;
	}

	BOOL IsHaveExceptIndexOctree( int nIndex )
	{
		// 값이 없는 경우는 가지고 있지 않다고 셋팅한다.
		if ( nIndex == -1 )
			return FALSE;

		SET_DWORD_CITER iter = g_setOctreeIndex.find( nIndex );
		if ( iter != g_setOctreeIndex.end() )
			return TRUE;

		return FALSE;
	}

	BOOL IsHaveExceptIndexCharZone( int nIndex )
	{
		// 값이 없는 경우는 가지고 있지 않다고 셋팅한다.
		if ( nIndex == -1 )
			return FALSE;

		SET_DWORD_CITER iter = g_setCharZoneIndex.find( nIndex );
		if ( iter != g_setCharZoneIndex.end() )
			return TRUE;

		return FALSE;
	}
};