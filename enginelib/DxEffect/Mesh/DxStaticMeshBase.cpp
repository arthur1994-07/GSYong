#include "pch.h"

#include "../../DxLand/LightMap/NSLightMapTools.h"

#include "DxStaticMeshBase.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxStaticMeshBase::DxStaticMeshBase()
{
}

DxStaticMeshBase::~DxStaticMeshBase()
{
}

// Piece의 TriangleLinkLight 를 생성하도록 한다.
BOOL DxStaticMeshBase::CreateTriangleLinkLightForPieceDetail( LPDIRECT3DDEVICEQ pd3dDevice, 
															   DxStaticMeshCollBase* pStaticMesh, 
															   DxPieceManager* pPieceManager, 
															   std::vector<DWORD>& vecLightID, 
															   const D3DXMATRIX& matWorld,
															   const std::vector<VERTEXNORCOLORTEX4>& vecVerticesS,
															   const D3DLIGHTQ& sLight,
															   float fAttEndLineValue ) const
{
	std::vector<VERTEXNOR> vecVerticesD;
	vecVerticesD.reserve( 3 );

	DWORD dwFace = vecVerticesS.size()/3;
	for ( DWORD i=0; i<dwFace; ++i )
	{
		VERTEXNOR sData;
		vecVerticesD.clear();

		for ( DWORD z=0; z<3; ++z )
		{
			sData.vPos = vecVerticesS[i*3+z].vPos;
			sData.vNor = vecVerticesS[i*3+z].vNor;
			D3DXVec3TransformCoord( &sData.vPos, &sData.vPos, &matWorld );
			D3DXVec3TransformNormal( &sData.vNor, &sData.vNor, &matWorld );
			vecVerticesD.push_back( sData );
		}		

		BOOL bOK = NSLightMapTools::OneTriangleIncludeLightPiece
		( 
			vecLightID,
			pd3dDevice, 
			pStaticMesh, 
			pPieceManager, 
			vecVerticesD,
			sLight,
			fAttEndLineValue
		);

		if ( bOK )
			return TRUE;
	}

	return FALSE;
}