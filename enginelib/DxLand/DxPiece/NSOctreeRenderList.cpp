#include "pch.h"

#include "../../DxTools/DxVertexFVF.h"

#include "NSOctreeRenderList.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSOctreeRenderList
{
	int g_nRenderIndex(0);

	VEC_DWORD g_vecRenderIndex;

	void ClearRenderIndex()
	{
		g_vecRenderIndex.clear();
	}

	void Insert( int nID )
	{
		g_vecRenderIndex.push_back( nID );
	}
}