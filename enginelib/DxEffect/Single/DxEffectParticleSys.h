#ifndef __CPARTICLESYSTEM_H_
#define __CPARTICLESYSTEM_H_

#pragma once

#include "../../DxMeshs/dxsimplemeshman.h"
#include "../../DxTools/TextureManager.h"
#include "./Thread/ParticlesForThread.h"
#include "./DxEffectParticleSysDef.h"
#include "./DxEffSingle.h"

 
// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  ( x = (x<min ? min : x<max ? x : max) )

struct PARTICLESYS_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_PARTICE, };

	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY				// Ver.107
	{
		//	Note : 파티클 시스템.
		//
		D3DXVECTOR3		m_vGVelocity;
		D3DXVECTOR3		m_vGGravityStart;
		D3DXVECTOR3		m_vGGravityEnd;

		D3DXVECTOR2		m_vPlayTime;
		D3DXVECTOR2		m_vSleepTime;

		DWORD			m_dwFlag;
		DWORD			m_dwFlagEX;		// PARTICLE_FLAG_EX		// ver.107추가

		int				m_iCenterPoint;

		D3DXVECTOR3		m_vRange;		// 범위	회전
		float			m_fRangeRate;	// 속 안에 파티클이 생성 안됨.
		float			m_fRotateAngel;

		float			m_fRotateLAngel;

		float			m_fRotateLRate1;
		float			m_fRotateLRate2;

		float			m_fLengthStart;
		float			m_fLengthMid1;
		float			m_fLengthMid2;
		float			m_fLengthEnd;

		DWORD			m_uParticlesPerSec;

		char			m_szTexture[STRING_NUM_256];
		char			m_szMeshFile[STRING_NUM_256];
		char			m_szEffFile[STRING_NUM_256];			// ver.106추가

		//	Note : 파티클.
		//
		D3DXVECTOR3		m_vWindDir;		//  바람 디폴트 추가

		float			m_fTexRotateAngel;

		D3DXVECTOR4		m_vTexScale;

		float			m_fGravityStart;
		float			m_fGravityVar;
		float			m_fGravityEnd;

		float			m_fSizeStart;
		float			m_fSizeVar;
		float			m_fSizeEnd;

		float			m_fAlphaRate1;
		float			m_fAlphaRate2;

		float			m_fAlphaStart;
		float			m_fAlphaMid1;
		float			m_fAlphaMid2;
		float			m_fAlphaEnd;

		D3DXCOLOR		m_cColorStart;
		D3DXCOLOR		m_cColorVar;
		D3DXCOLOR		m_cColorEnd;

		float			m_fSpeed;
		float			m_fSpeedVar;

		float			m_fLife;
		float			m_fLifeVar;

		float			m_fTheta;

		int				m_nCol;			// 행
		int				m_nRow;			// 열
		float			m_fAniTime;		// 설정 시간

		int				m_nBlend;
		int				m_nPower;

		PROPERTY()
		{
			memset( m_szTexture, 0, sizeof(char)*STRING_NUM_256 );
			memset( m_szMeshFile, 0, sizeof(char)*STRING_NUM_256 );
			memset( m_szEffFile, 0, sizeof(char)*STRING_NUM_256 );
		};
	};

	union
	{
		struct
		{
			PROPERTY		m_Property;
		};

		struct
		{
			D3DXVECTOR3		m_vGVelocity;
			D3DXVECTOR3		m_vGGravityStart;
			D3DXVECTOR3		m_vGGravityEnd;

			D3DXVECTOR2		m_vPlayTime;
			D3DXVECTOR2		m_vSleepTime;

			DWORD			m_dwFlag;
			DWORD			m_dwFlagEX;

			int				m_iCenterPoint;

			D3DXVECTOR3		m_vRange;		// 범위	회전
			float			m_fRangeRate;	// 속 안에 파티클이 생성 안됨.
			float			m_fRotateAngel;

			float			m_fRotateLAngel;

			float			m_fRotateLRate1;
			float			m_fRotateLRate2;

			float			m_fLengthStart;
			float			m_fLengthMid1;
			float			m_fLengthMid2;
			float			m_fLengthEnd;

			DWORD			m_uParticlesPerSec;

			char			m_szTexture[STRING_NUM_256];
			char			m_szMeshFile[STRING_NUM_256];
			char			m_szEffFile[STRING_NUM_256];

			//	Note : 파티클.
			//
			D3DXVECTOR3		m_vWindDir;		//  바람 디폴트 추가

			float			m_fTexRotateAngel;

			D3DXVECTOR4		m_vTexScale;

			float			m_fGravityStart;
			float			m_fGravityVar;
			float			m_fGravityEnd;

			float			m_fSizeStart;
			float			m_fSizeVar;
			float			m_fSizeEnd;

			float			m_fAlphaRate1;
			float			m_fAlphaRate2;

			float			m_fAlphaStart;
			float			m_fAlphaMid1;
			float			m_fAlphaMid2;
			float			m_fAlphaEnd;

			D3DXCOLOR		m_cColorStart;
			D3DXCOLOR		m_cColorVar;
			D3DXCOLOR		m_cColorEnd;

			float			m_fSpeed;
			float			m_fSpeedVar;

			float			m_fLife;
			float			m_fLifeVar;

			float			m_fTheta;

			int				m_nCol;			// 행
			int				m_nRow;			// 열
			float			m_fAniTime;		// 설정 시간

			int				m_nBlend;
			int				m_nPower;
		};
	};

	struct D3DMESHVERTEX
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vNormal;
		float			tu, tv;		
		
		static const DWORD FVF;
	};

	//DxSimMesh*				m_pMesh;
	DxSimMeshResource				m_SimMeshRes;
	TextureResource					m_textureRes;
	DxEffSinglePropGroupResource	m_sPropGroupRes;
	BOOL							m_bBackUpCache;

	 

	//	Note : 이팩트 생성자.
	//
	virtual DxEffSingle* NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup );
	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE );
	virtual HRESULT SaveFile ( sc::SerialFile &SFile );

	virtual void importMobileObjectFromXFile();

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache);
	virtual HRESULT DeleteDeviceObjects();

	PARTICLESYS_PROPERTY () : EFF_PROPERTY(),
		m_vGVelocity(0,0,0),
		m_vGGravityStart(0.f,0.f,0.f),
		m_vGGravityEnd(0.f,0.f,0.f),

		m_vPlayTime(0.5f,5.f),
		m_vSleepTime(0.5f,5.f),

		m_vRange(5.f,0.f,5.f),
		m_fRangeRate(0.0f),
		m_fRotateAngel(200.f),

		m_fRotateLAngel(2.f),

		m_fRotateLRate1(25.f),
		m_fRotateLRate2(75.f),

		m_fLengthStart(0.f),
		m_fLengthMid1(2.f),
		m_fLengthMid2(1.f),
		m_fLengthEnd(0.f),
		
		m_uParticlesPerSec(10),

		m_vWindDir(0.f,0.f,0.f),

		m_fTexRotateAngel(10.f),

		m_vTexScale(1.f,1.f,1.f,30.f),	// 

		m_fGravityStart(0.0f),
		m_fGravityVar(0.0f),
		m_fGravityEnd(0.0f),

		m_fSizeStart(15.0f),
		m_fSizeVar(0.0f),
		m_fSizeEnd(15.0f),

		m_fAlphaRate1(25.0f),
		m_fAlphaRate2(75.0f),

		m_fAlphaStart(0.0f),
		m_fAlphaMid1(1.0f),
		m_fAlphaMid2(1.0f),
		m_fAlphaEnd(0.0f),

		m_cColorStart(1.0f,1.0f,1.0f,1.0f),
		m_cColorVar(0.0f,0.0f,0.0f,0.0f),
		m_cColorEnd(1.0f,1.0f,1.0f,1.0f),

		m_fSpeed(5.0f),
		m_fSpeedVar(0.0f),

		m_fLife(2.0f),
		m_fLifeVar(0.5f),

		m_fTheta(360.f),

		m_nCol(4),			// 행
		m_nRow(4),		// 열
		m_fAniTime(0.03f),		// 설정 시간

		m_nPower(1),
		m_nBlend(0)

		, m_bBackUpCache(FALSE)
	{
		m_dwFlag = 0;
		m_dwFlag |= USETEXTURE;
		m_dwFlagEX = 0L;
		m_iCenterPoint = 1;		// 위, 중심, 아래

		StringCchCopy( m_szTexture,		STRING_NUM_256, "_Eff_flare.tga" );
		StringCchCopy( m_szMeshFile,	STRING_NUM_256, "sd_stone.X" );
		StringCchCopy( m_szEffFile,		STRING_NUM_256, "" );
	}
    virtual ~PARTICLESYS_PROPERTY()
    {
    }
};





//////////////////////////////////////////////////////////////////////////
//
//					EmitterBaseSetting
//					한번 셋팅되면 변하지 않는 값
//
//////////////////////////////////////////////////////////////////////////
class EmitterBaseSetting : public sc::PoolAllocMgr
{
private:
	// Emitter 움직임과 이동 관련
	D3DXVECTOR3		m_vGGravityDelta;
	DWORD			m_dwFlag;
	DWORD			m_dwFlagEX;
	int				m_iCenterPoint;
	D3DXVECTOR3		m_vRange;		// 범위	회전
	float			m_fRangeRate;	// 속 안에 파티클이 생성 안됨.
	float			m_fRotateLAngel;
	float			m_fRotateLDelta1;
	float			m_fRotateLDelta2;
	float			m_fRotateLDelta3;
	float			m_fRotateLTime1;
	float			m_fRotateLTime2;

public:
	const D3DXVECTOR3& GetGGravityDelta() const		{ return m_vGGravityDelta; }
	DWORD GetFlag() const							{ return m_dwFlag; }
	BOOL IsFlagEX( PARTICLE_FLAG_EX emFlag ) const	{ return (m_dwFlagEX&emFlag) ? TRUE : FALSE; }
	int GetCenterPoint() const						{ return m_iCenterPoint; }
	const D3DXVECTOR3& GetRange() const				{ return m_vRange; }
	float GetRangeRate() const						{ return m_fRangeRate; }
	float GetRotateLAngel()	 const					{ return m_fRotateLAngel; }
	float GetRotateLDelta1() const					{ return m_fRotateLDelta1; }
	float GetRotateLDelta2() const					{ return m_fRotateLDelta2; }
	float GetRotateLDelta3() const					{ return m_fRotateLDelta3; }
	float GetRotateLTime1() const					{ return m_fRotateLTime1; }
	float GetRotateLTime2() const					{ return m_fRotateLTime2; }

public:
	EmitterBaseSetting( const D3DXVECTOR3& vGGravityDelta,
						DWORD dwFlag,
						DWORD dwFlagEX,
						int iCenterPoint,
						const D3DXVECTOR3& vRange,
						float fRangeRate,
						float fRotateLAngel,
						float fRotateLDelta1,
						float fRotateLDelta2,
						float fRotateLDelta3,
						float fRotateLTime1,
						float fRotateLTime2 )
		: m_vGGravityDelta(vGGravityDelta)
		, m_dwFlag(dwFlag)
		, m_dwFlagEX(dwFlagEX)
		, m_iCenterPoint(iCenterPoint)
		, m_vRange(vRange)
		, m_fRangeRate(fRangeRate)
		, m_fRotateLAngel(fRotateLAngel)
		, m_fRotateLDelta1(fRotateLDelta1)		// 범위	회전
		, m_fRotateLDelta2(fRotateLDelta2)
		, m_fRotateLDelta3(fRotateLDelta3)
		, m_fRotateLTime1(fRotateLTime1)
		, m_fRotateLTime2(fRotateLTime2)
	{
	}
};





//////////////////////////////////////////////////////////////////////////
//
//					ParticlesBaseSetting
//					한번 셋팅되면 변하지 않는 값
//
//////////////////////////////////////////////////////////////////////////
class ParticlesBaseSetting : public sc::PoolAllocMgr
{
	//	Note : 파티클 각각 설정.
	//
private:
	DWORD			m_uParticlesPerSec;

	D3DXVECTOR3		m_vWindDir;		//  바람 디폴트 추가

	float			m_fTexRotateAngel;

	float			m_fGravityStart;
	float			m_fGravityVar;
	float			m_fGravityEnd;

	float			m_fSizeStart;
	float			m_fSizeVar;
	float			m_fSizeEnd;

	float			m_fAlpha;

	D3DXCOLOR		m_cColorStart;
	D3DXCOLOR		m_cColorVar;
	D3DXCOLOR		m_cColorEnd;

	float			m_fSpeed;
	float			m_fSpeedVar;

	float			m_fLife;
	float			m_fLifeVar;

	float			m_fTheta;

	int				m_nCol;			// 행
	int				m_nRow;			// 열
	float			m_fAniTime;		// 설정 시간

	int				m_nAllSprite;		// 전체 스프라이트 갯수

	float			m_fForceVar;

	float			m_fAlphaDelta1;
	float			m_fAlphaDelta2;
	float			m_fAlphaDelta3;

	float			m_fAlphaTime1;
	float			m_fAlphaTime2;

	D3DXVECTOR2		m_vTexScale;
	D3DXVECTOR2		m_vTexScaleDelta;

	// Particle 들의 Render 시 쓰이는 것.
	int				m_nPower;
	int				m_nBlend;

public:
	int GetCol() const					{ return m_nCol; }
	int GetRow() const					{ return m_nRow; }
	DWORD GetParticlesPerSec() const	{ return m_uParticlesPerSec; }
	float GetAniTime() const			{ return m_fAniTime; }
	int GetAllSprite() const			{ return m_nAllSprite; }

	// Render 시 쓰이는 것.
	int GetPower() const						{ return m_nPower; }
	int GetBlend() const						{ return m_nBlend; }

public:
	void NewParticleCreate( PARTICLE* pParticle, const DxEffectParticleSys* pParticleSysSource ) const;

	void NewParticleCreate2( PARTICLE* pParticle, 
							const DxEffSingleGroup* pThisGroup, 
							const DxEffectParticleSys* pParticleSysSource, 
							const D3DXVECTOR3& vPos_NowPrev,
							float fElapsedTime,
							DWORD dwFlag,
							const D3DXMATRIX& matRotate ) const;

	ParticlesBaseSetting(	DWORD				uParticlesPerSec,
							const D3DXVECTOR3&	vWindDir,
							float				fTexRotateAngel,
							float				fGravityStart,
							float				fGravityVar,
							float				fGravityEnd,
							float				fSizeStart,
							float				fSizeVar,
							float				fSizeEnd,
							float				fAlpha,
							const D3DXCOLOR&	cColorStart,
							const D3DXCOLOR&	cColorVar,
							const D3DXCOLOR&	cColorEnd,
							float				fSpeed,
							float				fSpeedVar,
							float				fLife,
							float				fLifeVar,
							float				fTheta,
							int					nCol,
							int					nRow,
							float				fAniTime,
							int					nAllSprite,
							float				fForceVar,
							float				fAlphaDelta1,
							float				fAlphaDelta2,
							float				fAlphaDelta3,
							float				fAlphaTime1,
							float				fAlphaTime2,
							const D3DXVECTOR2&	vTexScale,
							const D3DXVECTOR2&	vTexScaleDelta,
							int					nPower,
							int					nBlend )
		: m_uParticlesPerSec(uParticlesPerSec)
		, m_vWindDir(vWindDir)
		, m_fTexRotateAngel(fTexRotateAngel)
		, m_fGravityStart(fGravityStart)
		, m_fGravityVar(fGravityVar)
		, m_fGravityEnd(fGravityEnd)
		, m_fSizeStart(fSizeStart)
		, m_fSizeVar(fSizeVar)
		, m_fSizeEnd(fSizeEnd)
		, m_fAlpha(fAlpha)
		, m_cColorStart(cColorStart)
		, m_cColorVar(cColorVar)
		, m_cColorEnd(cColorEnd)
		, m_fSpeed(fSpeed)
		, m_fSpeedVar(fSpeedVar)
		, m_fLife(fLife)
		, m_fLifeVar(fLifeVar)
		, m_fTheta(fTheta)
		, m_nCol(nCol)
		, m_nRow(nRow)
		, m_fAniTime(fAniTime)
		, m_nAllSprite(nAllSprite)
		, m_fForceVar(fForceVar)
		, m_fAlphaDelta1(fAlphaDelta1)
		, m_fAlphaDelta2(fAlphaDelta2)
		, m_fAlphaDelta3(fAlphaDelta3)
		, m_fAlphaTime1(fAlphaTime1)
		, m_fAlphaTime2(fAlphaTime2)
		, m_vTexScale(vTexScale)
		, m_vTexScaleDelta(vTexScaleDelta)
		, m_nPower(nPower)
		, m_nBlend(nBlend)
	{

	}
};






//////////////////////////////////////////////////////////////////////////
//
class DxEffectParticleSys : public DxEffSingle
{
	//	Note : 이펙트 타입 정의.		--	정적 정의 코드부 [시작]
	//
public:
	const static DWORD	TYPEID;
	const static DWORD	FLAG;
	const static char	NAME[];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }

	//	Note : 정적 수치 참조 값.
	//
public:
	static const float	MIN_SPEED;		// in world units / sec
	static const float	MIN_LIFETIME;	// in seconds
	static const float	MIN_SIZE;		// in world units
	static const float	MIN_GRAVITY;	// as a multiple of normal gravity 
	static const float	MIN_ALPHA;		// as a ratio 

	static const float	MAX_SPEED;		// in world units / sec
	static const float	MAX_LIFETIME;	// in seconds
	static const float	MAX_SIZE;		// in world units
	static const float	MAX_GRAVITY;	// as a multiple of normal gravity 

	//	Note : 파티클 공통 리소스 자원들 관리.
	//
protected:
	static const char				m_strParticle[];
	static const DWORD				m_dwParticleVertexShader[];

	static LPDIRECT3DSTATEBLOCK9	m_pSavedRenderStats;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawRenderStats;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

	static void PutSleepParticle ( PARTICLE* pParticle );
	static void PutSleepParticles ( PARTICLE* pParticles );

	//-------------------------------------------------------------------------------------------
public:
	DxEffectParticleSys (	boost::shared_ptr<EmitterBaseSetting>& spEmitterSetting, 
							boost::shared_ptr<ParticlesBaseSetting>& spParticleSetting );
	virtual ~DxEffectParticleSys ();

public:
	virtual void CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

public:
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha );

	virtual HRESULT FinalCleanup();

public:
	float			m_fElapsedTime;

public:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	DxSimMeshResource				m_SimMeshRes;
	DxEffSinglePropGroupResource	m_PropGroupRes;

	std::string			m_strTexture;
	TextureResource		m_textureRes;
	std::string			m_szCollTex;
	TextureResource		m_textureResColl;		// 충돌된 텍스쳐.

private:
	boost::shared_ptr<EmitterBaseSetting>	m_spEmitterSetting;
	boost::shared_ptr<ParticlesBaseSetting> m_spParticleSetting;

public:
	const EmitterBaseSetting& GetEmitterSetting() const
	{
		return *m_spEmitterSetting.get();
	}

	const ParticlesBaseSetting& GetParticleSetting() const
	{
		return *m_spParticleSetting.get();
	}

	DWORD GetParticlesPerSec() const
	{
		return m_spParticleSetting->GetParticlesPerSec();
	}
};

#endif //__CPARTICLESYSTEM_H_
