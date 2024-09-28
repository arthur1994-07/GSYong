#pragma once

#include <boost/weak_ptr.hpp>

#include "../../../../SigmaCore/Memory/ObjectPool.h"

class DxEffSingleGroup;


//////////////////////////////////////////////////////////////////////////
//							ParticlesForThread
//////////////////////////////////////////////////////////////////////////
class SingleForThread : public sc::PoolAllocMgr
{
	// Thread 상에서 동작되는 것
protected:
	boost::weak_ptr<DxEffSingleGroup>	m_wpThisGroup;		// 

protected:
	int				m_nUSED;			// 렌더 되는 번호
	int				m_nBackUp;			// Thread 에서 작업되는 번호
	BOOL			m_bRenderCulling[2];




	//////////////////////////////////////////////////////////////////////////
	// Main Thread 에서 Swap 하는 것
	//
protected:
	BOOL IsSwap();

public:
	// SingleForThread 것이 사용된다면 사용 Effect 의 Render 쪽에서 꼭 호출이 필요하다.
	void SetRenderCullingTRUE();

	// virtual
	// 특별한일 작업 없으면 현재 것 그대로 사용하면 된다.
	virtual void Swap( LPDIRECT3DDEVICEQ pd3dDevice );




	//////////////////////////////////////////////////////////////////////////
	// Effect Thread 에서 작업되는 것
	//
public:
	BOOL IsRenderCulling() const;

	// virtual
public:
	virtual void FrameMove( float fElapsedTime )										{}
	virtual void SetVertices( float fElapsedTime, const D3DXMATRIX& matCameraView )		{}




public:
	SingleForThread( boost::shared_ptr<DxEffSingleGroup>& spThisGroup );
	~SingleForThread();
};