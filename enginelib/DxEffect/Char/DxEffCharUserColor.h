#pragma once

#include "DxEffChar.h"

#define		TEXTURE_LENGTH	128

#define		USETEXTURE		0x0001

struct EFFCHAR_PROPERTY_USERCOLOR_100 : public EFFCHAR_PROPERTY
{
	DWORD		m_dwFlag;
	D3DXCOLOR	m_cColor;
};

struct EFFCHAR_PROPERTY_USERCOLOR_101 : public EFFCHAR_PROPERTY	//	데이터 포맷 변경 진행
{
	DWORD		m_dwFlag;
	D3DXCOLOR	m_cColor;
	DWORD		m_dwMaterials;
};

struct EFFCHAR_PROPERTY_USERCOLOR : public EFFCHAR_PROPERTY
{
	DWORD		m_dwFlag;
	DWORD		m_dwMaterials;
	D3DXCOLOR	m_cColor;

	EFFCHAR_PROPERTY_USERCOLOR () :
		m_cColor(D3DCOLOR_ARGB(0,255,255,255)),
		m_dwMaterials(0L)
	{
		m_dwFlag = 0L;
	}

	void Assign ( EFFCHAR_PROPERTY_USERCOLOR_101& sOldData )
	{
		m_dwFlag = sOldData.m_dwFlag;
		m_dwMaterials = sOldData.m_dwMaterials;
		m_cColor = sOldData.m_cColor;
	}
};

class DxEffCharUserColor : public DxEffChar
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
			EFFCHAR_PROPERTY_USERCOLOR m_Property;
		};
		struct
		{
			DWORD		m_dwFlag;
			DWORD		m_dwMaterials;
			D3DXCOLOR	m_cColor;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_USERCOLOR*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

private:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedBaseSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectBaseSB;

private:
	DXMATERIAL_CHAR_EFF*	m_pMaterials;
	DXMATERIAL_CHAR_EFF*	m_pTempMaterials;
	DWORD					m_dwTempMaterials;

private:
	DxSkinMesh9*		m_pSkinMesh;
	PSMESHCONTAINER		m_pmcMesh;

public:
	virtual void SetMaterials( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials );
	virtual DXMATERIAL_CHAR_EFF*		GetMaterials () { return m_pMaterials; }

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	// Color 를 MaterialSystem 에서 사용하기 위한 작업.
	void LoadUserColorMaskTexture( int nMaterial );
	LPDIRECT3DTEXTUREQ GetUserColorMaskTexture( int nMaterial );
	void GetUserColor( int nMaterial, D3DXVECTOR4& sColor );

	//////////////////////////////////////////////////////////////////////////
	// Material System 에 현재 효과들을 붙이기 위한 작업.
public:

	// Effect Texture 들이 다 로딩이 되었는가~?
	BOOL IsEnableEffTexture( BOOL bTexThread ) OVERRIDE;

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender );

	// ThreadLoading 으로 인해 m_pmcMesh 정보가 없을 수 있어서 사용하는 곳에서 매번 체크하도록 한다.
private:
	BOOL IsExist_Copy_pmcMesh();

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행

public:
	DxEffCharUserColor(void);
	virtual ~DxEffCharUserColor(void);
};
