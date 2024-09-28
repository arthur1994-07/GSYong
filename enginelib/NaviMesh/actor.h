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
	D3DXVECTOR3		m_NextPosition;			// �ɸ����� ���� ������ ���ؼ��� ���δ�. ��ȿ�� �� = (FLT_MAX,FLT_MAX,FLT_MAX)
	D3DXVECTOR3		m_PreviousPosition;
	float			m_MaxSpeed;				// our maximum traveling distance per frame

	float			m_CorrectY;				// ��� Y�� 0�̸� ���� �� ����ص־� Ƣ�� �� ������

	NavigationPath	m_Path;					// our path object	
	WAYPOINT_CITER	m_NextWaypoint;			// ID of the next waypoint we will move to

	float			m_fCollRadius;			// �浹�� COLL_TRUE_START_IN �� �ȴٸ�, ����� �ٿ��� ���� ���� �� �ֵ��� ������.

public:
	Actor();
	~Actor();

	// *����* ������ �ǹ̷� ������ �ʴ´�.
	// ������() �� �ʱ�ȭ�ÿ� �������� �ʱ�ȭ�� �����ϰ� �ϱ� ���ؼ� ���.
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

