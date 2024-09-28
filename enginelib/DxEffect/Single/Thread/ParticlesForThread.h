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

	int				m_nNowSprite;		// ���� �༮�� ��������Ʈ ��ȣ
	BOOL			m_bSequenceRoop;

	D3DXMATRIX		m_matRotate;

	bool			m_bCollision;

	D3DXVECTOR3		m_vForceFull;		// �̵��ϰ� �ִ� �������� ������ �޴´�.
	float			m_fForceVar;

	BOOL				m_bPlay;
	D3DXMATRIX			m_matWorld;

	// Main Thread ���� ����,����,���õǰ�  Rendering �ȴ�.
	SPDXEFFSINGLEGROUP	m_spSingleGroup;	//	Ȱ�� ��ü.	��. ����. ���� ������ �ϳ�. �̤�


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

	// Sequence �� ���� FrameMove
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
	//	Note : ��ƼŬ �ý��� ��� ����.
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
	D3DXVECTOR3		m_vPlayTime;		// ���۰�, ��������, �Ź��� PlayTime
	D3DXVECTOR3		m_vSleepTime;		// ���۰�, ��������, �Ź��� SleepTime

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
	// Thread �󿡼� ���۵Ǵ� ��
private:
	boost::shared_ptr<ParticleEmitter>	m_spParicleEmitter;		// Emitter
	PARTICLE*							m_pParticleHead;		// Particles
	//boost::weak_ptr<DxEffSingleGroup>	m_wpThisGroup;			// 
	const DxEffectParticleSys*			m_rParticleSysSource;	// m_wpThisGroup �� ���� ��ȿ�ϸ� m_pParticleSysSource ���� ��ȿ�ϴ�.

	// ������ ����. ������ Thread �ʿ��� ������.
	// �̰� m_pParticleHead �� �ִ°� Main Thread ���� �ִ´�.
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
	// ���� ���� ���� ������.
	DWORD			m_uParticlesAlive;
	DWORD			m_uParticlesCreated;
	float			m_fEmissionResidue;

	// �ܺο��� �ִ� ���� �������� �����ؼ� ���̴� ����
	D3DXMATRIX		m_matPrevComb;
	D3DXMATRIX		m_matRotate;

	// �ܺο��� �ִ� ����.
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
	// Effect Thread ���� �۾��Ǵ� ��
	void FrameMove( float fElapsedTime ) OVERRIDE;
	void SetVertices( float fElapsedTime, const D3DXMATRIX& matCameraView ) OVERRIDE;

	// Main Thread ���� Swap �ϴ� ��
	void Swap( LPDIRECT3DDEVICEQ pd3dDevice ) OVERRIDE;	// Swap_ParticleCreateDelete

	// ����Thread ���� ȣ���� �Ѵ�.
public:
	BOOL					IsParticlesAlive() const;
	void					CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin ) const;
	DWORD					GetNumDrawPoints() const;
	const VERTEXCOLORTEX*	GetUsedVertices() const;
	const D3DXMATRIX&		GetMatWorld() const;
	const PARTICLE*			GetParticleHead() const;

	// �ܺο��� �ִ� ����.
	void SetCombine( const D3DXMATRIX& matCombine )		{ m_matCombine = matCombine; }
	void SetAlpha( float fAlpha )						{ m_fAlpha = fAlpha; }

	// ParticleEmitter ���� �Լ���
	const D3DXVECTOR3&	GetEmitterGLocation() const		{ return m_spParicleEmitter->GetGLocation(); }
	BOOL				GetEmitterRandomPlay()	const	{ return m_spParicleEmitter->GetRandomPlay(); }
	const D3DXMATRIX&	GetEmitterMatrixRandomR() const	{ return m_spParicleEmitter->GetMatrixRandomR(); }

public:
	ParticlesForThread( boost::shared_ptr<ParticleEmitter>& spParicleEmitter,
		boost::shared_ptr<DxEffSingleGroup>& spThisGroup,
		const DxEffectParticleSys* pEffectParticleSys );
	~ParticlesForThread();
};