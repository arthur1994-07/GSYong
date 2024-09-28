#include "pch.h"

#include "DxOctree.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

HRESULT DxOctree::MakeAABBTree ()
{
	HRESULT hr;

	if(m_bSubDivided)
	{
		if ( m_pOctreeNodes_0 )		m_pOctreeNodes_0->MakeAABBTree();
		if ( m_pOctreeNodes_1 )		m_pOctreeNodes_1->MakeAABBTree();
		if ( m_pOctreeNodes_2 )		m_pOctreeNodes_2->MakeAABBTree();
		if ( m_pOctreeNodes_3 )		m_pOctreeNodes_3->MakeAABBTree();
		if ( m_pOctreeNodes_4 )		m_pOctreeNodes_4->MakeAABBTree();
		if ( m_pOctreeNodes_5 )		m_pOctreeNodes_5->MakeAABBTree();
		if ( m_pOctreeNodes_6 )		m_pOctreeNodes_6->MakeAABBTree();
		if ( m_pOctreeNodes_7 )		m_pOctreeNodes_7->MakeAABBTree();
	}
	else
	{		
		DxMeshes *pmsMeshs;
		DxFrame	*pDxFrameCur = m_pDxFrameHead;

		while ( pDxFrameCur )
		{
			//	Note : AABB Tree 제작.
			//
			pmsMeshs = pDxFrameCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pDxOctreeMesh )
				{
					D3DXMATRIX matIdentity;
					D3DXMatrixIdentity( &matIdentity );

					hr = pmsMeshs->m_pDxOctreeMesh->MakeAABBTree ( matIdentity );
					if ( FAILED(hr) )	return hr;
				}

				pmsMeshs = pmsMeshs->m_pMeshNext;
			}

			pDxFrameCur = pDxFrameCur->pframeSibling;
		}
	}	

	return S_OK;
}

BOOL DxOctree::IsCollision_OCTREE ( const D3DXVECTOR3& vStartOrg, const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vCollision, 
									D3DXVECTOR3 &vNormal, BOOL &bCollision, LPDXFRAME &_pDxFrame, LPCSTR& szName, EMCOLLMODE emCullMode ) const
{
	D3DXVECTOR3 vP1=vPoint1, vP2=vPoint2;
	float fRatio;
	D3DXVECTOR3 vCullPos;

	//	Note : X,min
	//		N(-1,0,0) : -X + X,min = 0
	//		X < X,min is front
	//
	if ( vP1.x<m_vMin.x && vP2.x<m_vMin.x )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.x<m_vMin.x || vP2.x<m_vMin.x )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.x + m_vMin.x ) / ( -vP1.x + vP2.x );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.x < m_vMin.x )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : X,max
	//		N(1,0,0) : X - X,max = 0
	//		X > X,max is front
	//
	if ( vP1.x>m_vMax.x && vP2.x>m_vMax.x )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.x>m_vMax.x || vP2.x>m_vMax.x )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( vP1.x - m_vMax.x ) / ( vP1.x - vP2.x );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.x > m_vMax.x )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Y,min
	//		N(0,-1,0) : -Y + Y,min = 0
	//		Y < Y,min is front
	//
	if ( vP1.y<m_vMin.y && vP2.y<m_vMin.y )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.y<m_vMin.y || vP2.y<m_vMin.y )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.y + m_vMin.y ) / ( -vP1.y + vP2.y );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.y < m_vMin.y )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Y,max
	//		N(0,1,0) : Y - Y,max = 0
	//		Y > Y,max is front
	//
	if ( vP1.y>m_vMax.y && vP2.y>m_vMax.y )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.y>m_vMax.y || vP2.y>m_vMax.y )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( vP1.y - m_vMax.y ) / ( vP1.y - vP2.y );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.y > m_vMax.y )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Z,min
	//		N(0,0,-1) : -Z + Z,min = 0
	//		Z < Z,min is front
	//
	if ( vP1.z<m_vMin.z && vP2.z<m_vMin.z )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.z<m_vMin.z || vP2.z<m_vMin.z )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.z + m_vMin.z ) / ( -vP1.z + vP2.z );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.z < m_vMin.z )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Z,max
	//		N(0,0,-1) : Z - Z,max = 0
	//		Z > Z,max is front
	//
	if ( vP1.z > m_vMax.z && vP2.z > m_vMax.z )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( m_bSubDivided )
	{
		if (m_pOctreeNodes_0)	m_pOctreeNodes_0->IsCollision_OCTREE(vStartOrg, vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,emCullMode);
		if (m_pOctreeNodes_1)	m_pOctreeNodes_1->IsCollision_OCTREE(vStartOrg, vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,emCullMode);
		if (m_pOctreeNodes_2)	m_pOctreeNodes_2->IsCollision_OCTREE(vStartOrg, vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,emCullMode);
		if (m_pOctreeNodes_3)	m_pOctreeNodes_3->IsCollision_OCTREE(vStartOrg, vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,emCullMode);
		if (m_pOctreeNodes_4)	m_pOctreeNodes_4->IsCollision_OCTREE(vStartOrg, vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,emCullMode);
		if (m_pOctreeNodes_5)	m_pOctreeNodes_5->IsCollision_OCTREE(vStartOrg, vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,emCullMode);
		if (m_pOctreeNodes_6)	m_pOctreeNodes_6->IsCollision_OCTREE(vStartOrg, vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,emCullMode);
		if (m_pOctreeNodes_7)	m_pOctreeNodes_7->IsCollision_OCTREE(vStartOrg, vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,emCullMode);
	}
	else
	{		
		DxMeshes *pmsMeshs;
		DxFrame	*pDxFrameCur = m_pDxFrameHead;

		BOOL bCollisionCur=FALSE;

		while ( pDxFrameCur )
		{
			pmsMeshs = pDxFrameCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pDxOctreeMesh )
				{
					bCollisionCur=FALSE;
					DWORD dwAttribid=0;
					pmsMeshs->m_pDxOctreeMesh->IsCollision_OctreeMesh( vStartOrg, vP1, vP2,
						vCollision, bCollisionCur, vNormal, dwAttribid, emCullMode );

					if ( bCollisionCur )
					{
						bCollision = bCollisionCur;
						_pDxFrame = pDxFrameCur;

						szName = pmsMeshs->m_strTextureFiles[dwAttribid];
					}
				}

				pmsMeshs = pmsMeshs->m_pMeshNext;
			}

			pDxFrameCur = pDxFrameCur->pframeSibling;
		}
	}

_RETURN:
	return FALSE;
}

BOOL DxOctree::IsCollisionLineSimple ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, EMCOLLMODE emCullMode ) const
{
	D3DXVECTOR3 vP1=vStart, vP2=vEnd;
	float fRatio;
	D3DXVECTOR3 vCullPos;

	//	Note : X,min
	//		N(-1,0,0) : -X + X,min = 0
	//		X < X,min is front
	//
	if ( vP1.x<m_vMin.x && vP2.x<m_vMin.x )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.x<m_vMin.x || vP2.x<m_vMin.x )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.x + m_vMin.x ) / ( -vP1.x + vP2.x );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.x < m_vMin.x )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : X,max
	//		N(1,0,0) : X - X,max = 0
	//		X > X,max is front
	//
	if ( vP1.x>m_vMax.x && vP2.x>m_vMax.x )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.x>m_vMax.x || vP2.x>m_vMax.x )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( vP1.x - m_vMax.x ) / ( vP1.x - vP2.x );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.x > m_vMax.x )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Y,min
	//		N(0,-1,0) : -Y + Y,min = 0
	//		Y < Y,min is front
	//
	if ( vP1.y<m_vMin.y && vP2.y<m_vMin.y )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.y<m_vMin.y || vP2.y<m_vMin.y )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.y + m_vMin.y ) / ( -vP1.y + vP2.y );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.y < m_vMin.y )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Y,max
	//		N(0,1,0) : Y - Y,max = 0
	//		Y > Y,max is front
	//
	if ( vP1.y>m_vMax.y && vP2.y>m_vMax.y )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.y>m_vMax.y || vP2.y>m_vMax.y )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( vP1.y - m_vMax.y ) / ( vP1.y - vP2.y );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.y > m_vMax.y )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Z,min
	//		N(0,0,-1) : -Z + Z,min = 0
	//		Z < Z,min is front
	//
	if ( vP1.z<m_vMin.z && vP2.z<m_vMin.z )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.z<m_vMin.z || vP2.z<m_vMin.z )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.z + m_vMin.z ) / ( -vP1.z + vP2.z );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.z < m_vMin.z )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Z,max
	//		N(0,0,-1) : Z - Z,max = 0
	//		Z > Z,max is front
	//
	if ( vP1.z > m_vMax.z && vP2.z > m_vMax.z )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( m_bSubDivided )
	{
		for ( DWORD i=0; i<8; ++i )
		{
			if ( m_pOctreeNodes[i] && m_pOctreeNodes[i]->IsCollisionLineSimple(vP1,vP2,emCullMode) )
				return TRUE;
		}
	}
	else
	{		
		DxMeshes *pmsMeshs;
		DxFrame	*pDxFrameCur = m_pDxFrameHead;

		while ( pDxFrameCur )
		{
			pmsMeshs = pDxFrameCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pDxOctreeMesh )
				{
					if ( pmsMeshs->m_pDxOctreeMesh->IsCollisionLineSimple(vP1,vP2,emCullMode) )
						return TRUE;
				}

				pmsMeshs = pmsMeshs->m_pMeshNext;
			}

			pDxFrameCur = pDxFrameCur->pframeSibling;
		}
	}

_RETURN:
	return FALSE;
}

void DxOctree::GetCollisionLineInTriListExceptY ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ) // 주병진
{
	D3DXVECTOR3 vP1=vStart, vP2=vEnd;
	float fRatio;
	D3DXVECTOR3 vCullPos;

	//	Note : X,min
	//		N(-1,0,0) : -X + X,min = 0
	//		X < X,min is front
	//
	if ( vP1.x<m_vMin.x && vP2.x<m_vMin.x )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.x<m_vMin.x || vP2.x<m_vMin.x )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.x + m_vMin.x ) / ( -vP1.x + vP2.x );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.x < m_vMin.x )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : X,max
	//		N(1,0,0) : X - X,max = 0
	//		X > X,max is front
	//
	if ( vP1.x>m_vMax.x && vP2.x>m_vMax.x )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.x>m_vMax.x || vP2.x>m_vMax.x )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( vP1.x - m_vMax.x ) / ( vP1.x - vP2.x );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.x > m_vMax.x )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Z,min
	//		N(0,0,-1) : -Z + Z,min = 0
	//		Z < Z,min is front
	//
	if ( vP1.z<m_vMin.z && vP2.z<m_vMin.z )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.z<m_vMin.z || vP2.z<m_vMin.z )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.z + m_vMin.z ) / ( -vP1.z + vP2.z );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.z < m_vMin.z )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Z,max
	//		N(0,0,-1) : Z - Z,max = 0
	//		Z > Z,max is front
	//
	if ( vP1.z > m_vMax.z && vP2.z > m_vMax.z )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( m_bSubDivided )
	{
		if (m_pOctreeNodes_0)	m_pOctreeNodes_0->GetCollisionLineInTriListExceptY(vP1,vP2,TriList);
		if (m_pOctreeNodes_1)	m_pOctreeNodes_1->GetCollisionLineInTriListExceptY(vP1,vP2,TriList);
		if (m_pOctreeNodes_2)	m_pOctreeNodes_2->GetCollisionLineInTriListExceptY(vP1,vP2,TriList);
		if (m_pOctreeNodes_3)	m_pOctreeNodes_3->GetCollisionLineInTriListExceptY(vP1,vP2,TriList);
		if (m_pOctreeNodes_4)	m_pOctreeNodes_4->GetCollisionLineInTriListExceptY(vP1,vP2,TriList);
		if (m_pOctreeNodes_5)	m_pOctreeNodes_5->GetCollisionLineInTriListExceptY(vP1,vP2,TriList);
		if (m_pOctreeNodes_6)	m_pOctreeNodes_6->GetCollisionLineInTriListExceptY(vP1,vP2,TriList);
		if (m_pOctreeNodes_7)	m_pOctreeNodes_7->GetCollisionLineInTriListExceptY(vP1,vP2,TriList);
	}
	else
	{		
		DxMeshes  *pmsMeshs;
  		DxFrame   *pDxFrameCur = m_pDxFrameHead;

		D3DXVECTOR3 vCollision(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 vNormal(0.0f, 0.0f, 0.0f);

		BOOL bCollisionCur=FALSE;

		while ( pDxFrameCur )
		{
 			pmsMeshs = pDxFrameCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pDxOctreeMesh )
				{
					pmsMeshs->m_pDxOctreeMesh->GetCollisionLineInTriListExceptY( vP1, vP2, TriList );
				}

				pmsMeshs = pmsMeshs->m_pMeshNext;
			}

			pDxFrameCur = pDxFrameCur->pframeSibling;
		}
	}

_RETURN:
	return;
}
