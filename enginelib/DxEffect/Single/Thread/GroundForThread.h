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
	// Thread �󿡼� ���۵Ǵ� ��
private:
	const DxEffectGround*				m_rGroundSource;	// m_wpThisGroup �� ���� ��ȿ�ϸ� m_rGroundSource ���� ��ȿ�ϴ�.

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