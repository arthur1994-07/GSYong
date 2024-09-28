#pragma once

#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"

struct EFFCHAR_PROPERTY_CLONEBLUR_100	//	������ ���� ���� ����	
{
	BOOL			m_bUse;						// ���̺� �� �ʿ�� ���� ���δ�.. ���� �������� ������ ������.
	BOOL			m_bBezier;
	int				m_nBlend;
	char			m_szTraceUP[STRACE_NSIZE];		//	���� ������ �ε���.
	char			m_szTraceDOWN[STRACE_NSIZE];	//	���� ������ �ε���.
	DWORD			m_dwFaces;
	float			m_fWidth;
	D3DCOLOR		m_vColorUP;
	D3DCOLOR		m_vColorDOWN;

	char			m_szTexture[MAX_PATH];
};

struct EFFCHAR_PROPERTY_CLONEBLUR : public EFFCHAR_PROPERTY
{
	BOOL			m_bUse;						// ���̺� �� �ʿ�� ���� ���δ�.. ���� �������� ������ ������.
	BOOL			m_bBezier;
	float			m_fWidth;
	int				m_nBlend;
	DWORD			m_dwFaces;
	D3DCOLOR		m_vColorUP;
	D3DCOLOR		m_vColorDOWN;
	char			m_szTexture[MAX_PATH];
	char			m_szTraceUP[STRACE_NSIZE];		//	���� ������ �ε���.
	char			m_szTraceDOWN[STRACE_NSIZE];	//	���� ������ �ε���.

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
			char			m_szTraceUP[STRACE_NSIZE];		//	���� ������ �ε���.
			char			m_szTraceDOWN[STRACE_NSIZE];	//	���� ������ �ε���.
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
	D3DXVECTOR3			m_vTransUP;				//	��ġ.
	D3DXVECTOR3			m_vTransDOWN;			//	��ġ.
	D3DXVECTOR3			m_vPrevUP;				//	��ġ.
	D3DXVECTOR3			m_vPrevDOWN;			//	��ġ.
	D3DXVECTOR3			m_vPrevPrevUP;			//	��ġ.
	D3DXVECTOR3			m_vPrevPrevDOWN;		//	��ġ.

protected:
	float	m_fTime;
	float	m_fThisTime;
	float	m_fElapsedTime;
	float	m_fFPSSumTime;

	DWORD	m_dwCount;

	int		m_iBlurNum;		// �ѷ��� �� �ܻ� ����
	int		m_iAdd;			// �������� �ܻ� ����

	int		m_nAlphaDeltaUP;
	int		m_nAlphaDeltaDOWN;

	struct BLURTEX
	{
		D3DXVECTOR3	p;
		D3DCOLOR    d;
		D3DXVECTOR2	t;
		const static DWORD FVF;
	};

	int				m_nNowFaces;	// ���� �÷��� �Ǵ� ��
	int				m_nPlayFaces;	// ���� ������ �� ����.!

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
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	������ ���� ���� ����	

public:
	DxEffCharCloneBlur(void);
	virtual ~DxEffCharCloneBlur(void);
};
