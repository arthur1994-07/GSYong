#include "pch.h"

#include "../../DxTools/Collision.h"
#include "DxOctreeMesh.h"
#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../DxLandMan.h"
#include "../../DxTools/TextureManager.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Log/LogMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void DXOCMATERIAL::SaveFile( sc::SerialFile &SFile )
{
    SFile.WriteBuffer( &rgMaterial, sizeof(D3DMATERIALQ) );

    VOID* pTemp = NULL;
    SFile.WriteBuffer( pTemp, sizeof(VOID*) );

    SFile.WriteBuffer( &szTexture, sizeof(char) * MAX_PATH );
}

void DXOCMATERIAL::LoadFile( sc::SerialFile &SFile )
{
    SFile.ReadBuffer( &rgMaterial, sizeof(D3DMATERIALQ) );

    VOID* pTemp = NULL;
    SFile.ReadBuffer( &pTemp, sizeof(VOID*) );

    SFile.ReadBuffer( &szTexture, sizeof(char) * MAX_PATH );
}

DxOctreeMesh::~DxOctreeMesh()
{
	ReleaseDxOctreeMesh();
	ReleaseAttribure();
	SAFE_DELETE(m_pAABBTreeRoot);
}

HRESULT		DxOctreeMesh::CreateDxOctreeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD NumFaces, DWORD NumVertices, DWORD FVF )
{
	m_dwFVF = FVF;

	m_dwNumFaces = NumFaces;
	if (FAILED(pd3dDevice->CreateIndexBuffer(
		m_dwNumFaces * 3 * sizeof(WORD),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIB,
		NULL)))
	{
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::CreateDxOctreeMesh() -- CreateIndexBuffer() -- Failed"));
		return E_FAIL;
	}


	DWORD dwFVFSize = D3DXGetFVFVertexSize ( m_dwFVF );

	m_dwNumVertices = NumVertices;
	if (FAILED(pd3dDevice->CreateVertexBuffer(
		m_dwNumVertices * dwFVFSize,
		0,
		m_dwFVF,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL)))
	{
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::CreateDxOctreeMesh() -- CreateVertexBuffer() -- Failed"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT		DxOctreeMesh::CreateDxOctreeMesh_OLD( LPDIRECT3DDEVICEQ pd3dDevice, DWORD NumFaces, DWORD NumVertices, DWORD FVF )
{
	m_dwFVF = FVF;

	m_dwNumFaces = NumFaces;
	if (FAILED(pd3dDevice->CreateIndexBuffer(
		m_dwNumFaces * 3 * sizeof(WORD),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIB,
		NULL)))
	{
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::CreateDxOctreeMesh() -- CreateIndexBuffer() -- Failed"));
		return E_FAIL;
	}


	m_dwNumVertices = NumVertices;
	if (FAILED(pd3dDevice->CreateVertexBuffer(
		m_dwNumVertices * sizeof(VERTEX_WORLD),
		0,
		m_dwFVF,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL)))
	{
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::CreateDxOctreeMesh() -- CreateVertexBuffer() -- Failed"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	DxOctreeMesh::LockIndexBuffer ( VOID** ppbData )
{
	HRESULT hr(S_OK);
	hr = m_pIB->Lock( 0, 0, ppbData, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::LockIndexBuffer(VOID) -- m_pIB->Lock() -- Failed"));
	}
	return hr;
}

HRESULT	DxOctreeMesh::LockIndexBuffer ( DWORD dwFlag, VOID** ppbData )
{
	HRESULT hr(S_OK);
	hr = m_pIB->Lock( 0, 0, ppbData, dwFlag );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::LockIndexBuffer(DWORD,VOID) -- m_pIB->Lock() -- Failed"));
	}
	return hr;
}


HRESULT	DxOctreeMesh::LockVertexBuffer ( VOID** ppbData )
{
	HRESULT hr(S_OK);
	hr = m_pVB->Lock( 0, 0, ppbData, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::LockVertexBuffer(VOID) -- m_pVB->Lock() -- Failed"));
	}
	return hr;
}

HRESULT	DxOctreeMesh::LockVertexBuffer ( DWORD dwFlag, VOID** ppbData )
{
	HRESULT hr(S_OK);
	hr = m_pVB->Lock( 0, 0, ppbData, dwFlag );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::LockVertexBuffer(DWORD,VOID) -- m_pVB->Lock() -- Failed"));
	}
	return hr;
}


HRESULT	DxOctreeMesh::UnlockVertexBuffer()
{
	return m_pVB->Unlock();
}

HRESULT	DxOctreeMesh::UnlockIndexBuffer	()
{
	return m_pIB->Unlock();
}

HRESULT	DxOctreeMesh::ReleaseDxOctreeMesh	()
{
	m_dwNumFaces = 0;
	m_dwNumVertices = 0;
	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pIB );

	return S_OK;
}


BOOL	DxOctreeMesh::CreateAttribute	( DWORD		AttribTableSize )
{
	m_dwAttribTableSize = AttribTableSize;

	SAFE_DELETE_ARRAY ( m_pAttribTable );
	m_pAttribTable = new D3DXATTRIBUTERANGE [ m_dwAttribTableSize ];

	memset ( m_pAttribTable, 0, sizeof ( D3DXATTRIBUTERANGE ) * m_dwAttribTableSize );

	return TRUE;
}

LPD3DXATTRIBUTERANGE	DxOctreeMesh::GetAttributeTbl	()
{
	return m_pAttribTable;
}

BOOL	DxOctreeMesh::ReleaseAttribure()
{
	SAFE_DELETE_ARRAY ( m_pAttribTable );
	m_dwAttribTableSize = 0;

	return TRUE;
}

HRESULT	DxOctreeMesh::DrawSubset( DWORD	AttribID, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pAttribTable )
		return E_FAIL;

	if ( AttribID >= m_dwAttribTableSize )
		return E_FAIL;

	return pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST,
			0,
			m_pAttribTable[AttribID].VertexStart,
			m_pAttribTable[AttribID].VertexCount,
			m_pAttribTable[AttribID].FaceStart * 3,
			m_pAttribTable[AttribID].FaceCount
		);
}

HRESULT	DxOctreeMesh::MakeAABBTree ( D3DXMATRIX &matComb )
{
	HRESULT hr = S_OK;
	WORD *pLockIndices=NULL;
	PBYTE pbLockPoints = NULL;

	hr = LockIndexBuffer( (VOID**)&pLockIndices );
	if ( FAILED ( hr ) )
		return hr;

	hr = LockVertexBuffer( (VOID**)&pbLockPoints );
	if ( FAILED ( hr ) )
		return hr;


	WORD *pFaces;
	pFaces = new WORD[m_dwNumFaces];
	if ( pFaces == NULL )
	{
		UnlockIndexBuffer();
		UnlockVertexBuffer();

		return E_OUTOFMEMORY;
	}

	for ( WORD i=0; i<m_dwNumFaces; i++ )
		pFaces[i] = i;

	D3DXVECTOR3 vMax, vMin;
	hr = COLLISION::GetSizeNode ( matComb, pLockIndices, pbLockPoints, m_dwFVF, pFaces, m_dwNumFaces, vMax, vMin );
	if ( FAILED ( hr ) )
	{
		UnlockIndexBuffer();
		UnlockVertexBuffer();

		delete[] pFaces;
		return hr;
	}

	DxAABBNode *pNode = new DxAABBNode;
	pNode->fMaxX = vMax.x;
	pNode->fMaxY = vMax.y;
	pNode->fMaxZ = vMax.z;
	pNode->fMinX = vMin.x;
	pNode->fMinY = vMin.y;
	pNode->fMinZ = vMin.z;

	hr = COLLISION::MakeAABBNode ( pNode, matComb, pLockIndices, pbLockPoints, m_dwFVF,
		pFaces, m_dwNumFaces, vMax, vMin );
	delete[] pFaces;

	m_pAABBTreeRoot = pNode;
	
	UnlockIndexBuffer();
	UnlockVertexBuffer();	
	
	ExportProgress::CurPos += m_dwNumFaces;

	return hr;
}

void DxOctreeMesh::IsCollision_DxOctreeMesh ( DxAABBNode *pAABBCur, const D3DXVECTOR3& vStartOrg, D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2, D3DXVECTOR3 &vCollision, 
								D3DXVECTOR3 &vNormal, DWORD& dwAttribid, EMCOLLMODE emCullMode, IN OUT BOOL& bColl )
{
	D3DXVECTOR3 vNewP1=vP1, vNewP2=vP2;

	if ( pAABBCur->IsCollision(vNewP1,vNewP2) )
	{
		if ( pAABBCur->IsEnableFace() )
		{
			const D3DXVECTOR3* pvT0 = pAABBCur->GetPosition(0);
			const D3DXVECTOR3* pvT1 = pAABBCur->GetPosition(1);
			const D3DXVECTOR3* pvT2 = pAABBCur->GetPosition(2);

			D3DXVECTOR3 vNewCollision;
			D3DXVECTOR3 vNewNormal;
			if ( COLLISION::IsLineTriangleCollision( pvT0, pvT1, pvT2, &vP1, &vP2, &vNewCollision, &vNewNormal, emCullMode, TRUE ) )
			{
				D3DXVECTOR3 vDxVect;
				vDxVect = vStartOrg - vNewCollision;
				float fNewLength = D3DXVec3LengthSq ( &vDxVect );
				vDxVect = vStartOrg - vCollision;
				float fOldLength = D3DXVec3LengthSq ( &vDxVect );

				if ( !bColl )
				{
					vCollision = vNewCollision;
					vNormal = vNewNormal;
				}

				if ( fNewLength < fOldLength )
				{
					vCollision = vNewCollision;
					vNormal = vNewNormal;
				}

				bColl = TRUE;

				// Tex 정보 알아오기
				for( DWORD i=0; i<m_dwAttribTableSize; ++i )
				{
					if( (pAABBCur->GetFaceID() >= m_pAttribTable[i].FaceStart) &&
						(pAABBCur->GetFaceID() <= (m_pAttribTable[i].FaceStart+m_pAttribTable[i].FaceCount)) )
					{
						dwAttribid = m_pAttribTable[i].AttribId;
					}
				}
			}
		}
		else
		{
			if ( pAABBCur->pLeftChild )
				IsCollision_DxOctreeMesh ( pAABBCur->pLeftChild, vStartOrg, vP1, vP2, vCollision, vNormal, dwAttribid, emCullMode, bColl );

			if ( pAABBCur->pRightChild )
				IsCollision_DxOctreeMesh ( pAABBCur->pRightChild, vStartOrg, vP1, vP2, vCollision, vNormal, dwAttribid, emCullMode, bColl );
		}
	}
}

void DxOctreeMesh::IsCollision_OctreeMesh ( const D3DXVECTOR3& vStartOrg, D3DXVECTOR3 &vPoint1, D3DXVECTOR3 &vPoint2,
								D3DXVECTOR3 &vCollision, BOOL &bCollision, D3DXVECTOR3 &vNormal, DWORD& dwAttribid, EMCOLLMODE emCullMode )
{
	if ( !m_pAABBTreeRoot )	return;

	IsCollision_DxOctreeMesh ( m_pAABBTreeRoot, vStartOrg, vPoint1, vPoint2, vCollision, vNormal, dwAttribid, emCullMode, bCollision );
}

BOOL DxOctreeMesh::IsCollisionLineSimple( DxAABBNode *pAABBCur, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode )
{
	D3DXVECTOR3 vNewP1=vStart, vNewP2=vEnd;

	if ( pAABBCur->IsCollision(vNewP1,vNewP2) )
	{
		if ( pAABBCur->IsEnableFace() )
		{
			const D3DXVECTOR3* pvT0 = pAABBCur->GetPosition(0);
			const D3DXVECTOR3* pvT1 = pAABBCur->GetPosition(1);
			const D3DXVECTOR3* pvT2 = pAABBCur->GetPosition(2);

			D3DXVECTOR3 vNewCollision;
			D3DXVECTOR3 vNewNormal;
			if ( COLLISION::IsLineTriangleCollision( pvT0, pvT1, pvT2, &vStart, &vEnd, &vNewCollision, &vNewNormal, emCullMode, TRUE ) )
			{
				return TRUE;
			}
		}
		else
		{
			if ( pAABBCur->pLeftChild )
			{
				if ( IsCollisionLineSimple ( pAABBCur->pLeftChild, vStart, vEnd, emCullMode ) )
					return TRUE;
			}

			if ( pAABBCur->pRightChild )
			{
				if ( IsCollisionLineSimple ( pAABBCur->pRightChild, vStart, vEnd, emCullMode ) )
					return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL DxOctreeMesh::IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode )
{
	if ( !m_pAABBTreeRoot )
		return FALSE;

	if ( IsCollisionLineSimple( m_pAABBTreeRoot, vStart, vEnd, emCullMode ) )
		return TRUE;

	return FALSE;
}

void DxOctreeMesh::GetCollisionLineInTriListExceptY2 ( DxAABBNode *pAABBCur, const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ) // 주병진 추가 메소드
{
	D3DXVECTOR3 vNewP1=vStart, vNewP2=vEnd;

	if ( pAABBCur->IsCollisionExceptY(vNewP1,vNewP2) )
	{
		if ( pAABBCur->IsEnableFace() )
		{
			const D3DXVECTOR3* pvT0 = pAABBCur->GetPosition(0);
			const D3DXVECTOR3* pvT1 = pAABBCur->GetPosition(1);
			const D3DXVECTOR3* pvT2 = pAABBCur->GetPosition(2);

			/////////////////////////////////////////////////////////

			D3DXVECTOR2 v2Cur1   = D3DXVECTOR2( pvT0->x, pvT0->z );
			D3DXVECTOR2 v2Cur2   = D3DXVECTOR2( pvT1->x, pvT1->z );
			D3DXVECTOR2 v2Cur3   = D3DXVECTOR2( pvT2->x, pvT2->z );
			D3DXVECTOR2 v2Start  = D3DXVECTOR2( vStart.x, vStart.z );
			D3DXVECTOR2 v2End    = D3DXVECTOR2( vEnd.x,   vEnd.z   );

			D3DXVECTOR2 v2Out1,v2Out2,v2Out3;
			BOOL        bPick1,bPick2,bPick3;

			bPick1 = COLLISION::IsCollision2DLineToLine(v2Start, v2End, v2Cur1, v2Cur2, v2Out1);
			bPick2 = COLLISION::IsCollision2DLineToLine(v2Start, v2End, v2Cur1, v2Cur3, v2Out2);
			bPick3 = COLLISION::IsCollision2DLineToLine(v2Start, v2End, v2Cur2, v2Cur3, v2Out3);

			D3DXVECTOR3 vCollisionPoint;

			FLOAT       fMaxDistance = -FLT_MAX;
			BOOL        IsPick       = FALSE;

			if ( bPick1 )
			{
				FLOAT fDistance = D3DXVec2Length(&D3DXVECTOR2(v2Out1 - v2Start));

				if ( fMaxDistance < fDistance )
				{
					vCollisionPoint = D3DXVECTOR3(v2Out1.x, 0.0f, v2Out1.y);
					fMaxDistance    = fDistance;
				}

				IsPick = TRUE;
			}

			if ( bPick2 )
			{
				FLOAT fDistance = D3DXVec2Length(&D3DXVECTOR2(v2Out2 - v2Start));

				if ( fMaxDistance < fDistance )
				{
					vCollisionPoint = D3DXVECTOR3(v2Out2.x, 0.0f, v2Out2.y);
					fMaxDistance    = fDistance;
				}

				IsPick = TRUE;
			}

			if ( bPick3 )
			{
				FLOAT fDistance = D3DXVec2Length(&D3DXVECTOR2(v2Out3 - v2Start));

				if ( fMaxDistance < fDistance )
				{
					vCollisionPoint = D3DXVECTOR3(v2Out3.x, 0.0f, v2Out3.y);
					fMaxDistance    = fDistance;
				}

				IsPick = TRUE;
			}

			if ( IsPick )
			{
				FLOAT fHeight = -FLT_MAX;

				if ( pvT0->y > fHeight ) fHeight = pvT0->y;
				if ( pvT1->y > fHeight ) fHeight = pvT1->y;
				if ( pvT2->y > fHeight ) fHeight = pvT2->y; 

				COLLISION::TRIANGLE TriangleColl;

				TriangleColl.vTri1 = *pvT0;
				TriangleColl.vTri2 = *pvT1;
				TriangleColl.vTri3 = *pvT2;

				TriangleColl.vCollision = D3DXVECTOR3(vCollisionPoint.x, fHeight, vCollisionPoint.z);

				TriList.push_back(TriangleColl);
			}

			/////////////////////////////////////////////////////////
		}
		else
		{
			if ( pAABBCur->pLeftChild )
				GetCollisionLineInTriListExceptY2 ( pAABBCur->pLeftChild, vNewP1, vNewP2, TriList );

			if ( pAABBCur->pRightChild )
				GetCollisionLineInTriListExceptY2 ( pAABBCur->pRightChild, vNewP1, vNewP2, TriList );
		}
	}
}
void DxOctreeMesh::GetCollisionLineInTriListExceptY ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ) // 주병진 추가 메소드
{
	if ( !m_pAABBTreeRoot )	return;

	GetCollisionLineInTriListExceptY2 ( m_pAABBTreeRoot, vStart, vEnd, TriList );
}

HRESULT DxOctreeMesh::CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, D3DXMATRIX *pmatComb )
{
	HRESULT hr;
	if ( !pMesh )		return E_FAIL;
	GASSERT(!(pMesh->GetOptions()&D3DXMESH_32BIT)&&"32bit index를 지원하지 않습니다.");
	
	ReleaseDxOctreeMesh	();

	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//
	m_dwNumVertices = pMesh->GetNumVertices ();
	m_dwNumFaces = pMesh->GetNumFaces ();
	m_dwFVF = pMesh->GetFVF ();

	DWORD dwVertexSize = D3DXGetFVFVertexSize ( m_dwFVF );

	hr = CreateDxOctreeMesh ( pd3dDevice, m_dwNumFaces, m_dwNumVertices, m_dwFVF );
	if ( FAILED(hr) )	return E_FAIL;

	BYTE*			pDestVertices;
	WORD*			pDestIndices;
	BYTE*			pSrcVertices;
	WORD*			pSrcIndices;

	//	Note : 버텍스 버퍼 카피
	//
	hr = pMesh->LockVertexBuffer ( 0L, (VOID**)&pSrcVertices );
	if ( FAILED(hr) )	return E_FAIL;

	hr = LockVertexBuffer ( (VOID**)&pDestVertices );
	if ( FAILED(hr) )	
	{
		pMesh->UnlockVertexBuffer ();
		return E_FAIL;
	}

	memcpy ( pDestVertices, pSrcVertices, m_dwNumVertices*dwVertexSize );

	if ( pmatComb )
	{
		for ( DWORD i=0; i<m_dwNumVertices; i++ )
		{
			D3DXVec3TransformCoord ( (D3DXVECTOR3*)pDestVertices, (D3DXVECTOR3*)pSrcVertices, pmatComb );
			pDestVertices += sizeof(BYTE)*dwVertexSize;
			pSrcVertices += sizeof(BYTE)*dwVertexSize;
		}
	}

	pMesh->UnlockVertexBuffer ();
	UnlockVertexBuffer();

	
	//	Note : 인덱스 버퍼 카피
	//
	hr = pMesh->LockIndexBuffer ( 0L, (VOID**)&pSrcIndices );
	if ( FAILED(hr) )	return E_FAIL;

	hr = LockIndexBuffer ( (VOID**)&pDestIndices );
	if ( FAILED(hr) )
	{
		pMesh->UnlockIndexBuffer ();
		return E_FAIL;
	}

	memcpy ( pDestIndices, pSrcIndices, m_dwNumFaces*3*sizeof(WORD) );

	pMesh->UnlockIndexBuffer ();
	UnlockIndexBuffer	();

	//	Note : Attribute Table 가져오기.
	//
	DWORD					AttribTableSize = 0;
	LPD3DXATTRIBUTERANGE	pSrcAttribTable = NULL;

	pMesh->GetAttributeTable ( NULL, &m_dwAttribTableSize );
	m_pAttribTable = new D3DXATTRIBUTERANGE[m_dwAttribTableSize];
	pMesh->GetAttributeTable ( m_pAttribTable, &m_dwAttribTableSize );

	return S_OK;
}

HRESULT DxOctreeMesh::CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxOctreeMesh* pMesh, D3DXMATRIX *pmatComb )
{
	HRESULT hr;
	if ( !pMesh )		return E_FAIL;
	
	ReleaseDxOctreeMesh	();

	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//
	m_dwNumVertices = pMesh->GetNumVertices ();
	m_dwNumFaces = pMesh->GetNumFaces ();
	m_dwFVF = pMesh->GetFVF ();

	DWORD dwVertexSize = D3DXGetFVFVertexSize ( m_dwFVF );

	hr = CreateDxOctreeMesh ( pd3dDevice, m_dwNumFaces, m_dwNumVertices, m_dwFVF );
	if ( FAILED(hr) )	return E_FAIL;

	BYTE*			pDestVertices;
	WORD*			pDestIndices;
	BYTE*			pSrcVertices;
	WORD*			pSrcIndices;

	//	Note : 버텍스 버퍼 카피
	//
	hr = pMesh->LockVertexBuffer ( 0L, (VOID**)&pSrcVertices );
	if ( FAILED(hr) )	return E_FAIL;

	hr = LockVertexBuffer ( (VOID**)&pDestVertices );
	if ( FAILED(hr) )
	{
		pMesh->UnlockVertexBuffer ();
		return E_FAIL;
	}

	memcpy ( pDestVertices, pSrcVertices, m_dwNumVertices*dwVertexSize );

	if ( pmatComb )
	{
		for ( DWORD i=0; i<m_dwNumVertices; i++ )
		{
			D3DXVec3TransformCoord ( (D3DXVECTOR3*)pDestVertices, (D3DXVECTOR3*)pSrcVertices, pmatComb );
			pDestVertices += sizeof(BYTE)*dwVertexSize;
			pSrcVertices += sizeof(BYTE)*dwVertexSize;
		}
	}

	pMesh->UnlockVertexBuffer ();
	UnlockVertexBuffer();

	
	//	Note : 인덱스 버퍼 카피
	//
	hr = pMesh->LockIndexBuffer ( 0L, (VOID**)&pSrcIndices );
	if ( FAILED(hr) )	return E_FAIL;

	hr = LockIndexBuffer ( (VOID**)&pDestIndices );
	if ( FAILED(hr) )
	{
		pMesh->UnlockIndexBuffer ();
		return E_FAIL;
	}

	memcpy ( pDestIndices, pSrcIndices, m_dwNumFaces*3*sizeof(WORD) );

	pMesh->UnlockIndexBuffer ();
	UnlockIndexBuffer	();

	//	Note : Attribute Table 가져오기.
	//
	DWORD					AttribTableSize = 0;
	LPD3DXATTRIBUTERANGE	pSrcAttribTable = NULL;

	pMesh->GetAttributeTable ( NULL, &m_dwAttribTableSize );
	m_pAttribTable = new D3DXATTRIBUTERANGE[m_dwAttribTableSize];
	pMesh->GetAttributeTable ( m_pAttribTable, &m_dwAttribTableSize );

	return S_OK;
}

HRESULT DxOcMeshes::CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pMesh, D3DXMATRIX *pmatComb )
{
	GASSERT(pMesh);
	HRESULT hr;

	ClearAll ();

	pOcMesh = new DxOctreeMesh;
	hr = pOcMesh->CloneMesh ( pd3dDevice, pMesh->m_pLocalMesh, pmatComb );
	if ( FAILED(hr) )	return hr;

	cMaterials = pMesh->m_cMaterials;
	pMaterials = new DXOCMATERIAL[cMaterials];

	for ( DWORD i=0; i<cMaterials; i++ )
	{
		StringCchCopy( pMaterials[i].szTexture, MAX_PATH, pMesh->m_strTextureFiles[i].GetString () );

        pMaterials[i].m_textureRes = TextureManager::GetInstance().LoadTexture(
            pMaterials[i].szTexture,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            0,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_1024,
            TEXTURE_LAND,
			false,
			FALSE );

		pMaterials[i].rgMaterial = pMesh->m_rgMaterials[i];
	}

	return S_OK;
}

HRESULT DxOcMeshes::CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxOcMeshes *pMesh, D3DXMATRIX *pmatComb )
{
	GASSERT(pMesh);
	HRESULT hr;

	ClearAll ();

	pOcMesh = new DxOctreeMesh;
	hr = pOcMesh->CloneMesh ( pd3dDevice, pMesh->pOcMesh, pmatComb );
	if ( FAILED(hr) )	return hr;

	cMaterials = pMesh->cMaterials;
	pMaterials = new DXOCMATERIAL[cMaterials];

	memcpy ( pMaterials, pMesh->pMaterials, sizeof(DXOCMATERIAL)*cMaterials );

	for ( DWORD i=0; i<cMaterials; i++ )
	{
        pMaterials[i].m_textureRes = TextureManager::GetInstance().LoadTexture(
            pMaterials[i].szTexture,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            0,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_1024,
            TEXTURE_LAND,
			false,
			FALSE );
	}

	return S_OK;
}

HRESULT DxOcMeshes::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pOcMesh || !pOcMesh->m_pVB )		return S_FALSE;

	HRESULT hr;
	hr = pd3dDevice->SetStreamSource ( 0, pOcMesh->m_pVB, 0, D3DXGetFVFVertexSize(pOcMesh->m_dwFVF) );
	if ( FAILED ( hr ) ) return E_FAIL;
	
	hr = pd3dDevice->SetFVF ( pOcMesh->m_dwFVF );
	if ( FAILED ( hr ) ) return E_FAIL;

	hr = pd3dDevice->SetIndices ( pOcMesh->m_pIB );
	if ( FAILED ( hr ) ) return E_FAIL;


	for ( DWORD i=0; i<pOcMesh->m_dwAttribTableSize; i++ )
	{
		DWORD dwMatID = pOcMesh->m_pAttribTable[i].AttribId;

		//	Note : 메터리얼, 텍스쳐 지정.
		pd3dDevice->SetMaterial ( &pMaterials[dwMatID].rgMaterial );
		pd3dDevice->SetTexture ( 0, pMaterials[dwMatID].m_textureRes.GetTexture() );

		pOcMesh->DrawSubset ( i, pd3dDevice );
	}

	return S_OK;
}

void DxOcMeshes::ClearAll ()
{
	if ( pOcMesh )
	{
		SAFE_DELETE(pOcMesh);
		pOcMesh = NULL;
	}
	if ( pMaterials )
	{
		SAFE_DELETE_ARRAY(pMaterials);
		pMaterials = NULL;
	}
}


BOOL	 DxOcMeshes::SaveFile ( sc::SerialFile &SFile )
{
	SFile << cMaterials;

    for( DWORD i=0; i<cMaterials; ++i )
        pMaterials[ i ].SaveFile( SFile );

    pOcMesh->SaveFile ( SFile );

	return TRUE;
}

BOOL	 DxOcMeshes::LoadFile ( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	SFile >> cMaterials;

	SAFE_DELETE_ARRAY(pMaterials);
	pMaterials = new DXOCMATERIAL[cMaterials];
    for( DWORD i=0; i<cMaterials; ++i )
        pMaterials[ i ].LoadFile( SFile );

	for ( DWORD i=0; i<cMaterials; i++ )
	{
        pMaterials[i].m_textureRes = TextureManager::GetInstance().LoadTexture(
            pMaterials[i].szTexture,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            0,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_1024,
            TEXTURE_LAND,
			false,
			FALSE );
	}

	SAFE_DELETE(pOcMesh);
	pOcMesh = new DxOctreeMesh;
	pOcMesh->LoadFile_OLD( SFile, pd3dDevice );

	return TRUE;
}


