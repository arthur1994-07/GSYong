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

	// 높이 10 카메라 최대거리 200
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

	// 각 Point 마다 카메라 여러개 만듬
	RenderIndex_VECTOR sRenderIndex;
	for( DWORD i=0; i<m_vecPoint.size(); ++i )
	{
		++nCountCUR;

		// 강제로 Bake를 껐다면 렌더링 안하도록 작업한다.
		if ( DxOcclusionQuery::g_bForceDisable )
			continue;

		D3DXVECTOR3 vLookatPt = m_vecPoint[i];
		vLookatPt.y += 10.f;

		std::vector<D3DXVECTOR3> vecCameraFrom;	// 카메라 위치

		vCameraPos = vLookatPt;
		vCameraPos.y += 200.f;
		vCameraPos.z += 1.f;
		vecCameraFrom.push_back( vCameraPos );

		// 밑에서 바라보기 각도
		for ( DWORD j=0; j<4; ++j )
		{
			vCameraPos = vLookatPt + vDirDown[j];
			vecCameraFrom.push_back( vCameraPos );
		}

		// 10 각도
		for ( DWORD j=0; j<12; ++j )
		{
			vCameraPos = vLookatPt + vDir10[j];
			vecCameraFrom.push_back( vCameraPos );
		}

		// 15 각도
		for ( DWORD j=0; j<24; ++j )
		{
			vCameraPos = vLookatPt + vDir15[j];
			vecCameraFrom.push_back( vCameraPos );
		}

		// 22.5 각도
		for ( DWORD j=0; j<12; ++j )
		{
			vCameraPos = vLookatPt + vDir225[j];
			vecCameraFrom.push_back( vCameraPos );
		}

		//// 36 방향
		//for ( DWORD j=0; j<36; ++j )
		//{
		//	vCameraPos = vLookatPt + vDir36[j];
		//	vecCameraFrom.push_back( vCameraPos );
		//}

		//// 평면 방향
		//for ( DWORD j=0; j<4; ++j )
		//{
		//	vCameraPos = vLookatPt + vDir[j];
		//	vecCameraFrom.push_back( vCameraPos );
		//}

		//// 45 각도
		//for ( DWORD j=0; j<4; ++j )
		//{
		//	vCameraPos = vLookatPt + vDir45[j];
		//	vecCameraFrom.push_back( vCameraPos );
		//}

		// OcclusionQuery를 사용하여 걸리는 PieceList를 만들어야함.
		for ( size_t x=0; x<vecCameraFrom.size(); ++x )
		{
			// 기본은 옆으로만 넓혀서 본다.
			float fFOV = D3DX_PI/4;
			float fSurfaceWidth = fSurfaceSize*2.f;
			if ( x>=0 && x<=4 )
			{
				// 완전 위랑 완전 아래서 볼때는 각도를넓혀서 본다.
				// 이렇게 하는 이유는 유저의 화면 비율은 1:1 이 아니다. 2:1 도 나올 수 있다.
				// 그러므로 비율을 옆으로 넓게 해주자.
				// 유저가 상하로 비율을 늘린다면 어쩔 수 없다.!! 그냥 그렇게 못 늘리도록 막는게 좋겠다.
				fFOV = D3DX_PI/2;
				fSurfaceWidth = fSurfaceSize;
			}

			//// 지형과 충돌처리.
			D3DXVECTOR3 vEnd(vecCameraFrom[x]);
			D3DXVECTOR3 vCollision(0.f,0.f,0.f);
			BOOL bCollision(FALSE);

			D3DXVECTOR3 vCamDir = vEnd - vLookatPt;
			vCollision = vEnd;

			pDxLandMan->IsCollisionNEW( vLookatPt, vEnd, vCollision, bCollision, EMCC_CULL_CCW_EXCEPT_ALPHATEX, TRUE, FALSE );

			// 충돌이면, 양면 렌더링을 하고, 카메라를 최대치로 해서 렌더링 한다.
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

			// 충돌을 하건 안하건 작업하는 것
			{
				D3DXVECTOR3 vCamDir = vCollision - vLookatPt;

				D3DXVECTOR3 vCollTemp;
				BOOL bCollTemp(FALSE);
				for ( int z=0; z<5; ++z )
				{
					// 카메라를 좁혀서 다시 검사한다.
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
							// 한번이라도 충돌되면 검사 그만한다.
							break;
						}
					}

					// 충돌되지 않았다면 OK
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

	// 가시성 판별된 애들을 set 에 넣어놓는다.
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
	// 구멍으로 빠지는지 체크하는 것
	{
		BOOL bCollTemp(FALSE);
		D3DXVECTOR3 vCollTemp(0.f,0.f,0.f);
		D3DXVECTOR3 vStart = vFromPt + D3DXVECTOR3(0.f,1.f,0.f);
		D3DXVECTOR3 vEnd = vFromPt + D3DXVECTOR3(0.f,-10000.f,0.f);

		// 바닥으로 충돌처리를 했는데 충돌되는 것이 없다면 작업하면 안된다.
		// 구멍으로 빠진 상황이다.
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

		// Sort 에 알려줘서 LOD 기능이 동작 못하도록 한다.
		BOOL bViewAlphaLOD_BackUp = NSPieceQuickSort::g_bViewAlphaLOD;
		NSPieceQuickSort::g_bViewAlphaLOD = FALSE;

		// Note : View Port를 설정한다.
		D3DXVECTOR3 vUpVec( 0.f, 1.f, 0.f );
		DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );

		// Note : Projection를 설정한다.
		float fNearPlane = 2.f;	// 5.f
		float fFarPlane = DxFogMan::GetInstance().GetGameFogHighEnd( pDxLandMan->GetFogProperty() );
		DxViewPort::GetInstance().SetProjection ( fFOV, fSurfaceWidth, fSurfaceHeight, fNearPlane, fFarPlane );

		DxViewPort::GetInstance().ComputeClipVolume();

		CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume();

		//	Note : 월드 트렌스폼 초기화.
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

		pDxLandMan->ResetOcclusionQuery();
		pDxLandMan->PieceSort( sCV, vFromPt, vLookatPt );
		pDxLandMan->Render_Query( pd3dDevice, bFieldOcclusion, bPieceOcclusion, dwCullMode );

		pDxLandMan->FindOcclusionRenderIndex( pd3dDevice, sRenderIndex.m_vecOctree, sRenderIndex.m_vecPiece );

		// CharZone은 내부에서 Index를 찾도록 한다.
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

// 데이터를 압축한다.
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

// 압축데이터를 해제한다.
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

// 데이터를 넣어준다.
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

// Point 를 최적화 한다.
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

		// 27 개 점중 가까운 점들을 구한다.
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

		// 최적화된 Point를 구한다.
		vecBasePoint = m_vecPoint;
		m_vecPoint.clear();
		SET_DWORD_ITER iter = setDWORD.begin();
		for ( ; iter!=setDWORD.end(); ++iter )
		{
			m_vecPoint.push_back( vecBasePoint[(*iter)] );
		}

		// 뒷면과 충돌 횟수가 일정수준 이상이면 현 포인트는 작업하지 않도록 한다.
		// 이 작업을 하는 이유는, 계산되는 원점이 건물 안에 들어가 있다던가 하는 일이 있을 수도 있다.
		// 네비 메쉬와 렌더링 오브젝트가 정확하게 알맞게 만들었다고 가정을 할 수 없기 때문에,
		// 건물안에 들어가는 점을 찾아내서 계산하지 않도록 하기 위함이다.
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
				// x 파일 제작자가 max에서 양면으로 면을 만들었다면,
				// 앞면으로 인식을 시켜주기 위해서 2번 충돌처리를 한다.
				D3DXVECTOR3 vCollCCW(0.f,0.f,0.f);
				D3DXVECTOR3 vCollCW(0.f,0.f,0.f);
				BOOL bCollision(FALSE);
				pDxLandMan->IsCollisionNEW( vLookatPt, vecCameraFrom[x], vCollCW, bCollision, EMCC_CULL_CW, TRUE, FALSE );
				if ( bCollision )
				{
					pDxLandMan->IsCollisionNEW( vLookatPt, vecCameraFrom[x], vCollCCW, bCollision, EMCC_CULL_CCW, TRUE, FALSE );
					if ( bCollision )
					{
						// 뒤면 충돌체크 된 면이 앞면 충돌체크 된 것 보다 앞에 있을 경우 ( 0.1 보완을 해줬는데도..!! )
						// 그 충돌기준 중점은 면 안에 들어간다고 보면 된다.
						vCollCW -= vLookatPt;
						vCollCCW -= vLookatPt;
						if ( D3DXVec3Length(&vCollCW) < (D3DXVec3Length(&vCollCCW)-0.1f) )
						{
							++nBackFaceColl;
						}
					}
					else
					{
						// 앞면 충돌처리가안되었다면, 뒷면만 충돌되었으므로..
						// 그 충돌기준 중점은 면 안에 들어간다고 보면 된다.
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
				// 1/2 이하라면 정상데이터 임.
				nReCalculater = 0;
				vecTemp.push_back( m_vecPoint[i] );
			}
			else
			{
				// 많은 수가 뒷면을 검사한다면 문제가 있음 재검사 필요함.
				if ( nReCalculater < 3 )
				{
					++nReCalculater;
					m_vecPoint[i] = ((m_vecPoint[i] - vCenter) * 0.3f) + vCenter;	
					--i;
				}
				else
				{
					// 가능성이 없어보여서 재검사 더이상 하지 않음.
					nReCalculater = 0;
				}
			}
		}

		// 값을 넣어준다.
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

		// 가장 긴 선분을 분할 한다.
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

		// 균일하게 분할이 안되어서 그만 만든다.
		if( !nRight || !nLeft )		return FALSE;

		// Note : 작업
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

		// Note : 옮겨가서 지워준다.
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

		// FALSE 일 경우 더 이상 분할 불가
		int nDivide(0);
		float fHalf(0.f);
		float fLength(0.f);
		if( !OptimizeDivide( pTree, vMaxPoint, vMinPoint, nDivide, fHalf ) )
			return;

		// Left Right AABB 박스를 생성해준다.
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

		// 박스가 기준수치에 도달했다면 더 이상 분할 하지 않는다.
		if ( fLength < fSize )
		{
			return;
		}

		// 박스를 계속 해서 분할한다.
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

		// Note : 충돌을 위해서
		(*ppTree)->m_vMax	= vMax;
		(*ppTree)->m_vMin	= vMin;
		(*ppTree)->m_vMax	+= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		(*ppTree)->m_vMin	-= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );

		MakeTreeChild( (*ppTree), fSize );
	}
}