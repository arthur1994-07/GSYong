#include "pch.h"

#include "../../DxTools/TriangleLinkLight/DxTriangleLinkLight_LightMap.h"
#include "../LightMap/LightMapRenderMan.h"
#include "../DxPiece/DxAABBOctree.h"
#include "../DxPiece/NsOCTree.h"

#include "AABBRenderStrategy.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


AABBRenderStrategy::AABBRenderStrategy()
    : m_emTypeRender( TYPE_OCJOCTREE )
{
}

AABBRenderStrategy::~AABBRenderStrategy()
{
}

AABBRenderStrategy::TYPE_RENDER AABBRenderStrategy::GetTypeRender()
{
    return m_emTypeRender;
}

AABBRenderStrategy* AABBRenderStrategy::CreateFactory( TYPE_RENDER emType )
{
    switch( emType )
    {
    case TYPE_OCJOCTREE:
        return new DxAABBOctree;

    case TYPE_LIGHTMAP:
        return new LightMapRenderMan;

	case TYPE_TRIANGLE_LINK_LIGHT_LIGHTMAP:
		return new DxTriangleLinkLight_LightMap;
    };

    return NULL;
}

BOOL AABBRenderStrategy::GetLightMapColor( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR2& vUV )
{
	return NSOCTREE::GetLightMapColor( GetAABBBinaryBase(), vStart, vEnd, vUV );
}