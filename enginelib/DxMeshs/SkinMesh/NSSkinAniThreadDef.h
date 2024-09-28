#pragma once

#include <boost/weak_ptr.hpp>
#include <tr1/memory>

class DxLandMan;

struct ActorNameCollision
{
	DWORD							m_dwCollMode;		// EMCOLLMODE
	// Swap �� Custom Data.
	boost::weak_ptr<DxLandMan>	m_wpLandMan[2];
	BOOL						m_bCollision[2];

	
	// MainThread ���� ����ص� �Ǵ� ������.
	BOOL							m_bCollisionResult;	

	//////////////////////////////////////////////////////////////////////////
	// Swap �� �⺻ Data.
	INT								m_nCalculateIndex;	// Thread �� ��� Index
	INT								m_nGetIndex;		// ��� Index


	ActorNameCollision( DWORD dwCollMode )
		: m_nCalculateIndex(-1)
		, m_nGetIndex(0)
		, m_bCollisionResult(TRUE)	// ó������ �� �̸�ǥ�ð� �� �������� �Ѵ�.
		, m_dwCollMode(dwCollMode)
	{
		m_bCollision[0] = TRUE;		// ó������ �� �̸�ǥ�ð� �� �������� �Ѵ�.
		m_bCollision[1] = TRUE;		// ó������ �� �̸�ǥ�ð� �� �������� �Ѵ�.
	}


	// Swap
	// �� �Լ� ���� ������ �߿���.
	// Thread �� Collision() �Ϸᰡ �Ǿ� ��.
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

		m_bCollisionResult = m_bCollision[m_nGetIndex];	// �浹�̸� �����.
	}


	// Thread �󿡼� ����.
	void Collision( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd );


	// �̸��� �����.
	void SetHideNaming()
	{
		m_bCollisionResult = TRUE;			// �̸�ǥ�ð� �� �������� �Ѵ�.
		m_bCollision[0] = TRUE;		// �̸�ǥ�ð� �� �������� �Ѵ�.
		m_bCollision[1] = TRUE;		// �̸�ǥ�ð� �� �������� �Ѵ�.
	}


	// Char �ʿ��� ���̴� ��.
	void InitData( const boost::shared_ptr<DxLandMan>& spLandMan )
	{
		m_wpLandMan[m_nGetIndex] = spLandMan;
	}


	// Render �ʿ��� ����.
	// Char �ʿ��� ���̴� ��.
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
// ����		: World �ʿ� ���õ� Diffuse �� PointLight ���� ������ Thread �󿡼� ��´�.
// �۾����� : Diffuse ��� Collision ����� �ְ�,
//				PointLight �� ��쵵 Tree �� Ÿ�鼭 Collisiion �� �ϱ� ������ ���ϰ� �ִ�.
//

struct ActorWorldLight
{
	//////////////////////////////////////////////////////////////////////////
	// Swap �� Custom Data.
	boost::weak_ptr<DxLandMan>		m_wpLandMan[2];
	D3DXVECTOR3						m_vDiffuse[2];
	D3DXVECTOR3						m_vPointLight[2];

	//////////////////////////////////////////////////////////////////////////
	// MainThread ���� ����ص� �Ǵ� ������
	D3DXVECTOR3						m_vDiffuseResult;
	D3DXVECTOR3						m_vPointLightResult;
	BOOL							m_bNotCalculateColor;

	//////////////////////////////////////////////////////////////////////////
	// Swap �� �⺻ Data.
	INT								m_nCalculateIndex;	// Thread �� ��� Index
	INT								m_nGetIndex;		// ��� Index


	// ������.
	ActorWorldLight();


	// ���� ������ ����� ���� �ʾҴ�.
	BOOL IsNotCalculateColor() const	{ return m_bNotCalculateColor; }

	// Swap
	// �� �Լ� ���� ������ �߿���.
	// Thread �� Collision() �Ϸᰡ �Ǿ� ��.
	void SwapActorWorldLight()
	{
		// -1 �� �ƴ϶��, ��� �� ������.
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


	// Thread �󿡼� ����.
	void CalculateActorWorldLight( const D3DXVECTOR3& vPos );


	// Char �ʿ��� ���̴� ��.
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