#pragma once

#include <boost/weak_ptr.hpp>

#include "../DxEffSingle.h"
#include "./SingleForThread.h"

class ParticlesSwapData;
class DxEffectParticleSys;
class EmitterBaseSetting;
class ParticlesBaseSetting;

//////////////////////////////////////////////////////////////////////////
//
struct PARTICLE : public sc::PoolAllocMgr
{
public:
	D3DXVECTOR3		m_vPrevLocation;
	D3DXVECTOR3		m_vLocation;
	D3DXVECTOR3		m_vVelocity;

	float			m_fAlphaTime1;
	float			m_fAlphaTime2;

	float			m_fAlpha;
	float			m_fAlphaDelta1;
	float			m_fAlphaDelta2;
	float			m_fAlphaDelta3;

	D3DXVECTOR4		m_cColor;
	D3DXVECTOR3		m_cColorDelta;

	float			m_fAge;
	float			m_fLifetime;

	float			m_fSize;
	float			m_fSizeDelta;

	float			m_fGravity;
	float			m_fGravityDelta;

	D3DXVECTOR2		m_vTexScale;
	D3DXVECTOR2		m_vTexScaleDelta;

	float			m_fTexRotateAngel;
	float			m_fTexRotateAngelSum;

	int				m_nNowSprite;		// 지금 녀석의 스프라이트 번호
	BOOL			m_bSequenceRoop;

	D3DXMATRIX		m_matRotate;

	bool			m_bCollision;

	D3DXVECTOR3		m_vForceFull;		// 이동하고 있는 방향으로 영향을 받는다.
	float			m_fForceVar;

	BOOL				m_bPlay;
	D3DXMATRIX			m_matWorld;

	// Main Thread 에서 생성,삭제,셋팅되고  Rendering 된다.
	SPDXEFFSINGLEGROUP	m_spSingleGroup;	//	활성 개체.	음. 난감. 새로 만들어야 하네. ㅜㅜ


public:
	PARTICLE();
	~PARTICLE();

public:
	BOOL FrameMove( float fElapsedTime, 
		const D3DXVECTOR3 *pAttractLoc, 
		const D3DXVECTOR3 *pGravity, 
		BOOL bFromTo, 
		const D3DXVECTOR3& vTARGET_POS,
		const D3DXVECTOR3& vSTART_POS,
		DWORD dwFlag );

	// Sequence 일 관련 FrameMove
	void FrameMove_Sequence_Loop();
	void FrameMove_Sequence();

	void CreateSingleGroup_MainThread( LPDIRECT3DDEVICEQ pd3dDevice, DxEffSinglePropGroupResource& spPropGroup, const EmitterBaseSetting& sEmitterSetting );

	BOOL IsSingleGroupPlay() const { return m_bPlay; }

public:
	PARTICLE		*m_pNext;
};



//////////////////////////////////////////////////////////////////////////
//
class ParticleEmitter : public sc::PoolAllocMgr
{
	//	Note : 파티클 시스템 덩어리 제어.
	//
public:
	D3DXVECTOR3		m_vGLocation;
	D3DXVECTOR3		m_vGPrevLocation;

	BOOL			m_bRandomPlay;
	float			m_fRandomPlayTime;
	int				m_nCountR;
	D3DXMATRIX		m_matRandomR;

	float			m_fRotateLAngelSum;
	float			m_fElapsedTimeSum;
	float			m_fSequenceTime;

	D3DXVECTOR3		m_vGVelocity;
	D3DXVECTOR3		m_vGGravity;
	float			m_fLength;
	D3DXVECTOR3		m_vPlayTime;		// 시작값, 랜덤범위, 매번의 PlayTime
	D3DXVECTOR3		m_vSleepTime;		// 시작값, 랜덤범위, 매번의 SleepTime

public:
	void FrameMove( DWORD dwRunFlag, float fElapsedTime, const DxEffectParticleSys*	pParticleSysSource, PARTICLE* pParticleHead );

	void NewParticleCreateBase( const DxEffSingleGroup* pThisGroup, const DxEffectParticleSys*	pParticleSysSource, D3DXMATRIX &matComb, D3DXVECTOR3 vPos_NowPrev, float fElapsedTime );

	void NewParticleCreate
	( 
		PARTICLE* pParticle, 
		const DxEffSingleGroup* pThisGroup, 
		const EmitterBaseSetting& sEmitterSetting, 
		const D3DXMATRIX& matComb, 
		const D3DXMATRIX& matLocal,
		D3DXVECTOR3 vPos_NowPrev, 
		float fElapsedTime 
	);

public:
	const D3DXVECTOR3&	GetGLocation() const		{ return m_vGLocation; }
	BOOL				GetRandomPlay()	const		{ return m_bRandomPlay; }
	const D3DXMATRIX&	GetMatrixRandomR() const	{ return m_matRandomR; }

public:
	ParticleEmitter( const D3DXVECTOR3&	vGVelocity,
		const D3DXVECTOR3&	vGGravity,
		float				fLength,
		const D3DXVECTOR3&	vPlayTime,
		const D3DXVECTOR3&	vSleepTime );
	~ParticleEmitter();
};





//////////////////////////////////////////////////////////////////////////
//							ParticlesForThread
//////////////////////////////////////////////////////////////////////////
class ParticlesForThread : public SingleForThread	//sc::PoolAllocMgr
{
	// Thread 상에서 동작되는 것
private:
	boost::shared_ptr<ParticleEmitter>	m_spParicleEmitter;		// Emitter
	PARTICLE*							m_pParticleHead;		// Particles
	//boost::weak_ptr<DxEffSingleGroup>	m_wpThisGroup;			// 
	const DxEffectParticleSys*			m_rParticleSysSource;	// m_wpThisGroup 에 값이 유효하면 m_pParticleSysSource 값도 유효하다.

	// 생성과 삭제. 정보는 Thread 쪽에서 얻지만.
	// 이걸 m_pParticleHead 에 넣는건 Main Thread 에서 넣는다.
private:
	std::vector<PARTICLE*>					m_vecCreateParticles;
	std::vector<PARTICLE*>					m_vecDeleteParticles;

private:
	enum
	{
		MAX_PARTICLES = 300,
	};

	VERTEXCOLORTEX	m_sVertices[2][MAX_PARTICLES*4];
	DWORD			m_dwDrawPoints[2];
	D3DXMATRIX		m_matWorld[2];

private:
	// 생성 갯수 관련 변수들.
	DWORD			m_uParticlesAlive;
	DWORD			m_uParticlesCreated;
	float			m_fEmissionResidue;

	// 외부에서 주는 값을 바탕으로 응용해서 쓰이는 변수
	D3DXMATRIX		m_matPrevComb;
	D3DXMATRIX		m_matRotate;

	// 외부에서 주는 값들.
	D3DXMATRIX		m_matCombine;
	float			m_fAlpha;



private:
	void NewParticleCreate( const D3DXMATRIX &matComb, const D3DXVECTOR3& vPos_NowPrev, float fElapsedTime );
	void SetMatrix ( D3DXMATRIX &matWorld, const D3DXMATRIX &matComb );

public:
	void PutSleepParticles ( PARTICLE* pParticles );
	void PutSleepParticle ( PARTICLE* pParticle );

	// override
public:
	// Effect Thread 에서 작업되는 것
	void FrameMove( float fElapsedTime ) OVERRIDE;
	void SetVertices( float fElapsedTime, const D3DXMATRIX& matCameraView ) OVERRIDE;

	// Main Thread 에서 Swap 하는 것
	void Swap( LPDIRECT3DDEVICEQ pd3dDevice ) OVERRIDE;	// Swap_ParticleCreateDelete

	// 메인Thread 에서 호출을 한다.
public:
	BOOL					IsParticlesAlive() const;
	void					CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin ) const;
	DWORD					GetNumDrawPoints() const;
	const VERTEXCOLORTEX*	GetUsedVertices() const;
	const D3DXMATRIX&		GetMatWorld() const;
	const PARTICLE*			GetParticleHead() const;

	// 외부에서 주는 값들.
	void SetCombine( const D3DXMATRIX& matCombine )		{ m_matCombine = matCombine; }
	void SetAlpha( float fAlpha )						{ m_fAlpha = fAlpha; }

	// ParticleEmitter 관련 함수들
	const D3DXVECTOR3&	GetEmitterGLocation() const		{ return m_spParicleEmitter->GetGLocation(); }
	BOOL				GetEmitterRandomPlay()	const	{ return m_spParicleEmitter->GetRandomPlay(); }
	const D3DXMATRIX&	GetEmitterMatrixRandomR() const	{ return m_spParicleEmitter->GetMatrixRandomR(); }

public:
	ParticlesForThread( boost::shared_ptr<ParticleEmitter>& spParicleEmitter,
		boost::shared_ptr<DxEffSingleGroup>& spThisGroup,
		const DxEffectParticleSys* pEffectParticleSys );
	~ParticlesForThread();
};