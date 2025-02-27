#pragma once

#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"

#define		ISTEXTURE1		0x0001
#define		ISTEXTURE2		0x0002
#define		ISTEXTURE3		0x0004
#define		ISTEXTURE4		0x0008
#define		USETEXTURE1		0x0010
#define		USETEXTURE2		0x0020
#define		USETEXTURE3		0x0040
#define		USETEXTURE4		0x0080

struct EFFCHAR_PROPERTY_SPECULAR_100	//	데이터 포맷 변경 진행	
{
	DWORD			m_dwFlag;
	char			m_szSrcTex_1[MAX_PATH];
	char			m_szSrcTex_2[MAX_PATH];
	char			m_szSrcTex_3[MAX_PATH];
	char			m_szSrcTex_4[MAX_PATH];	//지워야함
};

struct EFFCHAR_PROPERTY_SPECULAR : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;
	char			m_szSrcTex_1[MAX_PATH];
	char			m_szSrcTex_2[MAX_PATH];
	char			m_szSrcTex_3[MAX_PATH];


	EFFCHAR_PROPERTY_SPECULAR ()
	{
		m_dwFlag = USETEXTURE1 | USETEXTURE2 | USETEXTURE3 | USETEXTURE4;

		StringCchCopy( m_szSrcTex_1, MAX_PATH, "" );
		StringCchCopy( m_szSrcTex_2, MAX_PATH, "" );
		StringCchCopy( m_szSrcTex_3, MAX_PATH, "" );
	}

	void Assign( EFFCHAR_PROPERTY_SPECULAR_100& sOldData )
	{
		m_dwFlag = sOldData.m_dwFlag;
		StringCchCopy( m_szSrcTex_1, MAX_PATH, sOldData.m_szSrcTex_1 );
		StringCchCopy( m_szSrcTex_2, MAX_PATH, sOldData.m_szSrcTex_2 );
		StringCchCopy( m_szSrcTex_3, MAX_PATH, sOldData.m_szSrcTex_3 );
	}
};

class DxEffCharSpecular : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

	union
	{
		struct
		{
			EFFCHAR_PROPERTY_SPECULAR m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			char			m_szSrcTex_1[MAX_PATH];
			char			m_szSrcTex_2[MAX_PATH];
			char			m_szSrcTex_3[MAX_PATH];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_SPECULAR*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:	
	static LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectStateBlock;
	static LPDIRECT3DSTATEBLOCK9	m_pSavedDisableSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectDisableSB;

	LPDIRECT3DDEVICEQ m_pd3dDevice;

protected:
	CRDT_TYPE		m_dwDetail_1;				// 각각의 뿌려야 되는 형식들
	CRDT_TYPE		m_dwDetail_2;
	CRDT_TYPE		m_dwDetail_3;

protected:
	DxSkinMesh9*				m_pSkinMesh;
	PSMESHCONTAINER				m_pmcMesh;

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
	DxEffCharSpecular(void);
	virtual ~DxEffCharSpecular(void);
};
