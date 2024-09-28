#pragma once

#include <boost/weak_ptr.hpp>

#include "../../../../SigmaCore/Memory/ObjectPool.h"

class DxEffSingleGroup;


//////////////////////////////////////////////////////////////////////////
//							ParticlesForThread
//////////////////////////////////////////////////////////////////////////
class SingleForThread : public sc::PoolAllocMgr
{
	// Thread �󿡼� ���۵Ǵ� ��
protected:
	boost::weak_ptr<DxEffSingleGroup>	m_wpThisGroup;		// 

protected:
	int				m_nUSED;			// ���� �Ǵ� ��ȣ
	int				m_nBackUp;			// Thread ���� �۾��Ǵ� ��ȣ
	BOOL			m_bRenderCulling[2];




	//////////////////////////////////////////////////////////////////////////
	// Main Thread ���� Swap �ϴ� ��
	//
protected:
	BOOL IsSwap();

public:
	// SingleForThread ���� ���ȴٸ� ��� Effect �� Render �ʿ��� �� ȣ���� �ʿ��ϴ�.
	void SetRenderCullingTRUE();

	// virtual
	// Ư������ �۾� ������ ���� �� �״�� ����ϸ� �ȴ�.
	virtual void Swap( LPDIRECT3DDEVICEQ pd3dDevice );




	//////////////////////////////////////////////////////////////////////////
	// Effect Thread ���� �۾��Ǵ� ��
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