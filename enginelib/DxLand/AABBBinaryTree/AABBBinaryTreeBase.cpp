#include "pch.h"

#include "../../DxTools/Occlusion/NSOcclusionQueryFastRender.h"
#include "../../DxTools/Material/NSShadowRenderForMaterial.h"
#include "../../DxTools/Collision.h"
#include "../LightMap/LightMapRenderMat.h"
#include "../LightMap/LightMapRender.h"
#include "../DxPiece/OBJOCTree.h"

#include "AABBBinaryTreeBase.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


AABBBinaryTreeBase::AABBBinaryTreeBase( AABB_BINARY_TREE_TYPE emType )
    : m_nID(-1)
	, m_bCheck(FALSE)
	, m_bLoad(FALSE)
	, m_dwFileCur(0)
    , m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX)
	, m_vMin(FLT_MAX,FLT_MAX,FLT_MAX)
	, m_vCenter(0.f,0.f,0.f)
    , m_dwFVF(0L)
    , m_pVertex(NULL)
	, m_dwFace(0)
	, m_pOcMesh(NULL)
	, m_pShadowMesh(NULL)
	, m_pVB(NULL)
	, m_pIB(NULL)
    , m_pCollsionTREE(NULL)
    , m_pLeftChild(NULL)
    , m_pRightChild(NULL)
    , m_emType(emType)
	, m_bAlphaTex(FALSE)

	, m_pd3dQuery( NULL )
	, m_bQueryTest( FALSE )
	, m_dwCullPixels( 1 )
{
}

AABBBinaryTreeBase::~AABBBinaryTreeBase()
{
    SAFE_DELETE( m_pLeftChild );
    SAFE_DELETE( m_pRightChild );

    CleanUp_Base();
}

void AABBBinaryTreeBase::InitAABBBBinaryTreeBase( const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, BYTE* pVertex, DWORD dwFaceNum )
{
    m_vMax = vMax;
	m_vMin = vMin;
    m_pVertex = pVertex;
	m_dwFace = dwFaceNum;

	m_vCenter = (m_vMax+m_vMin)*0.5f;
}

void AABBBinaryTreeBase::CleanUp_Base()
{
    m_dwFace = 0;
    DeleteVertexArray();

    SAFE_RELEASE( m_pOcMesh );
	SAFE_RELEASE( m_pShadowMesh );
    SAFE_DELETE( m_pCollsionTREE );

	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pIB );
}

int AABBBinaryTreeBase::GetID()	const
{
	return m_nID;
}

const D3DXVECTOR3& AABBBinaryTreeBase::GetMax() const
{
    return m_vMax;
}
 
const D3DXVECTOR3& AABBBinaryTreeBase::GetMin() const
{
    return m_vMin;
}

const D3DXVECTOR3& AABBBinaryTreeBase::GetCenter() const
{
	return m_vCenter;
}

const BYTE* AABBBinaryTreeBase::GetVertices() const
{
    return m_pVertex;
}

DWORD AABBBinaryTreeBase::GetFaceNum() const
{
    return m_dwFace;
}

BOOL AABBBinaryTreeBase::GetLoad()  const
{
    return m_bLoad;
}

AABB_BINARY_TREE_TYPE AABBBinaryTreeBase::GetType() const
{
    return m_emType;
}

LPD3DXMESH AABBBinaryTreeBase::GetOcMesh()  const
{
    return m_pOcMesh;
}

LPD3DXMESH* AABBBinaryTreeBase::GetppOcMesh()
{
    return &m_pOcMesh;
}

const DxAABBNode* AABBBinaryTreeBase::GetCollisionTree() const
{
    return m_pCollsionTREE;
}

DxAABBNode** AABBBinaryTreeBase::GetppCollisionTree()
{
    return &m_pCollsionTREE;
}

void AABBBinaryTreeBase::SetLoad()
{
    m_bLoad = TRUE;
}

void AABBBinaryTreeBase::SetFaceNum_Zero()
{
    m_dwFace = 0;
}

void AABBBinaryTreeBase::SetFVF( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF )
{
    m_dwFVF = dwFVF;

	LPD3DXMESH pTempMesh;
	if ( m_pOcMesh )
	{
		if ( FAILED( m_pOcMesh->CloneMeshFVF( m_pOcMesh->GetOptions(), dwFVF, pd3dDevice, &pTempMesh ) ) )
			return;

		SAFE_RELEASE( m_pOcMesh );
		m_pOcMesh = pTempMesh;
	}
}

void AABBBinaryTreeBase::DeleteVertexArray()
{
    SAFE_DELETE_ARRAY( m_pVertex );
}

AABBBinaryTreeBase* AABBBinaryTreeBase::CreateFactory( AABB_BINARY_TREE_TYPE emType )
{
    switch( emType )
    {
    case ABTT_OCTREE:
        return new OBJOCTree;

    case ABTT_LIGHTMAP:
        return new LightMapRender;

	case ABTT_MAT:
		return new LightMapRenderMat;
    };

    return NULL;
}

void AABBBinaryTreeBase::GetVertexNum( DWORD& dwVertexNum )
{
	if ( !m_pOcMesh )
		return;

	dwVertexNum += m_pOcMesh->GetNumFaces() * 3;
}

void AABBBinaryTreeBase::GetVectorVertices( std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV )
{
	if ( !m_pOcMesh )
		return;

	D3DXVECTOR3* pVectorPos;
	D3DXVECTOR3* pVectorNor;
	D3DXVECTOR2* pVector2;
	D3DXVECTOR2* pVectorTex2;
	D3DXVECTOR2* pVectorTex3;
	D3DXVECTOR2* pVectorTex4;
	D3DXVECTOR2* pVectorTex5;
	DWORD*       pColor;
	UINT nSIZE = D3DXGetFVFVertexSize ( m_dwFVF );
	DWORD dwSizeADD = 0;

	VERTEXNORCOLORTEX4 sVertex;
	BYTE* pVertices(NULL);
	WORD* pIndices(NULL);
	m_pOcMesh->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pVertices );
	m_pOcMesh->LockIndexBuffer ( D3DLOCK_READONLY, (VOID**)&pIndices );
	for ( DWORD i=0; i<m_pOcMesh->GetNumFaces(); ++i )
	{
		for ( DWORD j=0; j<3; ++j )
		{
			dwSizeADD = 0;

			WORD nIndex = pIndices[i*3+j];

			if ( m_dwFVF & D3DFVF_XYZ )
			{
				pVectorPos = reinterpret_cast<D3DXVECTOR3*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vPos = *pVectorPos;

				dwSizeADD += sizeof(D3DXVECTOR3);
			}
			if ( m_dwFVF & D3DFVF_NORMAL )
			{
				pVectorNor = reinterpret_cast<D3DXVECTOR3*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vNor = *pVectorNor;

				dwSizeADD += sizeof(D3DXVECTOR3);
			}
			if ( m_dwFVF & D3DFVF_DIFFUSE )
			{
				pColor = reinterpret_cast<DWORD*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.dwColor = *pColor;

				dwSizeADD += sizeof(DWORD);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX1 )
			{
				pVector2 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[0] = *pVector2;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX2 )
			{
				pVectorTex2 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[1] = *pVectorTex2;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX3 )
			{
				pVectorTex3 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[2] = *pVectorTex3;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX4 )
			{
				pVectorTex4 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[3] = *pVectorTex4;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX5 )
			{
				pVectorTex5 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				vecLightMapUV.push_back( *pVectorTex5 );

				dwSizeADD += sizeof(D3DXVECTOR2);
			}

			vecVertices.push_back( sVertex );
		}
	}
	m_pOcMesh->UnlockIndexBuffer();
	m_pOcMesh->UnlockVertexBuffer();
}

void AABBBinaryTreeBase::GetVectorVertices( std::vector<VERTEXNORCOLORTEX5>& vecVertices )
{
	if ( !m_pOcMesh )
		return;

	D3DXVECTOR3* pVectorPos;
	D3DXVECTOR3* pVectorNor;
	D3DXVECTOR2* pVector2;
	D3DXVECTOR2* pVectorTex2;
	D3DXVECTOR2* pVectorTex3;
	D3DXVECTOR2* pVectorTex4;
	D3DXVECTOR2* pVectorTex5;
	DWORD*       pColor;
	UINT nSIZE = D3DXGetFVFVertexSize ( m_dwFVF );
	DWORD dwSizeADD = 0;

	DWORD dwFVF = m_pOcMesh->GetFVF();

	VERTEXNORCOLORTEX5 sVertex;
	BYTE* pVertices(NULL);
	WORD* pIndices(NULL);
	m_pOcMesh->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pVertices );
	m_pOcMesh->LockIndexBuffer ( D3DLOCK_READONLY, (VOID**)&pIndices );
	for ( DWORD i=0; i<m_pOcMesh->GetNumFaces(); ++i )
	{
		for ( DWORD j=0; j<3; ++j )
		{
			dwSizeADD = 0;

			WORD nIndex = pIndices[i*3+j];

			if ( m_dwFVF & D3DFVF_XYZ )
			{
				pVectorPos = reinterpret_cast<D3DXVECTOR3*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vPos = *pVectorPos;

				dwSizeADD += sizeof(D3DXVECTOR3);
			}
			if ( m_dwFVF & D3DFVF_NORMAL )
			{
				pVectorNor = reinterpret_cast<D3DXVECTOR3*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vNor = *pVectorNor;

				dwSizeADD += sizeof(D3DXVECTOR3);
			}
			if ( m_dwFVF & D3DFVF_DIFFUSE )
			{
				pColor = reinterpret_cast<DWORD*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.dwColor = *pColor;

				dwSizeADD += sizeof(DWORD);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX1 )
			{
				pVector2 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[0] = *pVector2;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX2 )
			{
				pVectorTex2 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[1] = *pVectorTex2;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX3 )
			{
				pVectorTex3 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[2] = *pVectorTex3;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX4 )
			{
				pVectorTex4 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[3] = *pVectorTex4;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}
			if ( ( m_dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX5 )
			{
				pVectorTex5 = reinterpret_cast<D3DXVECTOR2*>( pVertices + nIndex*nSIZE + dwSizeADD );
				sVertex.vTex[4] = *pVectorTex5;

				dwSizeADD += sizeof(D3DXVECTOR2);
			}

			vecVertices.push_back( sVertex );
		}
	}
	m_pOcMesh->UnlockIndexBuffer();
	m_pOcMesh->UnlockVertexBuffer();
}

// PixelShader 1.1 도 지원하지 못하는 그래픽카드를 위해서 이렇게 작업됨.
void AABBBinaryTreeBase::Render( LPDIRECT3DDEVICEQ pd3dDevice ) const
{
	if ( !m_pOcMesh )
		return;

	if ( NSOcclusionQueryFastRender::g_bRendering )
	{
		NSOcclusionQueryFastRender::Render( m_pOcMesh );
	}
	else
	{
		m_pOcMesh->DrawSubset( 0 );
	}
}

// 그림자 렌더링용 Mesh 를 생성한다.
void AABBBinaryTreeBase::CreateShadowMesh( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex )
{
	if ( !m_pShadowMesh )
	{
		SAFE_RELEASE ( m_pVB );
		SAFE_RELEASE ( m_pIB );

		m_bAlphaTex = bAlphaTex;
		if ( bAlphaTex )
		{
			m_pOcMesh->CloneMeshFVF( m_pOcMesh->GetOptions(), POSTEX::FVF, pd3dDevice, &m_pShadowMesh );
		}
		else
		{
			m_pOcMesh->CloneMeshFVF( m_pOcMesh->GetOptions(), POSONLY::FVF, pd3dDevice, &m_pShadowMesh );
		}

		m_dwVertexNUM = m_pShadowMesh->GetNumVertices();
		m_dwFaceNUM = m_pShadowMesh->GetNumFaces();
		m_pShadowMesh->GetVertexBuffer( &m_pVB );
		m_pShadowMesh->GetIndexBuffer( &m_pIB );
	}
	else if ( m_bAlphaTex != bAlphaTex )	// Mesh가 있는 상태인데 다르다면..
	{
		SAFE_RELEASE ( m_pVB );
		SAFE_RELEASE ( m_pIB );
		SAFE_RELEASE( m_pShadowMesh );

		m_bAlphaTex = bAlphaTex;
		if ( bAlphaTex )
		{
			m_pOcMesh->CloneMeshFVF( m_pOcMesh->GetOptions(), POSTEX::FVF, pd3dDevice, &m_pShadowMesh );
		}
		else
		{
			m_pOcMesh->CloneMeshFVF( m_pOcMesh->GetOptions(), POSONLY::FVF, pd3dDevice, &m_pShadowMesh );
		}

		m_dwVertexNUM = m_pShadowMesh->GetNumVertices();
		m_dwFaceNUM = m_pShadowMesh->GetNumFaces();
		m_pShadowMesh->GetVertexBuffer( &m_pVB );
		m_pShadowMesh->GetIndexBuffer( &m_pIB );
	}
}

// 그림자를 렌더링 하기 위해 이렇게 쓴다.
void AABBBinaryTreeBase::RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex )
{
	if ( !m_pOcMesh )
		return;

	CreateShadowMesh( pd3dDevice, bAlphaTex );

	if ( m_pShadowMesh )
	{
		m_pShadowMesh->DrawSubset( 0 );
	}
}

// 그림자를 렌더링 하기 위해 이렇게 쓴다.
void AABBBinaryTreeBase::RenderCascadeShadow_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex )
{
	if ( !m_pOcMesh )
		return;

	CreateShadowMesh( pd3dDevice, bAlphaTex );

	if ( m_pShadowMesh )
	{
		if ( bAlphaTex )
		{
			pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( POSTEX ) );
		}
		else
		{
			pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( POSONLY ) );
		}

		pd3dDevice->SetIndices( m_pIB );

		NSShadowRenderForMaterial::CommitChanges();

		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwVertexNUM, 0, m_dwFaceNUM );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------
//			D	x		P	i	e	c	e		-		D	x		O	c	c	l	u	s	i	o	n		Q	u	e	r	y		시작
//void DxPiece::Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
//{
//	if ( !m_pd3dQuery )
//		return;
//
//	if( !m_pStaticAniFrame )	
//		return;
//
//	if( !COLLISION::IsCollisionVolume( sCV, m_vCenter, m_fRadius ) )	
//		return;
//
//	//if( m_bQueryTest )
//	//{
//	//	Render_Query2( pd3dDevice, sCV );
//	//}
//	//else
//	if( !m_bQueryTest )
//	{
//		// 1차검사 안되었으면 검사하고 플래그를 바꾼다.
//		HRESULT hr(D3D_OK);
//		hr = m_pd3dQuery->Issue( D3DISSUE_BEGIN );
//		if ( hr != D3D_OK )
//		{
//			int a=0;
//		}
//
//		Render_Query2( pd3dDevice, sCV );
//
//		hr = m_pd3dQuery->Issue( D3DISSUE_END );
//		if ( hr != D3D_OK )
//		{
//			int a=0;
//		}
//
//		m_bQueryTest = TRUE;
//	}
//}
//
//void DxPiece::Render_Query2( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
//{
//	pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
//
//	// Note : Render
//	D3DXMATRIX matTexScale;
//	D3DXMatrixIdentity( &matTexScale );
//	NSLightMapFX::SetWorld( m_matWorld, 1.f );
//	NSVERTEXNORTEX_FX::SetWorld( m_matWorld, 1.f );
//	m_pStaticAniFrame->Render
//		( 
//		pd3dDevice, 
//		m_matWorld, 
//		m_fAniTime, 
//		NULL, 
//		matTexScale, 
//		1.f
//		);
//}

VOID AABBBinaryTreeBase::CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DestroyQuery();

	pd3dDevice->CreateQuery( D3DQUERYTYPE_OCCLUSION, &m_pd3dQuery );
}

VOID AABBBinaryTreeBase::DestroyQuery()
{
	SAFE_RELEASE( m_pd3dQuery );
	m_bQueryTest = FALSE;
	m_dwCullPixels = 1;
}

HRESULT AABBBinaryTreeBase::GetDataQuery( DWORD& dwCullPixels )
{
	return m_pd3dQuery->GetData( (void*)&dwCullPixels, sizeof( DWORD ), 0 );
}

void AABBBinaryTreeBase::ResetOcclusionQuery()
{
	m_dwCullPixels = 0;
	m_bQueryTest = FALSE;
}

//BOOL DxPiece::VisibleOcclusionQueryTest( LPDIRECT3DDEVICEQ pd3dDevice, DWORD& dwCullPixels )
//{
//	if ( !m_bQueryTest )
//	{
//		dwCullPixels = 0;
//		return FALSE;
//	}
//
//	while ( GetDataQuery( dwCullPixels ) == S_FALSE )
//	{
//		CLIPVOLUME& sCV = DxViewPort::GetInstance().GetClipVolume();
//		Render_Query2( pd3dDevice, sCV );
//	}
//
//	// 1차검사 되었다고 2차검사가 제대로 완료 되었으므로 초기화 한다.
//	m_bQueryTest = FALSE;
//
//	return TRUE;
//}

//			D	x		P	i	e	c	e		-		D	x		O	c	c	l	u	s	i	o	n		Q	u	e	r	y		끝
//--------------------------------------------------------------------------------------------------------------------------------------