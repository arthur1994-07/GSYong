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

	// NavigationPath::AddWayPoint �� m_vecWaypoint.push_back ���� ������ �߻��ߴ�.
	// ������ list �� �Ǿ��־��� push_back ���� new �� �Ǵ� ��������.
	// ������ �ȵɲ� ���� �����̴�.
	// �޸� ����ȭ�� �ǽ��ϱ⿡�� LFH �� ���� ���ٵ� ���̴�.
	// �� ����� ���� �ϴ� vector �� �ϰ�, reserve ũ�⸦ 30���� �Ͽ� new Ƚ���� ���̵��� �Ѵ�.
	// 16byte(WAYPOINT) * 30 = 0.48KB * 200,000 ���� = 100MB

	// �̵��� �ȴٸ�, capacity �� 30 �̻����� ������ֱ� ���� �̷��� ��.
	if ( dwWayPointSize <= 30 )
		dwWayPointSize = 30;

	// dwWayPointSize �� 30 �̸�, m_vecWaypoint.capacity() �� 14 ���ϰ� �Ǿ� ��.
	//								15 ���ʹ� ���� �������� �Ѿ.
	if ( dwWayPointSize > (m_vecWaypoint.capacity()*2) )
	{
		// 2 �谡 �Ѵ´ٸ� �Ҵ�ũ�⸦ �װͿ� �°� �����Ѵ�.
		m_vecWaypoint.reserve( dwWayPointSize );
	}
	else if ( dwWayPointSize > m_vecWaypoint.capacity() )
	{
		// 2 �� �̳��� 2�谡 �ǵ��� �����Ѵ�.
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

// ������ ���� �߽������� �̵��Ǹ鼭 ���� ��ġ�� �ٲ��� �ʴµ�, 
// �̵���� ����ȭ �ϸ鼭 �ﰢ���� ���������� �̵��ǵ��� ������ �ʿ��� ���� �ִ�.
// �� �� m_vecWaypoint ���ΰ��� WAYPOINT::Position ���� ����ȴ�.
NavigationPath::WAYPOINT_VEC_CITER NavigationPath::GetFurthestVisibleWayPoint( const WAYPOINT_VEC_CITER& VantagePoint )
{
    // see if we are already talking about the last waypoint
    //	<--	���Ⱑ ���� �ִ� ���� ������ ���ΰ�?, �̹����� ��������?
    //	<**	�̰���, ���� �������� �ﰢ�� �� ���� �����̰�,
    //		��ǥ���� �� �ﰢ���� ��򰡶�� �� ��.... �����Ǵ� �����̴�.
    //	**>
    if( VantagePoint == m_vecWaypoint.end() )
        return VantagePoint;

	// VantagePoint �� �� �������� ������ ��ġ.
    const WAYPOINT& Vantage = *VantagePoint;
    WAYPOINT_VEC_CITER FirstTargetWaypoint = VantagePoint;

	// ���� Cell. ����� ������ �̵� ����.
    ++FirstTargetWaypoint;	

    //	<--	���� ���� ������ ���ΰ�?
    if( FirstTargetWaypoint == m_vecWaypoint.end() )
        return FirstTargetWaypoint;

	// ������ ��ġ���� �˻縦 �ϵ��� �Ѵ�.
    WAYPOINT_VEC_CITER VisibleWaypoint = m_vecWaypoint.end();
	--VisibleWaypoint;

    while( 1 )
    {
		// ���� ��ġ���� �ǵ�� �ȵ�.
		if ( VantagePoint == VisibleWaypoint )
		{
			//////////////////////////////////////////////////////////////////////////
			// �̰��� ���� ���� ����. ó�� ��� FirstTargetWaypoint �� ������ �̵� �����ϱ� ������ ���´ٴ� �� ������ �ִٴ� ����.
			//////////////////////////////////////////////////////////////////////////

			WAYPOINT_VEC_CITER VisibleWaypointNext = VisibleWaypoint;
			++VisibleWaypointNext;

			return VisibleWaypointNext;
		}

		// ���� Cell �� ���� ���� �ִ��� Ȯ���� �ʿ��ϴ�.
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

// VisibleWaypoint �� ���� ��θ� ã�� ���� ��� �� �� ���� waypoint �� ��ȯ�ؼ� ã�´�.
NavigationPath::WAYPOINT_VEC_CITER NavigationPath::GetFurthestVisibleWayPointChild( const WAYPOINT_VEC_CITER& VantagePoint,
																				   const WAYPOINT_VEC_CITER& FisrtTargetWaypoint, 
																				   const WAYPOINT_VEC_CITER& VisibleWaypoint, 
																				   int& nCount )
{
	// ���� ��ġ���� �ǵ�� �ȵ�.
	if ( VantagePoint == VisibleWaypoint )
	{
		WAYPOINT_VEC_CITER VisibleWaypointNext = VisibleWaypoint;
		++VisibleWaypointNext;

		return VisibleWaypointNext;
	}

	// ������ ���� �� �־ 100 ���� ����.
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

// VisibleWaypoint �� Position ���� ����ȭ �� ��ġ�� ���ϰ� �ȴ�.
// ���ϰ� : ��ġ�� ���ϸ� TRUE ( ������� �״�� ����ص� �� ) , ������ ������ FALSE ( �� �� ����ȭ �� ���� ã�ƺ� ������ ���� )
// VisibleWaypoint �� ���� waypoint �� ������ �̵� �� �� �ֵ��� VisibleWaypoint ���� �����Ѵ�.
BOOL NavigationPath::ChangePosition_FurthestVisibleWayPoint( const WAYPOINT_VEC_CITER& VantagePoint, const WAYPOINT_VEC_CITER& VisibleWaypoint )
{
	// VisibleWaypoint �̰��� �� �� �ִ� ǥ��
	WAYPOINT_VEC_CITER TestWaypointNext = VisibleWaypoint;
	++TestWaypointNext;

	// �ռ��� ������ ���̶�� ���������� �̵� �� �� �ֵ��� �Ѵ�.
	if ( TestWaypointNext == m_vecWaypoint.end() )
		return TRUE;

	// Line �� ������ ���� ����� �� ���� �򵵷� �Ѵ�. ( waypoint_�����̵� )
	D3DXVECTOR3 vPosition;
	if ( !m_Parent->GetTuringPoint( vPosition, (*VisibleWaypoint).CellID, (*VisibleWaypoint).nLinkWall, (*VantagePoint).Position, (*TestWaypointNext).Position, m_EndPoint.Position, FALSE ) )
		return TRUE;

	// ���� ��ġ���� waypoint_�����̵� ��ġ�� �� �� �ִ��� Ȯ���Ѵ�.
	bool bLineOfSightTest = m_Parent->LineOfSightTest( (*VantagePoint).CellID,
														(*VantagePoint).Position,
														(*VisibleWaypoint).CellID,
														vPosition );

	// ���� �ߴٸ� ���� �͵� ���� �ϴ��� Ȯ���� ����.
	if ( bLineOfSightTest )
	{
		WAYPOINT_VEC_CITER TestWaypointNextNext = TestWaypointNext;
		++TestWaypointNextNext;

		if ( TestWaypointNextNext != m_vecWaypoint.end() )
		{
			// waypoint_�����̵� ���� waypoint_Line����_NEXTNEXT ��ġ�� �� �� �ִ��� Ȯ���Ѵ�.
			// �����ٸ� ������ Ÿ���� �ؾ���.
			bLineOfSightTest = m_Parent->LineOfSightTest( (*VisibleWaypoint).CellID,
															vPosition,
															(*TestWaypointNext).CellID,
															(*TestWaypointNext).Position );

			if ( bLineOfSightTest )
			{
				// Line �� ������ ���� ����� �� ���� �򵵷� �Ѵ�. ( waypoint_�����̵�_NEXTNEXT ) 
				D3DXVECTOR3 vNextNext;
				if ( !m_Parent->GetTuringPoint( vNextNext, (*TestWaypointNext).CellID, (*TestWaypointNext).nLinkWall, (*VantagePoint).Position, (*TestWaypointNextNext).Position, m_EndPoint.Position, FALSE ) )
					return TRUE;

				// waypoint_�����̵� ���� waypoint_�����̵�_NEXTNEXT ��ġ�� �� �� �ִ��� Ȯ���Ѵ�.
				// �����ٸ� ������ Ÿ���� �ؾ���.
				bLineOfSightTest = m_Parent->LineOfSightTest( (*VisibleWaypoint).CellID,
																vPosition,
																(*TestWaypointNext).CellID,
																vNextNext );
			}

			// 2�� waypoint ��������Ʈ�� ������ �ȵȴٸ� ������ Ÿ���� �Ѵ�.
			// Line ������ ���� Cell �� ���� �� �� �ִ�.
			if ( !bLineOfSightTest )
			{
				//////////////////////////////////////////////////////////////////////////
				//
				//						�����̵��� �Ұ�
				//					�������� �̵���Ű�� �۾�
				//
				//////////////////////////////////////////////////////////////////////////
				//
				// ���ο� �̵� ��Ű�� ������ bTypeLine(FALSE) �� ���� �������� �� ���, 
				// ������ ����ϴ°� �ƴ϶� ���� �ﰢ������ �̵� ��ü�� �Ұ��� �� �� �ִ�.
				//
				//////////////////////////////////////////////////////////////////////////


				// bTypeLine - TRUE �� �����ν� Line �� �̵� ��Ű���� �Ѵ�. ( ���� Cell�� ������ �̵� ��Ű�� ����. )
				//
				// Line �� ������ ���� ����� �� ���� �򵵷� �Ѵ�. ( waypoint_line�̵� )
				if ( !m_Parent->GetTuringPoint( vPosition, (*VisibleWaypoint).CellID, (*VisibleWaypoint).nLinkWall, (*VantagePoint).Position, (*TestWaypointNext).Position, m_EndPoint.Position, TRUE ) )
					return TRUE;

				//////////////////////////////////////////////////////////////////////////
				// NavigationCell::ClassifyPathToCell �� 
				// m_Side[i].ClassifyPoint(MotionPath.EndPointA()) -> m_Side[i].ClassifyPoint(MotionPath.EndPointA(),0.001f) �����۾����� ����
				// ���� ��Ȯ�� Cell �� ���� �ʿ䰡 ��������.
				//////////////////////////////////////////////////////////////////////////
				//
				//// bTypeLine - TRUE �� ����, float ����� vPosition ���� ����Cell �� �ȵ�� ���� �� �ִ�.
				//// ���� �̵� ��Ű���� ��ġ�� ���� ���� ���� ���� �� �־ �����۾� ��.
				//NavigationCell* pCell =	m_Parent->GetCell( (*VisibleWaypoint).CellID );
				//if( !pCell || 
				//	(pCell && !pCell->IsPointInCellCollumn( vPosition )) )
				//{
				//	DWORD dwCellID = m_Parent->FindClosestCell( vPosition );

				//	// �λﰢ���� ���� ������ ���� �� ���̹Ƿ� ���߿� �ϳ���.
				//	// ������ VisibleWaypoint �ʿ� ���ٰ� ������ ������ TestWaypointNext �� �־�� �Ѵ�.
				//	if ( (*TestWaypointNext).CellID == dwCellID )
				//	{
				//		// ��ȭ��.
				//		const WAYPOINT* pWayPoint = &(*VisibleWaypoint);
				//		WAYPOINT* pChangeWayPointPosition = const_cast<WAYPOINT*>( pWayPoint );
				//		pChangeWayPointPosition->Position = vPosition;
				//		pChangeWayPointPosition->CellID = dwCellID;

				//		return TRUE;
				//	}
				//	else
				//	{
				//		// ������ �ִ�. �׳� �������� �̵� ��Ű�� �ٶ�.
				//		return TRUE;
				//	}
				//}
			}
		}

		// ( waypoint_�����̵� && waypoint_Line����_NEXTNEXT ���� && waypoint_�����̵�_NEXTNEXT ���� ) or ( waypoint_�����̵� && NEXTNEXT ���� )	-> vPosition (waypoint_�����̵�)
		// ( waypoint_�����̵� && (waypoint_�����̵�_NEXTNEXT ���� || waypoint_�����̵�_NEXTNEXT ����) )											-> vPosition (waypoint_Line�̵�)
		// 
		const WAYPOINT* pWayPoint = &(*VisibleWaypoint);
		WAYPOINT* pChangeWayPointPosition = const_cast<WAYPOINT*>( pWayPoint );
		pChangeWayPointPosition->Position = vPosition;

		return TRUE;
	}

	return FALSE;
}