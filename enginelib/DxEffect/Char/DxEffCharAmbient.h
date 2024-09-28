#pragma once

#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"

#define	USEALL			                0x0001
#define	USETEXCOLOR		                0x0002
#define	USE_EFFCHAR_AMBIENT_BASECOLOR   0x0004  // 어두운 지역에서 캐릭터가 어두워서 밝게 하기 위해 넣은 효과 - 그걸 알리기 위한 Flag

struct EFFCHAR_PROPERTY_AMBIENT_100
{
	DWORD		m_dwColorOP;
	float		m_fFullTime;
	D3DXCOLOR	m_cDiffuse;
	D3DXCOLOR	m_cAmbient;
};

struct EFFCHAR_PROPERTY_AMBIENT_101
{
	DWORD		m_dwFlage;
	DWORD		m_dwColorOP;
	int			m_nFrame;
	float		m_fFullTime;
	D3DXCOLOR	m_cDiffuse;
	D3DXCOLOR	m_cAmbient;
};

struct EFFCHAR_PROPERTY_AMBIENT_102
{
	DWORD		m_dwFlage;
	DWORD		m_dwColorOP;
	float		m_fFullTime;
	D3DXCOLOR	m_cDiffuse;
	D3DXCOLOR	m_cAmbient;
};

struct EFFCHAR_PROPERTY_AMBIENT_103	//	데이터 포맷 변경 진행
{
	DWORD		m_dwFlage;
	DWORD		m_dwColorOP;			// 삭제 하자.
	float		m_fFullTime;
	float		m_fSrcColor;			// 삭제 하자.
	D3DXCOLOR	m_cColor;

};

struct EFFCHAR_PROPERTY_AMBIENT : public EFFCHAR_PROPERTY	
{
	DWORD		m_dwFlage;
	float		m_fFullTime;
	D3DXCOLOR	m_cColor;
	float		m_fSrcColor;			// 삭제 하자.
	DWORD		m_dwColorOP;			// 삭제 하자.	

	EFFCHAR_PROPERTY_AMBIENT () :
		m_dwFlage(0),
		m_dwColorOP(D3DTOP_MODULATE),
		m_fFullTime ( 0.06f ),
		m_fSrcColor ( 0.4f ),
		m_cColor(D3DCOLOR_ARGB(0,255,0,0))
	{
		m_dwFlage |= USEALL;
	}
};

class DxEffCharAmbient : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID() override { return TYPEID; }
	virtual DWORD GetFlag() override   { return EMECF_AMBIENT; }
	virtual const char* GetName() override { return NAME; }

	D3DXCOLOR	  GetColor()		   { return m_cColor; }
	bool		  GetUseTextureColor() { if( m_dwFlage & USETEXCOLOR ) return TRUE; else return FALSE; }

    enum
    {

    };

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_AMBIENT m_Property;
		};
		struct
		{
			DWORD		m_dwFlage;
			float		m_fFullTime;
			D3DXCOLOR	m_cColor;
			float		m_fSrcColor;
			DWORD		m_dwColorOP;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_AMBIENT*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:
	float	m_fElapsedTime;

	DWORD	m_dwOldTexFactor;
	DWORD	m_dwOldColorARG1;
	DWORD	m_dwOldColorARG2;
	DWORD	m_dwOldColorOP;
	DWORD	m_dwOldAmbient;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
    DWORD GetEffAmbientType();
    const D3DXCOLOR& GetEffAmbientColor();

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );

	virtual DWORD GetStateOrder () const						{ return EMEFFSO_AMBIENT; }
	virtual HRESULT	SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA );
	virtual HRESULT	RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice );

	//////////////////////////////////////////////////////////////////////////
	// Material System 에 현재 효과들을 붙이기 위한 작업.
public:
	virtual void PrecalculateForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
												EffectMaterialData* pEffMaterialData, 
												const D3DXMATRIX* pMatrix, 
												const float fScale );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행	

public:
	DxEffCharAmbient(void);
	virtual ~DxEffCharAmbient(void);
};
