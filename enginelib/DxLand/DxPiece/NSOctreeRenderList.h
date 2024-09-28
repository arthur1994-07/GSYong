/**
 * \date    2012/09/18
 * \author	shhan
 */
#pragma once

class OBJOCTree;
class LightMapRender;
class LightMapRenderMat;

namespace NSOctreeRenderList
{
	extern int g_nRenderIndex;

	void ClearRenderIndex();
	void Insert( int nID );
}