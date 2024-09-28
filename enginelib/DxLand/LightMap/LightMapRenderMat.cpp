#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../../VisualMaterialLib/Util/DxTangentMesh.h"

#include "../../DxEffect/TexEff/DxTexEff.h"
#include "../../DxTools/Occlusion/NSOcclusionOctree.h"
#include "../../DxTools/Occlusion/DxOcclusionQuery.h"
#include "../../DxTools/Material/MaterialMesh.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/Collision.h"
#include "../DxPiece/NSOctreeRenderList.h"
#include "../DxPiece/NsOCTree.h"
#include "./NSLightMapFX.h"

#include "LightMapRenderMat.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


LightMapRenderMat::LightMapRenderMat()
	: AABBBinaryTreeBase( ABTT_MAT )
	, m_scpMaterialMesh( new MaterialMesh )
{
}

LightMapRenderMat::~LightMapRenderMat()
{
    AABBBinaryTreeBase::CleanUp_Base();
}

void LightMapRenderMat::CleanUp()
{
    AABBBinaryTreeBase::CleanUp_Base();
}

void LightMapRenderMat::DrawEff( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !NSOcclusionOctree::Visible( m_nID ) )
	{
		return;
	}

	m_pOcMesh->DrawSubset( 0 );
}

BOOL LightMapRenderMat::IsLoad() const
{
	if ( !m_bLoad )			
        return FALSE;

    if ( !m_pOcMesh )
        return FALSE;

	return TRUE; 
}

BOOL LightMapRenderMat::DynamicLoad( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bThread )
{
	BOOL	bUse = TRUE;

	// Note : Loading Complete
	if ( IsLoad() )		return TRUE;

	if ( !m_dwFileCur )	
	{
		bUse = FALSE;
		goto _RETURN;
	}

	SFile.SetOffSet( m_dwFileCur );

	m_bLoad = TRUE;		// 로딩 완료
	return bUse;

_RETURN:
	m_bLoad = FALSE;	// 로딩 실패
	return bUse;
}

void LightMapRenderMat::CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
    //if ( !m_pOcMesh )
    //    return;

    //switch ( m_dwFVF )
    //{
    //case VERTEXNORTEX2::FVF:
    //    m_spMaterialMesh = std::tr1::shared_ptr<DxTangentMesh>(new DxTangentMesh);
    //    m_spMaterialMesh->CreateMesh( pd3dDevice, m_pOcMesh );
    //    break;

    //case VERTEXNORCOLORTEX2::FVF:
    //    m_spMaterialMesh = std::tr1::shared_ptr<DxTangentMesh>(new DxTangentMesh);
    //    m_spMaterialMesh->CreateColorMeshLM( pd3dDevice, m_pOcMesh );
    //    break;
    //};
}

void LightMapRenderMat::RenderMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
										const MATERIAL_DATA& sMaterial_Data )
{
	if ( !NSOcclusionOctree::Visible( m_nID ) )
	{
		return;
	}

	m_scpMaterialMesh->Render( pd3dDevice, sMaterial_Data, m_pOcMesh );

	//if ( bShadowRender )
	//{
	//	m_spMaterialMesh->Render( pd3dDevice, pFX, dwFVF, MaterialMesh::EMRM_SHADOWMAP );
	//}
	//else if ( pTextureDay )
	//{
	//	m_spMaterialMesh->Render( pd3dDevice, pFX, dwFVF, MaterialMesh::EMRM_LIGHTMAP );
	//}
	//else
	//{
	//	m_spMaterialMesh->Render( pd3dDevice, pFX, dwFVF, MaterialMesh::EMRM_BASE );
	//}
}

void LightMapRenderMat::RenderPieceMaterial_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, 
														ID3DXEffect* pFX, 
														const MATERIAL_DATA& sMaterial_Data )
{
	m_scpMaterialMesh->RenderPiece_Instancing( pd3dDevice, pFX, sMaterial_Data, m_pOcMesh );
}

void LightMapRenderMat::RenderMaterialWater( LPDIRECT3DDEVICEQ pd3dDevice, 
											 const MATERIAL_DATA& sMaterial_Data, 
											 float& fHeight, 
											 float& fNearLength )
{
	const D3DXVECTOR3& vMax = GetMax();
	const D3DXVECTOR3& vCenter = GetCenter();
	if ( vMax.y <= (vCenter.y + 0.01f) )	// 조그만 차이는 인정해주자.
	{
		D3DXVECTOR3 vDir = DxViewPort::GetInstance().GetLookatPt() - vCenter;
		float fLength = D3DXVec3Length( &vDir );
		if ( fLength < fNearLength )
		{
			fNearLength = fLength;
			fHeight = vCenter.y;
		}
	}

	m_scpMaterialMesh->Render( pd3dDevice, sMaterial_Data, m_pOcMesh );
}

void LightMapRenderMat::Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex )
{
	if ( !m_pd3dQuery )
		return;

	if ( NSOcclusionExceptIndex::IsHaveExceptIndexOctree( m_nID ) )
		return;

	if( !m_bQueryTest )
	{
		// 1차검사 안되었으면 검사하고 플래그를 바꾼다.
		m_pd3dQuery->Issue( D3DISSUE_BEGIN );

		RenderCascadeShadow( pd3dDevice, bAlphaTex );

		m_pd3dQuery->Issue( D3DISSUE_END );

		m_bQueryTest = TRUE;
	}
}

void LightMapRenderMat::FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, VEC_DWORD& vecOctreeIndex )
{
	if ( !m_bQueryTest )
	{
		m_dwCullPixels = 0;
		return;
	}

	DWORD dwCullingDelay = timeGetTime();

	while ( GetDataQuery( m_dwCullPixels ) == S_FALSE )
	{
		//DrawEff( pd3dDevice );

		D3DXVECTOR3 VertBox[8];
		VertBox[0] = D3DXVECTOR3( 100.f,	100.f,	100.f );
		VertBox[1] = D3DXVECTOR3( 100.f,	100.f,	0.f );
		VertBox[2] = D3DXVECTOR3( 0.f,		100.f,	100.f );
		VertBox[3] = D3DXVECTOR3( 0.f,		100.f,	0.f );
		VertBox[4] = D3DXVECTOR3( 100.f,	0.f,	100.f );
		VertBox[5] = D3DXVECTOR3( 100.f,	0.f,	0.f );
		VertBox[6] = D3DXVECTOR3( 0.f,		0.f,	100.f );
		VertBox[7] = D3DXVECTOR3( 0.f,		0.f,	0.f );
		pd3dDevice->DrawPrimitiveUP ( D3DPT_POINTLIST, 8, VertBox, 12 );
	}

	// 딜레이를 누적한다.
	DxOcclusionQuery::g_dwCullingDelay += timeGetTime() - dwCullingDelay;

	// 1차검사 되었다고 2차검사가 제대로 완료 되었으므로 초기화 한다.
	m_bQueryTest = FALSE;

	if ( m_dwCullPixels != 0 )
	{
		vecOctreeIndex.push_back( m_nID );
	}
}

void LightMapRenderMat::MakeMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pOcMesh )
		return;

	if ( !m_scpMaterialMesh->Insert( pd3dDevice, m_pOcMesh ) )
		return;
}

void LightMapRenderMat::SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate )
{
	if ( !m_pOcMesh )
		return;

	m_scpMaterialMesh->SetLightMapUV_Rate( vLightMapUV_Rate );
}

void LightMapRenderMat::Save( sc::SerialFile& SFile, BOOL bPiece )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		SFile << m_vMax;
		SFile << m_vMin;

		if( m_pOcMesh )
		{
			SFile << (BOOL)TRUE;

            SFile << m_dwFVF;

			DWORD dwVertexNUM = m_pOcMesh->GetNumVertices();
			DWORD dwFacesNUM = m_pOcMesh->GetNumFaces();
			SFile << dwVertexNUM;
			SFile << dwFacesNUM;

			BYTE* pVertices(NULL);
			m_pOcMesh->LockVertexBuffer( 0L, (VOID**)&pVertices );
			SFile.WriteBuffer( pVertices, D3DXGetFVFVertexSize ( m_dwFVF )*dwVertexNUM );
			m_pOcMesh->UnlockVertexBuffer();

			WORD* pIndices(NULL);
			m_pOcMesh->LockIndexBuffer( 0L, (VOID**)&pIndices );
			SFile.WriteBuffer( pIndices, sizeof(WORD)*dwFacesNUM*3 );
			m_pOcMesh->UnlockIndexBuffer();

			// 충돌 Mesh
			{
				BOOL bExist = m_pCollsionTREE ? TRUE : FALSE;
				SFile << bExist;
				if ( m_pCollsionTREE )
				{
					m_pCollsionTREE->SaveFile( SFile );
				}
			}
		}
		else
		{
			SFile << (BOOL)FALSE;
		}

	}
	SFile.EndBlock();
}

void LightMapRenderMat::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece )
{
	CleanUp();

	if ( !bPiece )
	{
		m_nID = NSOctreeRenderList::g_nRenderIndex++;
	}

	DWORD dwVer(0);
	DWORD dwBufferSize(0);

	SFile >> dwVer;
	SFile >> dwBufferSize;

    if ( dwVer == 0x0101 )
    {
        SFile >> m_vMax;
	    SFile >> m_vMin;

		m_vCenter = (m_vMax+m_vMin)*0.5f;

	    BOOL bExist(FALSE);
	    SFile >> bExist;
	    if( bExist )
	    {
            SFile >> m_dwFVF;

		    DWORD dwVertexNUM(0L);
		    DWORD dwFacesNUM(0L);
		    SFile >> dwVertexNUM;
		    SFile >> dwFacesNUM;

		    SAFE_RELEASE( m_pOcMesh );
		    D3DXCreateMeshFVF( dwFacesNUM, dwVertexNUM, D3DXMESH_MANAGED, m_dwFVF, pd3dDevice, &m_pOcMesh );
		    if ( m_pOcMesh )
		    {
			    BYTE* pVertices(NULL);
			    m_pOcMesh->LockVertexBuffer( 0L, (VOID**)&pVertices );
			    SFile.ReadBuffer( pVertices, D3DXGetFVFVertexSize ( m_dwFVF )*dwVertexNUM );
			    m_pOcMesh->UnlockVertexBuffer();

			    WORD* pIndices(NULL);
			    m_pOcMesh->LockIndexBuffer( 0L, (VOID**)&pIndices );
			    SFile.ReadBuffer( pIndices, sizeof(WORD)*dwFacesNUM*3 );
			    m_pOcMesh->UnlockIndexBuffer();
		    }

			SFile >> bExist;
			if ( bExist )
			{
				m_pCollsionTREE = new DxAABBNode;
				if ( m_pCollsionTREE )	
					m_pCollsionTREE->LoadFile( SFile );
			}

			MakeMaterialMesh( pd3dDevice );
	    }
    }
	else if ( dwVer == 0x0100 )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;

		m_vCenter = (m_vMax+m_vMin)*0.5f;

		BOOL bExist(FALSE);
		SFile >> bExist;
		if( bExist )
		{
			SFile >> m_dwFVF;

			DWORD dwVertexNUM(0L);
			DWORD dwFacesNUM(0L);
			SFile >> dwVertexNUM;
			SFile >> dwFacesNUM;

			SAFE_RELEASE( m_pOcMesh );
			D3DXCreateMeshFVF( dwFacesNUM, dwVertexNUM, D3DXMESH_MANAGED, m_dwFVF, pd3dDevice, &m_pOcMesh );
			if ( m_pOcMesh )
			{
				BYTE* pVertices(NULL);
				m_pOcMesh->LockVertexBuffer( 0L, (VOID**)&pVertices );
				SFile.ReadBuffer( pVertices, D3DXGetFVFVertexSize ( m_dwFVF )*dwVertexNUM );
				m_pOcMesh->UnlockVertexBuffer();

				WORD* pIndices(NULL);
				m_pOcMesh->LockIndexBuffer( 0L, (VOID**)&pIndices );
				SFile.ReadBuffer( pIndices, sizeof(WORD)*dwFacesNUM*3 );
				m_pOcMesh->UnlockIndexBuffer();
			}

			SFile >> bExist;
			if ( bExist )
			{
				m_pCollsionTREE = new DxAABBNode;
				if ( m_pCollsionTREE )
				{
					m_pCollsionTREE->LoadFile_PREV( SFile );

					SAFE_DELETE( m_pCollsionTREE );
					NSOCTREE::MakeCollisionData2( m_pOcMesh, &m_pCollsionTREE );
				}
			}

			MakeMaterialMesh( pd3dDevice );
		}
	}
    else
    {
        SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
    }
	

    // 완료라고 일단 해 놓는다.
    // Dynamic Loading 들어가면 빼자
    m_bLoad = TRUE;		// 로딩 완료
}
