#pragma once

#include <boost/weak_ptr.hpp>

#include "../DxEffSingle.h"
#include "./SingleForThread.h"

class DxLandMan;
class DxEffectGround;


//////////////////////////////////////////////////////////////////////////
//							ParticlesForThread
//////////////////////////////////////////////////////////////////////////
class GroundForThread : public SingleForThread	//sc::PoolAllocMgr
{
	// Thread 상에서 동작되는 것
private:
	const DxEffectGround*				m_rGroundSource;	// m_wpThisGroup 에 값이 유효하면 m_rGroundSource 값도 유효하다.

	BOOL								m_bCollisionOK;
	D3DXVECTOR3							m_vCheckPoint;
	D3DXVECTOR3							m_vCheckPointPrev;
	float								m_fHeight;

	//////////////////////////////////////////////////////////////////////////
	// Main Thread
public:
	BOOL IsCollisionOK() const						{ return m_bCollisionOK; }
	float GetCllisionY() const						{ return m_fHeight; }
	void SetCheckPoint( const D3DXVECTOR3& vPos )	{ m_vCheckPoint = vPos; }


	//////////////////////////////////////////////////////////////////////////
	// Effect Thread
public:
	// override
	void FrameMove( float fElapsedTime ) OVERRIDE;


public:
	GroundForThread( boost::shared_ptr<DxEffSingleGroup>& spThisGroup, const DxEffectGround* pGroundSource );
	~GroundForThread();
};