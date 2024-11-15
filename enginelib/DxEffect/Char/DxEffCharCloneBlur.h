#pragma once

#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"

struct EFFCHAR_PROPERTY_CLONEBLUR_100	//	데이터 포맷 변경 진행	
{
	BOOL			m_bUse;						// 세이브 할 필요는 없어 보인다.. 다음 버전에는 밑으로 내리자.
	BOOL			m_bBezier;
	int				m_nBlend;
	char			m_szTraceUP[STRACE_NSIZE];		//	추적 포인터 인덱스.
	char			m_szTraceDOWN[STRACE_NSIZE];	//	추적 포인터 인덱스.
	DWORD			m_dwFaces;
	float			m_fWidth;
	D3DCOLOR		m_vColorUP;
	D3DCOLOR		m_vColorDOWN;

	char			m_szTexture[MAX_PATH];
};

struct EFFCHAR_PROPERTY_CLONEBLUR : public EFFCHAR_PROPERTY
{
	BOOL			m_bUse;						// 세이브 할 필요는 없어 보인다.. 다음 버전에는 밑으로 내리자.
	BOOL			m_bBezier;
	float			m_fWidth;
	int				m_nBlend;
	DWORD			m_dwFaces;
	D3DCOLOR		m_vColorUP;
	D3DCOLOR		m_vColorDOWN;
	char			m_szTexture[MAX_PATH];
	char			m_szTraceUP[STRACE_NSIZE];		//	추적 포인터 인덱스.
	char			m_szTraceDOWN[STRACE_NSIZE];	//	추적 포인터 인덱스.

	EFFCHAR_PROPERTY_CLONEBLUR () :
		m_bUse(TRUE),
		m_bBezier(TRUE),
		m_nBlend(1),
		m_dwFaces(7),
		m_fWidth(2.f),
		m_vColorUP(D3DCOLOR_ARGB(255,255,255,255)),
		m_vColorDOWN(D3DCOLOR_ARGB(255,255,255,255))
	{
		StringCchCopy( m_szTexture, MAX_PATH, "1d_Lighting.bmp" );

		StringCchCopy( m_szTraceUP,		STRACE_NSIZE, STRACE_IMAGE01 );
		StringCchCopy( m_szTraceDOWN,	STRACE_NSIZE, STRACE_IMAGE02 );
	}

	void Assign( EFFCHAR_PROPERTY_CLONEBLUR_100& sOldData )
	{
		m_bUse = sOldData.m_bUse;
		m_bBezier = sOldData.m_bBezier;
		m_fWidth = sOldData.m_fWidth;
		m_nBlend = sOldData.m_nBlend;
		m_dwFaces = sOldData.m_dwFaces;
		m_vColorUP = sOldData.m_vColorUP;
		m_vColorDOWN = sOldData.m_vColorDOWN;

		StringCchCopy( m_szTexture,MAX_PATH, sOldData.m_szTexture ); 
		StringCchCopy( m_szTraceUP,STRACE_NSIZE, sOldData.m_szTraceUP ); 
		StringCchCopy( m_szTraceDOWN,STRACE_NSIZE, sOldData.m_szTraceDOWN ); 
	}
};

class DxEffCharCloneBlur : public DxEffChar
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
			EFFCHAR_PROPERTY_CLONEBLUR m_Property;
		};
		
		struct
		{
			BOOL			m_bUse;
			BOOL			m_bBezier;
			float			m_fWidth;
			int				m_nBlend;
			DWORD			m_dwFaces;
			D3DCOLOR		m_vColorUP;
			D3DCOLOR		m_vColorDOWN;
			char			m_szTexture[MAX_PATH];
			char			m_szTraceUP[STRACE_NSIZE];		//	추적 포인터 인덱스.
			char			m_szTraceDOWN[STRACE_NSIZE];	//	추적 포인터 인덱스.
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_CLONEBLUR*)pProperty);
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
	D3DXVECTOR3			m_vTransUP;				//	위치.
	D3DXVECTOR3			m_vTransDOWN;			//	위치.
	D3DXVECTOR3			m_vPrevUP;				//	위치.
	D3DXVECTOR3			m_vPrevDOWN;			//	위치.
	D3DXVECTOR3			m_vPrevPrevUP;			//	위치.
	D3DXVECTOR3			m_vPrevPrevDOWN;		//	위치.

protected:
	float	m_fTime;
	float	m_fThisTime;
	float	m_fElapsedTime;
	float	m_fFPSSumTime;

	DWORD	m_dwCount;

	int		m_iBlurNum;		// 뿌려야 할 잔상 갯수
	int		m_iAdd;			// 더해지는 잔상 갯수

	int		m_nAlphaDeltaUP;
	int		m_nAlphaDeltaDOWN;

	struct BLURTEX
	{
		D3DXVECTOR3	p;
		D3DCOLOR    d;
		D3DXVECTOR2	t;
		const static DWORD FVF;
	};

	int				m_nNowFaces;	// 지금 플레이 되는 면
	int				m_nPlayFaces;	// 지금 생성된 면 갯수.!

	LPDIRECT3DVERTEXBUFFERQ		m_pVB;
	LPDIRECT3DINDEXBUFFERQ		m_pIB;

	TextureResource m_textureRes;

protected:
	HRESULT CreateBlurVB ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT UpdateDiffuse ();
	HRESULT	ResetBlur ();
	HRESULT	CreateVertex ();

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
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행	

public:
	DxEffCharCloneBlur(void);
	virtual ~DxEffCharCloneBlur(void);
};
