//
//	[struct DXMATERIAL], JDH, (2002.12.02)
//		메터리얼과 텍스쳐, 메터리얼(텍스쳐) 속성값을 가지고 있는 구조체.
//		이들 데이터는 같이 있을 경우가 많으므로 이렇게 처리함.
//
#pragma once
#include <string>

#include "../../DxTools/TextureManager.h"

struct DXMATERIAL
{
	enum
	{
		FLAG_ALPHA_HARD		= 0x0001,
		FLAG_ALPHA_SOFT		= 0x0002,
	};

	DWORD		dwFlags;
    VOID*	    pTemp;
	std::string	strTexture;

	DXMATERIAL ()
        : dwFlags(0)
        , pTemp(NULL)
	{
	}
};

struct DXMATERIAL_SPECULAR
{
	BOOL	bSpecUse;
    VOID*	pTemp;
	char	szTexture[MAX_PATH];
	char	szSpecTex[MAX_PATH];

	DXMATERIAL_SPECULAR ()
        : bSpecUse(FALSE)
        , pTemp(NULL)
	{
		memset(szTexture, 0, sizeof(char) * (MAX_PATH));
		memset(szSpecTex, 0, sizeof(char) * (MAX_PATH));
	}
};

struct DXMATERIAL_CHAR_EFF_100
{
	BOOL    bEffUse;
	float   fMaterial_Power;
	VOID*   pTemp;
	char    szTexture[MAX_PATH];
	char    szEffTex[MAX_PATH];

	DXMATERIAL_CHAR_EFF_100()
		: pTemp(NULL)
	{
		memset(szTexture, 0, sizeof(char) * (MAX_PATH));
		memset(szEffTex, 0, sizeof(char) * (MAX_PATH));
	}
};

struct DXMATERIAL_CHAR_EFF_OLD
{
	BOOL			bEffUse;
	D3DMATERIALQ	d3dMaterial;
    VOID*       	pTemp;
	char			szTexture[MAX_PATH];
	char			szEffTex[MAX_PATH];
};

enum EMCHAR_EFF_TYPE
{
	EMCET_NULL = 0,
	EMCET_BASE = 1,
	EMCET_TEXTURE_MATRIX = 2,
};

struct DXMATERIAL_CHAR_EFF
{
public:
	EMCHAR_EFF_TYPE		m_emType;
	D3DMATERIALQ		d3dMaterial;
	char				szTexture[MAX_PATH];
	char				szEffTex[MAX_PATH];
    TextureResource		textureRes;
	D3DXMATRIX			m_matTexture0;			// Dont Save Load

private:
    LPDIRECT3DTEXTUREQ	pTexture;

public:
	DXMATERIAL_CHAR_EFF() 
		: m_emType( EMCET_NULL )
		, pTexture(NULL)
	{
		d3dMaterial.Diffuse.r	= 1.f;
		d3dMaterial.Diffuse.g	= 1.f;
		d3dMaterial.Diffuse.b	= 1.f;
		d3dMaterial.Diffuse.a	= 1.f;
		d3dMaterial.Ambient		= d3dMaterial.Diffuse;
		d3dMaterial.Specular	= d3dMaterial.Diffuse;
		d3dMaterial.Emissive	= d3dMaterial.Diffuse;
		d3dMaterial.Power		= 14.f;

		memset(szTexture, 0, sizeof(char) * (MAX_PATH));
		memset(szEffTex, 0, sizeof(char) * (MAX_PATH));

		D3DXMatrixIdentity( &m_matTexture0 );
	}

public:
    void SaveFile( sc::BaseStream &SFile );
    void LoadFile( sc::BaseStream &SFile );

    bool LoadTexture( DWORD mipMap, bool bBackgroundLoading );
    bool UpdateTexture() { return textureRes.Update(); }

	void ClearEffTex();

	EMCHAR_EFF_TYPE GetType()				{ return m_emType; }
	void SetType( EMCHAR_EFF_TYPE emType )	{ m_emType = emType; }
	void SetType( const DXMATERIAL_CHAR_EFF_100& sMaterial )	{ m_emType = sMaterial.bEffUse ? EMCET_BASE : EMCET_NULL; }
	void SetType( const DXMATERIAL_CHAR_EFF_OLD& sMaterial )	{ m_emType = sMaterial.bEffUse ? EMCET_BASE : EMCET_NULL; }
	void SetType( const DXMATERIAL_SPECULAR& sMaterial )		{ m_emType = sMaterial.bSpecUse ? EMCET_BASE : EMCET_NULL; }
	BOOL IsType( EMCHAR_EFF_TYPE emType )		{ return m_emType==emType ? TRUE : FALSE; }

	BOOL IsPathFile();

    void SetTexture( LPDIRECT3DTEXTUREQ pTexture_ ) { pTexture = pTexture_; }
    LPDIRECT3DTEXTUREQ GetTexture() { return pTexture ? pTexture : textureRes.GetTexture(); }
};

//
//struct DXMATERIAL_CHAR_EFF
//{
//	BOOL				bEffUse;
//	D3DMATERIALQ		d3dMaterial;
//	DWORD				dwDiffuse;		// Optimize - 최적화 때문에 쓰인다.
//	LPDIRECT3DTEXTUREQ	pEffTex;
//	char				szTexture[MAX_PATH];
//	char				szEffTex[MAX_PATH];
//
//	DXMATERIAL_CHAR_EFF () :
//		bEffUse(FALSE),
//		pEffTex(NULL)
//	{
//		d3dMaterial.Diffuse.r	= 1.f;
//		d3dMaterial.Diffuse.g	= 1.f;
//		d3dMaterial.Diffuse.b	= 1.f;
//		d3dMaterial.Diffuse.a	= 1.f;
//		d3dMaterial.Ambient		= d3dMaterial.Diffuse;
//		d3dMaterial.Specular	= d3dMaterial.Diffuse;
//		d3dMaterial.Emissive	= d3dMaterial.Diffuse;
//		d3dMaterial.Power		= 14.f;
//
//		memset(szTexture, 0, sizeof(char) * (MAX_PATH));
//		memset(szEffTex, 0, sizeof(char) * (MAX_PATH));
//	}
//
//	~DXMATERIAL_CHAR_EFF();
//};

