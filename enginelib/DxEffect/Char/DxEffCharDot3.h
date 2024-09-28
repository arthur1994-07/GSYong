#pragma once

#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"

#define		ISTEXTURE1		0x0001
#define		ISTEXTURE2		0x0002
#define		ISTEXTURE3		0x0004
#define		USETEXTURE1		0x0010
#define		USETEXTURE2		0x0020
#define		USETEXTURE3		0x0040

struct EFFCHAR_PROPERTY_DOT3_100	//	데이터 포맷 변경 진행	
{
	DWORD			m_dwFlag;
	DWORD			m_dwColorOP;

	CRDT_TYPE		m_dwDetail_1;				// 각각의 뿌려야 되는 형식들
	CRDT_TYPE		m_dwDetail_2;
	CRDT_TYPE		m_dwDetail_3;

	char			m_szTexture[MAX_PATH];		// 반사

	char			m_szSrcTex_1[MAX_PATH];
	char			m_szSrcTex_2[MAX_PATH];
	char			m_szSrcTex_3[MAX_PATH];

	char			m_szTexture_1[MAX_PATH];	// 스펙큘러 맵
	char			m_szTexture_2[MAX_PATH];
	char			m_szTexture_3[MAX_PATH];

	char			m_szDot3Tex_1[MAX_PATH];	// 범프 맵
	char			m_szDot3Tex_2[MAX_PATH];
	char			m_szDot3Tex_3[MAX_PATH];
};

struct EFFCHAR_PROPERTY_DOT3 : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;
	DWORD			m_dwColorOP;

	CRDT_TYPE		m_dwDetail_1;				// 각각의 뿌려야 되는 형식들
	CRDT_TYPE		m_dwDetail_2;
	CRDT_TYPE		m_dwDetail_3;

	char			m_szTexture[MAX_PATH];		// 반사

	char			m_szTexture_1[MAX_PATH];	// 스펙큘러 맵
	char			m_szTexture_2[MAX_PATH];
	char			m_szTexture_3[MAX_PATH];

	char			m_szSrcTex_1[MAX_PATH];
	char			m_szSrcTex_2[MAX_PATH];
	char			m_szSrcTex_3[MAX_PATH];

	char			m_szDot3Tex_1[MAX_PATH];	// 범프 맵
	char			m_szDot3Tex_2[MAX_PATH];
	char			m_szDot3Tex_3[MAX_PATH];


	EFFCHAR_PROPERTY_DOT3 () :
		m_dwColorOP(D3DTOP_MODULATE)
	{
		m_dwFlag = USETEXTURE1 | USETEXTURE2 | USETEXTURE3;

		m_dwDetail_1 = CRDT_DEFAULT;
		m_dwDetail_2 = CRDT_DEFAULT;
		m_dwDetail_3 = CRDT_DEFAULT;

		StringCchCopy( m_szSrcTex_1, MAX_PATH, "" );
		StringCchCopy( m_szSrcTex_2, MAX_PATH, "" );
		StringCchCopy( m_szSrcTex_3, MAX_PATH, "" );

		StringCchCopy( m_szTexture_1, MAX_PATH, "1d_Lighting.bmp" );
		StringCchCopy( m_szTexture_2, MAX_PATH, "1d_Lighting.bmp" );
		StringCchCopy( m_szTexture_3, MAX_PATH, "1d_Lighting.bmp" );

		StringCchCopy( m_szDot3Tex_1, MAX_PATH, "Test_Day.dds" );
		StringCchCopy( m_szDot3Tex_2, MAX_PATH, "Test_Day.dds" );
		StringCchCopy( m_szDot3Tex_3, MAX_PATH, "Test_Day.dds" );

		StringCchCopy( m_szTexture, MAX_PATH, "Rain.dds" );
	}

	void Assign( EFFCHAR_PROPERTY_DOT3_100& sOldData )
	{
		m_dwFlag = sOldData.m_dwFlag;
		m_dwColorOP = sOldData.m_dwColorOP;

		m_dwDetail_1 = sOldData.m_dwDetail_1;
		m_dwDetail_2 = sOldData.m_dwDetail_2;
		m_dwDetail_3 = sOldData.m_dwDetail_3;

		StringCchCopy( m_szTexture, MAX_PATH, sOldData.m_szTexture );

		StringCchCopy( m_szTexture_1, MAX_PATH, sOldData.m_szTexture_1 );
		StringCchCopy( m_szTexture_2, MAX_PATH, sOldData.m_szTexture_2 );
		StringCchCopy( m_szTexture_3, MAX_PATH, sOldData.m_szTexture_3 );

		StringCchCopy( m_szSrcTex_1, MAX_PATH, sOldData.m_szSrcTex_1 );
		StringCchCopy( m_szSrcTex_2, MAX_PATH, sOldData.m_szSrcTex_2 );
		StringCchCopy( m_szSrcTex_3, MAX_PATH, sOldData.m_szSrcTex_3 );

		StringCchCopy( m_szDot3Tex_1, MAX_PATH, sOldData.m_szDot3Tex_1 );
		StringCchCopy( m_szDot3Tex_2, MAX_PATH, sOldData.m_szDot3Tex_2 );
		StringCchCopy( m_szDot3Tex_3, MAX_PATH, sOldData.m_szDot3Tex_3 );

	}
};

class DxEffCharDot3 : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return EMECF_DOT3; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_DOT3 m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			DWORD			m_dwColorOP;

			CRDT_TYPE		m_dwDetail_1;				// 각각의 뿌려야 되는 형식들
			CRDT_TYPE		m_dwDetail_2;
			CRDT_TYPE		m_dwDetail_3;

			char			m_szTexture[MAX_PATH];		// 반사

			char			m_szTexture_1[MAX_PATH];	// 스펙큘러 맵
			char			m_szTexture_2[MAX_PATH];
			char			m_szTexture_3[MAX_PATH];

			char			m_szSrcTex_1[MAX_PATH];
			char			m_szSrcTex_2[MAX_PATH];
			char			m_szSrcTex_3[MAX_PATH];

			char			m_szDot3Tex_1[MAX_PATH];	// 범프 맵
			char			m_szDot3Tex_2[MAX_PATH];
			char			m_szDot3Tex_3[MAX_PATH];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_DOT3*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

		//	Note : 쉐이더.
	//
protected:	
	//static DWORD		m_dwEffect;
	//static DWORD		dwDot3VertexShader[];
	//static DWORD		m_dw2VertexEffect;
	//static DWORD		dw2VertexShader[];
	//static DWORD		m_dw2PixelEffect;
	//static DWORD		dw2PixelShader[];

	//static DWORD		m_dwSavedStateBlock;
	//static DWORD		m_dwDetailStateBlock;
	//static DWORD		m_dwEffectStateBlock;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectStateBlock;

	LPDIRECT3DDEVICEQ m_pd3dDevice;

    TextureResource m_textureRes;   // 반사

    TextureResource m_textureRes1;	// 스펙큘러
    TextureResource m_textureRes2;
    TextureResource m_textureRes3;

    TextureResource m_textureResDot1;	// 범프
    TextureResource m_textureResDot2;
    TextureResource m_textureResDot3;

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
	DxEffCharDot3(void);
	virtual ~DxEffCharDot3(void);
};
