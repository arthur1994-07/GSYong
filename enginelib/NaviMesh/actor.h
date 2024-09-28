#ifndef _ENGINELIB_ACTION_H_
#define _ENGINELIB_ACTION_H_

#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>

#include "../DxTools/DxCustomTypes.h"
#include "./NavigationCell.h"
#include "./NavigationPath.h"
#include "./NavigationMesh.h"

class LayerCollision;

class Actor
{
private:
	typedef NavigationPath::WAYPOINT_VEC_CITER WAYPOINT_CITER;

protected:
	boost::shared_ptr<NavigationMesh>	m_spParent;				// the mesh we are sitting on
	DWORD			m_CurrentCellID;
	D3DXVECTOR3		m_Position;				// our 3D control point position
	D3DXVECTOR3		m_Movement;				// the current movement vector
	D3DXVECTOR3		m_NextPosition;			// 케릭터의 방향 설정을 위해서만 쓰인다. 무효한 값 = (FLT_MAX,FLT_MAX,FLT_MAX)
	D3DXVECTOR3		m_PreviousPosition;
	float			m_MaxSpeed;				// our maximum traveling distance per frame

	float			m_CorrectY;				// 노멀 Y가 0이면 이전 셀 기억해둬야 튀는 것 방지함

	NavigationPath	m_Path;					// our path object	
	WAYPOINT_CITER	m_NextWaypoint;			// ID of the next waypoint we will move to

	float			m_fCollRadius;			// 충돌이 COLL_TRUE_START_IN 로 된다면, 사이즈를 줄여서 빠져 나올 수 있도록 해주자.

public:
	Actor();
	~Actor();

	// *주의* 복사의 의미로 사용되지 않는다.
	// 생성자() 로 초기화시에 정상적인 초기화를 가능하게 하기 위해서 사용.
	Actor& operator= ( const Actor& Src )
	{
		Release();
		return *this;
	}

public:
	void Release();
	void Stop();
	
	void Create(boost::shared_ptr<NavigationMesh> spParent, const D3DXVECTOR3& Position, const bool bInstantMap = false, const DWORD CurrentCellID = -1);

	void SetPosition( const D3DXVECTOR3& Position, const DWORD CurrentCellID = -1, const bool bInstanceMap = false);
	BOOL GotoLocation( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2 );
	BOOL LineOfSightTest( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2 );

protected:
	void GotoLocation( const D3DXVECTOR3& Position, DWORD CellID );

public:
	const bool PathIsActive(void) const;
	void SetNextWayPoint( const NavigationPath::WAYPOINT& WayPoint );
	NavigationPath::WAYPOINT GetNextWayPoint() const;
	D3DXVECTOR3 GetTargetPosition();

public:
	HRESULT Update( const std::vector<boost::shared_ptr<LayerCollision>>& vecLayerCollision, float elapsedTime, BOOL bAccuracyUp = FALSE );
	//HRESULT Update_Room( float elapsedTime, BOOL bAccuracyUp );
	//HRESULT UpdateOld_Room( float elapsedTime, BOOL bAccuracyUp );

protected:
	HRESULT UpdateOld( const std::vector<boost::shared_ptr<LayerCollision>>& vecLayerCollision, float elapsedTime, BOOL bAccuracyUp );
	//HRESULT UpdateNew( float& elapsedTime, BOOL bAccuracyUp );

public:
	HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	const NavigationMesh* const GetParentMesh(void) const { return m_spParent.get(); }
	DWORD CurrentCellID() const { return m_CurrentCellID; }
	const D3DXVECTOR3& Position() const { return m_Position; }
	D3DXVECTOR3& Movement() { return m_Movement; }
	const D3DXVECTOR3& NextPosition() const { return m_NextPosition; }

	void SetMaxSpeed( float speed ) { m_MaxSpeed = speed; }
	float GetMaxSpeed() const { return m_MaxSpeed; }

	const D3DXVECTOR3& GetNextPosition() const { return m_NextWaypoint->Position; }
	const D3DXVECTOR3& GetPreviousPosition() const { return m_PreviousPosition; }
};

#endif // _ENGINELIB_ACTION_H_

