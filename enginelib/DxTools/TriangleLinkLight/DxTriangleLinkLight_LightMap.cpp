#include "pch.h"

#include "../../DxLand/LightMap/LightMapRenderMan.h"
#include "../../DxLand/DxPiece/DxSingleTexMesh.h"
#include "../Material/DxSingleMaterialMesh.h"

#include "DxTriangleLinkLight_LightMap.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


DxTriangleLinkLight_LightMap::DxTriangleLinkLight_LightMap()
{

}

DxTriangleLinkLight_LightMap::~DxTriangleLinkLight_LightMap()
{
	CleanUp();
}

void DxTriangleLinkLight_LightMap::CleanUp()
{
	m_vecSingleTexMesh.clear();
}

int DxTriangleLinkLight_LightMap::Create( const LPDIRECT3DDEVICEQ pd3dDevice, 
										   const TCHAR* szMaterialName, 
										   const TSTRING& strTextureName,
										   const DWORD dwFaceNUM, 
										   BYTE* pVertices, 
										   DWORD dwFlags,
										   const MaterialValue& sMaterialValue,
										   const DWORD dwFVF, 
										   const TSTRING& strTextureCombined,
										   const TSTRING& strTextureDirect_Shadow_1,
										   const TSTRING& strTextureDirect_Shadow_2,
										   const TSTRING& strTextureIndirect,
										   BOOL bUserLightMapUV,
										   const std::vector<WORDWORD>& vecTriangle_wwLightID,
										   float fCellSize )
{
	// Note : 처음 다 삭제
	CleanUp();

	// Note : 정점 데이터 얻기
	if ( !pVertices )	
		return 0;

	if ( vecTriangle_wwLightID.empty() )
		return 0;

	UINT nVERSIZE = D3DXGetFVFVertexSize ( dwFVF ) * 3;

	// 정점이 필요해.
	struct VERTEX_DATA
	{
		BYTE* m_pVertices;
		DWORD m_dwFaceNUM;
	};

	// 사용되는 Light를 기준으로 정점을 나누도록 한다.
	std::map<DWORD,VERTEX_DATA> mapVertexDataForLight;
	std::map<DWORD,VERTEX_DATA>::iterator iter;

	for ( DWORD i=0; i<dwFaceNUM; ++i )
	{
		iter = mapVertexDataForLight.find( vecTriangle_wwLightID[i].dwData );
		if ( iter == mapVertexDataForLight.end() )
		{
			// 새로운것을 만들도록 한다.
			VERTEX_DATA sVertexData;
			sVertexData.m_pVertices = new BYTE[nVERSIZE*dwFaceNUM];
			memcpy ( sVertexData.m_pVertices, pVertices+(i*nVERSIZE), nVERSIZE );
			sVertexData.m_dwFaceNUM = 1;
			mapVertexDataForLight.insert( std::make_pair( vecTriangle_wwLightID[i].dwData, sVertexData ) );
		}
		else
		{
			// 메모리 단편화 문제가 생겨서 처음 만들 때만 크게 만들고 지속적으로 new 를 하지 않도록 한다.
			// 기존것에 추가시켜준다.
			//BYTE* pVerticesBackUp = (*iter).second.m_pVertices;																	// 백업
			++((*iter).second.m_dwFaceNUM);																						// 삼각형 갯수를 늘린다.
			//(*iter).second.m_pVertices = new BYTE[(*iter).second.m_dwFaceNUM*nVERSIZE];											// 새로운 힙메모리 생성
			//memcpy ( (*iter).second.m_pVertices, pVerticesBackUp, ((*iter).second.m_dwFaceNUM-1)*nVERSIZE );					// 기존것 복제
			memcpy ( (*iter).second.m_pVertices+(((*iter).second.m_dwFaceNUM-1)*nVERSIZE), pVertices+(i*nVERSIZE), nVERSIZE );	// 새로운것 복제
			//SAFE_DELETE_ARRAY ( pVerticesBackUp );																				// 백업한것 삭제함.
		}
	}

	iter = mapVertexDataForLight.begin();
	for ( ; iter!=mapVertexDataForLight.end(); ++iter )
	{
		std::tr1::shared_ptr<DxSingleTexMesh> spSingleTexMesh = std::tr1::shared_ptr<DxSingleTexMesh>(new DxSingleTexMesh);
		spSingleTexMesh->CreateLightMap
		(
			pd3dDevice,
			szMaterialName,
			strTextureName,
			(*iter).second.m_dwFaceNUM,
			(*iter).second.m_pVertices,
			dwFlags,
			sMaterialValue,
			dwFVF,
			strTextureCombined,
			strTextureDirect_Shadow_1,
			strTextureDirect_Shadow_2,
			strTextureIndirect,
			bUserLightMapUV,
			FALSE,
			fCellSize
		);

		SAFE_DELETE_ARRAY ( (*iter).second.m_pVertices );

		spSingleTexMesh->SetLightID( WORDWORD((*iter).first) );

		m_vecSingleTexMesh.push_back( spSingleTexMesh );
	}

	return 1;
}

// DxStaticMaterialMesh 에서 mmapMesh 을 얻기위해 사용된다. 
void DxTriangleLinkLight_LightMap::Create_DxSingleMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice, MMAPSINGLEMATERIALMESH& mmapMesh, float fCellSize ) const
{
	for ( DWORD i=0; i<m_vecSingleTexMesh.size(); ++i )
	{
		DxSingleMaterialMesh* pNew = new DxSingleMaterialMesh;
		pNew->Import( pd3dDevice, m_vecSingleTexMesh[i].get(), fCellSize );
		pNew->SetLightID( m_vecSingleTexMesh[i]->GetLightID() );

		mmapMesh.insert( std::make_pair ( m_vecSingleTexMesh[i]->GetMaterialName(), pNew ) );		
	}
}