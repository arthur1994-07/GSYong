#include "pch.h"

#include "../../DxLand/DxPiece/NsOCTree.h"

#include "../DxRenderStates.h"
#include "../Collision.h"
#include "./NSMaterialManager.h"
#include "./MaterialFieldDef.h"

#include "MaterialMesh.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//------------------------------------------------------------------------------
//							MaterialMesh
MaterialMesh::MaterialMesh()
	: m_dwVer(-1)
	, m_pMesh(NULL)
	, m_dwVertices(0L)
	, m_dwFaces(0L)
	, m_dwVertSize(0L)
	, m_rVB(NULL)
	, m_rIB(NULL)
{
}

MaterialMesh::~MaterialMesh()
{
	m_vecLightMapUV.clear();

	SAFE_RELEASE( m_rVB );
	SAFE_RELEASE( m_rIB );
	SAFE_RELEASE ( m_pMesh );

	m_dwVertices = 0L;
	m_dwFaces = 0L;
	m_dwVertSize = 0L;
}

void MaterialMesh::Insert( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMeshSRC, LPD3DXATTRIBUTERANGE pAttribTable, DWORD dwAttIndex )
{
	DWORD dwFaces	 = pMeshSRC->GetNumFaces();

	DWORD*	pIndices32 = new DWORD[dwFaces*3];
	if ( pMeshSRC->GetOptions()&D3DXMESH_32BIT )
	{
		DWORD*	pIndex32;
		pMeshSRC->LockIndexBuffer ( 0L, (VOID**)&pIndex32 );
		memcpy ( pIndices32, pIndex32, sizeof(DWORD)*dwFaces*3 );
		pMeshSRC->UnlockIndexBuffer ();
	}
	else
	{
		WORD*	pIndex16;
		pMeshSRC->LockIndexBuffer ( 0L, (VOID**)&pIndex16 );
		for ( DWORD i=0; i<dwFaces*3; ++i )
		{
			pIndices32[i] = static_cast<DWORD>(pIndex16[i]);
		}
		pMeshSRC->UnlockIndexBuffer ();
	}

	DWORD dwFaceStart = pAttribTable[ dwAttIndex ].FaceStart;
	DWORD dwFaceCount = pAttribTable[ dwAttIndex ].FaceCount;

	SAFE_RELEASE( m_rVB );
	SAFE_RELEASE( m_rIB );
	SAFE_RELEASE ( m_pMesh );
	if ( (dwFaceCount*3) > 65000 )
	{
		D3DXCreateMeshFVF ( dwFaceCount, dwFaceCount*3, D3DXMESH_32BIT|D3DXMESH_MANAGED, pMeshSRC->GetFVF(), pd3dDevice, &m_pMesh );

		// Index
		DWORD* pIndices;
		m_pMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );
		for ( DWORD i=0; i<dwFaceCount*3; ++i )
		{
			pIndices[i] = i;
		}
		m_pMesh->UnlockIndexBuffer();
	}
	else
	{
		D3DXCreateMeshFVF ( dwFaceCount, dwFaceCount*3, D3DXMESH_MANAGED, pMeshSRC->GetFVF(), pd3dDevice, &m_pMesh );

		// Index
		WORD* pIndices;
		m_pMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );
		for ( DWORD i=0; i<dwFaceCount*3; ++i )
		{
			pIndices[i] = static_cast<WORD>(i);
		}
		m_pMesh->UnlockIndexBuffer();
	}

	DWORD dwFVFSize = D3DXGetFVFVertexSize( pMeshSRC->GetFVF() );

	// Vertex
	{
		BYTE* pVerticesSRC(NULL);
		BYTE* pVerticesDEST(NULL);
		m_pMesh->LockVertexBuffer ( 0L, (VOID**)&pVerticesDEST );
		pMeshSRC->LockVertexBuffer ( 0L, (VOID**)&pVerticesSRC );

		DWORD dwIndex(0);
		for ( DWORD i=dwFaceStart; i<dwFaceStart+dwFaceCount; ++i )
		{
			for ( DWORD j=0; j<3; ++j )
			{
				memcpy ( &pVerticesDEST[ dwIndex * dwFVFSize ], &pVerticesSRC[ pIndices32[(i*3)+j] * dwFVFSize ], dwFVFSize );
				++dwIndex;
			}
		}

		pMeshSRC->UnlockVertexBuffer ();
		m_pMesh->UnlockVertexBuffer ();
	}

	// Attribute
	{
		D3DXATTRIBUTERANGE* pAttribTableNEW = new D3DXATTRIBUTERANGE [ 1 ];
		pAttribTableNEW[ 0 ].AttribId = 0;
		pAttribTableNEW[ 0 ].FaceStart = 0;
		pAttribTableNEW[ 0 ].FaceCount = dwFaceCount;
		pAttribTableNEW[ 0 ].VertexStart = 0;
		pAttribTableNEW[ 0 ].VertexCount = dwFaceCount*3;

		m_pMesh->SetAttributeTable( pAttribTableNEW, 1 );

		SAFE_DELETE_ARRAY ( pAttribTableNEW );
	}

	SAFE_DELETE_ARRAY ( pIndices32 );


	m_dwVertices = m_pMesh->GetNumVertices();
	m_dwFaces = m_pMesh->GetNumFaces();
	m_dwVertSize = dwFVFSize;

	// VB, IB 참조
	m_pMesh->GetVertexBuffer( &m_rVB );
	m_pMesh->GetIndexBuffer( &m_rIB );
}

BOOL MaterialMesh::Insert( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMeshSRC )
{
	// 초기 버젼을 셋팅한다.
	m_dwVer = 0L;

	SAFE_RELEASE( m_rVB );
	SAFE_RELEASE( m_rIB );
	SAFE_RELEASE( m_pMesh );

	if ( FAILED( pMeshSRC->CloneMeshFVF( pMeshSRC->GetOptions(), pMeshSRC->GetFVF(), pd3dDevice, &m_pMesh ) ) )
		return FALSE;

	// LightMap 좌표를 빼낸다.
	if ( ( m_pMesh->GetFVF() & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX5 )
	{
		D3DXVECTOR2* pTex(NULL);
		UINT nSIZE = D3DXGetFVFVertexSize ( m_pMesh->GetFVF() );

		DWORD dwSizeADD = 0;
		if ( m_pMesh->GetFVF() & D3DFVF_XYZ )
		{
			dwSizeADD += sizeof(D3DXVECTOR3);
		}
		if ( m_pMesh->GetFVF() & D3DFVF_NORMAL )
		{
			dwSizeADD += sizeof(D3DXVECTOR3);
		}
		if ( m_pMesh->GetFVF() & D3DFVF_DIFFUSE )
		{
			dwSizeADD += sizeof(DWORD);
		}
		if ( ( m_pMesh->GetFVF() & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX1 )
		{
			if ( m_pMesh->GetFVF() & D3DFVF_TEXCOORDSIZE3(0) )
			{
				dwSizeADD += sizeof(D3DXVECTOR3);
			}
			else if ( m_pMesh->GetFVF() & D3DFVF_TEXCOORDSIZE4(0) )
			{
				dwSizeADD += sizeof(D3DXVECTOR4);
			}
			else if ( m_pMesh->GetFVF() & D3DFVF_TEXCOORDSIZE1(0) )
			{
				dwSizeADD += sizeof(float);
			}
			else
			{
				dwSizeADD += sizeof(D3DXVECTOR2);
			}
		}
		if ( ( m_pMesh->GetFVF() & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX2 )
		{
			if ( m_pMesh->GetFVF() & D3DFVF_TEXCOORDSIZE3(1) )
			{
				dwSizeADD += sizeof(D3DXVECTOR3);
			}
			else if ( m_pMesh->GetFVF() & D3DFVF_TEXCOORDSIZE4(1) )
			{
				dwSizeADD += sizeof(D3DXVECTOR4);
			}
			else if ( m_pMesh->GetFVF() & D3DFVF_TEXCOORDSIZE1(1) )
			{
				dwSizeADD += sizeof(float);
			}
			else
			{
				dwSizeADD += sizeof(D3DXVECTOR2);
			}
		}
		if ( ( m_pMesh->GetFVF() & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX3 )
		{
			if ( m_pMesh->GetFVF() & D3DFVF_TEXCOORDSIZE3(2) )
			{
				dwSizeADD += sizeof(D3DXVECTOR3);
			}
			else if ( m_pMesh->GetFVF() & D3DFVF_TEXCOORDSIZE4(2) )
			{
				dwSizeADD += sizeof(D3DXVECTOR4);
			}
			else if ( m_pMesh->GetFVF() & D3DFVF_TEXCOORDSIZE1(2) )
			{
				dwSizeADD += sizeof(float);
			}
			else
			{
				dwSizeADD += sizeof(D3DXVECTOR2);
			}
		}
		if ( ( m_pMesh->GetFVF() & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX4 )
		{
			if ( m_pMesh->GetFVF() & D3DFVF_TEXCOORDSIZE3(3) )
			{
				dwSizeADD += sizeof(D3DXVECTOR3);
			}
			else if ( m_pMesh->GetFVF() & D3DFVF_TEXCOORDSIZE4(3) )
			{
				dwSizeADD += sizeof(D3DXVECTOR4);
			}
			else if ( m_pMesh->GetFVF() & D3DFVF_TEXCOORDSIZE1(3) )
			{
				dwSizeADD += sizeof(float);
			}
			else
			{
				dwSizeADD += sizeof(D3DXVECTOR2);
			}
		}
		if ( ( m_pMesh->GetFVF() & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX5 )
		{
			if ( m_pMesh->GetFVF() & D3DFVF_TEXCOORDSIZE3(4) )
			{
				dwSizeADD += sizeof(D3DXVECTOR3);
			}
			else if ( m_pMesh->GetFVF() & D3DFVF_TEXCOORDSIZE4(4) )
			{
				dwSizeADD += sizeof(D3DXVECTOR4);
			}
			else if ( m_pMesh->GetFVF() & D3DFVF_TEXCOORDSIZE1(4) )
			{
				dwSizeADD += sizeof(float);
			}
			else
			{
				dwSizeADD += sizeof(D3DXVECTOR2);
			}
		}

		BYTE* pVerticesSRC(NULL);
		m_pMesh->LockVertexBuffer ( 0L, (VOID**)&pVerticesSRC );

		m_vecLightMapUV.reserve( m_pMesh->GetNumVertices() );
		for ( DWORD i=0; i<m_pMesh->GetNumVertices(); ++i )
		{
			pTex = reinterpret_cast<D3DXVECTOR2*>( pVerticesSRC + i*nSIZE + dwSizeADD - sizeof(D3DXVECTOR2) );
			m_vecLightMapUV.push_back( *pTex );
		}

		m_pMesh->UnlockVertexBuffer ();


		// VB, IB 참조

		m_dwVertices = m_pMesh->GetNumVertices();
		m_dwFaces = m_pMesh->GetNumFaces();
		m_dwVertSize = dwSizeADD;

		// VB, IB 참조
		m_pMesh->GetVertexBuffer( &m_rVB );
		m_pMesh->GetIndexBuffer( &m_rIB );
	}

	return TRUE;
}

void MaterialMesh::SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate )
{
	if ( !m_pMesh )
		return;

	SAFE_RELEASE( m_rVB );
	SAFE_RELEASE( m_rIB );

	m_vecLightMapUV.clear();
	if ( m_pMesh->GetNumVertices() > m_vecLightMapUV.capacity() )
	{
		m_vecLightMapUV.reserve( m_pMesh->GetNumVertices() );
	}

	UINT nSIZE = D3DXGetFVFVertexSize ( m_pMesh->GetFVF() );

	BYTE* pVerticesSRC(NULL);
	m_pMesh->LockVertexBuffer ( 0L, (VOID**)&pVerticesSRC );

	D3DXVECTOR2* pTex(NULL);
	D3DXVECTOR2 sTex;
	for ( DWORD i=0; i<m_pMesh->GetNumVertices(); ++i )
	{
		pTex = reinterpret_cast<D3DXVECTOR2*>( pVerticesSRC + (i+1)*nSIZE - sizeof(D3DXVECTOR2) );
		sTex.x = pTex->x * vLightMapUV_Rate.x;
		sTex.y = pTex->y * vLightMapUV_Rate.y;
		m_vecLightMapUV.push_back( sTex );
	}

	m_pMesh->UnlockVertexBuffer ();

	// VB, IB 참조
	m_pMesh->GetVertexBuffer( &m_rVB );
	m_pMesh->GetIndexBuffer( &m_rIB );
}

BOOL MaterialMesh::Render( LPDIRECT3DDEVICEQ pd3dDevice, 
						  const MATERIAL_DATA& sMaterial_Data,
						  std::tr1::shared_ptr<MaterialMesh> spSrcMesh )
{
	return Render( pd3dDevice, sMaterial_Data, spSrcMesh->m_pMesh );
}

BOOL MaterialMesh::Render( LPDIRECT3DDEVICEQ pd3dDevice, 
						  const MATERIAL_DATA& sMaterial_Data,
						  LPD3DXMESH pOcMesh )
{
	if ( !m_pMesh )
		return FALSE;

	MaterialSmall* pMaterialSmall(NULL);
	if ( sMaterial_Data.m_bGameMode )
	{
		pMaterialSmall = &sMaterial_Data.m_rMaterial->m_sMaterialGAME;
	}
	else
	{
		pMaterialSmall = &sMaterial_Data.m_rMaterial->m_sMaterialTOOL;
	}

	if ( m_dwVer != pMaterialSmall->m_dwVer )
	{
			m_dwVer = pMaterialSmall->m_dwVer;

		SAFE_RELEASE( m_rVB );
		SAFE_RELEASE( m_rIB );

		if ( !NSMATERIALMESH::CreateMesh( pd3dDevice, sMaterial_Data.m_rMaterial, pMaterialSmall, m_vecLightMapUV, pOcMesh, m_pMesh ) )
			return FALSE;

		m_dwVertices = m_pMesh->GetNumVertices();
		m_dwFaces = m_pMesh->GetNumFaces();
		m_dwVertSize = D3DXGetDeclVertexSize( pMaterialSmall->m_pELMT, 0 );

		// VB, IB 참조
		m_pMesh->GetVertexBuffer( &m_rVB );
		m_pMesh->GetIndexBuffer( &m_rIB );
	}

	m_pMesh->DrawSubset( 0 );

	return TRUE;
}

BOOL MaterialMesh::RenderPiece_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, 
										  ID3DXEffect* pFX, 
										  const MATERIAL_DATA& sMaterial_Data,
										  LPD3DXMESH pOcMesh )
{
	if ( !m_pMesh || !m_rVB )
		return FALSE;

	MaterialSmall* pMaterialSmall(NULL);
	pMaterialSmall = &sMaterial_Data.m_rMaterial->m_sMaterialGAME;

	if ( m_dwVer != pMaterialSmall->m_dwVer )
	{
		m_dwVer = pMaterialSmall->m_dwVer;

		SAFE_RELEASE( m_rVB );
		SAFE_RELEASE( m_rIB );

		if ( !NSMATERIALMESH::CreateMesh( pd3dDevice, sMaterial_Data.m_rMaterial, pMaterialSmall, m_vecLightMapUV, pOcMesh, m_pMesh ) )
			return FALSE;

		m_dwVertices = m_pMesh->GetNumVertices();
		m_dwFaces = m_pMesh->GetNumFaces();
		m_dwVertSize = D3DXGetDeclVertexSize( pMaterialSmall->m_pELMT, 0 );

		// VB, IB 참조
		m_pMesh->GetVertexBuffer( &m_rVB );
		m_pMesh->GetIndexBuffer( &m_rIB );
	}

	pd3dDevice->SetStreamSource( 0, m_rVB, 0, m_dwVertSize );
	pd3dDevice->SetIndices( m_rIB );

	pFX->CommitChanges();

	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwVertices, 0, m_dwFaces );

	return TRUE;
}

//------------------------------------------------------------------------------
//							MaterialMeshArray
MaterialMeshArray::MaterialMeshArray()
{


}

MaterialMeshArray::~MaterialMeshArray()
{
	m_vecMaterialMesh.clear();
	m_vecMaterialMeshBackUp.clear();
}

void MaterialMeshArray::Insert( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMeshSRC )
{
	DWORD dwAttibTableSize;
	pMeshSRC->GetAttributeTable ( NULL, &dwAttibTableSize );

	LPD3DXATTRIBUTERANGE pAttribTable = NULL;
	pAttribTable = new D3DXATTRIBUTERANGE [ dwAttibTableSize ];
	pMeshSRC->GetAttributeTable ( pAttribTable, &dwAttibTableSize );

	for ( DWORD i=0; i<dwAttibTableSize; ++i )
	{
		std::tr1::shared_ptr<MaterialMesh> spMaterialMesh = std::tr1::shared_ptr<MaterialMesh>(new MaterialMesh);
		spMaterialMesh->Insert( pd3dDevice, pMeshSRC, pAttribTable, i );
		m_vecMaterialMesh.push_back( spMaterialMesh );

		std::tr1::shared_ptr<MaterialMesh> spMaterialMeshNew = std::tr1::shared_ptr<MaterialMesh>(new MaterialMesh);
		spMaterialMeshNew->Insert( pd3dDevice, pMeshSRC, pAttribTable, i );
		m_vecMaterialMeshBackUp.push_back( spMaterialMeshNew );
	}

	SAFE_DELETE_ARRAY ( pAttribTable );
}

BOOL MaterialMeshArray::Render( LPDIRECT3DDEVICEQ pd3dDevice, 
							   DWORD dwAttIndex, 
							   const MATERIAL_DATA& sMaterial_Data,
							   DWORD dwPass,
							   BOOL bForwardPass )	// bForwardPass : TRUE - Pass2,Water,CustomAlpha 등 Deffered 로 렌더링 안하는 것들..
{
	if ( dwAttIndex >= m_vecMaterialMesh.size() )
		return FALSE;

	if ( sMaterial_Data.m_rMaterial->m_emSurfaceProp == NSMaterialManager::EMSURFACEPROP_CUSTOM_SOFT_ALPHA ||
		sMaterial_Data.m_rMaterial->m_emSurfaceProp == NSMaterialManager::EMSURFACEPROP_CUSTOM_UNLIT_UNFOG )
	{
		// EMSURFACEPROP_CUSTOM_SOFT_ALPHA, EMSURFACEPROP_CUSTOM_UNLIT_UNFOG 은 디퍼드로 렌더링 되면 안된다. 
		if ( !bForwardPass )
		{
			return TRUE;	// FALSE 가 넘어가면, Default texture 가 나와서 TRUE 로 해줌.
		}
	}
	else
	{
		// 기본은 Deffered 로 렌더링 되어야 한다.
		if ( bForwardPass )
		{
			return TRUE;	// FALSE 가 넘어가면, Default texture 가 나와서 TRUE 로 해줌.
		}
	}

	DWORD dwFVF_SAVE(0L);
	pd3dDevice->GetFVF( &dwFVF_SAVE );
	pd3dDevice->SetFVF( 0 );

	if ( sMaterial_Data.m_rMaterial->m_emSurfaceProp == NSMaterialManager::EMSURFACEPROP_CUSTOM_SOFT_ALPHA )
	{
		DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
	}
	else if ( sMaterial_Data.m_rMaterial->m_emSurfaceProp == NSMaterialManager::EMSURFACEPROP_CUSTOM_UNLIT_UNFOG )
	{
		DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
	}

	{
		UINT numPasses;
		sMaterial_Data.m_rMaterial->m_sFXnType.m_rFX->Begin( &numPasses, 0 );
		if( dwPass < numPasses )
		{
			sMaterial_Data.m_rMaterial->m_sFXnType.m_rFX->BeginPass( dwPass );
			{
				m_vecMaterialMesh[dwAttIndex]->Render( pd3dDevice, sMaterial_Data, m_vecMaterialMeshBackUp[dwAttIndex] );
			}
			sMaterial_Data.m_rMaterial->m_sFXnType.m_rFX->EndPass();
		}
		sMaterial_Data.m_rMaterial->m_sFXnType.m_rFX->End();
	}

	if ( sMaterial_Data.m_rMaterial->m_emSurfaceProp == NSMaterialManager::EMSURFACEPROP_CUSTOM_SOFT_ALPHA )
	{
		DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
	}
	else if ( sMaterial_Data.m_rMaterial->m_emSurfaceProp == NSMaterialManager::EMSURFACEPROP_CUSTOM_UNLIT_UNFOG )
	{
		DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
	}

	pd3dDevice->SetFVF( dwFVF_SAVE );

	return TRUE;
}

namespace NSMATERIALMESH
{
	// WorldEdit 에서 확인용 데이터
	// 코딩을 잘못 했을 경우, 너무 많은 호출이 될 수 있어서, 확인용 변수
	int g_nCreateCount(0);

	BOOL CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, 
						const Material* pMaterial, 
						const MaterialSmall* pMaterialSmall, 
						const std::vector<D3DXVECTOR2>& vecLightMapUV, 
						LPD3DXMESH pSrcMesh, 
						LPD3DXMESH& pMeshOUT )
	{
		++g_nCreateCount;

		// Clone mesh to the new vertex format
		LPD3DXMESH pTempMesh(NULL);
		HRESULT hr = pSrcMesh->CloneMesh( pSrcMesh->GetOptions(), pMaterialSmall->m_pELMT, pd3dDevice, &pTempMesh );	

		// 단순한 확인용
		//DWORD dwFVF = pSrcMesh->GetFVF();

		SAFE_RELEASE( pMeshOUT );
		pMeshOUT = pTempMesh;
		pTempMesh = NULL;

		// 단순한 확인용
		//dwFVF = pMeshOUT->GetFVF();

		// Tangent 혹은 bBinormal 가 필요하다면 생성작업한다.
		if ( pMaterialSmall->m_bTangent || pMaterialSmall->m_bBinormal )
		{
			if ( pMaterialSmall->m_bTangent && pMaterialSmall->m_bBinormal )
			{
				// Compute tangents, which are required for normal mapping
				hr = D3DXComputeTangentFrameEx( pMeshOUT, D3DDECLUSAGE_TEXCOORD, 0, D3DDECLUSAGE_TANGENT, 0,
					D3DDECLUSAGE_BINORMAL, 0, D3DDECLUSAGE_NORMAL, 0,
					0, NULL, -1, 0, -1, &pTempMesh, NULL );
			}
			else if ( pMaterialSmall->m_bTangent )
			{
				// Compute tangents, which are required for normal mapping
				hr = D3DXComputeTangentFrameEx( pMeshOUT, D3DDECLUSAGE_TEXCOORD, 0, D3DDECLUSAGE_TANGENT, 0,
					D3DX_DEFAULT, 0, D3DDECLUSAGE_NORMAL, 0,
					0, NULL, -1, 0, -1, &pTempMesh, NULL );
			}
			else if ( pMaterialSmall->m_bBinormal )
			{
				// Compute tangents, which are required for normal mapping
				hr = D3DXComputeTangentFrameEx( pMeshOUT, D3DDECLUSAGE_TEXCOORD, 0, D3DX_DEFAULT, 0,
					D3DDECLUSAGE_BINORMAL, 0, D3DDECLUSAGE_NORMAL, 0,
					0, NULL, -1, 0, -1, &pTempMesh, NULL );
			}
			else
			{
				return FALSE;
			}

			if( FAILED(hr) )
				return FALSE;

			// Copy the mesh and return
			SAFE_RELEASE( pMeshOUT );
			pMeshOUT = pTempMesh;
			pTempMesh = NULL;
		}

		// 원하는 위치에 LightMapIndex를 위치시킨다.
		if ( (vecLightMapUV.size() == pMeshOUT->GetNumVertices()) && (pMaterial->m_nLightMapIndex>=0) && (pMaterial->m_nLightMapIndex<=3) )
		{
			BOOL bChangeUV(FALSE);
			DWORD dwSizeADD(0L);
			for ( DWORD i=0; i<MAXD3DDECLLENGTH; ++i )
			{
				if ( pMaterialSmall->m_pELMT[i].Stream == 0x00ff )
					break;

				if ( pMaterialSmall->m_pELMT[i].Method == D3DDECLTYPE_UNUSED )
					break;

				if ( pMaterialSmall->m_pELMT[i].Usage == D3DDECLUSAGE_TEXCOORD && pMaterialSmall->m_pELMT[i].UsageIndex == pMaterial->m_nLightMapIndex )
				{
					bChangeUV = TRUE;
					dwSizeADD = pMaterialSmall->m_pELMT[i].Offset;
					break;
				}
			}

			UINT nSIZE = D3DXGetDeclVertexSize( pMaterialSmall->m_pELMT, 0 );

			if ( bChangeUV )
			{
				D3DXVECTOR2* pTex(NULL);
				BYTE* pVerticesSRC(NULL);
				pMeshOUT->LockVertexBuffer ( 0L, (VOID**)&pVerticesSRC );

				if ( pMaterial->m_bLightMap_float4 )
				{
					for ( DWORD i=0; i<pMeshOUT->GetNumVertices(); ++i )
					{
						pTex = reinterpret_cast<D3DXVECTOR2*>( pVerticesSRC + i*nSIZE + dwSizeADD + sizeof(D3DXVECTOR2) );
						*pTex = vecLightMapUV[i];
					}
				}
				else
				{
					for ( DWORD i=0; i<pMeshOUT->GetNumVertices(); ++i )
					{
						pTex = reinterpret_cast<D3DXVECTOR2*>( pVerticesSRC + i*nSIZE + dwSizeADD );
						*pTex = vecLightMapUV[i];
					}
				}

				pMeshOUT->UnlockVertexBuffer ();
			}
		}

		return TRUE;
	}
};
