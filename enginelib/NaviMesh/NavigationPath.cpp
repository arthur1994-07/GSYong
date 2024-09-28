#include "pch.h"
#include "./NavigationPath.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

NavigationPath::NavigationPath()
{
}

NavigationPath::~NavigationPath()
{
}

void NavigationPath::Clear()
{
    m_vecWaypoint.clear();
}

void NavigationPath::Setup( NavigationMesh* Parent, const D3DXVECTOR3& StartPoint, DWORD StartID, const D3DXVECTOR3& EndPoint, DWORD EndID, DWORD dwWayPointSize )
{
    Clear();

    m_Parent = Parent;
    m_StartPoint.Position = StartPoint;
    m_StartPoint.CellID = StartID;
	m_StartPoint.nLinkWall = -1;
    m_EndPoint.Position = EndPoint;
    m_EndPoint.CellID = EndID;
	m_EndPoint.nLinkWall = -1;

	// NavigationPath::AddWayPoint 의 m_vecWaypoint.push_back 에서 문제가 발생했다.
	// 이전은 list 로 되어있었고 push_back 마다 new 가 되는 구조였다.
	// 문제는 안될꺼 같은 구조이다.
	// 메모리 단편화를 의심하기에는 LFH 가 동작 될텐데 말이다.
	// 별 방법은 없고 일단 vector 로 하고, reserve 크기를 30으로 하여 new 횟수를 줄이도록 한다.
	// 16byte(WAYPOINT) * 30 = 0.48KB * 200,000 마리 = 100MB

	// 이동이 된다면, capacity 를 30 이상으로 만들어주기 위해 이렇게 씀.
	if ( dwWayPointSize <= 30 )
		dwWayPointSize = 30;

	// dwWayPointSize 가 30 이면, m_vecWaypoint.capacity() 는 14 이하가 되야 함.
	//								15 부터는 다음 조건으로 넘어감.
	if ( dwWayPointSize > (m_vecWaypoint.capacity()*2) )
	{
		// 2 배가 넘는다면 할당크기를 그것에 맞게 변형한다.
		m_vecWaypoint.reserve( dwWayPointSize );
	}
	else if ( dwWayPointSize > m_vecWaypoint.capacity() )
	{
		// 2 배 이내면 2배가 되도록 변경한다.
		m_vecWaypoint.reserve( m_vecWaypoint.capacity()*2 );
	}

    m_vecWaypoint.push_back( m_StartPoint );
}

void NavigationPath::AddWayPoint( const D3DXVECTOR3& Point, DWORD CellID, int nLinkWall )
{
    WAYPOINT NewPoint;
    NewPoint.Position = Point;
    NewPoint.CellID = CellID;
	NewPoint.nLinkWall = nLinkWall;

    m_vecWaypoint.push_back( NewPoint );
}

void NavigationPath::EndPath()
{	
    m_vecWaypoint.push_back( m_EndPoint );
}

// 보통은 변의 중심점으로 이동되면서 목적 위치가 바뀌지 않는데, 
// 이동경로 최적화 하면서 삼각형의 꼭지점으로 이동되도록 변경이 필요할 때가 있다.
// 그 때 m_vecWaypoint 내부값의 WAYPOINT::Position 값이 변경된다.
NavigationPath::WAYPOINT_VEC_CITER NavigationPath::GetFurthestVisibleWayPoint( const WAYPOINT_VEC_CITER& VantagePoint )
{
    // see if we are already talking about the last waypoint
    //	<--	여기가 현재 있는 곳이 마지막 셀인가?, 이번셀이 마지막셀?
    //	<**	이것은, 현재 포지션이 삼각형 한 변의 중점이고,
    //		목표점이 그 삼각형의 어딘가라고 할 때.... 만족되는 구문이다.
    //	**>
    if( VantagePoint == m_vecWaypoint.end() )
        return VantagePoint;

	// VantagePoint 은 막 목적지에 도착된 위치.
    const WAYPOINT& Vantage = *VantagePoint;
    WAYPOINT_VEC_CITER FirstTargetWaypoint = VantagePoint;

	// 다음 Cell. 여기는 무조건 이동 가능.
    ++FirstTargetWaypoint;	

    //	<--	다음 셀이 마지막 셀인가?
    if( FirstTargetWaypoint == m_vecWaypoint.end() )
        return FirstTargetWaypoint;

	// 마지막 위치부터 검사를 하도록 한다.
    WAYPOINT_VEC_CITER VisibleWaypoint = m_vecWaypoint.end();
	--VisibleWaypoint;

    while( 1 )
    {
		// 시작 위치까지 건들면 안됨.
		if ( VantagePoint == VisibleWaypoint )
		{
			//////////////////////////////////////////////////////////////////////////
			// 이곳을 들어올 수는 없다. 처음 얻는 FirstTargetWaypoint 는 무조건 이동 가능하기 때문에 들어온다는 건 문제가 있다는 것임.
			//////////////////////////////////////////////////////////////////////////

			WAYPOINT_VEC_CITER VisibleWaypointNext = VisibleWaypoint;
			++VisibleWaypointNext;

			return VisibleWaypointNext;
		}

		// 현재 Cell 에 현재 점이 있는지 확인이 필요하다.
		bool bLineOfSightTest = m_Parent->LineOfSightTest( Vantage.CellID,
															Vantage.Position,
															(*VisibleWaypoint).CellID,
															(*VisibleWaypoint).Position );

		if( bLineOfSightTest )
		{
			if ( ChangePosition_FurthestVisibleWayPoint( VantagePoint, VisibleWaypoint ) )
			{
				return VisibleWaypoint;
			}
			else
			{
				WAYPOINT_VEC_CITER VisibleWaypointPrev = VisibleWaypoint;
				--VisibleWaypointPrev;

				int nCount(0);
				return GetFurthestVisibleWayPointChild( VantagePoint, FirstTargetWaypoint, VisibleWaypointPrev, nCount );
			}
		}
		else
		{
			--VisibleWaypoint;
		}
    }

	return VisibleWaypoint;
}

// VisibleWaypoint 의 최적 경로를 찾고 없을 경우 좀 더 이전 waypoint 를 순환해서 찾는다.
NavigationPath::WAYPOINT_VEC_CITER NavigationPath::GetFurthestVisibleWayPointChild( const WAYPOINT_VEC_CITER& VantagePoint,
																				   const WAYPOINT_VEC_CITER& FisrtTargetWaypoint, 
																				   const WAYPOINT_VEC_CITER& VisibleWaypoint, 
																				   int& nCount )
{
	// 시작 위치까지 건들면 안됨.
	if ( VantagePoint == VisibleWaypoint )
	{
		WAYPOINT_VEC_CITER VisibleWaypointNext = VisibleWaypoint;
		++VisibleWaypointNext;

		return VisibleWaypointNext;
	}

	// 문제가 있을 수 있어서 100 으로 한정.
	++nCount;
	if ( nCount > 100 )
	{
		return VisibleWaypoint;
	}

	if ( ChangePosition_FurthestVisibleWayPoint( VantagePoint, VisibleWaypoint ) )
	{
		return VisibleWaypoint;
	}
	else
	{
		WAYPOINT_VEC_CITER VisibleWaypointPrev = VisibleWaypoint;
		--VisibleWaypointPrev;

		return GetFurthestVisibleWayPointChild( VantagePoint, FisrtTargetWaypoint, VisibleWaypointPrev, nCount );
	}
}

// VisibleWaypoint 의 Position 값이 최적화 된 위치로 변하게 된다.
// 리턴값 : 위치가 변하면 TRUE ( 현재것을 그대로 사용해도 됨 ) , 변하지 않으면 FALSE ( 좀 더 최적화 된 길을 찾아볼 여지가 있음 )
// VisibleWaypoint 의 다음 waypoint 도 무조건 이동 할 수 있도록 VisibleWaypoint 값을 변경한다.
BOOL NavigationPath::ChangePosition_FurthestVisibleWayPoint( const WAYPOINT_VEC_CITER& VantagePoint, const WAYPOINT_VEC_CITER& VisibleWaypoint )
{
	// VisibleWaypoint 이것은 갈 수 있다 표시
	WAYPOINT_VEC_CITER TestWaypointNext = VisibleWaypoint;
	++TestWaypointNext;

	// 앞셀이 마지막 셀이라면 목적지까지 이동 될 수 있도록 한다.
	if ( TestWaypointNext == m_vecWaypoint.end() )
		return TRUE;

	// Line 의 끝점의 제일 가까운 한 점을 얻도록 한다. ( waypoint_중점이동 )
	D3DXVECTOR3 vPosition;
	if ( !m_Parent->GetTuringPoint( vPosition, (*VisibleWaypoint).CellID, (*VisibleWaypoint).nLinkWall, (*VantagePoint).Position, (*TestWaypointNext).Position, m_EndPoint.Position, FALSE ) )
		return TRUE;

	// 현재 위치에서 waypoint_중점이동 위치로 갈 수 있는지 확인한다.
	bool bLineOfSightTest = m_Parent->LineOfSightTest( (*VantagePoint).CellID,
														(*VantagePoint).Position,
														(*VisibleWaypoint).CellID,
														vPosition );

	// 성공 했다면 다음 것도 성공 하는지 확인해 본다.
	if ( bLineOfSightTest )
	{
		WAYPOINT_VEC_CITER TestWaypointNextNext = TestWaypointNext;
		++TestWaypointNextNext;

		if ( TestWaypointNextNext != m_vecWaypoint.end() )
		{
			// waypoint_중점이동 에서 waypoint_Line중점_NEXTNEXT 위치로 갈 수 있는지 확인한다.
			// 못간다면 라인을 타도록 해야함.
			bLineOfSightTest = m_Parent->LineOfSightTest( (*VisibleWaypoint).CellID,
															vPosition,
															(*TestWaypointNext).CellID,
															(*TestWaypointNext).Position );

			if ( bLineOfSightTest )
			{
				// Line 의 끝점의 제일 가까운 한 점을 얻도록 한다. ( waypoint_중점이동_NEXTNEXT ) 
				D3DXVECTOR3 vNextNext;
				if ( !m_Parent->GetTuringPoint( vNextNext, (*TestWaypointNext).CellID, (*TestWaypointNext).nLinkWall, (*VantagePoint).Position, (*TestWaypointNextNext).Position, m_EndPoint.Position, FALSE ) )
					return TRUE;

				// waypoint_중점이동 에서 waypoint_중점이동_NEXTNEXT 위치로 갈 수 있는지 확인한다.
				// 못간다면 라인을 타도록 해야함.
				bLineOfSightTest = m_Parent->LineOfSightTest( (*VisibleWaypoint).CellID,
																vPosition,
																(*TestWaypointNext).CellID,
																vNextNext );
			}

			// 2번 waypoint 최적포인트에 성공이 안된다면 라인을 타도록 한다.
			// Line 에서는 다음 Cell 의 어디든 갈 수 있다.
			if ( !bLineOfSightTest )
			{
				//////////////////////////////////////////////////////////////////////////
				//
				//						중점이동은 불가
				//					라인으로 이동시키는 작업
				//
				//////////////////////////////////////////////////////////////////////////
				//
				// 라인에 이동 시키는 이유는 bTypeLine(FALSE) 인 중점 방향으로 할 경우, 
				// 선에서 출발하는게 아니라서 다음 삼각형으로 이동 자체가 불가능 할 수 있다.
				//
				//////////////////////////////////////////////////////////////////////////


				// bTypeLine - TRUE 를 줌으로써 Line 에 이동 시키도록 한다. ( 다음 Cell로 무조건 이동 시키기 위함. )
				//
				// Line 의 끝점의 제일 가까운 한 점을 얻도록 한다. ( waypoint_line이동 )
				if ( !m_Parent->GetTuringPoint( vPosition, (*VisibleWaypoint).CellID, (*VisibleWaypoint).nLinkWall, (*VantagePoint).Position, (*TestWaypointNext).Position, m_EndPoint.Position, TRUE ) )
					return TRUE;

				//////////////////////////////////////////////////////////////////////////
				// NavigationCell::ClassifyPathToCell 의 
				// m_Side[i].ClassifyPoint(MotionPath.EndPointA()) -> m_Side[i].ClassifyPoint(MotionPath.EndPointA(),0.001f) 보완작업으로 인해
				// 굳이 정확한 Cell 을 구할 필요가 없어졌다.
				//////////////////////////////////////////////////////////////////////////
				//
				//// bTypeLine - TRUE 로 인해, float 연산상 vPosition 값이 기존Cell 에 안들어 있을 수 있다.
				//// 새로 이동 시키려는 위치가 기존 셀에 들어가지 않을 수 있어서 보완작업 함.
				//NavigationCell* pCell =	m_Parent->GetCell( (*VisibleWaypoint).CellID );
				//if( !pCell || 
				//	(pCell && !pCell->IsPointInCellCollumn( vPosition )) )
				//{
				//	DWORD dwCellID = m_Parent->FindClosestCell( vPosition );

				//	// 두삼각형의 빗변 사이의 값을 한 것이므로 둘중에 하나다.
				//	// 위에서 VisibleWaypoint 쪽에 없다고 했으니 무조건 TestWaypointNext 에 있어야 한다.
				//	if ( (*TestWaypointNext).CellID == dwCellID )
				//	{
				//		// 변화함.
				//		const WAYPOINT* pWayPoint = &(*VisibleWaypoint);
				//		WAYPOINT* pChangeWayPointPosition = const_cast<WAYPOINT*>( pWayPoint );
				//		pChangeWayPointPosition->Position = vPosition;
				//		pChangeWayPointPosition->CellID = dwCellID;

				//		return TRUE;
				//	}
				//	else
				//	{
				//		// 문제가 있다. 그냥 중점으로 이동 시키기 바람.
				//		return TRUE;
				//	}
				//}
			}
		}

		// ( waypoint_중점이동 && waypoint_Line중점_NEXTNEXT 성공 && waypoint_중점이동_NEXTNEXT 성공 ) or ( waypoint_중점이동 && NEXTNEXT 없음 )	-> vPosition (waypoint_중점이동)
		// ( waypoint_중점이동 && (waypoint_중점이동_NEXTNEXT 실패 || waypoint_중점이동_NEXTNEXT 실패) )											-> vPosition (waypoint_Line이동)
		// 
		const WAYPOINT* pWayPoint = &(*VisibleWaypoint);
		WAYPOINT* pChangeWayPointPosition = const_cast<WAYPOINT*>( pWayPoint );
		pChangeWayPointPosition->Position = vPosition;

		return TRUE;
	}

	return FALSE;
}