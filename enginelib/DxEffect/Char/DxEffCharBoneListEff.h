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

#define	USE_REMAIN		            0x0001	// ��ƼŬ�� �����.
#define	USE_ABSOLUTE	            0x0002	// ������ǥ : �����ǥ
#define	USE_RANDOMRANGE	            0x0004	// ������ ����
#define	USE_SEQUENCE	            0x0008	// ������ ���
#define	USE_ROTATE		            0x0010	// ȸ�� ���
#define	USE_CREATERANDOM_DONTMOVE   0x0020	// ��ġ�������� & �̵��ȵǵ���

#define	USE_PLAY		0x0001	// ������ǥ�� ��� Play �ϴ��� �� �ϴ��� �˷���.
#define	USE_POS1		0x0002	// ��ġ 1�� ����.

struct EFFCHAR_PROPERTY_BONELISTEFF_100	//	������ ���� ���� ����
{
	DWORD		m_dwFlag;
	DWORD		m_dwParticlePec;	// �ʴ� ������ ��ƼŬ ����
	DWORD		m_dwColor;
	D3DXVECTOR2	m_vLife;			// �ּ�,�ִ�
	D3DXVECTOR2	m_vRotate;			// �ּ�,�ִ�
	D3DXVECTOR2	m_vVelocity;		// �ּ�,�ִ�
	D3DXVECTOR3 m_vSize;			// �ּ�,�ִ�,��ȭ��
	DWORD		m_dwSequenceCol;
	DWORD		m_dwSequenceRow;
	float		m_fCreateLength;	// ���� �ɶ� �����Ÿ���ŭ ����߷��ش�.
	char		m_szTexture[BONELISTEFF_TEXNAME_SIZE];

};

struct EFFCHAR_PROPERTY_BONELISTEFF : public EFFCHAR_PROPERTY
{
	DWORD		m_dwFlag;
	DWORD		m_dwParticlePec;	// �ʴ� ������ ��ƼŬ ����
	DWORD		m_dwColor;
	DWORD		m_dwSequenceCol;
	DWORD		m_dwSequenceRow;
	float		m_fCreateLength;	// ���� �ɶ� �����Ÿ���ŭ ����߷��ش�.
	
	D3DXVECTOR2	m_vLife;			// �ּ�,�ִ�
	D3DXVECTOR2	m_vRotate;			// �ּ�,�ִ�
	D3DXVECTOR2	m_vVelocity;		// �ּ�,�ִ�
	D3DXVECTOR3 m_vSize;			// �ּ�,�ִ�,��ȭ��
	
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
			DWORD		m_dwParticlePec;	// �ʴ� ������ ��ƼŬ ����
			DWORD		m_dwColor;
			DWORD		m_dwSequenceCol;
			DWORD		m_dwSequenceRow;
			float		m_fCreateLength;	// ���� �ɶ� �����Ÿ���ŭ ����߷��ش�.
			D3DXVECTOR2	m_vLife;			// �ּ�,�ִ�
			D3DXVECTOR2	m_vRotate;			// �ּ�,�ִ�
			D3DXVECTOR2	m_vVelocity;		// �ּ�,�ִ�
			D3DXVECTOR3 m_vSize;			// �ּ�,�ִ�,��ȭ��
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
	// Material System �� ���� ȿ������ ���̱� ���� �۾�.
public:
	//////////////////////////////////////////////////////////////////////////
	// Material System �� ���� ȿ������ ���̱� ���� �۾�.
	void PrecalculateBeforeRenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix ) OVERRIDE;

	virtual void RenderEffForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice,
												const D3DXMATRIX* pMatrix, 
												const float fScale ) OVERRIDE;

	// Note : ���õ� ���� �˱� ����.
private:
	const DxBoneTransMaintain*	m_pBoneTran;

	// Note : ��ü ��ǻ���� ������
	//		���� ��ƼŬ�� �ֱ� ����.
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
		int							m_nIndex;		// ----	// DxSkeleton::m_vecReferenceBoneTrans �� Index
															// ���� �ٲ�� ���� �ʿ�.  �ε��ô� ���� �� ����־ FrameMove �� ���õȴ�.

	public:
		BOOL						m_bVertex;		// SAVE	// ����ġ�� �ƴ� ���� ������� ��ġ�� ����� ���ΰ� ?
		D3DXVECTOR3					m_vPos;			// ----	// ��ġ
		D3DXVECTOR3					m_vBase;		// SAVE // �⺻ ��ġ
		float						m_fRate;		// SAVE // �ڽ��� ��ġ ��

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

		// m_nIndex ������ ����Ʈ ���� �����ڸ� ������.
		DXPOSDELTA( const DXPOSDELTA &sSrc ) 
			: m_strBoneName(sSrc.m_strBoneName)
			, m_nIndex(-1)
			, m_bVertex(sSrc.m_bVertex)
			, m_vPos(sSrc.m_vPos)
			, m_vBase(sSrc.m_vBase)
			, m_fRate(sSrc.m_fRate)	
		{
		}

		// m_nIndex ������ ����Ʈ operator= �� ������.
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
		// Note : ���� ��
		float		m_fLife;		// �� ������Ʈ�� ����.	// ���Ĵ� �������ν� �����ֵ��� ����.	�� ���μ� �ӵ��� �����Ѵ�.
		D3DXVECTOR3	m_vPosOffSet;	// �����Ÿ��� ����.
        float       m_fPos_Rate;    // USE_CREATERANDOM_DONTMOVE ���� ���̴� ��

		//Note : ��ȭ �ϴ� ��
		float		m_fTime;			// ���� ����
		D3DXVECTOR3 m_vPos;				// ���� ��ġ.
		float		m_fSize;			// ũ��
		float		m_fRotate;			// ȸ����
		DWORD		m_dwColor;			// ���� �÷�
		float		m_fRate;			// ������ ����
		DWORD		m_dwSequenceCount;	// �������� Count

		// Note : ��ȭ�� ���� ������ ���� ��.
		float		m_fSizeBase;
		float		m_fSizeDiff;
		float		m_fRotateVel;

		// Note : ������ǥ�� �������� �۵��� ���	// ������ǥ�϶��� �ʱ�ȭ ��.
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
	float	m_fParticleADD;	// Particle�� ����.

	float	m_fSizeDiff;
	float	m_fLifeDiff;
	float	m_fRotateDiff;
	float	m_fVelocityDiff;
	DxSequenceUV	m_sSequenceUV;

private:
	void ComputeCreatePos();							// �ִϸ��̼��� �ٲ𶧸� ����.
	void CreateFrameMoveParticle( float fElapsedTime );	// 
	void DxCreateParticle( DWORD dwNUM );				// ���ο� ��ƼŬ ����.

	// Note : ������.
private:
	VECPOSITION		m_vecPosition;

	// Note : Edit �ÿ� ���̴� ��
	LISTCUSTOMBONE	m_listCustomBoneEDIT;

private:
	void UpdateVECPOSION();

	// Note : Edit���� ����ϴ� �Լ�.
public:
	void InsertBone( const char* szName );
	BOOL DeleteBone( const DWORD dwIndex );
	int DataUp( DWORD dwIndex );			// Note : Return ������ ���õǾ��� ��ġ�� ����Ų��.
	int DataDown( DWORD dwIndex );			// Note : Return ������ ���õǾ��� ��ġ�� ����Ų��.

	std::string GetBoneString( const DWORD dwIndex );
	void SetCheckBone( const DWORD dwIndex );					// Bone�� �����Ѵ�.
	BOOL GetEnableBonePos();									// Bone Pos�� Enable �Ǿ����� �˷��ش�.
	D3DXVECTOR3 GetInfluencePosition();							// Bone �ɼ��� vVertex�� �������� �˷��ش�.
	D3DXMATRIX GetInfluenceMatrix();							// Bone Matrix�� �����ش�.
	D3DXVECTOR3 SetChangeMatrix( const D3DXMATRIX& matWorld );	// ���� Matrix�� ������ ������ ��ġ���� ���´�.
	void		SetCheckBoneOffSetPos( BOOL bEnable );			// Bone �ɼ��� bVertex�� ���� �����Ѵ�.

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
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	������ ���� ���� ����	

public:
	DxEffCharBoneListEff(void);
	virtual ~DxEffCharBoneListEff(void);
};


