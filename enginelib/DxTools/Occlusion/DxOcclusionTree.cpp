#include "pch.h"

#include "../../DxLand/DxPiece/NSPieceQuickSort.h"
#include "../../DxLand/DxLandMan.h"
#include "../DxViewPort.h"
#include "../EditMeshs.h"
#include "../Collision.h"
#include "./NSOcclusionOctree.h"
#include "./DxOcclusionQuery.h"

#include "DxOcclusionTree.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

struct OCC_LINE
{
	D3DXVECTOR3 vStart;
	D3DXVECTOR3 vEnd;
	DWORD		dwCollMode;
};
std::vector<OCC_LINE>	g_vecCollLine;

DxOcclusionTree::DxOcclusionTree()
	: m_nID(-1)
	, m_vMax(1.f,1.f,1.f)
	, m_vMin(0.f,0.f,0.f)
	, m_pLeftChild(NULL)
	, m_pRightChild(NULL)
{
}

DxOcclusionTree::~DxOcclusionTree()
{
	SAFE_DELETE( m_pLeftChild );
	SAFE_DELETE( m_pRightChild );
}

void DxOcclusionTree::LastBoxNum( DWORD& dwBoxNum, DWORD& dwCount )
{
	if ( !m_vecPoint.empty() )
	{
		dwCount += m_vecPoint.size();
		dwBoxNum += m_vecPoint.size();
	}

	if ( m_pLeftChild )
		m_pLeftChild->LastBoxNum( dwBoxNum, dwCount );
	if ( m_pRightChild )
		m_pRightChild->LastBoxNum( dwBoxNum, dwCount );
}

void DxOcclusionTree::RenderBox( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos )
{
	if ( m_nID != -1 )
	{
		if ( COLLISION::IsWithInPoint ( m_vMax, m_vMin, vPos ) )
		{
			for ( DWORD i=0; i<g_vecCollLine.size(); ++i )
			{
				if ( g_vecCollLine[i].dwCollMode == D3DCULL_NONE )
				{
					EDITMESHS::RENDERLINE( pd3dDevice, g_vecCollLine[i].vStart, g_vecCollLine[i].vEnd, 0xffff0000 );
					EDITMESHS::RENDERBOX( g_vecCollLine[i].vStart, 1.f, 0xffff0000 );
				}
				else
				{
					EDITMESHS::RENDERLINE( pd3dDevice, g_vecCollLine[i].vStart, g_vecCollLine[i].vEnd );
					EDITMESHS::RENDERBOX( g_vecCollLine[i].vStart, 1.f );
				}
			}

			D3DXVECTOR3 vMax(m_vMax);
			D3DXVECTOR3 vMin(m_vMin);
			vMax += D3DXVECTOR3(0.5f,0.5f,0.5f);
			vMin -= D3DXVECTOR3(0.5f,0.5f,0.5f);
			EDITMESHS::RENDERAABB( pd3dDevice, vMax, vMin, 0xffff0000 );
		}
		else
		{
			EDITMESHS::RENDERAABB( pd3dDevice, m_vMax, m_vMin );
		}
	}

	//EDITMESHS::RENDERAABB( pd3dDevice, m_vMax, m_vMin );

	if ( m_pLeftChild )
		m_pLeftChild->RenderBox( pd3dDevice, vPos );
	if ( m_pRightChild )
		m_pRightChild->RenderBox( pd3dDevice, vPos );
}

void DxOcclusionTree::Bake( LPDIRECT3DDEVICEQ pd3dDevice, 
						   DxLandMan* pDxLandMan, 
						   int& nCountCUR,
						   BOOL bFieldOcclusion,
						   BOOL bPieceOcclusion,
						   BOOL bCharOcclusion )
{
	BakeOneCell
	( 
		pd3dDevice, 
		pDxLandMan, 
		nCountCUR,
		bFieldOcclusion,
		bPieceOcclusion,
		bCharOcclusion,
		256.f
	);

	if ( m_pLeftChild )
		m_pLeftChild->Bake( pd3dDevice, pDxLandMan, nCountCUR, bFieldOcclusion, bPieceOcclusion, bCharOcclusion );
	if ( m_pRightChild )
		m_pRightChild->Bake( pd3dDevice, pDxLandMan, nCountCUR, bFieldOcclusion, bPieceOcclusion, bCharOcclusion );
}

void DxOcclusionTree::BakePosition( LPDIRECT3DDEVICEQ pd3dDevice, 
									DxLandMan* pDxLandMan, 
									const D3DXVECTOR3& vPos,
									BOOL bFieldOcclusion,
									BOOL bPieceOcclusion,
									BOOL bCharOcclusion )
{
	if ( !COLLISION::IsWithInPoint ( m_vMax, m_vMin, vPos ) )
		return;

	if ( m_nID != -1 )
	{
		int nCount(0);
		BakeOneCell
		( 
			pd3dDevice, 
			pDxLandMan, 
			nCount,
			bFieldOcclusion,
			bPieceOcclusion,
			bCharOcclusion,
			256.f
		);
	}
	
	if ( m_pLeftChild )
		m_pLeftChild->BakePosition( pd3dDevice, pDxLandMan, vPos, bFieldOcclusion, bPieceOcclusion, bCharOcclusion );
	if ( m_pRightChild )
		m_pRightChild->BakePosition( pd3dDevice, pDxLandMan, vPos, bFieldOcclusion, bPieceOcclusion, bCharOcclusion );
}

void DxOcclusionTree::BakeOneCell( LPDIRECT3DDEVICEQ pd3dDevice, 
									 DxLandMan* pDxLandMan, 
									 int& nCountCUR,
									 BOOL bFieldOcclusion,
									 BOOL bPieceOcclusion,
									 BOOL bCharOcclusion,
									 float fSurfaceSize )
{
	NSOcclusionExceptIndex::Init();

	// ���� 10 ī�޶� �ִ�Ÿ� 200
	D3DXMATRIX matRotate;
	D3DXMATRIX matRotateX;
	D3DXMATRIX matRotateY;
	D3DXVECTOR3 vCameraPos;
	D3DXVECTOR3 vDir[4];
	D3DXVECTOR3 vDir10[12];
	D3DXVECTOR3 vDir15[24];
	D3DXVECTOR3 vDir225[12];
	D3DXVECTOR3 vDir45[4];
	D3DXVECTOR3 vDirDown[4];
	D3DXVECTOR3 vDir36[36];
	D3DXVECTOR3 vDirSrc(0.f,0.f,200.f);

	for ( DWORD i=0; i<4; ++i )
	{
		D3DXMatrixRotationY( &matRotate, D3DX_PI/2*i );
		D3DXVec3TransformCoord( &vDir[i], &vDirSrc, &matRotate );
	}

	for ( DWORD i=0; i<4; ++i )
	{
		D3DXMatrixRotationX( &matRotateX, D3DX_PI/4 );	// -45
		D3DXMatrixRotationY( &matRotateY, D3DX_PI/2*i + (D3DX_PI/3) );
		D3DXMatrixMultiply( &matRotate, &matRotateX, &matRotateY );
		D3DXVec3TransformCoord( &vDirDown[i], &vDirSrc, &matRotate );
	}

	for ( DWORD i=0; i<12; ++i )
	{
		D3DXMatrixRotationX( &matRotateX, -D3DX_PI/18 );	// 10
		D3DXMatrixRotationY( &matRotateY, D3DX_PI/6*i + (D3DX_PI/24) );
		D3DXMatrixMultiply( &matRotate, &matRotateX, &matRotateY );
		D3DXVec3TransformCoord( &vDir10[i], &vDirSrc, &matRotate );
	}

	for ( DWORD i=0; i<24; ++i )
	{
		D3DXMatrixRotationX( &matRotateX, -D3DX_PI/12 );	// 15
		D3DXMatrixRotationY( &matRotateY, D3DX_PI/24*2*i );
		D3DXMatrixMultiply( &matRotate, &matRotateX, &matRotateY );
		D3DXVec3TransformCoord( &vDir15[i], &vDirSrc, &matRotate );
	}

	for ( DWORD i=0; i<12; ++i )
	{
		D3DXMatrixRotationX( &matRotateX, -D3DX_PI/8 );	// 22.5
		D3DXMatrixRotationY( &matRotateY, D3DX_PI/6*i + (D3DX_PI/24) + (D3DX_PI/12) );
		D3DXMatrixMultiply( &matRotate, &matRotateX, &matRotateY );
		D3DXVec3TransformCoord( &vDir225[i], &vDirSrc, &matRotate );
	}

	for ( DWORD i=0; i<4; ++i )
	{
		D3DXMatrixRotationX( &matRotateX, -D3DX_PI/4 );
		D3DXMatrixRotationY( &matRotateY, D3DX_PI/2*i + (D3DX_PI/6) );
		D3DXMatrixMultiply( &matRotate, &matRotateX, &matRotateY );
		D3DXVec3TransformCoord( &vDir45[i], &vDirSrc, &matRotate );
	}

	for ( DWORD i=0; i<36; ++i )
	{
		D3DXMatrixRotationX( &matRotateX, D3DX_PI/4 );
		D3DXMatrixRotationY( &matRotateY, D3DX_PI/36*i );
		D3DXMatrixMultiply( &matRotate, &matRotateX, &matRotateY );
		D3DXVec3TransformCoord( &vDir36[i], &vDirSrc, &matRotate );
	}

	g_vecCollLine.clear();

	// �� Point ���� ī�޶� ������ ����
	RenderIndex_VECTOR sRenderIndex;
	for( DWORD i=0; i<m_vecPoint.size(); ++i )
	{
		++nCountCUR;

		// ������ Bake�� ���ٸ� ������ ���ϵ��� �۾��Ѵ�.
		if ( DxOcclusionQuery::g_bForceDisable )
			continue;

		D3DXVECTOR3 vLookatPt = m_vecPoint[i];
		vLookatPt.y += 10.f;

		std::vector<D3DXVECTOR3> vecCameraFrom;	// ī�޶� ��ġ

		vCameraPos = vLookatPt;
		vCameraPos.y += 200.f;
		vCameraPos.z += 1.f;
		vecCameraFrom.push_back( vCameraPos );

		// �ؿ��� �ٶ󺸱� ����
		for ( DWORD j=0; j<4; ++j )
		{
			vCameraPos = vLookatPt + vDirDown[j];
			vecCameraFrom.push_back( vCameraPos );
		}

		// 10 ����
		for ( DWORD j=0; j<12; ++j )
		{
			vCameraPos = vLookatPt + vDir10[j];
			vecCameraFrom.push_back( vCameraPos );
		}

		// 15 ����
		for ( DWORD j=0; j<24; ++j )
		{
			vCameraPos = vLookatPt + vDir15[j];
			vecCameraFrom.push_back( vCameraPos );
		}

		// 22.5 ����
		for ( DWORD j=0; j<12; ++j )
		{
			vCameraPos = vLookatPt + vDir225[j];
			vecCameraFrom.push_back( vCameraPos );
		}

		//// 36 ����
		//for ( DWORD j=0; j<36; ++j )
		//{
		//	vCameraPos = vLookatPt + vDir36[j];
		//	vecCameraFrom.push_back( vCameraPos );
		//}

		//// ��� ����
		//for ( DWORD j=0; j<4; ++j )
		//{
		//	vCameraPos = vLookatPt + vDir[j];
		//	vecCameraFrom.push_back( vCameraPos );
		//}

		//// 45 ����
		//for ( DWORD j=0; j<4; ++j )
		//{
		//	vCameraPos = vLookatPt + vDir45[j];
		//	vecCameraFrom.push_back( vCameraPos );
		//}

		// OcclusionQuery�� ����Ͽ� �ɸ��� PieceList�� ��������.
		for ( size_t x=0; x<vecCameraFrom.size(); ++x )
		{
			// �⺻�� �����θ� ������ ����.
			float fFOV = D3DX_PI/4;
			float fSurfaceWidth = fSurfaceSize*2.f;
			if ( x>=0 && x<=4 )
			{
				// ���� ���� ���� �Ʒ��� ������ ������������ ����.
				// �̷��� �ϴ� ������ ������ ȭ�� ������ 1:1 �� �ƴϴ�. 2:1 �� ���� �� �ִ�.
				// �׷��Ƿ� ������ ������ �а� ������.
				// ������ ���Ϸ� ������ �ø��ٸ� ��¿ �� ����.!! �׳� �׷��� �� �ø����� ���°� ���ڴ�.
				fFOV = D3DX_PI/2;
				fSurfaceWidth = fSurfaceSize;
			}

			//// ������ �浹ó��.
			D3DXVECTOR3 vEnd(vecCameraFrom[x]);
			D3DXVECTOR3 vCollision(0.f,0.f,0.f);
			BOOL bCollision(FALSE);

			D3DXVECTOR3 vCamDir = vEnd - vLookatPt;
			vCollision = vEnd;

			pDxLandMan->IsCollisionNEW( vLookatPt, vEnd, vCollision, bCollision, EMCC_CULL_CCW_EXCEPT_ALPHATEX, TRUE, FALSE );

			// �浹�̸�, ��� �������� �ϰ�, ī�޶� �ִ�ġ�� �ؼ� ������ �Ѵ�.
			if ( bCollision )
			{
				BakeOneCell_Detail
				(
					pd3dDevice, 
					pDxLandMan, 
					vEnd,
					vLookatPt,
					bFieldOcclusion,
					bPieceOcclusion,
					bCharOcclusion,
					fFOV,
					fSurfaceWidth,
					fSurfaceSize,
					sRenderIndex,
					D3DCULL_NONE 
				);
			}

			// �浹�� �ϰ� ���ϰ� �۾��ϴ� ��
			{
				D3DXVECTOR3 vCamDir = vCollision - vLookatPt;

				D3DXVECTOR3 vCollTemp;
				BOOL bCollTemp(FALSE);
				for ( int z=0; z<5; ++z )
				{
					// ī�޶� ������ �ٽ� �˻��Ѵ�.
					vCollision = vLookatPt + vCamDir * ((5.f-z)/5.f );

					D3DXVECTOR3 vUpVec( 0.f, 1.f, 0.f );
					DxViewPort::GetInstance().SetViewTrans ( vCollision, vLookatPt, vUpVec );
					DxViewPort::GetInstance().SetProjection ( fFOV, fSurfaceWidth, fSurfaceSize, 2.f, 1000.f );
					DxViewPort::GetInstance().ComputeClipVolume();

					bCollTemp = FALSE;
					for ( int y=0; y<4; ++y )
					{
						const D3DXVECTOR3& vPos = DxViewPort::GetInstance().GetViewPortPoint( y );
						pDxLandMan->IsCollisionNEW( vLookatPt, vPos, vCollTemp, bCollTemp, EMCC_CULL_NONE_EXCEPT_ALPHATEX, TRUE, FALSE );
						if ( bCollTemp )
						{
							// �ѹ��̶� �浹�Ǹ� �˻� �׸��Ѵ�.
							break;
						}
					}

					// �浹���� �ʾҴٸ� OK
					if ( !bCollTemp )
					{
						BakeOneCell_Detail
						(
							pd3dDevice, 
							pDxLandMan, 
							vCollision,
							vLookatPt,
							bFieldOcclusion,
							bPieceOcclusion,
							bCharOcclusion,
							fFOV,
							fSurfaceWidth,
							fSurfaceSize,
							sRenderIndex,
							D3DCULL_NONE 
						);

						break;
					}
				}
			}

			NSOcclusionExceptIndex::SetExceptIndex( sRenderIndex.m_vecOctree, sRenderIndex.m_vecPiece, sRenderIndex.m_vecChar[0] );
		}
	}

	// ���ü� �Ǻ��� �ֵ��� set �� �־���´�.
	{
		m_sRenderIndex.m_setPiece.clear();
		m_sRenderIndex.m_setOctree.clear();
		for ( DWORD i=0; i<sRenderIndex.m_vecPiece.size(); ++i )
		{
			m_sRenderIndex.m_setPiece.insert( sRenderIndex.m_vecPiece[i] );
		}
		for ( DWORD i=0; i<sRenderIndex.m_vecOctree.size(); ++i )
		{
			m_sRenderIndex.m_setOctree.insert( sRenderIndex.m_vecOctree[i] );
		}

		for ( int ntype=0; ntype<NSOcclusionCharZone::HEIGHT_TYPE_SIZE; ++ntype )
		{
			m_sRenderIndex.m_setChar[ntype].clear();
			for ( DWORD i=0; i<sRenderIndex.m_vecChar[ntype].size(); ++i )
			{
				DWORD dwID = sRenderIndex.m_vecChar[ntype][i];

				BOOL bInsert(TRUE);
				for ( int z=0; z<ntype; ++z )
				{
					SET_DWORD_ITER iter = m_sRenderIndex.m_setChar[z].find( dwID );
					if ( iter!=m_sRenderIndex.m_setChar[z].end() )
					{
						bInsert = FALSE;
						break;
					}
				}

				if ( bInsert )
				{
					m_sRenderIndex.m_setChar[ntype].insert( dwID );
				}
			}
		}
	}
}

void DxOcclusionTree::BakeOneCell_Detail( LPDIRECT3DDEVICEQ pd3dDevice, 
										DxLandMan* pDxLandMan, 
										const D3DXVECTOR3& vFromPt,		// Coliision
										const D3DXVECTOR3& vLookatPt,	// vLookatPt
										BOOL bFieldOcclusion,
										BOOL bPieceOcclusion,
										BOOL bCharOcclusion,
										float fFOV,
										float fSurfaceWidth,
										float fSurfaceHeight,
										RenderIndex_VECTOR& sRenderIndex,
										DWORD dwCullMode )
{
	// �������� �������� üũ�ϴ� ��
	{
		BOOL bCollTemp(FALSE);
		D3DXVECTOR3 vCollTemp(0.f,0.f,0.f);
		D3DXVECTOR3 vStart = vFromPt + D3DXVECTOR3(0.f,1.f,0.f);
		D3DXVECTOR3 vEnd = vFromPt + D3DXVECTOR3(0.f,-10000.f,0.f);

		// �ٴ����� �浹ó���� �ߴµ� �浹�Ǵ� ���� ���ٸ� �۾��ϸ� �ȵȴ�.
		// �������� ���� ��Ȳ�̴�.
		pDxLandMan->IsCollisionNEW( vStart, vEnd, vCollTemp, bCollTemp, EMCC_CULL_CCW_EXCEPT_WATER, TRUE, FALSE );
		if ( !bCollTemp )
		{
			return;
		}
	}

	if( DxOcclusionQuery::BeginScene() )
	{
		DxOcclusionQuery::ClearSurface();
		DxOcclusionQuery::CaptureState();

		// Sort �� �˷��༭ LOD ����� ���� ���ϵ��� �Ѵ�.
		BOOL bViewAlphaLOD_BackUp = NSPieceQuickSort::g_bViewAlphaLOD;
		NSPieceQuickSort::g_bViewAlphaLOD = FALSE;

		// Note : View Port�� �����Ѵ�.
		D3DXVECTOR3 vUpVec( 0.f, 1.f, 0.f );
		DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );

		// Note : Projection�� �����Ѵ�.
		float fNearPlane = 2.f;	// 5.f
		float fFarPlane = DxFogMan::GetInstance().GetGameFogHighEnd( pDxLandMan->GetFogProperty() );
		DxViewPort::GetInstance().SetProjection ( fFOV, fSurfaceWidth, fSurfaceHeight, fNearPlane, fFarPlane );

		DxViewPort::GetInstance().ComputeClipVolume();

		CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume();

		//	Note : ���� Ʈ������ �ʱ�ȭ.
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

		pDxLandMan->ResetOcclusionQuery();
		pDxLandMan->PieceSort( sCV, vFromPt, vLookatPt );
		pDxLandMan->Render_Query( pd3dDevice, bFieldOcclusion, bPieceOcclusion, dwCullMode );

		pDxLandMan->FindOcclusionRenderIndex( pd3dDevice, sRenderIndex.m_vecOctree, sRenderIndex.m_vecPiece );

		// CharZone�� ���ο��� Index�� ã���� �Ѵ�.
		if ( bCharOcclusion )
		{
			NSOcclusionCharZone::InsertSort( sCV );
			for ( int ntype=0; ntype<NSOcclusionCharZone::HEIGHT_TYPE_SIZE; ++ntype )
			{
				NSOcclusionCharZone::Render_Query( sCV, pd3dDevice, ntype, sRenderIndex.m_vecChar[ntype] );
			}
		}

		NSPieceQuickSort::g_bViewAlphaLOD = bViewAlphaLOD_BackUp;

		DxOcclusionQuery::RollbackState();
		DxOcclusionQuery::EndScene();
	}

	OCC_LINE sLine;
	sLine.vStart = vFromPt;
	sLine.vEnd = vLookatPt;
	sLine.dwCollMode = dwCullMode;
	g_vecCollLine.push_back( sLine );
}

void DxOcclusionTree::ReInsertPoint( const std::vector<D3DXVECTOR3>& vecPoint, const std::vector<D3DXVECTOR3>& vecPointChar )
{
	if ( m_nID != -1 )
	{
		m_vecPoint.clear();
		for ( DWORD i=0; i<vecPoint.size(); ++i )
		{
			if ( COLLISION::IsWithInPoint ( m_vMax, m_vMin, vecPoint[i] ) )
			{
				m_vecPoint.push_back( vecPoint[i] );
			}
		}

		m_vecPointChar.clear();
		for ( DWORD i=0; i<vecPointChar.size(); ++i )
		{
			if ( COLLISION::IsWithInPoint ( m_vMax, m_vMin, vecPointChar[i] ) )
			{
				m_vecPointChar.push_back( vecPointChar[i] );
			}
		}		
	}

	if ( m_pLeftChild )
		m_pLeftChild->ReInsertPoint( vecPoint, vecPointChar );
	if ( m_pRightChild )
		m_pRightChild->ReInsertPoint( vecPoint, vecPointChar );
}

void DxOcclusionTree::GetRenderIndexPos( const D3DXVECTOR3& vPos, VEC_DWORD& vecPieceIndex, VEC_DWORD& vecOctreeIndex, VEC_DWORD vecCharIndex[] )
{
	if ( !COLLISION::IsWithInPoint ( m_vMax, m_vMin, vPos ) )
		return;

	SET_DWORD_ITER iter;
	if ( !m_sRenderIndex.m_setPiece.empty() )
	{
		iter = m_sRenderIndex.m_setPiece.begin();
		for ( ; iter!=m_sRenderIndex.m_setPiece.end(); ++iter )
		{
			vecPieceIndex.push_back( (*iter) );
		}
	}
	if ( !m_sRenderIndex.m_setOctree.empty() )
	{
		iter = m_sRenderIndex.m_setOctree.begin();
		for ( ; iter!=m_sRenderIndex.m_setOctree.end(); ++iter )
		{
			vecOctreeIndex.push_back( (*iter) );
		}
	}

	for ( DWORD i=0; i<NSOcclusionCharZone::HEIGHT_TYPE_SIZE; ++i )
	{
		if ( !m_sRenderIndex.m_setChar[i].empty() )
		{
			iter = m_sRenderIndex.m_setChar[i].begin();
			for ( ; iter!=m_sRenderIndex.m_setChar[i].end(); ++iter )
			{
				vecCharIndex[i].push_back( (*iter) );
			}
		}
	}

	if ( m_pLeftChild )
		m_pLeftChild->GetRenderIndexPos( vPos, vecPieceIndex, vecOctreeIndex, &vecCharIndex[0] );
	if ( m_pRightChild )
		m_pRightChild->GetRenderIndexPos( vPos, vecPieceIndex, vecOctreeIndex, &vecCharIndex[0] );
}

//void DxOcclusionTree::GetCollCharZoneIndex( const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, VEC_DWORD& vecCollCharZone )
//{
//	if ( !COLLISION::IsCollisionAABBToAABB ( m_vMax, m_vMin, vMax, vMin ) )
//		return;
//
//	if ( m_nID != -1 )
//		vecCollCharZone.push_back( m_nID );
//
//	if ( m_pLeftChild )
//		m_pLeftChild->GetCollCharZoneIndex( vMax, vMin, vecCollCharZone );
//	if ( m_pRightChild )
//		m_pRightChild->GetCollCharZoneIndex( vMax, vMin, vecCollCharZone );
//}

// 
void DxOcclusionTree::SetOcclusionCharZone( LPDIRECT3DDEVICEQ pd3dDevice, int& nID_Count )
{
	if ( !m_vecPointChar.empty() )
	{
		m_nID = nID_Count++;
		NSOcclusionCharZone::Insert( pd3dDevice, m_nID, m_vecPointChar );
	}

	if ( m_pLeftChild )
		m_pLeftChild->SetOcclusionCharZone( pd3dDevice, nID_Count );
	if ( m_pRightChild )
		m_pRightChild->SetOcclusionCharZone( pd3dDevice, nID_Count );
}

void DxOcclusionTree::GetRenderIndex( RenderIndex& sRenderIndex )
{
	sRenderIndex = m_sRenderIndex;
}

void DxOcclusionTree::SetSameRenderIndex( RenderIndex& sRenderIndex )
{
	RenderIndex sRenderIndexSame;

	SET_DWORD_ITER iter;
	iter = m_sRenderIndex.m_setPiece.begin();
	for ( ; iter!=m_sRenderIndex.m_setPiece.end(); ++iter )
	{
		SET_DWORD_ITER iter_f = sRenderIndex.m_setPiece.find( (*iter) );
		if ( iter_f != sRenderIndex.m_setPiece.end() )
		{
			sRenderIndexSame.m_setPiece.insert( (*iter_f) );
		}
	}

	iter = m_sRenderIndex.m_setOctree.begin();
	for ( ; iter!=m_sRenderIndex.m_setOctree.end(); ++iter )
	{
		SET_DWORD_ITER iter_f = sRenderIndex.m_setOctree.find( (*iter) );
		if ( iter_f != sRenderIndex.m_setOctree.end() )
		{
			sRenderIndexSame.m_setOctree.insert( (*iter_f) );
		}
	}

	for ( DWORD i=0; i<NSOcclusionCharZone::HEIGHT_TYPE_SIZE; ++i )
	{
		iter = m_sRenderIndex.m_setChar[i].begin();
		for ( ; iter!=m_sRenderIndex.m_setChar[i].end(); ++iter )
		{
			SET_DWORD_ITER iter_f = sRenderIndex.m_setChar[i].find( (*iter) );
			if ( iter_f != sRenderIndex.m_setChar[i].end() )
			{
				sRenderIndexSame.m_setChar[i].insert( (*iter_f) );
			}
		}
	}

	sRenderIndex = sRenderIndexSame;
}

void DxOcclusionTree::DeleteIndex( RenderIndex& sRenderIndex )
{
	SET_DWORD_ITER iter;
	iter = sRenderIndex.m_setPiece.begin();
	for ( ; iter!=sRenderIndex.m_setPiece.end(); ++iter )
	{
		SET_DWORD_ITER iter_f = m_sRenderIndex.m_setPiece.find( (*iter) );
		if ( iter_f != m_sRenderIndex.m_setPiece.end() )
		{
			m_sRenderIndex.m_setPiece.erase( iter_f );
		}
	}

	iter = sRenderIndex.m_setOctree.begin();
	for ( ; iter!=sRenderIndex.m_setOctree.end(); ++iter )
	{
		SET_DWORD_ITER iter_f = m_sRenderIndex.m_setOctree.find( (*iter) );
		if ( iter_f != m_sRenderIndex.m_setOctree.end() )
		{
			m_sRenderIndex.m_setOctree.erase( iter_f );
		}
	}

	for ( DWORD i=0; i<NSOcclusionCharZone::HEIGHT_TYPE_SIZE; ++i )
	{
		iter = sRenderIndex.m_setChar[i].begin();
		for ( ; iter!=sRenderIndex.m_setChar[i].end(); ++iter )
		{
			SET_DWORD_ITER iter_f = m_sRenderIndex.m_setChar[i].find( (*iter) );
			if ( iter_f != m_sRenderIndex.m_setChar[i].end() )
			{
				m_sRenderIndex.m_setChar[i].erase( iter_f );
			}
		}
	}
}

// �����͸� �����Ѵ�.
void DxOcclusionTree::CompressData()
{
	BOOL bCompressed(TRUE);
	while ( bCompressed )
	{
		bCompressed = FALSE;
		CompressDataDetail( bCompressed );
	}
}

void DxOcclusionTree::CompressDataDetail( BOOL& bCompressed )
{
	RenderIndex sRenderIndex;

	if ( m_pLeftChild && m_pRightChild )
	{
		m_pLeftChild->GetRenderIndex( sRenderIndex );
		m_pRightChild->SetSameRenderIndex( sRenderIndex );
	}	

	if ( sRenderIndex.IsHaveData() )
	{
		bCompressed = TRUE;

		m_sRenderIndex = sRenderIndex;

		m_pLeftChild->DeleteIndex( sRenderIndex );
		m_pRightChild->DeleteIndex( sRenderIndex );
	}

	if ( m_pLeftChild )
		m_pLeftChild->CompressDataDetail( bCompressed );
	if ( m_pRightChild )
		m_pRightChild->CompressDataDetail( bCompressed );
}

void DxOcclusionTree::RenderIndex::CleanUp()
{
	m_setPiece.clear();
	m_setOctree.clear();

	for ( DWORD i=0; i<NSOcclusionCharZone::HEIGHT_TYPE_SIZE; ++i )
	{
		m_setChar[i].clear();
	}
}

BOOL DxOcclusionTree::RenderIndex::IsHaveData()
{
	if ( !m_setPiece.empty() )
		return TRUE;

	if ( !m_setOctree.empty() )
		return TRUE;

	for ( DWORD i=0; i<NSOcclusionCharZone::HEIGHT_TYPE_SIZE; ++i )
	{
		if ( !m_setChar[i].empty() )
			return TRUE;
	}

	return FALSE;
}

// ���൥���͸� �����Ѵ�.
void DxOcclusionTree::DeCompressData()
{
	if ( m_pLeftChild )
		m_pLeftChild->InsertData( m_sRenderIndex );
	if ( m_pRightChild )
		m_pRightChild->InsertData( m_sRenderIndex );

	if ( m_pLeftChild || m_pRightChild )
	{
		m_sRenderIndex.CleanUp();
	}

	if ( m_pLeftChild )
		m_pLeftChild->DeCompressData();
	if ( m_pRightChild )
		m_pRightChild->DeCompressData();
}

// �����͸� �־��ش�.
void DxOcclusionTree::InsertData( const RenderIndex& sRenderIndex )
{
	SET_DWORD_CITER citer = sRenderIndex.m_setPiece.begin();
	for ( ; citer!=sRenderIndex.m_setPiece.end(); ++citer )
	{
		m_sRenderIndex.m_setPiece.insert( (*citer) );
	}

	citer = sRenderIndex.m_setOctree.begin();
	for ( ; citer!=sRenderIndex.m_setOctree.end(); ++citer )
	{
		m_sRenderIndex.m_setOctree.insert( (*citer) );
	}

	for ( DWORD i=0; i<NSOcclusionCharZone::HEIGHT_TYPE_SIZE; ++i )
	{
		citer = sRenderIndex.m_setChar[i].begin();
		for ( ; citer!=sRenderIndex.m_setChar[i].end(); ++citer )
		{
			m_sRenderIndex.m_setChar[i].insert( (*citer) );
		}
	}
}

// Point �� ����ȭ �Ѵ�.
void DxOcclusionTree::OptimizePoint( DxLandMan* pDxLandMan, int nSumulationLength )
{
	if ( !m_vecPoint.empty() )
	{
		D3DXVECTOR3 vMaxPoint = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		D3DXVECTOR3 vMinPoint = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		for ( size_t i=0; i<m_vecPoint.size(); ++i )
		{
			const D3DXVECTOR3& vPos = m_vecPoint[i];
			if( vMaxPoint.x < vPos.x )	vMaxPoint.x = vPos.x;
			if( vMaxPoint.y < vPos.y )	vMaxPoint.y = vPos.y;
			if( vMaxPoint.z < vPos.z )	vMaxPoint.z = vPos.z;

			if( vMinPoint.x > vPos.x )	vMinPoint.x = vPos.x;
			if( vMinPoint.y > vPos.y )	vMinPoint.y = vPos.y;
			if( vMinPoint.z > vPos.z )	vMinPoint.z = vPos.z;
		}

		DWORD dwLoopX(2);
		DWORD dwLoopY(2);
		DWORD dwLoopZ(2);
		D3DXVECTOR3 vDir = vMaxPoint - vMinPoint;
		D3DXVECTOR3 vCenter = (vMaxPoint + vMinPoint) * 0.5f;
		
		//if ( (vMaxPoint.x - vMinPoint.x) > (0.5f*0.75f*nSumulationLength) )
		//{
		//	dwLoopX = 3;
		//	vDir.x /= 2;
		//}
		//if ( (vMaxPoint.y - vMinPoint.y) > (0.5f*0.75f*nSumulationLength) )
		//{
		//	dwLoopY = 3;
		//	vDir.y /= 2;
		//}
		//if ( (vMaxPoint.z - vMinPoint.z) > (0.5f*0.75f*nSumulationLength) )
		//{
		//	dwLoopZ = 3;
		//	vDir.z /= 2;
		//}

		std::vector<D3DXVECTOR3> vecBasePoint;
		for ( DWORD y=0; y<dwLoopY; ++y )
		{
			for ( DWORD x=0; x<dwLoopX; ++x )
			{
				for ( DWORD z=0; z<dwLoopZ; ++z )
				{
					vecBasePoint.push_back( vMinPoint + D3DXVECTOR3( vDir.x*x, vDir.y*y, vDir.z*z ) );
				}
			}
		}

		// 27 �� ���� ����� ������ ���Ѵ�.
		DWORD dwID(0L);
		SET_DWORD setDWORD;
		float fLength(0.f);
		float fLengthMIN(FLT_MAX);
		for ( size_t j=0; j<vecBasePoint.size(); ++j )
		{
			fLengthMIN = FLT_MAX;
			for ( size_t i=0; i<m_vecPoint.size(); ++i )
			{
				vDir = m_vecPoint[i] - vecBasePoint[j];
				fLength = D3DXVec3Length( &vDir );
				if ( fLength < fLengthMIN )
				{
					dwID = i;
					fLengthMIN = fLength;
				}
			}

			setDWORD.insert( dwID );
		}

		// ����ȭ�� Point�� ���Ѵ�.
		vecBasePoint = m_vecPoint;
		m_vecPoint.clear();
		SET_DWORD_ITER iter = setDWORD.begin();
		for ( ; iter!=setDWORD.end(); ++iter )
		{
			m_vecPoint.push_back( vecBasePoint[(*iter)] );
		}

		// �޸�� �浹 Ƚ���� �������� �̻��̸� �� ����Ʈ�� �۾����� �ʵ��� �Ѵ�.
		// �� �۾��� �ϴ� ������, ���Ǵ� ������ �ǹ� �ȿ� �� �ִٴ��� �ϴ� ���� ���� ���� �ִ�.
		// �׺� �޽��� ������ ������Ʈ�� ��Ȯ�ϰ� �˸°� ������ٰ� ������ �� �� ���� ������,
		// �ǹ��ȿ� ���� ���� ã�Ƴ��� ������� �ʵ��� �ϱ� �����̴�.
		int nReCalculater(0);
		std::vector<D3DXVECTOR3> vecTemp;
		for ( unsigned int i=0; i<m_vecPoint.size(); ++i )
		{
			D3DXVECTOR3 vLookatPt = m_vecPoint[i];
			vLookatPt.y += 10.f;

			std::vector<D3DXVECTOR3> vecCameraFrom;
			vecCameraFrom.push_back( vLookatPt + D3DXVECTOR3( 1000.f, 0.f, 1000.f ) );
			vecCameraFrom.push_back( vLookatPt + D3DXVECTOR3( -1000.f, 0.f, 1000.f ) );
			vecCameraFrom.push_back( vLookatPt + D3DXVECTOR3( 1000.f, 0.f, -1000.f ) );
			vecCameraFrom.push_back( vLookatPt + D3DXVECTOR3( -1000.f, 0.f, -1000.f ) );

			size_t nBackFaceColl(0);
			for ( size_t x=0; x<vecCameraFrom.size(); ++x )
			{
				// x ���� �����ڰ� max���� ������� ���� ������ٸ�,
				// �ո����� �ν��� �����ֱ� ���ؼ� 2�� �浹ó���� �Ѵ�.
				D3DXVECTOR3 vCollCCW(0.f,0.f,0.f);
				D3DXVECTOR3 vCollCW(0.f,0.f,0.f);
				BOOL bCollision(FALSE);
				pDxLandMan->IsCollisionNEW( vLookatPt, vecCameraFrom[x], vCollCW, bCollision, EMCC_CULL_CW, TRUE, FALSE );
				if ( bCollision )
				{
					pDxLandMan->IsCollisionNEW( vLookatPt, vecCameraFrom[x], vCollCCW, bCollision, EMCC_CULL_CCW, TRUE, FALSE );
					if ( bCollision )
					{
						// �ڸ� �浹üũ �� ���� �ո� �浹üũ �� �� ���� �տ� ���� ��� ( 0.1 ������ ����µ���..!! )
						// �� �浹���� ������ �� �ȿ� ���ٰ� ���� �ȴ�.
						vCollCW -= vLookatPt;
						vCollCCW -= vLookatPt;
						if ( D3DXVec3Length(&vCollCW) < (D3DXVec3Length(&vCollCCW)-0.1f) )
						{
							++nBackFaceColl;
						}
					}
					else
					{
						// �ո� �浹ó�����ȵǾ��ٸ�, �޸鸸 �浹�Ǿ����Ƿ�..
						// �� �浹���� ������ �� �ȿ� ���ٰ� ���� �ȴ�.
						++nBackFaceColl;
					}
				}

				if ( nBackFaceColl >= vecCameraFrom.size()/2 )
				{
					break;
				}
			}

			if ( nBackFaceColl < vecCameraFrom.size()/2 )
			{
				// 1/2 ���϶�� �������� ��.
				nReCalculater = 0;
				vecTemp.push_back( m_vecPoint[i] );
			}
			else
			{
				// ���� ���� �޸��� �˻��Ѵٸ� ������ ���� ��˻� �ʿ���.
				if ( nReCalculater < 3 )
				{
					++nReCalculater;
					m_vecPoint[i] = ((m_vecPoint[i] - vCenter) * 0.3f) + vCenter;	
					--i;
				}
				else
				{
					// ���ɼ��� ������� ��˻� ���̻� ���� ����.
					nReCalculater = 0;
				}
			}
		}

		// ���� �־��ش�.
		m_vecPoint = vecTemp;
	}

	if ( m_pLeftChild )
		m_pLeftChild->OptimizePoint( pDxLandMan, nSumulationLength );
	if ( m_pRightChild )
		m_pRightChild->OptimizePoint( pDxLandMan, nSumulationLength );
}

void DxOcclusionTree::SaveMain( sc::SerialFile& SFile, const TSTRING& strMakeTimeWLD )
{
	SFile << VERSION;
	SFile.BeginBlock();
	{
		SFile << strMakeTimeWLD;

		SFile << NSOcclusionCharZone::g_fCharHeightAvg;
		SFile << NSOcclusionCharZone::g_fCharHeightHigh;
		SFile << NSOcclusionCharZone::g_fCharHeightHighMax;

		Save( SFile );

		NSOcclusionCharZone::Save( SFile );
	}
	SFile.EndBlock();
}

void DxOcclusionTree::Save( sc::SerialFile& SFile )
{
	SFile << m_nID;
	SFile << m_vMax;
	SFile << m_vMin;

	SFile << static_cast<DWORD>( m_sRenderIndex.m_setPiece.size() );
	SET_DWORD_ITER iter = m_sRenderIndex.m_setPiece.begin();
	for ( ; iter!=m_sRenderIndex.m_setPiece.end(); ++iter )
	{
		SFile << (*iter);
	}

	SFile << static_cast<DWORD>( m_sRenderIndex.m_setOctree.size() );
	iter = m_sRenderIndex.m_setOctree.begin();
	for ( ; iter!=m_sRenderIndex.m_setOctree.end(); ++iter )
	{
		SFile << (*iter);
	}

	SFile << static_cast<DWORD>( NSOcclusionCharZone::HEIGHT_TYPE_SIZE );
	for ( DWORD i=0; i<NSOcclusionCharZone::HEIGHT_TYPE_SIZE; ++i )
	{
		SFile << static_cast<DWORD>( m_sRenderIndex.m_setChar[i].size() );
		iter = m_sRenderIndex.m_setChar[i].begin();
		for ( ; iter!=m_sRenderIndex.m_setChar[i].end(); ++iter )
		{
			SFile << (*iter);
		}
	}

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

void DxOcclusionTree::LoadMain( sc::SerialFile& SFile, TSTRING& strMakeTimeWLD )
{
	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	if ( dwVer == 0x103 )
	{
		SFile >> strMakeTimeWLD;

		SFile >> NSOcclusionCharZone::g_fCharHeightAvg;
		SFile >> NSOcclusionCharZone::g_fCharHeightHigh;
		SFile >> NSOcclusionCharZone::g_fCharHeightHighMax;

		Load( SFile );

		NSOcclusionCharZone::Load( SFile );
	}
	else if ( dwVer == 0x102 )
	{
		SFile >> strMakeTimeWLD;

		SFile >> NSOcclusionCharZone::g_fCharHeightAvg;
		SFile >> NSOcclusionCharZone::g_fCharHeightHigh;
		SFile >> NSOcclusionCharZone::g_fCharHeightHighMax;

		Load( SFile );
	}
	else if ( dwVer == 0x101 )
	{
		SFile >> strMakeTimeWLD;
		Load_101( SFile );
	}
	else if ( dwVer == 0x100 )
	{
		Load_101( SFile );
	}
}

void DxOcclusionTree::Load( sc::SerialFile& SFile )
{
	m_sRenderIndex.m_setPiece.clear();
	m_sRenderIndex.m_setOctree.clear();

	SFile >> m_nID;
	SFile >> m_vMax;
	SFile >> m_vMin;

	DWORD dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		DWORD dwValue;
		SFile >> dwValue;
		m_sRenderIndex.m_setPiece.insert( dwValue );
	}

	dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		DWORD dwValue;
		SFile >> dwValue;
		m_sRenderIndex.m_setOctree.insert( dwValue );
	}

	DWORD dwCHAR_HEIGHT_TYPE_SIZE(0);
	SFile >> dwCHAR_HEIGHT_TYPE_SIZE;
	for ( DWORD i=0; i<dwCHAR_HEIGHT_TYPE_SIZE; ++i )
	{
		SFile >> dwSize;
		for ( DWORD z=0; z<dwSize; ++z )
		{
			DWORD dwValue;
			SFile >> dwValue;

			if ( i<NSOcclusionCharZone::HEIGHT_TYPE_SIZE )
			{
				m_sRenderIndex.m_setChar[i].insert( dwValue );
			}
		}
	}

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
		m_pLeftChild = new DxOcclusionTree;
		m_pLeftChild->Load( SFile );
	}

	SFile >> bExist;
	if ( bExist )
	{
		m_pRightChild = new DxOcclusionTree;
		m_pRightChild->Load( SFile );
	}
}

void DxOcclusionTree::Load_101( sc::SerialFile& SFile )
{
	m_sRenderIndex.m_setPiece.clear();
	m_sRenderIndex.m_setOctree.clear();

	SFile >> m_vMax;
	SFile >> m_vMin;

	DWORD dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		DWORD dwValue;
		SFile >> dwValue;
		m_sRenderIndex.m_setPiece.insert( dwValue );
	}

	dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		DWORD dwValue;
		SFile >> dwValue;
		m_sRenderIndex.m_setOctree.insert( dwValue );
	}

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
		m_pLeftChild = new DxOcclusionTree;
		m_pLeftChild->Load_101( SFile );
	}

	SFile >> bExist;
	if ( bExist )
	{
		m_pRightChild = new DxOcclusionTree;
		m_pRightChild->Load_101( SFile );
	}
}

namespace NSOcclusiontreeMaker
{
	BOOL OptimizeDivide( DxOcclusionTree* pTree, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, int& nDivide, float& fHalf )
	{
		float fLengthX = vMax.x - vMin.x;
		float fLengthY = vMax.y - vMin.y;
		float fLengthZ = vMax.z - vMin.z;
		fLengthX = fabsf(fLengthX);
		fLengthY = fabsf(fLengthY);
		fLengthZ = fabsf(fLengthZ);

		float fHalfX = (vMax.x + vMin.x)*0.5f;
		float fHalfY = (vMax.y + vMin.y)*0.5f;
		float fHalfZ = (vMax.z + vMin.z)*0.5f;

		// ���� �� ������ ���� �Ѵ�.
		nDivide = 0;
		if( fLengthX>=fLengthY && fLengthX>=fLengthZ )	nDivide = 1;
		else if( fLengthY>=fLengthZ )					nDivide = 2;
		else											nDivide = 3;

		int nLeft = 0;
		int nRight = 0;
		D3DXVECTOR3 vMaxAABB, vMinAABB;
		for (size_t i=0; i<pTree->m_vecPoint.size(); ++i )
		{
			D3DXVECTOR3 vPos = pTree->m_vecPoint[i];
			switch ( nDivide )
			{
			case 1:
				{
					fHalf = fHalfX;
					if( vPos.x > fHalfX )	++nRight;
					else					++nLeft;
				}
				break;

			case 2:
				{
					fHalf = fHalfY;
					if( vPos.y > fHalfY )	++nRight;
					else					++nLeft;
				}
				break;

			case 3:
				{
					fHalf = fHalfZ;
					if( vPos.z > fHalfZ )	++nRight;
					else					++nLeft;
				}
				break;
			};
		}

		// �����ϰ� ������ �ȵǾ �׸� �����.
		if( !nRight || !nLeft )		return FALSE;

		// Note : �۾�
		pTree->m_pLeftChild = new DxOcclusionTree;
		pTree->m_pRightChild = new DxOcclusionTree;
		for (size_t i=0; i<pTree->m_vecPoint.size(); ++i)
		{
			D3DXVECTOR3 vPos = pTree->m_vecPoint[i];
			switch( nDivide )
			{
			case 1:
				if( vPos.x > fHalfX )
				{
					pTree->m_pRightChild->m_vecPoint.push_back( vPos );
				}
				else
				{
					pTree->m_pLeftChild->m_vecPoint.push_back( vPos );
				}
				break;
			case 2:
				if( vPos.y > fHalfY )
				{
					pTree->m_pRightChild->m_vecPoint.push_back( vPos );
				}
				else
				{
					pTree->m_pLeftChild->m_vecPoint.push_back( vPos );
				}
				break;
			case 3:
				if( vPos.z > fHalfZ )
				{
					pTree->m_pRightChild->m_vecPoint.push_back( vPos );
				}
				else
				{
					pTree->m_pLeftChild->m_vecPoint.push_back( vPos );
				}
				break;
			};
		}

		for (size_t i=0; i<pTree->m_vecPointChar.size(); ++i)
		{
			D3DXVECTOR3 vPos = pTree->m_vecPointChar[i];
			switch( nDivide )
			{
			case 1:
				if( vPos.x > fHalfX )
				{
					pTree->m_pRightChild->m_vecPointChar.push_back( vPos );
				}
				else
				{
					pTree->m_pLeftChild->m_vecPointChar.push_back( vPos );
				}
				break;
			case 2:
				if( vPos.y > fHalfY )
				{
					pTree->m_pRightChild->m_vecPointChar.push_back( vPos );
				}
				else
				{
					pTree->m_pLeftChild->m_vecPointChar.push_back( vPos );
				}
				break;
			case 3:
				if( vPos.z > fHalfZ )
				{
					pTree->m_pRightChild->m_vecPointChar.push_back( vPos );
				}
				else
				{
					pTree->m_pLeftChild->m_vecPointChar.push_back( vPos );
				}
				break;
			};
		}

		// Note : �Űܰ��� �����ش�.
		pTree->m_vecPoint.clear();
		pTree->m_vecPointChar.clear();

		return TRUE;
	}

	void MakeTreeChild( DxOcclusionTree* pTree, float fSize )
	{
		D3DXVECTOR3 vMaxPoint = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		D3DXVECTOR3 vMinPoint = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		for ( size_t i=0; i<pTree->m_vecPoint.size(); ++i )
		{
			const D3DXVECTOR3& vPos = pTree->m_vecPoint[i];
			if( vMaxPoint.x < vPos.x )	vMaxPoint.x = vPos.x;
			if( vMaxPoint.y < vPos.y )	vMaxPoint.y = vPos.y;
			if( vMaxPoint.z < vPos.z )	vMaxPoint.z = vPos.z;

			if( vMinPoint.x > vPos.x )	vMinPoint.x = vPos.x;
			if( vMinPoint.y > vPos.y )	vMinPoint.y = vPos.y;
			if( vMinPoint.z > vPos.z )	vMinPoint.z = vPos.z;
		}

		// FALSE �� ��� �� �̻� ���� �Ұ�
		int nDivide(0);
		float fHalf(0.f);
		float fLength(0.f);
		if( !OptimizeDivide( pTree, vMaxPoint, vMinPoint, nDivide, fHalf ) )
			return;

		// Left Right AABB �ڽ��� �������ش�.
		pTree->m_pLeftChild->m_vMax		= pTree->m_vMax;
		pTree->m_pLeftChild->m_vMin		= pTree->m_vMin;
		pTree->m_pRightChild->m_vMax	= pTree->m_vMax;
		pTree->m_pRightChild->m_vMin	= pTree->m_vMin;
		switch( nDivide )
		{
		case 1:
			pTree->m_pLeftChild->m_vMax.x	= fHalf;
			pTree->m_pRightChild->m_vMin.x	= fHalf;
			pTree->m_pLeftChild->m_vMax.x	+= 0.0001f;
			pTree->m_pRightChild->m_vMin.x	-= 0.0001f;
			fLength = pTree->m_pRightChild->m_vMax.x	- fHalf;
			break;
		case 2:
			pTree->m_pLeftChild->m_vMax.y = fHalf;
			pTree->m_pRightChild->m_vMin.y = fHalf;
			pTree->m_pLeftChild->m_vMax.y	+= 0.0001f;
			pTree->m_pRightChild->m_vMin.y	-= 0.0001f;
			fLength = pTree->m_pRightChild->m_vMax.y	- fHalf;
			break;
		case 3:
			pTree->m_pLeftChild->m_vMax.z = fHalf;
			pTree->m_pRightChild->m_vMin.z = fHalf;
			pTree->m_pLeftChild->m_vMax.z	+= 0.0001f;
			pTree->m_pRightChild->m_vMin.z	-= 0.0001f;
			fLength = pTree->m_pRightChild->m_vMax.z	- fHalf;
			break;
		}

		// �ڽ��� ���ؼ�ġ�� �����ߴٸ� �� �̻� ���� ���� �ʴ´�.
		if ( fLength < fSize )
		{
			return;
		}

		// �ڽ��� ��� �ؼ� �����Ѵ�.
		MakeTreeChild( pTree->m_pLeftChild, fSize );
		MakeTreeChild( pTree->m_pRightChild, fSize );
	}

	void MakeTree( DxOcclusionTree** ppTree, 
					const std::vector<D3DXVECTOR3>& vecPoint, 
					const std::vector<D3DXVECTOR3>& vecPointChar,
					const D3DXVECTOR3& vMax, 
					const D3DXVECTOR3& vMin, 
					float fSize )
	{
		SAFE_DELETE( (*ppTree) );
		(*ppTree) = new DxOcclusionTree;
		(*ppTree)->m_vecPoint = vecPoint;
		(*ppTree)->m_vecPointChar = vecPointChar;

		// Note : �浹�� ���ؼ�
		(*ppTree)->m_vMax	= vMax;
		(*ppTree)->m_vMin	= vMin;
		(*ppTree)->m_vMax	+= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		(*ppTree)->m_vMin	-= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );

		MakeTreeChild( (*ppTree), fSize );
	}
}