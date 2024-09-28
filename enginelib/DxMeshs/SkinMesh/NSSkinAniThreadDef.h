#pragma once

#include <boost/weak_ptr.hpp>
#include <tr1/memory>

class DxLandMan;

struct ActorNameCollision
{
	DWORD							m_dwCollMode;		// EMCOLLMODE
	// Swap 용 Custom Data.
	boost::weak_ptr<DxLandMan>	m_wpLandMan[2];
	BOOL						m_bCollision[2];

	
	// MainThread 에서 사용해도 되는 데이터.
	BOOL							m_bCollisionResult;	

	//////////////////////////////////////////////////////////////////////////
	// Swap 용 기본 Data.
	INT								m_nCalculateIndex;	// Thread 상 계산 Index
	INT								m_nGetIndex;		// 등록 Index


	ActorNameCollision( DWORD dwCollMode )
		: m_nCalculateIndex(-1)
		, m_nGetIndex(0)
		, m_bCollisionResult(TRUE)	// 처음봤을 때 이름표시가 안 나오도록 한다.
		, m_dwCollMode(dwCollMode)
	{
		m_bCollision[0] = TRUE;		// 처음봤을 때 이름표시가 안 나오도록 한다.
		m_bCollision[1] = TRUE;		// 처음봤을 때 이름표시가 안 나오도록 한다.
	}


	// Swap
	// 이 함수 사용시 순서가 중요함.
	// Thread 상 Collision() 완료가 되야 함.
	void SwapNameCollision()
	{
		if ( m_nGetIndex == 0 )
		{
			m_nGetIndex = 1;
			m_nCalculateIndex = 0;
		}
		else
		{
			m_nGetIndex = 0;
			m_nCalculateIndex = 1;
		}

		m_bCollisionResult = m_bCollision[m_nGetIndex];	// 충돌이면 숨긴다.
	}


	// Thread 상에서 쓰임.
	void Collision( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd );


	// 이름을 숨긴다.
	void SetHideNaming()
	{
		m_bCollisionResult = TRUE;			// 이름표시가 안 나오도록 한다.
		m_bCollision[0] = TRUE;		// 이름표시가 안 나오도록 한다.
		m_bCollision[1] = TRUE;		// 이름표시가 안 나오도록 한다.
	}


	// Char 쪽에서 쓰이는 것.
	void InitData( const boost::shared_ptr<DxLandMan>& spLandMan )
	{
		m_wpLandMan[m_nGetIndex] = spLandMan;
	}


	// Render 쪽에서 사용됨.
	// Char 쪽에서 쓰이는 것.
	BOOL GetCollsion() const;
};


struct ActorNameCollisionThread
{
	std::tr1::shared_ptr<ActorNameCollision>	m_spNameCollision;
	D3DXVECTOR3									m_vStart;
	D3DXVECTOR3									m_vEnd;

	ActorNameCollisionThread( std::tr1::shared_ptr<ActorNameCollision>& spNameCollision,
								const D3DXVECTOR3& vStart,
								const D3DXVECTOR3& vEnd )
		: m_spNameCollision( spNameCollision )
		, m_vStart( vStart )
		, m_vEnd( vEnd )
	{
	}

	void Collision();
	void SwapNameCollision();
};




//////////////////////////////////////////////////////////////////////////
//				ActorWorldLight	& ActorWorldLightThread
//
// 설명		: World 쪽에 셋팅된 Diffuse 와 PointLight 들의 정보를 Thread 상에서 얻는다.
// 작업이유 : Diffuse 경우 Collision 계산이 있고,
//				PointLight 의 경우도 Tree 도 타면서 Collisiion 도 하기 때문에 부하가 있다.
//

struct ActorWorldLight
{
	//////////////////////////////////////////////////////////////////////////
	// Swap 용 Custom Data.
	boost::weak_ptr<DxLandMan>		m_wpLandMan[2];
	D3DXVECTOR3						m_vDiffuse[2];
	D3DXVECTOR3						m_vPointLight[2];

	//////////////////////////////////////////////////////////////////////////
	// MainThread 에서 사용해도 되는 데이터
	D3DXVECTOR3						m_vDiffuseResult;
	D3DXVECTOR3						m_vPointLightResult;
	BOOL							m_bNotCalculateColor;

	//////////////////////////////////////////////////////////////////////////
	// Swap 용 기본 Data.
	INT								m_nCalculateIndex;	// Thread 상 계산 Index
	INT								m_nGetIndex;		// 등록 Index


	// 생성자.
	ActorWorldLight();


	// 아직 라이팅 계산이 되지 않았다.
	BOOL IsNotCalculateColor() const	{ return m_bNotCalculateColor; }

	// Swap
	// 이 함수 사용시 순서가 중요함.
	// Thread 상 Collision() 완료가 되야 함.
	void SwapActorWorldLight()
	{
		// -1 이 아니라면, 계산 된 상태임.
		if ( m_nCalculateIndex != -1 )
		{
			m_bNotCalculateColor = FALSE;
		}

		if ( m_nGetIndex == 0 )
		{
			m_nGetIndex = 1;
			m_nCalculateIndex = 0;
		}
		else
		{
			m_nGetIndex = 0;
			m_nCalculateIndex = 1;
		}

		m_vDiffuseResult = m_vDiffuse[m_nGetIndex];
		m_vPointLightResult = m_vPointLight[m_nGetIndex];
	}


	// Thread 상에서 쓰임.
	void CalculateActorWorldLight( const D3DXVECTOR3& vPos );


	// Char 쪽에서 쓰이는 것.
	void InitData( const boost::shared_ptr<DxLandMan>& spLandMan );
};

struct ActorWorldLightThread
{
	std::tr1::shared_ptr<ActorWorldLight>	m_spActorWorldLight;
	D3DXVECTOR3								m_vPos;

	ActorWorldLightThread( std::tr1::shared_ptr<ActorWorldLight> &spActorWorldLight, const D3DXVECTOR3 &vPos )
		: m_spActorWorldLight(spActorWorldLight)
		, m_vPos(vPos)
	{
	}

	void CalculateActorWorldLight();
	void SwapActorWorldLight();
};