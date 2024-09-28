#include "pch.h"

#include "../DxLand/DxLandMan.h"
#include "../GlobalParam.h"

#include "./Collision/LayerCollision.h"

#include "./actor.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

Actor::Actor()
	: m_fCollRadius(5.f)
{
	Release();
}

Actor::~Actor()
{
}

void Actor::Release()
{
	//m_spParent = NULL;
	m_CurrentCellID = UINT_MAX;
	m_Position = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_Movement = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_NextPosition = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
	m_PreviousPosition = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_MaxSpeed = 5.0f;

	m_CorrectY = 0.0f;

	Stop();
}

void Actor::Stop()
{
    m_Path.Clear();
	m_NextWaypoint = m_Path.WaypointList().end();
}

void Actor::Create(boost::shared_ptr<NavigationMesh> spParent, const D3DXVECTOR3& Position, const bool bInstant /* = false*/, const DWORD dwCurrentCellID /* = -1*/)
{
	m_spParent = spParent;

	SetPosition(Position, dwCurrentCellID, bInstant);
}

void Actor::SetPosition( const D3DXVECTOR3& Position, const DWORD CurrentCellID /* = -1*/, const bool bInstanceMap /* = false*/)
{
	m_Position = Position;
	m_CurrentCellID = CurrentCellID;
	m_Movement = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	if( !m_spParent )
		return;

	D3DXVECTOR3 vFromPt = m_Position + D3DXVECTOR3( 0, 5, 0 );
	D3DXVECTOR3 vTargetPt = m_Position + D3DXVECTOR3( 0, -5, 0 );
	D3DXVECTOR3 vDest;
	DWORD CollisionID;
	BOOL bCollision = FALSE;

	m_spParent->IsCollision( vFromPt, vTargetPt, vDest, CollisionID, bCollision );

	if( bCollision )
	{
		m_CurrentCellID = CollisionID;
		m_Position = vDest;
	}
	else
	{
		if( !bInstanceMap )
			m_CurrentCellID = m_spParent->FindClosestCell( m_Position );

		m_Position =  m_spParent->SnapPointToCell( m_CurrentCellID, m_Position );
	}
}

BOOL Actor::GotoLocation( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2 )
{
	if( !m_spParent )
        return FALSE;

	D3DXVECTOR3 vFromPt( vP1 );
	D3DXVECTOR3	vTargetPt( vP2 );
	D3DXVECTOR3 vDest;
	DWORD CollisionID = 0;
	BOOL bCollision = FALSE;

	m_spParent->IsCollision( vFromPt, vTargetPt, vDest, CollisionID, bCollision );

	if( !bCollision )
		return FALSE;

	GotoLocation( vDest, CollisionID );

	return TRUE;
}

void Actor::GotoLocation( const D3DXVECTOR3& Position, DWORD CellID )
{
	if( !m_spParent )
		return;

	m_Movement = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	// Appended Code [Author:서광수 2006.12.06] Begin ====>
	NavigationCell* StartCell =	m_spParent->GetCell( m_CurrentCellID );
	if( !StartCell )
		return;

	if( !StartCell->IsPointInCellCollumn( m_Position ) )
		m_CurrentCellID = m_spParent->FindClosestCell( m_Position );

	if( m_spParent->LineOfSightTest( m_CurrentCellID, m_Position, CellID, Position ) )
	{
		m_Path.Setup( m_spParent.get(), m_Position, m_CurrentCellID, Position, CellID, 3 );

		m_Path.AddWayPoint( m_Position, m_CurrentCellID, -1 );
		m_Path.AddWayPoint( Position, CellID, -1 );

		m_NextWaypoint = m_Path.WaypointList().begin();

		return;
	}
	// Appended Code [Author:서광수 2006.12.06] <==== End

	bool bPathActive = m_spParent->BuildNavigationPath(
		m_Path,
		m_CurrentCellID,
		m_Position,
		CellID,
		Position );

	if( bPathActive )
	{
		m_NextPosition = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
		m_NextWaypoint = m_Path.WaypointList().begin();
	}
	else
	{
		m_NextWaypoint = m_Path.WaypointList().end();
	}
}

BOOL Actor::LineOfSightTest( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2 )
{
	if( !m_spParent )
		return FALSE;

	D3DXVECTOR3 vFromPt( vP1 );
	D3DXVECTOR3	vTargetPt( vP2 );
	D3DXVECTOR3 vDest;
	DWORD CollisionID = 0;
	BOOL bCollision = FALSE;

	m_spParent->IsCollision( vFromPt, vTargetPt, vDest, CollisionID, bCollision );
	if( !bCollision )
		return FALSE;

	return m_spParent->LineOfSightTest( m_CurrentCellID, m_Position, CollisionID, vDest );
} //Actor::LineOfSightTest

const bool Actor::PathIsActive(void) const
{
	return ( m_NextWaypoint != m_Path.WaypointList().end() ) ? true : false;
}

void Actor::SetNextWayPoint( const NavigationPath::WAYPOINT& WayPoint )
{
	const NavigationPath::WAYPOINT_VEC& listWaypoint = m_Path.WaypointList();
	NavigationPath::WAYPOINT_VEC_CITER iter = listWaypoint.begin();

	for( ; iter != listWaypoint.end(); ++iter )
	{
		const NavigationPath::WAYPOINT& waypoint= *iter;

		if( waypoint.CellID == WayPoint.CellID )
		{
			m_NextPosition = waypoint.Position;
			m_NextWaypoint = iter;
			break;
		}
	}
}

NavigationPath::WAYPOINT Actor::GetNextWayPoint() const
{
	if( m_NextWaypoint != m_Path.WaypointList().end() )
		return *m_NextWaypoint;

	return NavigationPath::WAYPOINT();
}

D3DXVECTOR3 Actor::GetTargetPosition()
{
	const NavigationPath::WAYPOINT_VEC& listWaypoint = m_Path.WaypointList();
	if( !listWaypoint.empty() )
		return listWaypoint.back().Position;

	return D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
}

HRESULT Actor::Update( const std::vector<boost::shared_ptr<LayerCollision>>& vecLayerCollision, float elapsedTime, BOOL bAccuracyUp )
{
	return UpdateOld( vecLayerCollision, elapsedTime, bAccuracyUp );
}
//
//HRESULT Actor::Update_Room( float elapsedTime, BOOL bAccuracyUp )
//{
//	return UpdateOld_Room( elapsedTime, bAccuracyUp );
//}
//
//HRESULT Actor::UpdateOld_Room( float elapsedTime, BOOL bAccuracyUp )
//{
//	if( !m_spParent )
//		return E_FAIL;
//
//	if( !PathIsActive() )
//		return S_OK;
//
//	CTime sTime = CTime::GetCurrentTime();
//
//	//	크기가 있는 방향벡터를 얻어냄
//	m_Movement = m_NextWaypoint->Position - m_Position;
//
//	//	방향벡터의 길이가 시간당 움직일 수 있는 최대 길이보다 길 경우 제한함
//	float fMoveDist = m_MaxSpeed * elapsedTime;
//	float fTotalDist = D3DXVec3Length( &m_Movement );
//	if( fTotalDist > fMoveDist )
//	{
//		D3DXVec3Normalize( &m_Movement, &m_Movement );
//		m_Movement *= fMoveDist;
//	}
//
//	//	달달달~ 떠는 것을 방지함
//	if( fabs( m_Movement.x ) < 0.001f )
//		m_Movement.x = 0.0f;
//	if( fabs( m_Movement.y ) < 0.001f )
//		m_Movement.y = 0.0f;
//	if( fabs( m_Movement.z ) < 0.001f )
//		m_Movement.z = 0.0f;
//
//	//	하나의 구간내에서 아직 움직여야 하는 곳이 있다면 처리한다.
//	if( fTotalDist > 0.01f )
//	{
//		D3DXVECTOR3 NextPosition( m_Position + m_Movement );
//		DWORD NextCellID;
//		m_spParent->ResolveMotionOnMesh( m_Position, m_CurrentCellID, NextPosition, &NextCellID );
//
//		// [shhan][2014.11.18] rm #385 - 화면 떨림 현상 수정 요청
//		//						ResolveMotionOnMesh 에서 넘어온 값은 덜덜덜 떨리는 현상이 나타난다.
//		//						그래서 NaviMesh Collision 값을 쓴다.
//		//						정확도를 향상시킨다.
//		if ( bAccuracyUp )
//		{
//			BOOL bCollision(FALSE);
//			D3DXVECTOR3 vCollisionPos(0.f,0.f,0.f);
//			DWORD dwNextCellID_COLL(-1);
//			m_spParent->IsCollision( NextPosition + D3DXVECTOR3(0.f,5.f,0.f), NextPosition - D3DXVECTOR3(0.f,5.f,0.f), vCollisionPos, dwNextCellID_COLL, bCollision );
//			if ( bCollision )
//			{
//				NextPosition = vCollisionPos;
//				NextCellID = dwNextCellID_COLL;
//			}
//		}
//
//		NavigationCell* pCell = m_spParent->GetCell( NextCellID );
//		if( !pCell )
//		{
//			Stop();
//			return E_FAIL;
//		}
//
//		//	계단과 같은 곳에서 캐릭터가 y축에 대해서 튀는것을 방지한다.
//		if( pCell->Normal().y <= 0.0001f )
//			NextPosition.y = m_CorrectY;
//		else
//			m_CorrectY = NextPosition.y;
//
//		if ( sTime.GetSecond() > 5 && sTime.GetSecond() < 50)
//			m_Position = NextPosition;
//
//		m_CurrentCellID = NextCellID;
//	}
//	else
//	{
//		if ( sTime.GetSecond() > 5 && sTime.GetSecond() < 50)
//			m_Position = m_NextWaypoint->Position;
//
//		NavigationCell* pCell = m_spParent->GetCell( m_NextWaypoint->CellID );
//		if( !pCell )
//		{
//			Stop();
//			return E_FAIL;
//		}
//
//		//	계단과 같은 곳에서 캐릭터가 y축에 대해서 튀는것을 방지한다.
//		if( pCell->Normal().y <= 0.0001f )
//			m_Position.y = m_CorrectY;
//		else
//			m_CorrectY = m_Position.y;
//
//		m_Movement = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
//		m_NextWaypoint = m_Path.GetFurthestVisibleWayPoint( m_NextWaypoint );
//
//		if( PathIsActive() )
//		{
//			m_NextPosition = m_NextWaypoint->Position;
//
//			UpdateOld( elapsedTime, bAccuracyUp );
//		}
//		else
//		{
//			m_NextPosition = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
//		}
//	}
//
//	return S_OK;
//}

HRESULT Actor::UpdateOld( const std::vector<boost::shared_ptr<LayerCollision>>& vecLayerCollision, float elapsedTime, BOOL bAccuracyUp )
{
	if( !m_spParent )
		return E_FAIL;

	if( !PathIsActive() )
		return S_OK;

	CTime sTime = CTime::GetCurrentTime();

	//	크기가 있는 방향벡터를 얻어냄
	m_Movement = m_NextWaypoint->Position - m_Position;

	//	방향벡터의 길이가 시간당 움직일 수 있는 최대 길이보다 길 경우 제한함
	float fMoveDist = m_MaxSpeed * elapsedTime;
	float fTotalDist = D3DXVec3Length( &m_Movement );
	if( fTotalDist > fMoveDist )
	{
		D3DXVec3Normalize( &m_Movement, &m_Movement );
		m_Movement *= fMoveDist;
	}

	//	달달달~ 떠는 것을 방지함
	if( fabs( m_Movement.x ) < 0.001f )
        m_Movement.x = 0.0f;
	if( fabs( m_Movement.y ) < 0.001f )
        m_Movement.y = 0.0f;
	if( fabs( m_Movement.z ) < 0.001f )
        m_Movement.z = 0.0f;

	//	하나의 구간내에서 아직 움직여야 하는 곳이 있다면 처리한다.
	if( fTotalDist > 0.01f )
	{
		D3DXVECTOR3 NextPosition( m_Position + m_Movement );

		// 충돌이 된다면 이동을 못하게 한다.
		SIMPLE_COLLISION_RESULT enResult(COLL_FALSE);
		for ( DWORD i=0; i<vecLayerCollision.size(); ++i )
		{
			enResult = vecLayerCollision[i]->IsCollision( m_Position, NextPosition, m_fCollRadius );
			if ( enResult == COLL_TRUE )
			{
				// 기본적으로는 이동하지 않는다.
				Stop();
				return S_OK;
			}
			else if ( enResult == COLL_TRUE_START_IN )		// 시작점이 내부에 있다면, m_fCollRadius 를 줄이면서 Start 를 내부에 존재하지 않도록 한다.
			{												// 수치가 너무 적을 경우 충돌 안되는것으로 인식하게 한다.
				m_fCollRadius *= 0.1f;	// 10% 줄인다.

				// 어느정도 사이즈가 있을 경우는 Stop 시키고 아무것도 안하게 한다.
				// 사이즈가 너무 적을 경우, 충동이 안되는것으로 인식하게 한다.
				if ( m_fCollRadius > 0.001f )
				{
					Stop();
					return S_OK;
				}
			}
		}

		// 혹시 COLL_TRUE_START_IN 로 값이 줄어져 있을 수 있어서, 다시 올려주도록 한다.
		m_fCollRadius *= 10.f;
		if ( m_fCollRadius > 5.f )
			m_fCollRadius = 5.f;

		DWORD NextCellID;
		m_spParent->ResolveMotionOnMesh( m_Position, m_CurrentCellID, NextPosition, &NextCellID );

		// [shhan][2014.11.18] rm #385 - 화면 떨림 현상 수정 요청
		//						ResolveMotionOnMesh 에서 넘어온 값은 덜덜덜 떨리는 현상이 나타난다.
		//						그래서 NaviMesh Collision 값을 쓴다.
		//						정확도를 향상시킨다.
		if ( bAccuracyUp )
		{
			BOOL bCollision(FALSE);
			D3DXVECTOR3 vCollisionPos(0.f,0.f,0.f);
			DWORD dwNextCellID_COLL(-1);
			m_spParent->IsCollision( NextPosition + D3DXVECTOR3(0.f,5.f,0.f), NextPosition - D3DXVECTOR3(0.f,5.f,0.f), vCollisionPos, dwNextCellID_COLL, bCollision );
			if ( bCollision )
			{
				NextPosition = vCollisionPos;
				NextCellID = dwNextCellID_COLL;
			}
		}

		NavigationCell* pCell = m_spParent->GetCell( NextCellID );
		if( !pCell )
		{
			Stop();
			return E_FAIL;
		}

		//	계단과 같은 곳에서 캐릭터가 y축에 대해서 튀는것을 방지한다.
		if( pCell->Normal().y <= 0.0001f )
			NextPosition.y = m_CorrectY;
		else
			m_CorrectY = NextPosition.y;

		m_PreviousPosition = m_Position;
		m_Position = NextPosition;

		m_CurrentCellID = NextCellID;
	}
	else
	{
		m_PreviousPosition = m_Position;
		m_Position = m_NextWaypoint->Position;

		NavigationCell* pCell = m_spParent->GetCell( m_NextWaypoint->CellID );
		if( !pCell )
		{
			Stop();
			return E_FAIL;
		}

		//	계단과 같은 곳에서 캐릭터가 y축에 대해서 튀는것을 방지한다.
		if( pCell->Normal().y <= 0.0001f )
			m_Position.y = m_CorrectY;
		else
			m_CorrectY = m_Position.y;

		m_Movement = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_NextWaypoint = m_Path.GetFurthestVisibleWayPoint( m_NextWaypoint );

		if( PathIsActive() )
		{
			m_NextPosition = m_NextWaypoint->Position;

			UpdateOld( vecLayerCollision, elapsedTime, bAccuracyUp );
		}
		else
		{
			m_NextPosition = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
		}
	}

	return S_OK;
}
//
//HRESULT Actor::UpdateNew( float& elapsedTime, BOOL bAccuracyUp )
//{
//	if( !m_spParent )
//		return E_FAIL;
//
//	if( !PathIsActive() )
//		return S_OK;
//
//	if( elapsedTime <= 0.0f )
//		return S_OK;
//
//	if( m_MaxSpeed <= 0.0f )
//		return S_OK;
//
//	m_Movement = m_NextWaypoint->Position - m_Position;
//
//	float fTotalDist = D3DXVec3Length( &m_Movement );
//	float fMoveDist = m_MaxSpeed * elapsedTime;
//
//	if( fMoveDist < fTotalDist )
//	{
//		D3DXVec3Normalize( &m_Movement, &m_Movement );
//		m_Movement *= fMoveDist;
//
//		//	달달달~ 떠는 것을 방지함
//		if( fabs( m_Movement.x ) < 0.001f )
//			m_Movement.x = 0.0f;
//		if( fabs( m_Movement.y ) < 0.001f )
//			m_Movement.y = 0.0f;
//		if( fabs( m_Movement.z ) < 0.001f )
//			m_Movement.z = 0.0f;
//
//		D3DXVECTOR3 NextPosition( m_Position + m_Movement );
//		DWORD NextCellID;
//		m_spParent->ResolveMotionOnMesh( m_Position, m_CurrentCellID, NextPosition, &NextCellID );
//
//		// [shhan][2014.11.18] rm #385 - 화면 떨림 현상 수정 요청
//		//						ResolveMotionOnMesh 에서 넘어온 값은 덜덜덜 떨리는 현상이 나타난다.
//		//						그래서 NaviMesh Collision 값을 쓴다.
//		//						정확도를 향상시킨다.
//		if ( bAccuracyUp )
//		{
//			BOOL bCollision(FALSE);
//			D3DXVECTOR3 vCollisionPos(0.f,0.f,0.f);
//			DWORD dwNextCellID_COLL(-1);
//			m_spParent->IsCollision( NextPosition + D3DXVECTOR3(0.f,5.f,0.f), NextPosition - D3DXVECTOR3(0.f,5.f,0.f), vCollisionPos, dwNextCellID_COLL, bCollision );
//			if ( bCollision )
//			{
//				NextPosition = vCollisionPos;
//				NextCellID = dwNextCellID_COLL;
//			}
//		}
//
//		NavigationCell* pCell = m_spParent->GetCell( NextCellID );
//		if( !pCell )
//		{
//			Stop();
//			return E_FAIL;
//		}
//
//		//	계단과 같은 곳에서 캐릭터가 y축에 대해서 튀는것을 방지한다.
//		if( pCell->Normal().y <= 0.0001f )
//			NextPosition.y = m_CorrectY;
//		else
//			m_CorrectY = NextPosition.y;
//
//		m_Position = NextPosition;
//		m_CurrentCellID = NextCellID;
//	}
//	else
//	{
//		D3DXVECTOR3 vOldPos = m_Position;
//
//		m_Position = m_NextWaypoint->Position;
//
//		NavigationCell* pCell = m_spParent->GetCell( m_NextWaypoint->CellID );
//		if( !pCell )
//		{
//			Stop();
//			return E_FAIL;
//		}
//
//		//	계단과 같은 곳에서 캐릭터가 y축에 대해서 튀는것을 방지한다.
//		if( pCell->Normal().y <= 0.0001f )
//			m_Position.y = m_CorrectY;
//		else
//			m_CorrectY = m_Position.y;
//
//		m_Movement = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
//		m_NextWaypoint = m_Path.GetFurthestVisibleWayPoint( m_NextWaypoint );
//
//		if( PathIsActive() )
//		{
//			m_NextPosition = m_NextWaypoint->Position;
//
//			fMoveDist = D3DXVec3Length( &D3DXVECTOR3( m_Position - vOldPos ) );
//			if( fMoveDist > 0.0f )
//				elapsedTime -= fMoveDist / m_MaxSpeed;
//
//			return UpdateNew( elapsedTime, bAccuracyUp );
//		}
//		else
//		{
//			m_NextPosition = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
//		}
//	}
//
//	return S_OK;
//}

HRESULT Actor::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !PathIsActive() )
		return S_OK;

	const int nLineVertex = 2;
	DIFFUSEVERTEX LineVB[ nLineVertex ] =
	{
		DIFFUSEVERTEX( 0.0f, 0.0f, 0.0f, D3DCOLOR_XRGB( 0, 0, 0 ) ),
		DIFFUSEVERTEX( 0.0f, 0.0f, 0.0f, D3DCOLOR_XRGB( 0, 0, 0 ) )
	};

	const int nLineIndex = 2;
	WORD LineIB[ nLineIndex ] = { 0, 1 };

	DWORD dwLight;
	pd3dDevice->GetRenderState( D3DRS_LIGHTING, &dwLight );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	DWORD dwColorOP, dwColorARG2;
	pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP, &dwColorOP );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG2, &dwColorARG2 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld ); 

	pd3dDevice->SetFVF( DIFFUSEVERTEX::FVF );

	const NavigationPath::WAYPOINT_VEC& listWaypoint = m_Path.WaypointList();
	if( !listWaypoint.empty() )
	{
		NavigationPath::WAYPOINT_VEC_CITER iter = listWaypoint.begin();
		NavigationPath::WAYPOINT LastWaypoint = *iter;
		++iter;

		for( ; iter != listWaypoint.end(); ++iter )
		{
			const NavigationPath::WAYPOINT& waypoint= *iter;

			LineVB[ 0 ].x = LastWaypoint.Position.x;
			LineVB[ 0 ].y = LastWaypoint.Position.y + 1.0f;
			LineVB[ 0 ].z = LastWaypoint.Position.z;
			LineVB[ 0 ].Diffuse = D3DCOLOR_XRGB( 0, 255, 255 );

			LineVB[ 1 ].x = waypoint.Position.x;
			LineVB[ 1 ].y = waypoint.Position.y + 1.0f;
			LineVB[ 1 ].z = waypoint.Position.z;
			LineVB[ 1 ].Diffuse = D3DCOLOR_XRGB( 0, 255, 255 );

			pd3dDevice->DrawIndexedPrimitiveUP(
				D3DPT_LINELIST,
				0,
				nLineIndex,
				nLineIndex / 2,
				LineIB,
				D3DFMT_INDEX16,
				LineVB,
				D3DXGetFVFVertexSize( DIFFUSEVERTEX::FVF ) );

			LastWaypoint = waypoint;
		}
	}

	const NavigationPath::WAYPOINT& waypoint= *m_NextWaypoint;

	LineVB[ 0 ].x = m_Position.x;
	LineVB[ 0 ].y = m_Position.y + 0.1f;
	LineVB[ 0 ].z = m_Position.z;
	LineVB[ 0 ].Diffuse = D3DCOLOR_XRGB( 255, 0, 0 );

	LineVB[ 1 ].x = waypoint.Position.x;
	LineVB[ 1 ].y = waypoint.Position.y + 0.1f;
	LineVB[ 1 ].z = waypoint.Position.z;
	LineVB[ 1 ].Diffuse = D3DCOLOR_XRGB( 255, 0, 0 );

	pd3dDevice->DrawIndexedPrimitiveUP(
		D3DPT_LINELIST,
		0,
		nLineIndex,
		nLineIndex / 2,
		LineIB,
		D3DFMT_INDEX16,
		LineVB,
		D3DXGetFVFVertexSize( DIFFUSEVERTEX::FVF ) );

	pd3dDevice->SetRenderState( D3DRS_LIGHTING, dwLight );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, dwColorOP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, dwColorARG2 );

	return S_OK;
}
