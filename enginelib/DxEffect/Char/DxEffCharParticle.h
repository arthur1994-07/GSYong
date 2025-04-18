#pragma once

#include <vector>
#include "../../../SigmaCore/Memory/ObjectPool.h"
#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/DxSequenceUV.h"

#include "DxEffChar.h"

#define	TEX_SIZE_64		64
#define	MAX_FACE		500

struct EFF_PROPGROUP;
class DxEffSingleGroup;

#define	USE_REMAIN		0x0001	// 방향성 : 퍼짐
#define	USE_ABSOLUTE	0x0002	// 절대좌표 : 상대좌표
#define	USE_RANDOMRANGE	0x0004	// 랜덤한 범위
#define	USE_SEQUENCE	0x0008	// 시퀀스 사용
#define	USE_ROTATE		0x0010	// 회전 사용

#define	USE_PLAY		0x0001	// 절대좌표일 경우 Play 하는지 안 하는지 알려줌.
#define	USE_POS1		0x0002	// 위치 1개 얻음.

struct EFFCHAR_PROPERTY_PARTICLE_100
{
	DWORD		m_dwFlag;
	DWORD		m_dwParticlePec;	// 초당 나오는 파티클 갯수
	float		m_fRandomRange;
	DWORD		m_dwColor;
	float		m_fSizeMin;
	float		m_fSizeMax;
	float		m_fLifeMin;
	float		m_fLifeMax;
	D3DXVECTOR2	m_vRotate;			// 최소,최대
	D3DXVECTOR2	m_vVelocity;		// 최소,최대
	DWORD		m_dwSequenceCol;
	DWORD		m_dwSequenceRow;
	char		m_szTexture[TEX_SIZE_64];
};

struct EFFCHAR_PROPERTY_PARTICLE : public EFFCHAR_PROPERTY
{
	DWORD		m_dwFlag;
	DWORD		m_dwParticlePec;	// 초당 나오는 파티클 갯수
	float		m_fRandomRange;
	DWORD		m_dwColor;
	D3DXVECTOR3	m_vSize;
	float		m_fLifeMin;
	float		m_fLifeMax;
	D3DXVECTOR2	m_vRotate;			// 최소,최대
	D3DXVECTOR2	m_vVelocity;		// 최소,최대
	DWORD		m_dwSequenceCol;
	DWORD		m_dwSequenceRow;
	char		m_szTexture[TEX_SIZE_64];

	EFFCHAR_PROPERTY_PARTICLE() :
		m_dwFlag(0L),
		m_dwParticlePec(10),
		m_fRandomRange(1.f),
		m_dwColor(0xffffffff),
		m_vSize(2.f,3.f,1.f),
		m_fLifeMin(2.f),
		m_fLifeMax(3.f),
		m_vRotate(1.f,5.f),
		m_vVelocity(0.5f,1.f),
		m_dwSequenceCol(8),
		m_dwSequenceRow(8)
	{
		m_dwFlag = 0L;
		StringCchCopy( m_szTexture, TEX_SIZE_64, "" );
	}
};

typedef std::list<D3DXVECTOR3>	LISTPOSITION;
typedef LISTPOSITION::iterator	LISTPOSITION_ITER;

class DxEffCharParticle : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_PARTICLE m_Property;
		};
		
		struct
		{
			DWORD		m_dwFlag;
			DWORD		m_dwParticlePec;
			float		m_fRandomRange;
			DWORD		m_dwColor;
			D3DXVECTOR3	m_vSize;
			float		m_fLifeMin;
			float		m_fLifeMax;
			D3DXVECTOR2	m_vRotate;			// 최소,최대
			D3DXVECTOR2	m_vVelocity;		// 최소,최대
			DWORD		m_dwSequenceCol;
			DWORD		m_dwSequenceRow;
			char		m_szTexture[TEX_SIZE_64];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_PARTICLE*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

private:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawSB;

public:
	static HRESULT OnCreateDevice_STATIC ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT OnDestroyDevice_STATIC ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix, const float fScale );

	virtual DWORD GetStateOrder () const { return EMEFFSO_SINGLE_EFF; }

	//////////////////////////////////////////////////////////////////////////
	// Material System 에 현재 효과들을 붙이기 위한 작업.
public:
	// PrecalculateForMaterialSystem의 2번 중 값만 미리 백업해 놓는거라면, 이곳에서 렌더가 된다.
	virtual void RenderEffForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
											const D3DXMATRIX* pMatrix, 
											const float fScale );

private:
	void RenderTestViewPoint( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	float m_fTimeADD;

	// Note : 전체 디퓨즈의 데이터
	//		작은 파티클로 넣기 위함.
public:
	struct DXDATA
	{
		DWORD dwColorR;
		DWORD dwColorG;
		DWORD dwColorB;
		DWORD dwColorBASE;
	};

private:
	DXDATA m_sPlayData;

private:
	struct DXPOSDELTA
	{
		D3DXVECTOR3		vPos;			// SAVE // 기본 위치
		float			fRate;			// SAVE // 자신의 위치 값

		void Save( sc::BaseStream& SFile );
		void Load( sc::BaseStream& SFile );
	};
	typedef std::vector<DXPOSDELTA>	VECPOSITION;

private:
	struct DXPARTICLE
	{
		// Note : 고정 값
		float		m_fLife;		// 이 오브젝트의 생명.	// 알파는 생명으로써 빼어주도록 하자.	삶 으로서 속도를 결정한다.
		float		m_fSize;		// 크기
		D3DXVECTOR3	m_vRandomRange;	// 랜덤한 범위.

		//Note : 변화 하는 값
		D3DXVECTOR3 m_vPos;				// 현재 위치.
		float		m_fRotate;			// 회전값
		DWORD		m_dwColor;			// 최종 컬러
		float		m_fTime;			// 현재 생명
		float		m_fRate;			// 생명의 비율
		DWORD		m_dwSequenceCount;	// 시퀀스의 Count

		// Note : 변화를 위해 저장해 놓는 값.
		float		m_fSizeBase;
		float		m_fSizeDiff;
		float		m_fRotateVel;

		// Note : 절대좌표의 퍼짐으로 작동될 경우	// 절대좌표일때만 초기화 함.
		BOOL		m_dwFlag;
		float		m_fVelocity;
		float		m_fFirstPosRate;
		D3DXVECTOR3 m_vFirstPos;
		D3DXVECTOR3	m_vDirDiff;

		DXPARTICLE* m_pNext;

		void ComputeCreatePos( const D3DXMATRIX* pMatrix, VECPOSITION& vecData );
		BOOL FrameMove( const EFFCHAR_PROPERTY_PARTICLE& sProp, VECPOSITION& vecData, const float fElpasedTime, DXDATA& sData );
		BOOL FrameMoveAbsolute( const EFFCHAR_PROPERTY_PARTICLE& sProp, VECPOSITION& vecData, const float fElpasedTime, DXDATA& sData, const float fScale );

		DXPARTICLE() :
			m_fTime(0.f),
			m_fRotate(0.f),
			m_fRate(0.f),
			m_vRandomRange(0.f,0.f,0.f),
			m_dwSequenceCount(0L),
			m_fSizeBase(0.f),
			m_fSizeDiff(0.f),
			m_fRotateVel(0.f),
			m_pNext(NULL)
		{
		}
	};
    typedef sc::ObjectPool<DXPARTICLE> PARTICLEPOOL;

public:
	static PARTICLEPOOL*			m_pParticlePool;
	static VERTEXCOLORTEX			m_pVertices[MAX_FACE*4];

private:
	DXPARTICLE*				m_pParticleHEAD;
	TextureResource m_textureRes;

private:
	float	m_fElapsedTime;
	int		m_nPrevKeyTime;
	float	m_fParticleADD;	// Particle을 더함.

	float	m_fSizeDiff;		// Size Max, Min 차이
	float	m_fLifeDiff;		// Life Max, Min 차이
	float	m_fRotateDiff;
	float	m_fVelocityDiff;
	DxSequenceUV	m_sSequenceUV;

private:
	void ComputeCreatePos( const D3DXMATRIX* pMatrix );
	void CreateFrameMoveParticle( float fElapsedTime, const float fScale );
	void DxCreateParticle( DWORD dwNUM );	// 새로운 파티클 생성.

	// Note : 렌더시.
private:
	VECPOSITION		m_vecPosition;

	// Note : Edit 시에 쓰이는 것
	LISTPOSITION	m_listPosition;
	DWORD			m_dwCheckindex;

	// Note : Edit에서 사용하는 함수.
public:
	void InsertPoint( const D3DXVECTOR3& vPos );
	BOOL DeletePoint( const DWORD dwIndex );
	int DataUp( DWORD dwIndex );			// Note : Return 값으로 선택되었던 위치를 가르킨다.
	int DataDown( DWORD dwIndex );			// Note : Return 값으로 선택되었던 위치를 가르킨다.

	std::string GetPositionString( const DWORD dwIndex );		// 선택된 아이템의 String을 가져온다.
	void SetPoistionEditPos( const DWORD dwIndex );				// 선택된 인덱스를 얻고 그 위치에 카메라를 옮겨 놓는다.
	D3DXVECTOR3 GetInfluencePosition();							// 무엇인지 알려준다.
	D3DXVECTOR3 SetMatrixToMakePos( const D3DXMATRIX& matWorld );

	DWORD GetPoistionSize()								{ return (DWORD)m_listPosition.size(); }

	BOOL EditApplyResetData();

private:
    void CleanUp_Particle();

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행

public:
	DxEffCharParticle(void);
	virtual ~DxEffCharParticle(void);
};


