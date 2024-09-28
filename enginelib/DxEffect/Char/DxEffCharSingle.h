#pragma once

#include "DxEffChar.h"
#include "../../DxTools/DxMethods.h"

struct EFF_PROPGROUP;
class DxEffSingleGroup;

#define	USE_REPEAT			0x0001
#define	USE_REPEATMOVE		0x0002
#define	USE_AUTOMOVE		0x0004
#define	USE_1POINT			0x0010
#define	USE_2POINT			0x0020
#define	USE_MATRIX			0x0040
#define	USE_NOT_TIMEDAY		0x0100			// ���� �ߵ� ����
#define	USE_NOT_TIMENIGHT	0x0200			// �㿡 �ߵ� ����

struct EFFCHAR_PROPERTY_SINGLE_100
{
	int				m_nType;

	char			m_szFileName[MAX_PATH];	//	����Ʈ �̱� �̸�.
	char			m_szTrace[STRACE_NSIZE];		//	���� ������ �ε���.
	char			m_szTraceBack[STRACE_NSIZE];	//	���� ������ �ε���.
	int				m_nWeather;				//	����..

	float			m_fMoveSpeed;

	BOOL			m_bAllMType;				//	Ÿ�̹� �� �ʿ��� �κп� �ִ� ��
	BOOL			m_bAllSType;				//
	BOOL			m_bAllTime;					//
	DWORD			m_dwKeyStartTime;			//
	DWORD			m_dwKeyEndTime;				//
	EMANI_MAINTYPE	m_SetAniMType;				//
	EMANI_SUBTYPE	m_SetAniSType;				//

	DWORD			m_dwPlaySlngle;

	EFFCHAR_PROPERTY_SINGLE_100()
	{
		memset( m_szFileName, 0, sizeof(char)*MAX_PATH );

		memset( m_szTrace, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTraceBack, 0, sizeof(char)*STRACE_NSIZE );
	};
};

struct EFFCHAR_PROPERTY_SINGLE_101
{
	int				m_nType;
	char			m_szFileName[MAX_PATH];			//	����Ʈ �̱� �̸�.
	char			m_szTrace[STRACE_NSIZE];		//	���� ������ �ε���.
	char			m_szTraceBack[STRACE_NSIZE];	//	���� ������ �ε���.
	int				m_nWeather;						//	����..
	int				m_nAniMType;					//	���� Ÿ��	0 ��ü, 1 ����, 2, ���� ��
	DWORD			m_dwPlaySlngle;

	EFFCHAR_PROPERTY_SINGLE_101()
	{
		memset( m_szFileName, 0, sizeof(char)*MAX_PATH );

		memset( m_szTrace, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTraceBack, 0, sizeof(char)*STRACE_NSIZE );
	};
};

struct EFFCHAR_PROPERTY_SINGLE_102
{
	int				m_nType;
	char			m_szFileName[MAX_PATH];			//	����Ʈ �̱� �̸�.
	char			m_szTrace[STRACE_NSIZE];		//	���� ������ �ε���.
	char			m_szTraceBack[STRACE_NSIZE];	//	���� ������ �ε���.
	int				m_nWeather;						//	����..
	int				m_nAniMType;					//	���� Ÿ��	0 ��ü, 1 ����, 2, ���� ��
	DWORD			m_dwPlaySlngle;
	float			m_fScale;

	EFFCHAR_PROPERTY_SINGLE_102()
	{
		memset( m_szFileName, 0, sizeof(char)*MAX_PATH );

		memset( m_szTrace, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTraceBack, 0, sizeof(char)*STRACE_NSIZE );
	};
};

struct EFFCHAR_PROPERTY_SINGLE_103
{
	int				m_nType;
	char			m_szFileName[MAX_PATH];			//	����Ʈ �̱� �̸�.
	char			m_szTrace[STRACE_NSIZE];		//	���� ������ �ε���.
	char			m_szTraceBack[STRACE_NSIZE];	//	���� ������ �ε���.
	int				m_nWeather;						//	����..
	int				m_nAniMType;					//	���� Ÿ��	0 ��ü, 1 ����, 2, ���� ��
	DWORD			m_dwPlaySlngle;
	float			m_fScale;
	float			m_fDepthUp;
	float			m_fDepthDown;

	EFFCHAR_PROPERTY_SINGLE_103()
	{
		memset( m_szFileName, 0, sizeof(char)*MAX_PATH );

		memset( m_szTrace, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTraceBack, 0, sizeof(char)*STRACE_NSIZE );
	};
};

struct EFFCHAR_PROPERTY_SINGLE_104
{
	int				m_nType;
	DWORD			m_dwFlag;
	float			m_fCoolTime;
	char			m_szFileName[MAX_PATH];			//	����Ʈ �̱� �̸�.
	char			m_szTrace[STRACE_NSIZE];		//	���� ������ �ε���.
	char			m_szTraceBack[STRACE_NSIZE];	//	���� ������ �ε���.
	int				m_nWeather;						//	����..
	int				m_nAniMType;					//	���� Ÿ��	0 ��ü, 1 ����, 2, ���� ��
	DWORD			m_dwPlaySlngle;
	float			m_fScale;
	float			m_fDepthUp;
	float			m_fDepthDown;

	EFFCHAR_PROPERTY_SINGLE_104()
	{
		memset( m_szFileName, 0, sizeof(char)*MAX_PATH );

		memset( m_szTrace, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTraceBack, 0, sizeof(char)*STRACE_NSIZE );
	};
};

struct EFFCHAR_PROPERTY_SINGLE_105	//	������ ���� ���� ����
{
	DWORD			m_dwFlag;
	float			m_fCoolTime;
	char			m_szFileName[MAX_PATH];			//	����Ʈ �̱� �̸�.
	char			m_szTrace[STRACE_NSIZE];		//	���� ������ �ε���.
	char			m_szTraceBack[STRACE_NSIZE];	//	���� ������ �ε���.
	int				m_nWeather;						//	����..
	int				m_nAniMType;					//	���� Ÿ��	0 ��ü, 1 ����, 2, ���� ��
	DWORD			m_dwPlaySlngle;
	float			m_fScale;
	float			m_fDepthUp;
	float			m_fDepthDown;
	float			m_fRepeatSpeed;
};

struct EFFCHAR_PROPERTY_SINGLE_106 : public EFFCHAR_PROPERTY
{
    DWORD			m_dwFlag;
    DWORD			m_dwPlaySlngle;
    int				m_nWeather;						//	����..
    int				m_nAniMType;					//	���� Ÿ��	0 ��ü, 1 ����, 2, ���� ��
    float			m_fCoolTime;
    float			m_fScale;
    float			m_fDepthUp;
    float			m_fDepthDown;
    float			m_fRepeatSpeed;
    char			m_szFileName[MAX_PATH];			//	����Ʈ �̱� �̸�.
    char			m_szTrace[STRACE_NSIZE];		//	���� ������ �ε���.
    char			m_szTraceBack[STRACE_NSIZE];	//	���� ������ �ε���.
};


struct EFFCHAR_PROPERTY_SINGLE : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;
	DWORD			m_dwPlaySlngle;
	int				m_nWeather;						//	����..
	int				m_nAniMType;					//	���� Ÿ��	0 ��ü, 1 ����, 2, ���� ��
	float			m_fCoolTime;
	float			m_fScale;
	float			m_fDepthUp;
	float			m_fDepthDown;
	float			m_fRepeatSpeed;
	char			m_szFileName[MAX_PATH];			//	����Ʈ �̱� �̸�.
	char			m_szTrace[STRACE_NSIZE];		//	���� ������ �ε���.
	char			m_szTraceBack[STRACE_NSIZE];	//	���� ������ �ε���.

    D3DXMATRIX		m_matBase;

	EFFCHAR_PROPERTY_SINGLE () :
	m_nWeather(0),
		m_nAniMType(0),

		m_fCoolTime(10.f),
		m_dwPlaySlngle(10),
		m_fScale(1.f),
		m_fDepthUp(0.5f),
		m_fDepthDown(0.5f),
		m_fRepeatSpeed(1.f)
	{
		m_dwFlag = USE_1POINT;
		memset( m_szFileName, 0, sizeof(char)*MAX_PATH );

		StringCchCopy( m_szTrace,		STRACE_NSIZE, STRACE_IMAGE01 );
		StringCchCopy( m_szTraceBack,	STRACE_NSIZE, STRACE_IMAGE02 );

        D3DXMatrixIdentity( &m_matBase );
	}

	void Assign( EFFCHAR_PROPERTY_SINGLE_105& sOldData )
	{
		m_dwFlag = sOldData.m_dwFlag;
		m_dwPlaySlngle = sOldData.m_dwPlaySlngle;
		m_nWeather = sOldData.m_nWeather;
		m_nAniMType = sOldData.m_nAniMType;
		m_fCoolTime = sOldData.m_fCoolTime;
		m_fScale = sOldData.m_fScale;
		m_fDepthUp = sOldData.m_fDepthUp;
		m_fDepthDown = sOldData.m_fDepthDown;
		m_fRepeatSpeed = sOldData.m_fRepeatSpeed;
		StringCchCopy( m_szFileName, MAX_PATH, sOldData.m_szFileName );
		StringCchCopy( m_szTrace, STRACE_NSIZE, sOldData.m_szTrace );
		StringCchCopy( m_szTraceBack, STRACE_NSIZE, sOldData.m_szTraceBack );
	}

    void Assign( EFFCHAR_PROPERTY_SINGLE_106& sOldData )
    {
        m_dwFlag = sOldData.m_dwFlag;
        m_dwPlaySlngle = sOldData.m_dwPlaySlngle;
        m_nWeather = sOldData.m_nWeather;
        m_nAniMType = sOldData.m_nAniMType;
        m_fCoolTime = sOldData.m_fCoolTime;
        m_fScale = sOldData.m_fScale;
        m_fDepthUp = sOldData.m_fDepthUp;
        m_fDepthDown = sOldData.m_fDepthDown;
        m_fRepeatSpeed = sOldData.m_fRepeatSpeed;
        StringCchCopy( m_szFileName, MAX_PATH, sOldData.m_szFileName );
        StringCchCopy( m_szTrace, STRACE_NSIZE, sOldData.m_szTrace );
        StringCchCopy( m_szTraceBack, STRACE_NSIZE, sOldData.m_szTraceBack );
    }
};

class DxEffCharSingle : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

	const char* GetFileName() { return m_szFileName; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_SINGLE m_Property;
		};

		struct
		{
			DWORD			m_dwFlag;
			DWORD			m_dwPlaySlngle;
			int				m_nWeather;						//	����..
			int				m_nAniMType;					//	���� Ÿ��	0 ��ü, 1 ����, 2, ���� ��
			float			m_fCoolTime;
			float			m_fScale;
			float			m_fDepthUp;
			float			m_fDepthDown;
			float			m_fRepeatSpeed;
			char			m_szFileName[MAX_PATH];			//	����Ʈ �̱� �̸�.
			char			m_szTrace[STRACE_NSIZE];		//	���� ������ �ε���.
			char			m_szTraceBack[STRACE_NSIZE];	//	���� ������ �ε���.

            D3DXMATRIX		m_matBase;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_SINGLE*)pProperty);
	}

	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:
	DxSkinMesh9*		m_pSkinMesh;

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	D3DXVECTOR3			m_vTransUp;				//	��ġ.
	D3DXVECTOR3			m_vPrevUp;				//	��ġ.

	D3DXVECTOR3			m_vTransDown;			//	��ġ.

	D3DXVECTOR3			m_vBackUp;				//	��� ��.

	float				m_fTarDirection;
	D3DXVECTOR3			m_vDirection;

	D3DXMATRIX						m_matTrans;			//	Ʈ���� ��Ʈ����.
	DxEffSinglePropGroupResource	m_sPropGroupRes;		//	�Ӽ���.
	SPDXEFFSINGLEGROUP				m_spSingleGroup;	//	Ȱ�� ��ü.

	float				m_fCoolTimeADD;

	D3DXVECTOR3			m_vCurPos;				// ���� ��ġ
	float				m_fTime;

	float				m_fPlayTime;

	// ����� �־�� �ȵǴ� �ڵ�. ������ �������� �츮�� ���� �־�� ��.
	// Matrix ������ ���� �߸��Ǿ���.
	// 1 �� ���� 2014.8.4 ������ �߸��� ����. m_matBase ���浵 ������ �ƴϴ�. ��ũ�ʿ� ������ �־���.
	// 2 �� ���� 2014.8.4 ���� �߸��� ����. 2014.4.23 ��ũ ������ ��ġ�ٰ� ��Ų�� ������ ȣȯ�� �ȵǹ��ȴ�. m_matBase ���浵 ������ �ƴϴ�.
	// [2014.4.23] ~ [2014.8.4] ���� ����.
	// ���ο� �������� Matrix ������ �ʿ��ϴ�.
	int					m_nCalculationMatrixMode;
   // D3DXMATRIX		m_matWorld;

private:
	D3DXVECTOR3 m_vNormalUp;
	D3DXVECTOR3 m_vNormalDown;
	D3DXMATRIX m_matVertex;

protected:
	BOOL	SetWeather ();
	BOOL    SetTime ();

	// DxEffSinglePropGMan�� ThreadLoading �ϵ��� �ٲپ��� ������ DxSingleGroup�� �ε��ȵǾ����� �ε��� �õ��Ѵ�.
	void CreateDxEffSingleGroup();

private:
	void	PrecalculateBeforeRenderEff();
	void	OnlyRenderEff( LPDIRECT3DDEVICEQ pd3dDevice );

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

	virtual DWORD GetStateOrder () const					{ return EMEFFSO_SINGLE_EFF; }
	virtual void ConvertTracePoint();

	//////////////////////////////////////////////////////////////////////////
	// Material System �� ���� ȿ������ ���̱� ���� �۾�.
public:
	virtual void PrecalculateBeforeRenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix ) OVERRIDE;

	virtual void RenderEffForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
												const D3DXMATRIX* pMatrix, 
												const float fScale );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );
	virtual void SetDirection ( const D3DXVECTOR3 *pDir, float fTarDir )	{ m_vDirection = *pDir; m_fTarDirection = fTarDir; }

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0102( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0103( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0104( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0105( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0106( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	������ ���� ���� ����
    HRESULT LoadFile_0107( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	
	HRESULT LoadFile_0108( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	�־��� �Ǽ����� �����۾�. ( m_nCalculationMatrixMode )

public:
	virtual void SetCharSingleHellMatrixMode( int nMode );

public:
	DxEffCharSingle(void);
	virtual ~DxEffCharSingle(void);
};

