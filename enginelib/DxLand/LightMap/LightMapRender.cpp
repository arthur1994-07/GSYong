#include "pch.h"

#include "../../../VisualMaterialLib/Util/DxTangentMesh.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxEffect/TexEff/DxTexEff.h"
#include "../../DxTools/Occlusion/NSOcclusionOctree.h"
#include "../../DxTools/Occlusion/DxOcclusionQuery.h"
#include "../../DxTools/Collision.h"
#include "../DxPiece/NSOctreeRenderList.h"
#include "../DxPiece/NsOCTree.h"
#include "./NSLightMapFX.h"

#include "LightMapRender.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


LightMapRender::LightMapRender()
	: AABBBinaryTreeBase( ABTT_LIGHTMAP )
{
}

LightMapRender::~LightMapRender()
{
    AABBBinaryTreeBase::CleanUp_Base();
}

void LightMapRender::CleanUp()
{
    AABBBinaryTreeBase::CleanUp_Base();
}

void LightMapRender::DrawEff( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !NSOcclusionOctree::Visible( m_nID ) )
	{
		return;
	}

	if( m_pOcMesh )
	{
        switch ( m_dwFVF )
        {
        case VERTEXNORTEX2::FVF:
            NSLightMapFX::RenderGeneral( pd3dDevice, m_pOcMesh );
            break;

        case VERTEXNORCOLORTEX2::FVF:
            NSLightMapFX::RenderColor( pd3dDevice, m_pOcMesh );
            break;
        };
	}
}

void LightMapRender::Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex )
{
	if ( !m_pd3dQuery )
		return;

	if ( NSOcclusionExceptIndex::IsHaveExceptIndexOctree( m_nID ) )
		return;

	if( !m_bQueryTest )
	{
		// 1차검사 안되었으면 검사하고 플래그를 바꾼다.
		m_pd3dQuery->Issue( D3DISSUE_BEGIN );

		DrawEff( pd3dDevice );

		m_pd3dQuery->Issue( D3DISSUE_END );

		m_bQueryTest = TRUE;
	}
}

void LightMapRender::FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, VEC_DWORD& vecOctreeIndex )
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

BOOL LightMapRender::IsLoad() const
{
	if ( !m_bLoad )			
        return FALSE;

    if ( !( m_pOcMesh || m_spTangentMesh.get() ) )
        return FALSE;

	return TRUE; 
}

BOOL LightMapRender::DynamicLoad( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bThread )
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

void LightMapRender::CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if ( !m_pOcMesh )
        return;

    switch ( m_dwFVF )
    {
    case VERTEXNORTEX2::FVF:
        m_spTangentMesh = std::tr1::shared_ptr<DxTangentMesh>(new DxTangentMesh);
        m_spTangentMesh->CreateMesh( pd3dDevice, m_pOcMesh );
        break;

    case VERTEXNORCOLORTEX2::FVF:
        m_spTangentMesh = std::tr1::shared_ptr<DxTangentMesh>(new DxTangentMesh);
        m_spTangentMesh->CreateColorMeshLM( pd3dDevice, m_pOcMesh );
        break;
    };
}

void LightMapRender::RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
                                            DxTexEffBase* pEff, 
                                            const D3DXMATRIX& matWorld, 
                                            const D3DXVECTOR2& vLightMapUV_Offset,
                                            float fAlpha,
											LPDIRECT3DTEXTUREQ pTextureDay, 
											LPDIRECT3DTEXTUREQ pTextureNight )
{
    if ( !m_spTangentMesh.get() )
	{
		CreateTangentMesh( pd3dDevice );

		if ( !m_spTangentMesh.get() )
		{
			return;
		}
	}

    switch ( m_dwFVF )
    {
    case VERTEXNORTEX2::FVF:
        pEff->Render( pd3dDevice, m_spTangentMesh.get(), matWorld, pTextureDay, pTextureNight, NULL, NULL, vLightMapUV_Offset, fAlpha );
        break;

    case VERTEXNORCOLORTEX2::FVF:
        pEff->RenderColorLM( pd3dDevice, m_spTangentMesh.get(), matWorld, pTextureDay, pTextureNight, NULL, NULL, vLightMapUV_Offset, fAlpha );
        break;
    };
}

void LightMapRender::Save( sc::SerialFile& SFile, BOOL bPiece )
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

void LightMapRender::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece )
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

    if ( dwVer == 0x0103 )
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
	    }
    }
	else if ( dwVer == 0x0102 )
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
		}
	}
	else if ( dwVer == 0x0101 )
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
		}
	}
    else if (dwVer == 0x0100 )
    {
        SFile >> m_vMax;
	    SFile >> m_vMin;

		m_vCenter = (m_vMax+m_vMin)*0.5f;

	    BOOL bExist(FALSE);
	    SFile >> bExist;
	    if( bExist )
	    {
		    DWORD dwVertexNUM(0L);
		    DWORD dwFacesNUM(0L);
		    SFile >> dwVertexNUM;
		    SFile >> dwFacesNUM;

		    SAFE_RELEASE( m_pOcMesh );
		    D3DXCreateMeshFVF( dwFacesNUM, dwVertexNUM, D3DXMESH_MANAGED, VERTEXNORTEX2::FVF, pd3dDevice, &m_pOcMesh );
		    if ( m_pOcMesh )
		    {
			    VERTEXNORTEX2* pVertices(NULL);
			    m_pOcMesh->LockVertexBuffer( 0L, (VOID**)&pVertices );
			    SFile.ReadBuffer( pVertices, sizeof(VERTEXNORTEX2)*dwVertexNUM );
			    m_pOcMesh->UnlockVertexBuffer();

			    WORD* pIndices(NULL);
			    m_pOcMesh->LockIndexBuffer( 0L, (VOID**)&pIndices );
			    SFile.ReadBuffer( pIndices, sizeof(WORD)*dwFacesNUM*3 );
			    m_pOcMesh->UnlockIndexBuffer();
		    }
	    }

        SAFE_RELEASE( m_pOcMesh );
    }
    else
    {
        SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
    }
	

    // 완료라고 일단 해 놓는다.
    // Dynamic Loading 들어가면 빼자
    m_bLoad = TRUE;		// 로딩 완료
}

void LightMapRender::Load_Prev( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece )
{
	Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
}