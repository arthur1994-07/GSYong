#pragma once

#include "DxEffAni.h"
#include "../../DxTools/DxMethods.h"
#include "../Single/DxEffSinglePropGMan.h"

#define STRING_NUM_128	128

#define	USEDRAWPOINT	0x00001	// ����
#define	USEDRAWRANGE	0x00002	// ����
#define	USEDRAWDIRECT	0x00004	// �̵�
#define	USEDRAWMATRIX	0x00008	// Matrix
#define	USETIMEDAY		0x00010			// ��
#define	USETIMENIGHT	0x00020			// ��
#define	USESKYFINE		0x00100				// ����
#define	USESKYRAIN		0x00200				// ��
#define	USESKYSNOW		0x00400				// ��
#define	USESKYLEAVES	0x00800				// ����
#define	USECHARMOVE		0x01000					// ĳ������
#define	USEEFFLOOP		0x02000					// Effect�� Loop �Ѵ�.

class DxSkinChar;
class DxEffSingleGroup;
class DxSkeletonMaintain;


struct EFFANI_PROPERTY_SINGLE_100
{
	DWORD			m_dwFlags;
	DWORD			m_dwFrame;
	float			m_fFact;
	float			m_fScale;

	char			m_szEff[ STRING_NUM_128 ];
	char			m_szBipFirst[ STRING_NUM_128 ];
	char			m_szBipSecond[ STRING_NUM_128 ];
};

struct EFFANI_PROPERTY_SINGLE : public EFFANI_PROPERTY
{
	DWORD			m_dwFlags;
	DWORD			m_dwFrame;
	float			m_fFact;
	float			m_fScale;
	D3DXMATRIX		m_matBase;

	char			m_szEff[ STRING_NUM_128 ];
	char			m_szBipFirst[ STRING_NUM_128 ];
	char			m_szBipSecond[ STRING_NUM_128 ];

	EFFANI_PROPERTY_SINGLE () :
		m_dwFlags(0L),
		m_dwFrame(0),
		m_fFact(0.f),
		m_fScale(1.f)	
	{
		m_dwFlags |= ( USEDRAWPOINT | 
						USETIMEDAY|USETIMENIGHT | 
						USESKYFINE|USESKYRAIN|USESKYSNOW|USESKYLEAVES );

		D3DXMatrixIdentity( &m_matBase );

		memset( m_szEff, 0, sizeof(char)*STRING_NUM_128 );
		memset( m_szBipFirst, 0, sizeof(char)*STRING_NUM_128 );
		memset( m_szBipSecond, 0, sizeof(char)*STRING_NUM_128 );
	}
};

class DxEffAniData_Single : public DxEffAniData
{
public:
	static DWORD		VERSION;
	static DWORD		TYPEID;
	static char			NAME[64];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

public:
	union
	{
		struct
		{
			EFFANI_PROPERTY_SINGLE		m_Property;
		};

		struct
		{
			DWORD			m_dwFlags;
			DWORD			m_dwFrame;
			float			m_fFact;
			float			m_fScale;
			D3DXMATRIX		m_matBase;	// ���� �������� ��.

			char			m_szEff[ STRING_NUM_128 ];
			char			m_szBipFirst[ STRING_NUM_128 ];
			char			m_szBipSecond[ STRING_NUM_128 ];
		};
	};

protected:
	DxEffSinglePropGroupResource	m_sPropGroupRes;			//	�Ӽ���.

public:
	BOOL CheckEff ( DWORD dwCurKeyTime, int nPrevFrame );

public:
	virtual void SetProperty ( EFFANI_PROPERTY *pProperty )
	{
		m_Property = *((EFFANI_PROPERTY_SINGLE*)pProperty);
	}
	
	virtual EFFANI_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

public:
	virtual void SetEffAniData ( DxEffAniData*	pData );
	virtual BOOL IsSingleEffLoop();

public:
	virtual DxEffAni* NEWOBJ ( DxSkinAnimation* pAnimContainer );	//	Note : ����Ʈ ������.
	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::SerialFile &SFile );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	DxEffAniData_Single ()
        : DxEffAniData()
	{
	}

    virtual ~DxEffAniData_Single()
    {
    }
};

class DxEffAniSingle : public DxEffAni
{
public:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	DWORD				m_dwFlags;
	DWORD				m_dwFrame;
	float				m_fFact;
	float				m_fScale;


	char			m_szEff[128];
	char			m_szBipFirst[128];
	char			m_szBipSecond[128];

	BOOL			m_bDataSetting;
	D3DXVECTOR3		m_vPos_1;
	D3DXVECTOR3		m_vPos_2;
	D3DXMATRIX		m_matBase;
	D3DXMATRIX		m_matWorld;
	float			m_fRandomFact;

	DxEffSinglePropGroupResource*	m_pPropGroupRes;			//	�Ӽ���.

protected:
	D3DXMATRIX			m_matTrans;				//	Ʈ���� ��Ʈ����.
	SPDXEFFSINGLEGROUP	m_spSingleGroup;			//	Ȱ�� ��ü.

protected:
	BOOL	SetPosition ( const DxSkeletonMaintain* pSkeletonMaintain, const D3DXMATRIXA16** ppMatrix );
	void	SetMatrix ( const D3DXMATRIX& matCurPos, const D3DXMATRIX& matBone );

protected:
	BOOL	SetWeather ();
	BOOL	SetTime ();

	// DxEffSinglePropGMan�� ThreadLoading �ϵ��� �ٲپ��� ������ DxSingleGroup�� �ε��ȵǾ����� �ε��� �õ��Ѵ�.
	void	CreateDxEffSingleGroup( LPDIRECT3DDEVICEQ pd3dDevice );

	// DxEffSingleGroup �� �����Ѵ�.
	void	DeleteDxEffSingleGroup();

public:
	virtual HRESULT OneTimeSceneInit () override;
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice ) override;
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice ) override;
	virtual HRESULT InvalidateDeviceObjects () override;
	virtual HRESULT DeleteDeviceObjects () override;
	virtual HRESULT FinalCleanup () override;

public:
	virtual HRESULT FrameMove ( DxSkinCharEffAniBase* pSkinCharBase, float fTime, float fElapsedTime ) override;
	virtual HRESULT	Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharEffAniBase* pSkinChar, const D3DXMATRIX& matCurPos, float fAlpha ) override;
	//virtual HRESULT	Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxAttBone* pAttBone, const D3DXMATRIX& matCurPos, float fAlpha ) override;
	//virtual HRESULT	Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxVehicle* pVehicle, const D3DXMATRIX& matCurPos, float fAlpha ) override;

	virtual void SetNeverDieErase();

public:
	DxEffAniSingle(void);
	virtual ~DxEffAniSingle(void);
};

