#pragma once

#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"

struct EFFCHAR_PROPERTY_MARK_0100	//	데이터 포맷 변경 진행
{
	DWORD			m_dwFlag;

	int				m_nBlend;
	int				m_nTexNum;

	D3DXCOLOR		m_cDiffuse1;
	D3DXCOLOR		m_cDiffuse2;

	D3DXVECTOR2		m_vTex01;
	D3DXVECTOR2		m_vTex02;

	D3DXVECTOR2		m_vTexUV01;
	D3DXVECTOR2		m_vTexUV02;

	char			m_szTex1[MAX_PATH];
	char			m_szTex2[MAX_PATH];
};

struct EFFCHAR_PROPERTY_MARK : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;

	int				m_nBlend;
	int				m_nTexNum;

	char			m_szTex1[MAX_PATH];
	char			m_szTex2[MAX_PATH];

	D3DXCOLOR		m_cDiffuse1;
	D3DXCOLOR		m_cDiffuse2;

	D3DXVECTOR2		m_vTex01;
	D3DXVECTOR2		m_vTex02;

	D3DXVECTOR2		m_vTexUV01;
	D3DXVECTOR2		m_vTexUV02;

	EFFCHAR_PROPERTY_MARK () :
		m_nBlend(0),
		m_nTexNum(1),
		m_vTex01(0.1f,0.f),
		m_vTex02(0.f,0.1f),
		m_vTexUV01(1.f,1.f),
		m_vTexUV02(1.f,1.f),
		m_cDiffuse1(0xffdddddd),
		m_cDiffuse2(0xffdddddd)
	{
		m_dwFlag = 0L;

		StringCchCopy( m_szTex1, MAX_PATH, "_Wa_water1.bmp" );	//
		StringCchCopy( m_szTex2, MAX_PATH, "_Wa_water1.bmp" );	//
	}

	void Assign( EFFCHAR_PROPERTY_MARK_0100& sOldData ) 
	{
		m_dwFlag = sOldData.m_dwFlag;
		m_nBlend = sOldData.m_nBlend;
		m_nTexNum = sOldData.m_nTexNum;
		StringCchCopy( m_szTex1, MAX_PATH, sOldData.m_szTex1 );
		StringCchCopy( m_szTex2, MAX_PATH, sOldData.m_szTex2 );
		m_cDiffuse1 = sOldData.m_cDiffuse1;
		m_cDiffuse2 = sOldData.m_cDiffuse2;
		m_vTex01 = sOldData.m_vTex01;
		m_vTex02 = sOldData.m_vTex02;
		m_vTexUV01 = sOldData.m_vTexUV01;
		m_vTexUV02 = sOldData.m_vTexUV02;
	}
};

class DxEffCharMark : public DxEffChar
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
			EFFCHAR_PROPERTY_MARK m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;

			int				m_nBlend;
			int				m_nTexNum;

			char			m_szTex1[MAX_PATH];
			char			m_szTex2[MAX_PATH];

			D3DXCOLOR		m_cDiffuse1;
			D3DXCOLOR		m_cDiffuse2;

			D3DXVECTOR2		m_vTex01;
			D3DXVECTOR2		m_vTex02;

			D3DXVECTOR2		m_vTexUV01;
			D3DXVECTOR2		m_vTexUV02;

		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_MARK*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:	
	static LPDIRECT3DSTATEBLOCK9		m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9		m_pEffectStateBlock;

protected:
	D3DXVECTOR2			m_vSumTex01;
	D3DXVECTOR2			m_vSumTex02;

    TextureResource m_textureRes1;
    TextureResource m_textureRes2;

protected:
	DxSkinMesh9*				m_pSkinMesh;
	PSMESHCONTAINER				m_pmcMesh;

protected:
	DXMATERIAL_CHAR_EFF*		m_pMaterials;
	DWORD						m_dwMaterials;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );

	virtual DWORD GetStateOrder () const						{ return EMEFFSO_MARK; }
	virtual HRESULT	SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA );
	virtual HRESULT	RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행	

public:
	DxEffCharMark(void);
	virtual ~DxEffCharMark(void);
};
