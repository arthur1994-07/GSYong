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

struct EFFCHAR_PROPERTY_REFLECTION2_100
{
	DWORD			m_dwFlag;
	DWORD			m_dwColorOP;
	char			m_szTexture[MAX_PATH];
	char			m_szSrcTex_1[MAX_PATH];
	char			m_szSrcTex_2[MAX_PATH];
	char			m_szSrcTex_3[MAX_PATH];
	char			m_szSrcTex_4[MAX_PATH];

	char			m_szTexture_1[MAX_PATH];
	char			m_szTexture_2[MAX_PATH];
	char			m_szTexture_3[MAX_PATH];
	char			m_szTexture_4[MAX_PATH];

	EFFCHAR_PROPERTY_REFLECTION2_100()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_3, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_4, 0, sizeof(char)*MAX_PATH );

		memset( m_szTexture_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_3, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_4, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_REFLECTION2_101
{
	DWORD			m_dwFlag;
	DWORD			m_dwColorOP;

	D3DXCOLOR		m_cSpecular;

	char			m_szTexture[MAX_PATH];
	char			m_szSrcTex_1[MAX_PATH];
	char			m_szSrcTex_2[MAX_PATH];
	char			m_szSrcTex_3[MAX_PATH];
	char			m_szSrcTex_4[MAX_PATH];		//지워야함

	char			m_szTexture_1[MAX_PATH];
	char			m_szTexture_2[MAX_PATH];
	char			m_szTexture_3[MAX_PATH];
	char			m_szTexture_4[MAX_PATH];	//지워야함

	EFFCHAR_PROPERTY_REFLECTION2_101()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_3, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_4, 0, sizeof(char)*MAX_PATH );

		memset( m_szTexture_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_3, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_4, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_REFLECTION2_102_104 // Ver.102,103_104	//	데이터 포맷 변경 진행
{
	DWORD			m_dwFlag;
	DWORD			m_dwColorOP;
	DWORD			m_dwMaterials;

	D3DXCOLOR		m_cSpecular;

	char			m_szTexture[MAX_PATH];
};

struct EFFCHAR_PROPERTY_REFLECTION2 : public EFFCHAR_PROPERTY		// Ver105
{
	DWORD			m_dwFlag;
	D3DXCOLOR		m_cSpecular;
	DWORD			m_dwMaterials;
	DWORD			m_dwColorOP;
	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_REFLECTION2 () :
		m_dwColorOP(D3DTOP_MODULATE4X),
		m_dwMaterials(0L),
		m_cSpecular(0.8f,0.8f,0.8f,1.0f)
	{
		m_dwFlag = 0L;
		StringCchCopy( m_szTexture, MAX_PATH, "Rain.dds" );	//
	}

	void Assign( EFFCHAR_PROPERTY_REFLECTION2_102_104& sOldData )
	{
		m_dwFlag = sOldData.m_dwFlag;
		m_cSpecular = sOldData.m_cSpecular;
		m_dwMaterials = sOldData.m_dwMaterials;
		m_dwColorOP = sOldData.m_dwColorOP;
		StringCchCopy( m_szTexture, MAX_PATH, sOldData.m_szTexture );
	}
};

class DxEffCharReflection2 : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return EMECF_RELFECT; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_REFLECTION2 m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			D3DXCOLOR		m_cSpecular;
			DWORD			m_dwMaterials;
			DWORD			m_dwColorOP;
			char			m_szTexture[MAX_PATH];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_REFLECTION2*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:
	static	LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static	LPDIRECT3DSTATEBLOCK9	m_pEffectStateBlock;

protected:
	TextureResource m_textureRes;

	DXMATERIAL_CHAR_EFF*	m_pMaterials;
	DXMATERIAL_CHAR_EFF*	m_pTempMaterials;
	DWORD					m_dwTempMaterials;

protected:
	DxSkinMesh9*				m_pSkinMesh;
	PSMESHCONTAINER				m_pmcMesh;

public:
	virtual void SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials );
	virtual DXMATERIAL_CHAR_EFF*		GetMaterials () { return m_pMaterials; }

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender );

private:
	void	RenderNORMAL( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender );
	void	RenderHLSL( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender );

	// ThreadLoading 으로 인해 m_pmcMesh 정보가 없을 수 있어서 사용하는 곳에서 매번 체크하도록 한다.
private:
	BOOL IsExist_Copy_pmcMesh();

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행

public:
	DxEffCharReflection2(void);
	virtual ~DxEffCharReflection2(void);
};
