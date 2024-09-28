#pragma once

#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"
#include "../../DxMeshs/dxsimplemeshman.h"

#define	USEA			0x0001
#define	USEB			0x0002
#define	USEC			0x0004
#define	USED			0x0008
#define	USESEC			0x0010
#define	USESIZE			0x0020
#define	USERANDROTATE	0x0040
#define	USEARROW		0x0080
#define	USEEFFECT		0x0100

struct EFFCHAR_PROPERTY_ARROW_100 // Ver.100
{
	DWORD			m_dwFlag;
	BOOL			m_bUse;
	int				m_nBlend;
	int				m_nSec;
	float			m_fSetRate;				//	꼽히는 비율
	float			m_fFullTime;			// 
	float			m_fAlphaTime;			// 
	float			m_fDepth;
	float			m_fSize;
	D3DCOLOR		m_vColor;

	char			m_szMeshFile[MAX_PATH];

	EFFCHAR_PROPERTY_ARROW_100()
	{
		memset( m_szMeshFile, 0, sizeof(char)*MAX_PATH );
	}
};

struct EFFCHAR_PROPERTY_ARROW_101 // Ver.101	//	데이터 포맷 변경 진행	
{
	DWORD			m_dwFlag;
	BOOL			m_bUse;
	int				m_nBlend;
	int				m_nSec;
	float			m_fSetRate;				//	꼽히는 비율
	float			m_fFullTime;			// 
	float			m_fAlphaTime;			// 
	float			m_fDepth;
	float			m_fSize;

	D3DXVECTOR3		m_vEffStart;
	D3DXVECTOR3		m_vEffEnd;

	D3DCOLOR		m_vColor;

	char			m_szMeshFile[MAX_PATH];
	char			m_szFileName_R[MAX_PATH];
	char			m_szFileName_C[MAX_PATH];
};

struct EFFCHAR_PROPERTY_ARROW : public EFFCHAR_PROPERTY		// Ver.102
{
	BOOL			m_bUse;
	DWORD			m_dwFlag;
	int				m_nBlend;
	int				m_nSec;
	float			m_fSize;
	float			m_fDepth;
	float			m_fSetRate;				//	꼽히는 비율
	float			m_fFullTime;			// 
	float			m_fAlphaTime;			// 

	D3DCOLOR		m_vColor;

	D3DXVECTOR3		m_vEffStart;
	D3DXVECTOR3		m_vEffEnd;

	char			m_szFileName_R[MAX_PATH];
	char			m_szFileName_C[MAX_PATH];
	char			m_szMeshFile[MAX_PATH];


	EFFCHAR_PROPERTY_ARROW () :
		m_bUse(TRUE),
		m_nBlend(4),
		m_nSec(10),
		m_fSetRate(-0.3f),
		m_fFullTime(10.f),
		m_fAlphaTime(2.f),
		m_fDepth(-1.4f),
		m_fSize(1.f),
		m_vEffStart(0.f,0.f,0.f),
		m_vEffEnd(0.f,-5.f,0.f),
		m_vColor(0xffffffff)
	{
		m_dwFlag = 0;
		m_dwFlag |= USEA|USEB;
		m_dwFlag |= USERANDROTATE;
		StringCchCopy( m_szMeshFile, MAX_PATH, "sd_stone.x" );

		memset( m_szFileName_R, 0, sizeof(char)*MAX_PATH );
		memset( m_szFileName_C, 0, sizeof(char)*MAX_PATH );
	}

	void Assign( EFFCHAR_PROPERTY_ARROW_101& sOldData )
	{
		m_dwFlag = sOldData.m_dwFlag;
		m_bUse= sOldData.m_bUse;
		m_nBlend= sOldData.m_nBlend;
		m_nSec= sOldData.m_nSec;
		m_fSetRate= sOldData.m_fSetRate;
		m_fFullTime= sOldData.m_fFullTime;
		m_fAlphaTime= sOldData.m_fAlphaTime;
		m_fDepth= sOldData.m_fDepth;
		m_fSize= sOldData.m_fSize;

		m_vEffStart= sOldData.m_vEffStart;
		m_vEffEnd= sOldData.m_vEffEnd;

		m_vColor= sOldData.m_vColor;

		StringCchCopy( m_szMeshFile, MAX_PATH, sOldData.m_szMeshFile );
		StringCchCopy( m_szFileName_R, MAX_PATH, sOldData.m_szFileName_R );
		StringCchCopy( m_szFileName_C, MAX_PATH, sOldData.m_szFileName_C );	
	}
};

class DxEffCharArrow : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID() override { return TYPEID; }
	virtual DWORD GetFlag() override   { return NULL; }
	virtual const char* GetName() override { return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_ARROW m_Property;
		};
		
		struct
		{
			BOOL			m_bUse;
			DWORD			m_dwFlag;
			int				m_nBlend;
			int				m_nSec;
			float			m_fSize;
			float			m_fDepth;
			float			m_fSetRate;				//	꼽히는 비율
			float			m_fFullTime;			// 
			float			m_fAlphaTime;			// 

			D3DCOLOR		m_vColor;

			D3DXVECTOR3		m_vEffStart;
			D3DXVECTOR3		m_vEffEnd;

			char			m_szFileName_R[MAX_PATH];
			char			m_szFileName_C[MAX_PATH];
			char			m_szMeshFile[MAX_PATH];

		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_ARROW*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:	
	static LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectStateBlock;

	LPDIRECT3DDEVICEQ m_pd3dDevice;

protected:
	D3DXVECTOR3			m_vTrans;				//	위치.
	D3DXVECTOR3			m_vNormal;				//	위치.
	D3DXMATRIX			m_matWorld;
	D3DXMATRIX			m_matScale;

protected:
	float	m_fTime;
	float	m_fElapsedTime;

	struct SARROW
	{
		char		szTrace[STRACE_NSIZE];
		D3DXVECTOR3	vDir;
		float		fStartTime;
		float		fRotate;
		float		fHeight;
		SARROW*		pNext;

		SARROW()
            : pNext(NULL)
		{
			memset( szTrace, 0, sizeof(char)*STRACE_NSIZE );
		}
	};

	//struct VERTEX 
	//{ 
	//	D3DXVECTOR3 p; 
	//	D3DCOLOR	cColor; 
	//	D3DXVECTOR2 t; 
	//};

	float		m_fTarDirection;
	D3DXVECTOR3	m_vDirection;
	float		m_fAlphaDelta;
	float		m_fAlphaStart;
	D3DXCOLOR	m_cBaseColor;
	D3DXCOLOR	m_cColor;

	float		m_fNumSec;		// 갯수 / 1초

	SARROW*		m_pArrowHead;

	DxSimMeshResource	m_SimMeshRes;					//	Note: 렌더링 메쉬 저장 부분 

protected:
	DxEffSinglePropGroupResource	m_spPropGroup_R;	//	속성값.
	SPDXEFFSINGLEGROUP				m_spSingleGroup_R;	//	활성 개체.
	DxEffSinglePropGroupResource	m_spPropGroup_C;	//	속성값.
	SPDXEFFSINGLEGROUP				m_spSingleGroup_C;	//	활성 개체.

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender );

public:
	HRESULT	InsertArrow ( D3DXVECTOR3& vDirc );	

protected:
	HRESULT RenderSingle ( LPDIRECT3DDEVICEQ pd3dDevice, D3DMATRIX *matWorld, D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd );

	// DxEffSinglePropGMan를 ThreadLoading 하도록 바꾸었기 때문에 DxEffSingleGroup가 로딩안되었으면 로딩을 시도한다.
	void CreateDxEffSingleGroup( DxEffSinglePropGroupResource& spPropGroup, SPDXEFFSINGLEGROUP& spSingleGroup );

	// DxEffSingleGroup 을 정리한다.
	void DeleteDxEffSingleGroup();

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );
	virtual void SetDirection ( const D3DXVECTOR3 *pDir, float fTarDir )	{ m_vDirection = *pDir; m_fTarDirection = fTarDir; }

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행

public:
	DxEffCharArrow(void);
	virtual ~DxEffCharArrow(void);
};