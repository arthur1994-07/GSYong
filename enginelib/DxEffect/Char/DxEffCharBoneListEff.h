#pragma once

#include <vector>
#include "../../../SigmaCore/Memory/ObjectPool.h"
#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/DxSequenceUV.h"

#include "DxEffChar.h"

#define	BONELISTEFF_TEXNAME_SIZE	64
#define	MAX_FACE					500

struct EFF_PROPGROUP;
class DxEffSingleGroup;
class DxBoneTransMaintain;

#define	USE_REMAIN		            0x0001	// 파티클을 남긴다.
#define	USE_ABSOLUTE	            0x0002	// 절대좌표 : 상대좌표
#define	USE_RANDOMRANGE	            0x0004	// 랜덤한 범위
#define	USE_SEQUENCE	            0x0008	// 시퀀스 사용
#define	USE_ROTATE		            0x0010	// 회전 사용
#define	USE_CREATERANDOM_DONTMOVE   0x0020	// 위치생성랜덤 & 이동안되도록

#define	USE_PLAY		0x0001	// 절대좌표일 경우 Play 하는지 안 하는지 알려줌.
#define	USE_POS1		0x0002	// 위치 1개 얻음.

struct EFFCHAR_PROPERTY_BONELISTEFF_100	//	데이터 포맷 변경 진행
{
	DWORD		m_dwFlag;
	DWORD		m_dwParticlePec;	// 초당 나오는 파티클 갯수
	DWORD		m_dwColor;
	D3DXVECTOR2	m_vLife;			// 최소,최대
	D3DXVECTOR2	m_vRotate;			// 최소,최대
	D3DXVECTOR2	m_vVelocity;		// 최소,최대
	D3DXVECTOR3 m_vSize;			// 최소,최대,변화률
	DWORD		m_dwSequenceCol;
	DWORD		m_dwSequenceRow;
	float		m_fCreateLength;	// 생성 될때 일정거리만큼 떨어뜨려준다.
	char		m_szTexture[BONELISTEFF_TEXNAME_SIZE];

};

struct EFFCHAR_PROPERTY_BONELISTEFF : public EFFCHAR_PROPERTY
{
	DWORD		m_dwFlag;
	DWORD		m_dwParticlePec;	// 초당 나오는 파티클 갯수
	DWORD		m_dwColor;
	DWORD		m_dwSequenceCol;
	DWORD		m_dwSequenceRow;
	float		m_fCreateLength;	// 생성 될때 일정거리만큼 떨어뜨려준다.
	
	D3DXVECTOR2	m_vLife;			// 최소,최대
	D3DXVECTOR2	m_vRotate;			// 최소,최대
	D3DXVECTOR2	m_vVelocity;		// 최소,최대
	D3DXVECTOR3 m_vSize;			// 최소,최대,변화률
	
	char		m_szTexture[BONELISTEFF_TEXNAME_SIZE];

	EFFCHAR_PROPERTY_BONELISTEFF() :
		m_dwFlag(0L),
		m_dwParticlePec(5),
		m_dwColor(0xffffffff),
		m_vLife(2.f,3.f),
		m_vRotate(1.f,5.f),
		m_vVelocity(0.5f,1.f),
		m_vSize(5.f,8.f,1.f),
		m_dwSequenceCol(8),
		m_dwSequenceRow(8),
		m_fCreateLength(1.f)
	{
		m_dwFlag = 0L;
		StringCchCopy( m_szTexture, BONELISTEFF_TEXNAME_SIZE, "" );
	}

	void Assign( EFFCHAR_PROPERTY_BONELISTEFF_100& sOldData )
	{
		m_dwFlag = sOldData.m_dwFlag;
		m_dwParticlePec = sOldData.m_dwParticlePec;
		m_dwColor = sOldData.m_dwColor;
		m_dwSequenceCol = sOldData.m_dwSequenceCol;
		m_dwSequenceRow = sOldData.m_dwSequenceRow;
		m_fCreateLength = sOldData.m_fCreateLength;
		
		m_vLife = sOldData.m_vLife;
		m_vRotate = sOldData.m_vRotate;
		m_vVelocity = sOldData.m_vVelocity;
		m_vSize = sOldData.m_vSize;

		StringCchCopy( m_szTexture, BONELISTEFF_TEXNAME_SIZE, sOldData.m_szTexture );	

	}
};

struct DXCUSTOMBONE
{
	std::string					m_strBoneName;
	const DxBoneTransMaintain*	m_pBoneTran;
	BOOL						m_bVertex;
	D3DXVECTOR3					m_vVertex;

	DXCUSTOMBONE()
		: m_strBoneName(_T(""))
		, m_pBoneTran(NULL)
		, m_bVertex(FALSE)
		, m_vVertex(0.f,0.f,0.f)
	{
	}

	void RenderCount( DWORD nCount, const D3DXMATRIX& matWorld );

	void Save( sc::BaseStream& SFile );
	void Load( sc::BaseStream& SFile );
};
typedef std::list<DXCUSTOMBONE>		LISTCUSTOMBONE;
typedef LISTCUSTOMBONE::iterator	LISTCUSTOMBONE_ITER;

class DxEffCharBoneListEff : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD		VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_BONELISTEFF m_Property;
		};
		
		struct
		{
			DWORD		m_dwFlag;
			DWORD		m_dwParticlePec;	// 초당 나오는 파티클 갯수
			DWORD		m_dwColor;
			DWORD		m_dwSequenceCol;
			DWORD		m_dwSequenceRow;
			float		m_fCreateLength;	// 생성 될때 일정거리만큼 떨어뜨려준다.
			D3DXVECTOR2	m_vLife;			// 최소,최대
			D3DXVECTOR2	m_vRotate;			// 최소,최대
			D3DXVECTOR2	m_vVelocity;		// 최소,최대
			D3DXVECTOR3 m_vSize;			// 최소,최대,변화률
			char		m_szTexture[BONELISTEFF_TEXNAME_SIZE];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_BONELISTEFF*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

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
	virtual void	RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix, const float fScale );

	virtual DWORD GetStateOrder () const						{ return EMEFFSO_SINGLE_EFF; }

	//////////////////////////////////////////////////////////////////////////
	// Material System 에 현재 효과들을 붙이기 위한 작업.
public:
	//////////////////////////////////////////////////////////////////////////
	// Material System 에 현재 효과들을 붙이기 위한 작업.
	void PrecalculateBeforeRenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix ) OVERRIDE;

	virtual void RenderEffForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice,
												const D3DXMATRIX* pMatrix, 
												const float fScale ) OVERRIDE;

	// Note : 선택된 본을 알기 위함.
private:
	const DxBoneTransMaintain*	m_pBoneTran;

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

	// Note : DXPOSDELTA
private:
	class DXPOSDELTA
	{
	private:
		std::string					m_strBoneName;	// SAVE 
		int							m_nIndex;		// ----	// DxSkeleton::m_vecReferenceBoneTrans 쪽 Index
															// 본이 바뀌면 변경 필요.  로딩시는 값이 안 들어있어서 FrameMove 시 셋팅된다.

	public:
		BOOL						m_bVertex;		// SAVE	// 본위치가 아닌 본에 영향받은 위치를 사용할 것인가 ?
		D3DXVECTOR3					m_vPos;			// ----	// 위치
		D3DXVECTOR3					m_vBase;		// SAVE // 기본 위치
		float						m_fRate;		// SAVE // 자신의 위치 값

	public:
		BOOL SetBoneName( const DxSkeletonMaintain &sSkeleton, const TSTRING& strName );
		const TSTRING& GetBoneName() const				{ return m_strBoneName; }
		BOOL IsExistBone() const						{ return m_nIndex==-1 ? FALSE : TRUE; }
		int	GetBoneIndex() const						{ return m_nIndex; }

		void Save( sc::BaseStream& SFile );
		void Load( sc::BaseStream& SFile );

		DXPOSDELTA() 
			: m_strBoneName(_T(""))
			, m_nIndex(-1)
			, m_bVertex(FALSE)
			, m_vPos(0.f,0.f,0.f)
			, m_vBase(0.f,0.f,0.f)
			, m_fRate(0.f)	
		{
		}

		// m_nIndex 문제로 디폴트 복사 생성자를 못쓴다.
		DXPOSDELTA( const DXPOSDELTA &sSrc ) 
			: m_strBoneName(sSrc.m_strBoneName)
			, m_nIndex(-1)
			, m_bVertex(sSrc.m_bVertex)
			, m_vPos(sSrc.m_vPos)
			, m_vBase(sSrc.m_vBase)
			, m_fRate(sSrc.m_fRate)	
		{
		}

		// m_nIndex 문제로 디폴트 operator= 를 못쓴다.
		DXPOSDELTA& operator=( const DXPOSDELTA &sSrc )
		{
			m_strBoneName = sSrc.m_strBoneName;
			m_nIndex = -1;
			m_bVertex = sSrc.m_bVertex;
			m_vPos = sSrc.m_vPos;
			m_vBase = sSrc.m_vBase;
			m_fRate = sSrc.m_fRate;

			return *this;
		}
	};
	typedef std::vector<DXPOSDELTA>	VECPOSITION;

	// Note : DXPARTICLE
private:
	struct DXPARTICLE
	{
		// Note : 고정 값
		float		m_fLife;		// 이 오브젝트의 생명.	// 알파는 생명으로써 빼어주도록 하자.	삶 으로서 속도를 결정한다.
		D3DXVECTOR3	m_vPosOffSet;	// 일정거리를 띄운다.
        float       m_fPos_Rate;    // USE_CREATERANDOM_DONTMOVE 에서 쓰이는 값

		//Note : 변화 하는 값
		float		m_fTime;			// 현재 생명
		D3DXVECTOR3 m_vPos;				// 현재 위치.
		float		m_fSize;			// 크기
		float		m_fRotate;			// 회전값
		DWORD		m_dwColor;			// 최종 컬러
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

		void ComputeCreatePos( VECPOSITION& vecData );
		BOOL FrameMove( EFFCHAR_PROPERTY_BONELISTEFF& sProp, DXDATA& sData, VECPOSITION& vecData, const float fElpasedTime );
		BOOL FrameMoveAbsolute( EFFCHAR_PROPERTY_BONELISTEFF& sProp, DXDATA& sData, VECPOSITION& vecData, const float fElpasedTime );

		DXPARTICLE()
            : m_fTime(0.f)
			, m_vPos(0.f,0.f,0.f)
            , m_fPos_Rate(0.f)
			, m_fRotate(0.f)
			, m_dwColor(0L)
			, m_fRate(0.f)
			, m_dwSequenceCount(0L)
			, m_fSizeBase(0.f)
			, m_fSizeDiff(0.f)
			, m_fRotateVel(0.f)
            , m_dwFlag(0L)
			, m_pNext(NULL)
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

	float	m_fSizeDiff;
	float	m_fLifeDiff;
	float	m_fRotateDiff;
	float	m_fVelocityDiff;
	DxSequenceUV	m_sSequenceUV;

private:
	void ComputeCreatePos();							// 애니메이션이 바뀔때만 동작.
	void CreateFrameMoveParticle( float fElapsedTime );	// 
	void DxCreateParticle( DWORD dwNUM );				// 새로운 파티클 생성.

	// Note : 렌더시.
private:
	VECPOSITION		m_vecPosition;

	// Note : Edit 시에 쓰이는 것
	LISTCUSTOMBONE	m_listCustomBoneEDIT;

private:
	void UpdateVECPOSION();

	// Note : Edit에서 사용하는 함수.
public:
	void InsertBone( const char* szName );
	BOOL DeleteBone( const DWORD dwIndex );
	int DataUp( DWORD dwIndex );			// Note : Return 값으로 선택되었던 위치를 가르킨다.
	int DataDown( DWORD dwIndex );			// Note : Return 값으로 선택되었던 위치를 가르킨다.

	std::string GetBoneString( const DWORD dwIndex );
	void SetCheckBone( const DWORD dwIndex );					// Bone을 선택한다.
	BOOL GetEnableBonePos();									// Bone Pos가 Enable 되었는지 알려준다.
	D3DXVECTOR3 GetInfluencePosition();							// Bone 옵션의 vVertex가 무엇인지 알려준다.
	D3DXMATRIX GetInfluenceMatrix();							// Bone Matrix를 돌려준다.
	D3DXVECTOR3 SetChangeMatrix( const D3DXMATRIX& matWorld );	// 변한 Matrix를 가지고 원래의 위치값을 얻어온다.
	void		SetCheckBoneOffSetPos( BOOL bEnable );			// Bone 옵션의 bVertex의 값을 수정한다.

	DWORD GetCustomBoneSize()								{ return (DWORD)m_listCustomBoneEDIT.size(); }

	BOOL EditApplyResetData();

private:
    void CleanUp_Particle();

private:
	void OnlyRenderEff( LPDIRECT3DDEVICEQ pd3dDevice, float fScale );

	// Note : Clone, Save, Load
public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행	

public:
	DxEffCharBoneListEff(void);
	virtual ~DxEffCharBoneListEff(void);
};


