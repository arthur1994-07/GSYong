#ifndef NAVIGATIONPATH_H
#define NAVIGATIONPATH_H

#include <list>
#include "./navigationcell.h"
#include "./navigationmesh.h"

// forward declaration of our parents
class NavigationPath
{
public:
	struct WAYPOINT
	{
		D3DXVECTOR3 Position;
		DWORD CellID;
		int nLinkWall;
	};

	typedef std::vector< WAYPOINT >			WAYPOINT_VEC;
	typedef WAYPOINT_VEC::iterator			WAYPOINT_VEC_ITER;
	typedef WAYPOINT_VEC::const_iterator	WAYPOINT_VEC_CITER;

private:
	NavigationMesh*	m_Parent;
	WAYPOINT		m_StartPoint;
	WAYPOINT		m_EndPoint;
    WAYPOINT_VEC	m_vecWaypoint;
	DWORD			m_dwTuringPositionIndex;

public:
	NavigationPath();
	~NavigationPath();

private:
	NavigationPath( const NavigationPath& Src );
	NavigationPath& operator= ( const NavigationPath& Src );

public:
	void Clear();

	void Setup( NavigationMesh* Parent, const D3DXVECTOR3& StartPoint, DWORD StartID, const D3DXVECTOR3& EndPoint, DWORD EndID, DWORD dwWayPointSize );
	void AddWayPoint( const D3DXVECTOR3& Point, DWORD CellID, int nLinkWall );
	void EndPath();
	
	// 보통은 변의 중심점으로 이동되면서 목적 위치가 바뀌지 않는데, 
	// 이동경로 최적화 하면서 삼각형의 꼭지점으로 이동되도록 변경이 필요할 때가 있다. 
	// 그 때 m_vecWaypoint 내부값의 WAYPOINT::Position 값이 변경된다.
	WAYPOINT_VEC_CITER GetFurthestVisibleWayPoint( const WAYPOINT_VEC_CITER& VantagePoint );

	// VisibleWaypoint 의 최적 경로를 찾고 없을 경우 좀 더 이전 waypoint 를 순환해서 찾는다.
	WAYPOINT_VEC_CITER GetFurthestVisibleWayPointChild( const WAYPOINT_VEC_CITER& VantagePoint,
														const WAYPOINT_VEC_CITER& FisrtTargetWaypoint, 
														const WAYPOINT_VEC_CITER& VisibleWaypoint, 
														int& nCount );

	// VisibleWaypoint 의 Position 값이 최적화 된 위치로 변하게 된다.
	// 리턴값 : 위치가 변하면 TRUE ( 현재것을 그대로 사용해도 됨 ) , 변하지 않으면 FALSE ( 좀 더 최적화 된 길을 찾아볼 여지가 있음 )
	BOOL ChangePosition_FurthestVisibleWayPoint( const WAYPOINT_VEC_CITER& VantagePoint, const WAYPOINT_VEC_CITER& VisibleWaypoint );

public:
	NavigationMesh* Parent() const { return m_Parent; }
	const WAYPOINT& StartPoint() const { return m_StartPoint; }
	const WAYPOINT& EndPoint() const { return m_EndPoint; }
	const WAYPOINT_VEC& WaypointList() const { return m_vecWaypoint; }
};

#endif  // end of file      ( NavigationPath.h )