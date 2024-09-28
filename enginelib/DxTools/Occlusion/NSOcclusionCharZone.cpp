#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../Common/StlFunctions.h"
#include "../DxViewPort.h"
#include "../Collision.h"
#include "../EditMeshs.h"
#include "./NSOcclusionQueryFastRender.h"
#include "./NSOcclusionOctree.h"
#include "./DxOcclusionQuery.h"

#include "NSOcclusionCharZone.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------------------
//											DxOcclusionChar
DxOcclusionChar::DxOcclusionChar()
	: m_nID(-1)
	, m_pd3dQuery(NULL)
	, m_bQueryTest(FALSE)
	, m_dwCullPixels(0)
	, m_pSimpleMesh(NULL)
{
	for ( DWORD i=0; i<NSOcclusionCharZone::HEIGHT_TYPE_SIZE; ++i )
	{
		m_pMesh[i] = NULL;
		m_vMax[i] = D3DXVECTOR3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		m_vMin[i] = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
	}
}

DxOcclusionChar::~DxOcclusionChar()
{
	SAFE_RELEASE ( m_pSimpleMesh );

	for ( DWORD i=0; i<NSOcclusionCharZone::HEIGHT_TYPE_SIZE; ++i )
	{
		SAFE_RELEASE ( m_pMesh[i] );
	}

	DestroyQuery();
}

VOID DxOcclusionChar::CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DestroyQuery();

	pd3dDevice->CreateQuery( D3DQUERYTYPE_OCCLUSION, &m_pd3dQuery );
}

HRESULT DxOcclusionChar::GetDataQuery( DWORD& dwCullPixels )
{
	return m_pd3dQuery->GetData( (void*)&dwCullPixels, sizeof( DWORD ), 0 );
}

VOID DxOcclusionChar::DestroyQuery()
{
	SAFE_RELEASE( m_pd3dQuery );
	m_bQueryTest = FALSE;
	m_dwCullPixels = 1;
}

void DxOcclusionChar::ResetOcclusionQuery()
{
	m_dwCullPixels = 0;
	m_bQueryTest = FALSE;
}

BOOL DxOcclusionChar::FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, int nHeightType )
{
	if ( !m_bQueryTest )
	{
		m_dwCullPixels = 0;
		return FALSE;
	}

	DWORD dwCullingDelay = timeGetTime();

	while ( GetDataQuery( m_dwCullPixels ) == S_FALSE )
	{
		RenderSimple( pd3dDevice );
	}
	// 딜레이를 누적한다.
	DxOcclusionQuery::g_dwCullingDelay += timeGetTime() - dwCullingDelay;

	// 1차검사 되었다고 2차검사가 제대로 완료 되었으므로 초기화 한다.
	m_bQueryTest = FALSE;

	if ( m_dwCullPixels != 0 )
	{
		return TRUE;
	}

	return FALSE;
}

void DxOcclusionChar::CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, const std::vector<D3DXVECTOR3>& vecPoint, int nID )
{
	m_nID = nID;

	if ( vecPoint.empty() )
		return;

	for ( DWORD t=0; t<NSOcclusionCharZone::HEIGHT_TYPE_SIZE; ++t )
	{
		float fRadius(10.f);
		float fHeight(20.f);

		switch ( t )
		{
		case 1:
			fHeight = NSOcclusionCharZone::g_fCharHeightHigh;
			break;
		case 2:
			fHeight = NSOcclusionCharZone::g_fCharHeightHighMax;
			break;
		default:
			fHeight = NSOcclusionCharZone::g_fCharHeightAvg;
			break;
		}

		D3DXVECTOR3 vPos[8];
		vPos[0] = D3DXVECTOR3 ( fRadius,	fHeight,	fRadius );
		vPos[1] = D3DXVECTOR3 ( fRadius,	fHeight,	0.f );
		vPos[2] = D3DXVECTOR3 ( 0.f,		fHeight,	fRadius );
		vPos[3] = D3DXVECTOR3 ( 0.f,		fHeight,	0.f );
		vPos[4] = D3DXVECTOR3 ( fRadius,	1.f,		fRadius );
		vPos[5] = D3DXVECTOR3 ( fRadius,	1.f,		0.f );
		vPos[6] = D3DXVECTOR3 ( 0.f,		1.f,		fRadius );
		vPos[7] = D3DXVECTOR3 ( 0.f,		1.f,		0.f );

		WORD wIndex[]=
		{
			0,1,2,
			1,2,3,

			0,1,4,
			5,1,4,

			2,0,6,
			4,0,6,

			2,3,6,
			7,3,6,

			3,1,7,
			5,1,7,

			6,4,7,
			5,4,7
		};

		{
			SAFE_RELEASE ( m_pSimpleMesh );
			D3DXCreateMeshFVF ( 12, 8, D3DXMESH_MANAGED, D3DFVF_XYZ, pd3dDevice, &m_pSimpleMesh );
			D3DXVECTOR3* pVertices;
			m_pSimpleMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
			for ( DWORD z=0; z<8; ++z )
			{
				pVertices[z] = vPos[z] + vecPoint[0];

				if ( pVertices[z].x > m_vMax[t].x )
					m_vMax[t].x = pVertices[z].x;
				if ( pVertices[z].y > m_vMax[t].y )
					m_vMax[t].y = pVertices[z].y;
				if ( pVertices[z].z > m_vMax[t].z )
					m_vMax[t].z = pVertices[z].z;

				if ( pVertices[z].x < m_vMin[t].x )
					m_vMin[t].x = pVertices[z].x;
				if ( pVertices[z].y < m_vMin[t].y )
					m_vMin[t].y = pVertices[z].y;
				if ( pVertices[z].z < m_vMin[t].z )
					m_vMin[t].z = pVertices[z].z;
			}
			m_pSimpleMesh->UnlockVertexBuffer();

			WORD* pIndices;
			m_pSimpleMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );
			for ( DWORD z=0; z<12*3; ++z )
			{
				pIndices[z] = wIndex[z];
			}
			m_pSimpleMesh->UnlockIndexBuffer();
		}
		
		

		size_t unPointSize = vecPoint.size();

		SAFE_RELEASE ( m_pMesh[t] );
		D3DXCreateMeshFVF ( unPointSize*12, unPointSize*8, D3DXMESH_MANAGED, D3DFVF_XYZ, pd3dDevice, &m_pMesh[t] );

		D3DXVECTOR3 vAddPos(0.f,0.f,0.f);
		for ( size_t i=0; i<unPointSize; ++i )
		{
			D3DXVECTOR3* pVertices;
			m_pMesh[t]->LockVertexBuffer ( 0L, (VOID**)&pVertices );
			for ( DWORD z=0; z<8; ++z )
			{
				pVertices[i*8+z] = vPos[z] + vecPoint[i];

				if ( pVertices[i*8+z].x > m_vMax[t].x )
					m_vMax[t].x = pVertices[i*8+z].x;
				if ( pVertices[i*8+z].y > m_vMax[t].y )
					m_vMax[t].y = pVertices[i*8+z].y;
				if ( pVertices[i*8+z].z > m_vMax[t].z )
					m_vMax[t].z = pVertices[i*8+z].z;

				if ( pVertices[i*8+z].x < m_vMin[t].x )
					m_vMin[t].x = pVertices[i*8+z].x;
				if ( pVertices[i*8+z].y < m_vMin[t].y )
					m_vMin[t].y = pVertices[i*8+z].y;
				if ( pVertices[i*8+z].z < m_vMin[t].z )
					m_vMin[t].z = pVertices[i*8+z].z;
			}
			m_pMesh[t]->UnlockVertexBuffer();

			WORD* pIndices;
			m_pMesh[t]->LockIndexBuffer ( 0L, (VOID**)&pIndices );
			for ( DWORD z=0; z<12*3; ++z )
			{
				pIndices[i*8+z] = (WORD)(i*8) + wIndex[z];
			}
			m_pMesh[t]->UnlockIndexBuffer();

			vAddPos += vecPoint[i];
		}

		if ( t == 0 )
		{
			m_vCenter = vAddPos / static_cast<float>( unPointSize );
		}
	}
}

void DxOcclusionChar::InsertSort( const CLIPVOLUME& sCV, const D3DXVECTOR3& vFromPt, int nHeightType )
{
	if( !COLLISION::IsCollisionVolume( sCV, m_vMax[nHeightType], m_vMin[nHeightType] ) )	
		return;

	D3DXVECTOR3 vDir = m_vCenter - vFromPt;
	float fLength = D3DXVec3Length( &vDir );

	NSOcclusionCharZoneSort::Insert( fLength, this );
}

void DxOcclusionChar::Render_Query( const CLIPVOLUME& sCV, LPDIRECT3DDEVICEQ pd3dDevice, int nHeightType )
{
	if ( nHeightType >= NSOcclusionCharZone::HEIGHT_TYPE_SIZE)
		return;

	if ( NSOcclusionExceptIndex::IsHaveExceptIndexCharZone( m_nID ) )
		return;

	if ( !m_pd3dQuery )
		return;

	if( !COLLISION::IsCollisionVolume( sCV, m_vMax[nHeightType], m_vMin[nHeightType] ) )	
		return;

	if ( !m_pMesh[nHeightType] )
		return;

	if( !m_bQueryTest )
	{
		// 1차검사 안되었으면 검사하고 플래그를 바꾼다.
		m_pd3dQuery->Issue( D3DISSUE_BEGIN );

		Render( pd3dDevice, nHeightType );

		m_pd3dQuery->Issue( D3DISSUE_END );

		m_bQueryTest = TRUE;
	}
}

void DxOcclusionChar::Render( LPDIRECT3DDEVICEQ pd3dDevice, int nHeightType )
{
	m_pMesh[nHeightType]->DrawSubset( 0 );
}

void DxOcclusionChar::RenderSimple( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSimpleMesh->DrawSubset( 0 );
}

const D3DXVECTOR3* DxOcclusionChar::GetMax( int nHeightType ) const
{
	if ( nHeightType >= NSOcclusionCharZone::HEIGHT_TYPE_SIZE)
		return NULL;

	return &m_vMax[nHeightType];
}

const D3DXVECTOR3* DxOcclusionChar::GetMin( int nHeightType ) const
{
	if ( nHeightType >= NSOcclusionCharZone::HEIGHT_TYPE_SIZE)
		return NULL;

	return &m_vMin[nHeightType];
}

//-------------------------------------------------------------------------------------------------------------------
//											DxOcclusionCharTree
DxOcclusionCharTree::DxOcclusionCharTree()
	: m_nID(-1)
	, m_pLeftChild(NULL)
	, m_pRightChild(NULL)
{
}

DxOcclusionCharTree::~DxOcclusionCharTree()
{
	m_vecOcclusionChar.clear();

	SAFE_DELETE ( m_pLeftChild );
	SAFE_DELETE ( m_pRightChild );
}

void DxOcclusionCharTree::InsertSort( const CLIPVOLUME& sCV, const D3DXVECTOR3& vFromPt )
{
	if ( !COLLISION::IsCollisionVolume ( sCV, m_vMax, m_vMin ) )
		return;

	for ( DWORD i=0; i<m_vecOcclusionChar.size(); ++i )
	{
		m_vecOcclusionChar[i]->InsertSort( sCV, vFromPt, 2 );
	}

	if ( m_pLeftChild )
		m_pLeftChild->InsertSort( sCV, vFromPt );
	if ( m_pRightChild )
		m_pRightChild->InsertSort( sCV, vFromPt );
}

void DxOcclusionCharTree::GetCollCharZoneIndex( const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, VEC_DWORD& vecCollCharZone )
{
	if ( !COLLISION::IsCollisionAABBToAABB ( m_vMax, m_vMin, vMax, vMin ) )
		return;

	if ( m_nID != -1 )
		vecCollCharZone.push_back( m_nID );

	if ( m_pLeftChild )
		m_pLeftChild->GetCollCharZoneIndex( vMax, vMin, vecCollCharZone );
	if ( m_pRightChild )
		m_pRightChild->GetCollCharZoneIndex( vMax, vMin, vecCollCharZone );
}

void DxOcclusionCharTree::Save( sc::SerialFile& SFile )
{
	SFile << m_nID;
	SFile << m_vMax;
	SFile << m_vMin;

	if ( m_pLeftChild )
	{
		SFile << static_cast<BOOL>( TRUE );
		m_pLeftChild->Save( SFile );
	}
	else
	{
		SFile << static_cast<BOOL>( FALSE );
	}

	if ( m_pRightChild )
	{
		SFile << static_cast<BOOL>( TRUE );
		m_pRightChild->Save( SFile );
	}
	else
	{
		SFile << static_cast<BOOL>( FALSE );
	}
}

void DxOcclusionCharTree::Load( sc::SerialFile& SFile )
{
	SFile >> m_nID;
	SFile >> m_vMax;
	SFile >> m_vMin;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
		m_pLeftChild = new DxOcclusionCharTree;
		m_pLeftChild->Load( SFile );
	}

	SFile >> bExist;
	if ( bExist )
	{
		m_pRightChild = new DxOcclusionCharTree;
		m_pRightChild->Load( SFile );
	}
}

//-------------------------------------------------------------------------------------------------------------------
//											NSOcclusiontreeMaker
namespace NSOcclusiontreeMaker
{
	BOOL OptimizeDivide( DxOcclusionCharTree* pTree )
	{
		// 마지막 한개 남았을 경우 그만 분할 한다.
		if ( pTree->m_vecOcclusionChar.size() == 1 )
		{
			return FALSE;
		}

		D3DXVECTOR3 vMaxPoint = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		D3DXVECTOR3 vMinPoint = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		for ( size_t i=0; i<pTree->m_vecOcclusionChar.size(); ++i )
		{
			DxOcclusionChar* pOcc = pTree->m_vecOcclusionChar[i];
			const D3DXVECTOR3& vPos = pOcc->GetCenter();

			if( vMaxPoint.x < vPos.x )	vMaxPoint.x = vPos.x;
			if( vMaxPoint.y < vPos.y )	vMaxPoint.y = vPos.y;
			if( vMaxPoint.z < vPos.z )	vMaxPoint.z = vPos.z;

			if( vMinPoint.x > vPos.x )	vMinPoint.x = vPos.x;
			if( vMinPoint.y > vPos.y )	vMinPoint.y = vPos.y;
			if( vMinPoint.z > vPos.z )	vMinPoint.z = vPos.z;
		}

		float fLengthX = vMaxPoint.x - vMinPoint.x;
		float fLengthY = vMaxPoint.y - vMinPoint.y;
		float fLengthZ = vMaxPoint.z - vMinPoint.z;
		fLengthX = fabsf(fLengthX);
		fLengthY = fabsf(fLengthY);
		fLengthZ = fabsf(fLengthZ);

		float fHalfX = (vMaxPoint.x + vMinPoint.x)*0.5f;
		float fHalfY = (vMaxPoint.y + vMinPoint.y)*0.5f;
		float fHalfZ = (vMaxPoint.z + vMinPoint.z)*0.5f;

		// 순서 구하기
		VEC_DWORD vecDWORD;
		if ( (fLengthX>fLengthY) && (fLengthX>fLengthZ) )	// X 길다.
		{
			vecDWORD.push_back(0);
			if (fLengthY>fLengthZ)
			{
				vecDWORD.push_back(1);	
				vecDWORD.push_back(2);
			}
			else
			{
				vecDWORD.push_back(2);
				vecDWORD.push_back(1);
			}
		}
		else if (fLengthY>fLengthZ)									// Y 길다.
		{
			vecDWORD.push_back(1);
			if (fLengthX>fLengthZ)
			{
				vecDWORD.push_back(0);	
				vecDWORD.push_back(2);
			}
			else
			{
				vecDWORD.push_back(2);
				vecDWORD.push_back(0);
			}
		}								
		else 
		{
			vecDWORD.push_back(2);
			if (fLengthX>fLengthY)
			{
				vecDWORD.push_back(0);	
				vecDWORD.push_back(1);
			}
			else
			{
				vecDWORD.push_back(1);
				vecDWORD.push_back(0);
			}
		}

		int nDivide = -1;
		for ( DWORD z=0; z<vecDWORD.size(); ++z )
		{
			int nLeft = 0;
			int nRight = 0;
			for ( size_t i=0; i<pTree->m_vecOcclusionChar.size(); ++i )
			{
				DxOcclusionChar* pOcc = pTree->m_vecOcclusionChar[i];
				D3DXVECTOR3 vPos = pOcc->GetCenter();
				switch ( vecDWORD[z] )
				{
				case 0:
					{
						if( vPos.x > fHalfX )	++nRight;
						else					++nLeft;
					}
					break;

				case 1:
					{
						if( vPos.y > fHalfY )	++nRight;
						else					++nLeft;
					}
					break;

				case 2:
					{
						if( vPos.z > fHalfZ )	++nRight;
						else					++nLeft;
					}
					break;
				};
			}

			if ( nLeft && nRight )
			{
				// 분할이 잘 되었다.
				nDivide = vecDWORD[z];
				break;
			}
		}

		// 균일하게 분할이 안되어서 그만 만든다.
		if( nDivide == -1 )
			return FALSE;

		// Note : 작업
		pTree->m_pLeftChild = new DxOcclusionCharTree;
		pTree->m_pRightChild = new DxOcclusionCharTree;
		for ( size_t i=0; i<pTree->m_vecOcclusionChar.size(); ++i )
		{
			DxOcclusionChar* pOcc = pTree->m_vecOcclusionChar[i];
			D3DXVECTOR3 vPos = pOcc->GetCenter();
			switch( nDivide )
			{
			case 0:
				if( vPos.x > fHalfX )
				{
					pTree->m_pRightChild->m_vecOcclusionChar.push_back( pOcc );
				}
				else
				{
					pTree->m_pLeftChild->m_vecOcclusionChar.push_back( pOcc );
				}
				break;
			case 1:
				if( vPos.y > fHalfY )
				{
					pTree->m_pRightChild->m_vecOcclusionChar.push_back( pOcc );
				}
				else
				{
					pTree->m_pLeftChild->m_vecOcclusionChar.push_back( pOcc );
				}
				break;
			case 2:
				if( vPos.z > fHalfZ )
				{
					pTree->m_pRightChild->m_vecOcclusionChar.push_back( pOcc );
				}
				else
				{
					pTree->m_pLeftChild->m_vecOcclusionChar.push_back( pOcc );
				}
				break;
			};
		}

		// Note : 옮겨가서 지워준다.
		pTree->m_vecOcclusionChar.clear();

		return TRUE;
	}

	void MakeTreeChild( DxOcclusionCharTree* pTree )
	{
		// FALSE 일 경우 더 이상 분할 불가
		if( !OptimizeDivide( pTree ) )
		{
			// ID 를 셋팅해준다.
			// 2개가 나오는건 구조상 안됨.
			// 혹시 모르니 2개 이상일 경우도 셋팅은 해놓자. 첫번째 ID를 셋팅하지.
			if ( !pTree->m_vecOcclusionChar.empty() )
			{
				pTree->m_nID = pTree->m_vecOcclusionChar[0]->GetID();
			}
			return;
		}

		D3DXVECTOR3 vMaxPoint = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		D3DXVECTOR3 vMinPoint = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		for ( size_t i=0; i<pTree->m_pLeftChild->m_vecOcclusionChar.size(); ++i )
		{
			DxOcclusionChar* pOcc = pTree->m_pLeftChild->m_vecOcclusionChar[i];

			const D3DXVECTOR3* pMax = pOcc->GetMax( 2 );
			if ( !pMax )
				continue;

			const D3DXVECTOR3* pMin = pOcc->GetMin( 2 );
			if ( !pMin )
				continue;

			if( vMaxPoint.x < pMax->x )	vMaxPoint.x = pMax->x;
			if( vMaxPoint.y < pMax->y )	vMaxPoint.y = pMax->y;
			if( vMaxPoint.z < pMax->z )	vMaxPoint.z = pMax->z;

			if( vMinPoint.x > pMin->x )	vMinPoint.x = pMin->x;
			if( vMinPoint.y > pMin->y )	vMinPoint.y = pMin->y;
			if( vMinPoint.z > pMin->z )	vMinPoint.z = pMin->z;
		}

		// Left Right AABB 박스를 생성해준다.
		pTree->m_pLeftChild->m_vMax		= vMaxPoint;
		pTree->m_pLeftChild->m_vMin		= vMinPoint;
		pTree->m_pLeftChild->m_vMax	+= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		pTree->m_pLeftChild->m_vMin	-= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );


		vMaxPoint = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		vMinPoint = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		for ( size_t i=0; i<pTree->m_pRightChild->m_vecOcclusionChar.size(); ++i )
		{
			DxOcclusionChar* pOcc = pTree->m_pRightChild->m_vecOcclusionChar[i];

			const D3DXVECTOR3* pMax = pOcc->GetMax( 2 );
			if ( !pMax )
				continue;

			const D3DXVECTOR3* pMin = pOcc->GetMin( 2 );
			if ( !pMin )
				continue;

			if( vMaxPoint.x < pMax->x )	vMaxPoint.x = pMax->x;
			if( vMaxPoint.y < pMax->y )	vMaxPoint.y = pMax->y;
			if( vMaxPoint.z < pMax->z )	vMaxPoint.z = pMax->z;

			if( vMinPoint.x > pMin->x )	vMinPoint.x = pMin->x;
			if( vMinPoint.y > pMin->y )	vMinPoint.y = pMin->y;
			if( vMinPoint.z > pMin->z )	vMinPoint.z = pMin->z;
		}

		pTree->m_pRightChild->m_vMax	= vMaxPoint;
		pTree->m_pRightChild->m_vMin	= vMinPoint;
		pTree->m_pRightChild->m_vMax	+= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		pTree->m_pRightChild->m_vMin	-= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );


		// 박스를 계속 해서 분할한다.
		MakeTreeChild( pTree->m_pLeftChild );
		MakeTreeChild( pTree->m_pRightChild );
	}

	void MakeTree( DxOcclusionCharTree** ppTree, std::vector<DxOcclusionChar*>& vecOcclusionChar )
	{
		SAFE_DELETE( (*ppTree) );
		(*ppTree) = new DxOcclusionCharTree;
		(*ppTree)->m_vecOcclusionChar = vecOcclusionChar;

		D3DXVECTOR3 vMaxPoint = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		D3DXVECTOR3 vMinPoint = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		for ( size_t i=0; i<(*ppTree)->m_vecOcclusionChar.size(); ++i )
		{
			DxOcclusionChar* pOcc = (*ppTree)->m_vecOcclusionChar[i];

			const D3DXVECTOR3* pMax = pOcc->GetMax( 2 );
			if ( !pMax )
				continue;

			const D3DXVECTOR3* pMin = pOcc->GetMin( 2 );
			if ( !pMin )
				continue;

			if( vMaxPoint.x < pMax->x )	vMaxPoint.x = pMax->x;
			if( vMaxPoint.y < pMax->y )	vMaxPoint.y = pMax->y;
			if( vMaxPoint.z < pMax->z )	vMaxPoint.z = pMax->z;

			if( vMinPoint.x > pMin->x )	vMinPoint.x = pMin->x;
			if( vMinPoint.y > pMin->y )	vMinPoint.y = pMin->y;
			if( vMinPoint.z > pMin->z )	vMinPoint.z = pMin->z;
		}

		// Note : 충돌을 위해서
		(*ppTree)->m_vMax	= vMaxPoint;
		(*ppTree)->m_vMin	= vMinPoint;
		(*ppTree)->m_vMax	+= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		(*ppTree)->m_vMin	-= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );

		MakeTreeChild( (*ppTree) );
	}
};

//-------------------------------------------------------------------------------------------------------------------
//											NSOcclusionCharZone
namespace NSOcclusionCharZone
{
	DWORD		g_dwVERSION(0x100);

	float		g_fCharHeightAvg(20.f);			// 기본사이즈
	float		g_fCharHeightHigh(50.f);		// 큰사이즈

	// 만약 100.f 이 넘어간다면 그냥 렌더링 되도록 했음.
	float		g_fCharHeightHighMax(200.f);	// 엄청큰사이즈

	typedef std::map<int,DxOcclusionChar*>	MAP_OCCLUSION_CHAR;
	typedef MAP_OCCLUSION_CHAR::iterator	MAP_OCCLUSION_CHAR_ITER;
	MAP_OCCLUSION_CHAR				g_mapOcclusionChar;				// DxOcclusionChar* 참조값을 사용하기 때문에 삭제할 필요는 없다.
	DxOcclusionCharTree*			g_pOcclusionCharTree(NULL);		// Save, Load
	LPDIRECT3DVERTEXDECLARATION9	g_pDCRT(NULL);

	void CleanUp()
	{
		std::for_each( g_mapOcclusionChar.begin(), g_mapOcclusionChar.end(), std_afunc::DeleteMapObject() );
		g_mapOcclusionChar.clear();

		SAFE_DELETE( g_pOcclusionCharTree );
	}

	void Insert( LPDIRECT3DDEVICEQ pd3dDevice, int nID, const std::vector<D3DXVECTOR3>& vecPoint )
	{
		if ( vecPoint.empty() )
			return;

		DxOcclusionChar* pNew = new DxOcclusionChar;
		g_mapOcclusionChar.insert( std::make_pair( nID, pNew ) );
		pNew->CreateMesh( pd3dDevice, vecPoint, nID );
	}

	void MakeTree()
	{
		std::vector<DxOcclusionChar*> vecOcclusionChar;
		MAP_OCCLUSION_CHAR_ITER iter = g_mapOcclusionChar.begin();
		for ( ; iter!=g_mapOcclusionChar.end(); ++iter )
		{
			vecOcclusionChar.push_back( (*iter).second );
		}

		SAFE_DELETE( g_pOcclusionCharTree );
		NSOcclusiontreeMaker::MakeTree( &g_pOcclusionCharTree, vecOcclusionChar );
	}

	VOID CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		D3DVERTEXELEMENT9 VertexDecl[] =
		{
			{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			D3DDECL_END()
		};
		pd3dDevice->CreateVertexDeclaration( VertexDecl,  &g_pDCRT );

		MAP_OCCLUSION_CHAR_ITER iter = g_mapOcclusionChar.begin();
		for ( ; iter!=g_mapOcclusionChar.end(); ++iter )
		{
			(*iter).second->CreateQuery( pd3dDevice );
		}
	}

	VOID DestroyQuery()
	{
		SAFE_RELEASE( g_pDCRT );

		MAP_OCCLUSION_CHAR_ITER iter = g_mapOcclusionChar.begin();
		for ( ; iter!=g_mapOcclusionChar.end(); ++iter )
		{
			(*iter).second->DestroyQuery();
		}
	}

	void ResetOcclusionQuery()
	{
		MAP_OCCLUSION_CHAR_ITER iter = g_mapOcclusionChar.begin();
		for ( ; iter!=g_mapOcclusionChar.end(); ++iter )
		{
			(*iter).second->ResetOcclusionQuery();
		}
	}

	void InsertSort( const CLIPVOLUME& sCV )
	{
		NSOcclusionCharZoneSort::Clear();
		g_pOcclusionCharTree->InsertSort( sCV, DxViewPort::GetInstance().GetFromPt() );
		NSOcclusionCharZoneSort::Sort();
	}

	void GetCollCharZoneIndex( const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, VEC_DWORD& vecCollCharZone )
	{
		if ( g_pOcclusionCharTree )
		{
			g_pOcclusionCharTree->GetCollCharZoneIndex( vMax, vMin, vecCollCharZone );
		}
	}

	void Render_Query( const CLIPVOLUME& sCV, LPDIRECT3DDEVICEQ pd3dDevice, int nHeightType, VEC_DWORD& vecZoneIndex )
	{
		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );

		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		DWORD dwCullMode;
		pd3dDevice->GetRenderState( D3DRS_CULLMODE,		&dwCullMode );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,		D3DCULL_NONE );

		pd3dDevice->SetFVF( D3DFVF_XYZ );
		{
			ResetOcclusionQuery();

			NSOcclusionCharZoneSort::Render_Query( sCV, pd3dDevice, nHeightType );

			DWORD dwZEnable, ZWriteEnable;
			pd3dDevice->GetRenderState( D3DRS_ZENABLE,		&dwZEnable );
			pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE, &ZWriteEnable );
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			NSOcclusionCharZoneSort::FindOcclusionRenderIndex( pd3dDevice, nHeightType, vecZoneIndex );
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,		dwZEnable );
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, ZWriteEnable );
		}
		
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,		dwCullMode );
	}

	void Save( sc::SerialFile& SFile )
	{
		SFile << g_dwVERSION;
		SFile.BeginBlock();
		{
			if ( g_pOcclusionCharTree )
			{
				SFile << static_cast<BOOL>( TRUE );
				g_pOcclusionCharTree->Save( SFile );
			}
			else
			{
				SFile << static_cast<BOOL>( FALSE );
			}
		}
		SFile.EndBlock();
	}

	void Load( sc::SerialFile& SFile )
	{
		SAFE_DELETE ( g_pOcclusionCharTree );

		DWORD dwVer, dwBufferSize;
		SFile >> dwVer;
		SFile >> dwBufferSize;

		BOOL bExist(FALSE);
		SFile >> bExist;
		if ( bExist )
		{
			g_pOcclusionCharTree = new DxOcclusionCharTree;
			g_pOcclusionCharTree->Load( SFile );
		}
	}
};

namespace NSOcclusionCharZoneSort
{
	//typedef std::map<float,DxOcclusionChar*>		MAP_OCCLUSION_CHAR;
	//typedef MAP_OCCLUSION_CHAR::reverse_iterator	MAP_OCCLUSION_CHAR_RITER;
	//MAP_OCCLUSION_CHAR		g_mapOccCharSort;		// DxOcclusionChar* 참조값을 사용하기 때문에 삭제할 필요는 없다.

	//void Insert( float fLength, DxOcclusionChar* pOcclusionChar )
	//{
	//	g_mapOccCharSort.insert( std::make_pair(fLength,pOcclusionChar) );
	//}

	//void Clear()
	//{
	//	g_mapOccCharSort.clear();
	//}

	//void Render_Query( const CLIPVOLUME& sCV, LPDIRECT3DDEVICEQ pd3dDevice, int nHeightType )
	//{
	//	MAP_OCCLUSION_CHAR_RITER iter = g_mapOccCharSort.rbegin();
	//	for ( ; iter!=g_mapOccCharSort.rend(); ++iter )
	//	{
	//		(*iter).second->Render_Query( sCV, pd3dDevice, nHeightType );
	//	}
	//}

	//void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, int nHeightType, VEC_DWORD& vecZoneIndex )
	//{
	//	MAP_OCCLUSION_CHAR_RITER iter = g_mapOccCharSort.rbegin();
	//	for ( ; iter!=g_mapOccCharSort.rend(); ++iter )
	//	{
	//		if ( (*iter).second->FindOcclusionRenderIndex( pd3dDevice, nHeightType ) )
	//		{
	//			vecZoneIndex.push_back( (*iter).second->GetID() );
	//		}
	//	}
	//}

	struct DxOcclusionCharSort
	{
		float m_fLength;
		DxOcclusionChar* m_pOcclusionChar;		// DxOcclusionChar* 참조값을 사용하기 때문에 삭제할 필요는 없다.

		bool operator < ( const DxOcclusionCharSort& sSrc )
		{
			if ( m_fLength < sSrc.m_fLength ) return true;

			return false;
		}

		DxOcclusionCharSort()
			: m_fLength(0.f)
			, m_pOcclusionChar(NULL)
		{
		}
	};
	typedef std::vector<DxOcclusionCharSort*>		VEC_OCCLUSION_CHAR;
	VEC_OCCLUSION_CHAR		g_vecOccCharSort;		

	void Insert( float fLength, DxOcclusionChar* pOcclusionChar )
	{
		DxOcclusionCharSort* pNew = new DxOcclusionCharSort;
		pNew->m_fLength = fLength;
		pNew->m_pOcclusionChar = pOcclusionChar;
		g_vecOccCharSort.push_back( pNew );
	}

	void Sort()
	{
		std::sort( g_vecOccCharSort.begin(), g_vecOccCharSort.end() );
	}

	void Clear()
	{
		std::for_each ( g_vecOccCharSort.begin(), g_vecOccCharSort.end(), std_afunc::DeleteObject() );
		g_vecOccCharSort.clear();
	}

	void Render_Query( const CLIPVOLUME& sCV, LPDIRECT3DDEVICEQ pd3dDevice, int nHeightType )
	{
		int nSize = static_cast<int>( g_vecOccCharSort.size() );
		for ( int i=nSize-1; i>=0; --i )
		{
			g_vecOccCharSort[i]->m_pOcclusionChar->Render_Query( sCV, pd3dDevice, nHeightType );
		}
	}

	void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, int nHeightType, VEC_DWORD& vecZoneIndex )
	{
		int nSize = static_cast<int>( g_vecOccCharSort.size() );
		for ( int i=nSize-1; i>=0; --i )
		{
			if ( g_vecOccCharSort[i]->m_pOcclusionChar->FindOcclusionRenderIndex( pd3dDevice, nHeightType ) )
			{
				vecZoneIndex.push_back( g_vecOccCharSort[i]->m_pOcclusionChar->GetID() );
			}
		}
	}
};