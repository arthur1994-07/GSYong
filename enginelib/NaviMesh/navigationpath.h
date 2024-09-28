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
	
	// ������ ���� �߽������� �̵��Ǹ鼭 ���� ��ġ�� �ٲ��� �ʴµ�, 
	// �̵���� ����ȭ �ϸ鼭 �ﰢ���� ���������� �̵��ǵ��� ������ �ʿ��� ���� �ִ�. 
	// �� �� m_vecWaypoint ���ΰ��� WAYPOINT::Position ���� ����ȴ�.
	WAYPOINT_VEC_CITER GetFurthestVisibleWayPoint( const WAYPOINT_VEC_CITER& VantagePoint );

	// VisibleWaypoint �� ���� ��θ� ã�� ���� ��� �� �� ���� waypoint �� ��ȯ�ؼ� ã�´�.
	WAYPOINT_VEC_CITER GetFurthestVisibleWayPointChild( const WAYPOINT_VEC_CITER& VantagePoint,
														const WAYPOINT_VEC_CITER& FisrtTargetWaypoint, 
														const WAYPOINT_VEC_CITER& VisibleWaypoint, 
														int& nCount );

	// VisibleWaypoint �� Position ���� ����ȭ �� ��ġ�� ���ϰ� �ȴ�.
	// ���ϰ� : ��ġ�� ���ϸ� TRUE ( ������� �״�� ����ص� �� ) , ������ ������ FALSE ( �� �� ����ȭ �� ���� ã�ƺ� ������ ���� )
	BOOL ChangePosition_FurthestVisibleWayPoint( const WAYPOINT_VEC_CITER& VantagePoint, const WAYPOINT_VEC_CITER& VisibleWaypoint );

public:
	NavigationMesh* Parent() const { return m_Parent; }
	const WAYPOINT& StartPoint() const { return m_StartPoint; }
	const WAYPOINT& EndPoint() const { return m_EndPoint; }
	const WAYPOINT_VEC& WaypointList() const { return m_vecWaypoint; }
};

#endif  // end of file      ( NavigationPath.h )